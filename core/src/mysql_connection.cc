/*
** Copyright 2018 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/mysql_manager.hh"

using namespace com::centreon::broker;

const int STR_SIZE = 200;
const int MAX_ATTEMPTS = 10;

void (mysql_connection::* const mysql_connection::_task_processing_table[])(mysql_task* task) = {
  &mysql_connection::_query,
  &mysql_connection::_commit,
  &mysql_connection::_prepare,
  &mysql_connection::_statement,
  &mysql_connection::_get_last_insert_id_sync,
  &mysql_connection::_check_affected_rows,
  &mysql_connection::_get_affected_rows_sync,
  &mysql_connection::_fetch_row_sync,
  &mysql_connection::_finish,
};

/******************************************************************************/
/*                      Methods executed by this thread                       */
/******************************************************************************/

void mysql_connection::_query(mysql_task* t) {
  mysql_task_run* task(static_cast<mysql_task_run*>(t));
  logging::debug(logging::low)
    << "mysql: run query: "
    << task->query.c_str();
  if (mysql_query(_conn, task->query.c_str())) {
    logging::debug(logging::low)
      << "mysql: run query failed: "
      << ::mysql_error(_conn);
    if (task->fatal) {
      if (task->promise) {
        exceptions::msg e;
        e << ::mysql_error(_conn);
        task->promise->set_exception(
                         std::make_exception_ptr<exceptions::msg>(e));
      }
      else
        mysql_manager::instance().set_error(::mysql_error(_conn), true);
    }
    else {
      logging::error(logging::medium) << task->error_msg
        << "could not execute query: " << ::mysql_error(_conn) << " (" << task->query << ")";
    }
  }
  else if (task->promise) {
    /* All is good here */
    task->promise->set_value(mysql_result(this, mysql_store_result(_conn)));
  }
}

void mysql_connection::_commit(mysql_task* t) {
  mysql_task_commit* task(static_cast<mysql_task_commit*>(t));
  int attempts(0);
  while (attempts++ < MAX_ATTEMPTS && mysql_commit(_conn)) {
    logging::error(logging::medium)
      << "could not commit queries: " << ::mysql_error(_conn);
  }
  if (--task->count == 0)
    task->promise->set_value(true);
}

void mysql_connection::_prepare(mysql_task* t) {
  mysql_task_prepare* task(static_cast<mysql_task_prepare*>(t));
  if (_stmt[task->id]) {
    logging::info(logging::low)
      << "mysql: Statement already prepared: "
      << task->id << " ( " << task->query << " )";
    return ;
  }

  // FIXME DBR: to debug
  _stmt_query[task->id] = task->query;

  logging::debug(logging::low)
    << "mysql: prepare query: "
    << task->id << " ( " << task->query << " )";
  MYSQL_STMT* stmt(mysql_stmt_init(_conn));
  if (!stmt)
    mysql_manager::instance().set_error(
      "statement initialization failed: insuffisant memory", true);
  else {
    if (mysql_stmt_prepare(stmt, task->query.c_str(), task->query.size())) {
      logging::debug(logging::low)
        << "mysql: prepare failed ("
        << ::mysql_stmt_error(stmt);
      std::ostringstream oss;
      oss << "statement preparation failed ("
          << mysql_stmt_error(stmt) << ")";
      mysql_manager::instance().set_error(oss.str(), true);
    }
    else
      _stmt[task->id] = stmt;
  }
}

void mysql_connection::_statement(mysql_task* t) {
  mysql_task_statement* task(static_cast<mysql_task_statement*>(t));
  logging::debug(logging::low)
    << "mysql: execute statement: "
    << task->statement_id;
  MYSQL_STMT* stmt(_stmt[task->statement_id]);
  if (!stmt) {
    logging::debug(logging::low)
      << "mysql: no statement to execute";
    mysql_manager::instance().set_error("statement not prepared", true);
    return ;
  }
  MYSQL_BIND* bb(NULL);
  if (task->bind.get())
    bb = const_cast<MYSQL_BIND*>(task->bind->get_bind());

//  int row_size = 0;
//  if (task->array_size) {
//    mysql_stmt_attr_set(stmt, STMT_ATTR_ARRAY_SIZE, &task->array_size);
//  }

  if (bb && mysql_stmt_bind_param(stmt, bb)) {
    logging::debug(logging::low)
      << "mysql: statement binding failed ("
      << mysql_stmt_error(stmt) << ")";
    if (task->fatal) {
      if (task->promise) {
        exceptions::msg e;
        e << mysql_stmt_error(stmt);
        task->promise->set_exception(
                         std::make_exception_ptr<exceptions::msg>(e));
      }
      else
        mysql_manager::instance().set_error(mysql_stmt_error(stmt), task->fatal);
    }
    else {
      logging::error(logging::medium)
        << "mysql: Error while binding values in statement: "
        << mysql_stmt_error(stmt);
    }
  }
  else {
    int attempts(0);
    while (true) {
      if (mysql_stmt_execute(stmt)) {
        if (mysql_stmt_errno(stmt) != 1213
            && mysql_stmt_errno(stmt) != 1205)  // Dead Lock error
          attempts = MAX_ATTEMPTS;

        mysql_commit(_conn);

        if (++attempts >= MAX_ATTEMPTS) {
          if (task->fatal) {
            if (task->promise) {
              exceptions::msg e;
              e << mysql_stmt_error(stmt);
              task->promise->set_exception(
                  std::make_exception_ptr<exceptions::msg>(e));
            }
            else
              mysql_manager::instance().set_error(mysql_stmt_error(stmt), task->fatal);
          }
          else {
            logging::error(logging::medium)
              << "mysql: Error while sending prepared query: "
              << mysql_stmt_error(stmt)
              << " (" << task->error_msg << ")";
          }
          break;
        }
      }
      else {
        if (task->promise) {
          mysql_result res(this, task->statement_id);
          MYSQL_STMT* stmt(_stmt[task->statement_id]);
          MYSQL_RES* prepare_meta_result(mysql_stmt_result_metadata(stmt));
          if (prepare_meta_result == NULL) {
            if (mysql_stmt_errno(stmt)) {
              exceptions::msg e;
              e << mysql_stmt_error(stmt);
              task->promise->set_exception(
                  std::make_exception_ptr<exceptions::msg>(e));
            }
            else
              task->promise->set_value(nullptr);
          }
          else {
            int size(mysql_num_fields(prepare_meta_result));
            std::unique_ptr<database::mysql_bind> bind(new database::mysql_bind(size, STR_SIZE));

            if (mysql_stmt_bind_result(stmt, bind->get_bind())) {
              exceptions::msg e;
              e << mysql_stmt_error(stmt);
              task->promise->set_exception(
                  std::make_exception_ptr<exceptions::msg>(e));
            }
            else {
              if (mysql_stmt_store_result(stmt)) {
                exceptions::msg e;
                e << mysql_stmt_error(stmt);
                task->promise->set_exception(
                    std::make_exception_ptr<exceptions::msg>(e));
              }
              // Here, we have the first row.
              res.set(prepare_meta_result);
              bind->set_empty(true);
            }
            res.set_bind(move(bind));
            task->promise->set_value(std::move(res));
          }
        }
        break;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
  }
}

/**
 *  Run a query synchronously. The result is stored in _result and if an error
 *  occurs, it is stored in _error_msg.
 *
 *  @param task         The task to realize, it contains a query.
 */
void mysql_connection::_get_last_insert_id_sync(mysql_task* t) {
  mysql_task_last_insert_id* task(static_cast<mysql_task_last_insert_id*>(t));
  task->promise->set_value(mysql_insert_id(_conn));
}

void mysql_connection::_check_affected_rows(mysql_task* t) {
  mysql_task_check_affected_rows* task(static_cast<mysql_task_check_affected_rows*>(t));
  int count;
  if (task->statement_id)
    count = mysql_stmt_affected_rows(_stmt[task->statement_id]);
  else
    count = mysql_affected_rows(_conn);
  if (count == 0)
    logging::error(logging::medium)
      << task->message;
}

void mysql_connection::_get_affected_rows_sync(mysql_task* t) {
  mysql_task_affected_rows* task(static_cast<mysql_task_affected_rows*>(t));
  if (task->statement_id)
    task->promise->set_value(mysql_stmt_affected_rows(_stmt[task->statement_id]));
  else
    task->promise->set_value(mysql_affected_rows(_conn));
}

void mysql_connection::_fetch_row_sync(mysql_task* t) {
  mysql_task_fetch* task(static_cast<mysql_task_fetch*>(t));
  int stmt_id(task->result->get_statement_id());
  if (stmt_id) {
    MYSQL_STMT* stmt(_stmt[stmt_id]);
    int res(mysql_stmt_fetch(stmt));
    if (res != 0)
      task->result->get_bind()->set_empty(true);
    task->promise->set_value(res == 0);
  }
  else {
    MYSQL_ROW r(mysql_fetch_row(task->result->get()));
    task->result->set_row(r);
    task->promise->set_value(r != nullptr);
  }
}

void mysql_connection::_finish(mysql_task* t) {
  _finished = true;
}

std::string mysql_connection::_get_stack() {
  std::string retval;
  for (std::shared_ptr<mysql_task> t : _tasks_list) {
    switch (t->type) {
      case mysql_task::RUN:
        retval += "RUN ; ";
        break;
      case mysql_task::COMMIT:
        retval += "COMMIT ; ";
        break;
      case mysql_task::PREPARE:
        retval += "PREPARE ; ";
        break;
      case mysql_task::STATEMENT:
        retval += "STATEMENT ; ";
        break;
      case mysql_task::LAST_INSERT_ID:
        retval += "LAST_INSERT_ID ; ";
        break;
      case mysql_task::CHECK_AFFECTED_ROWS:
        retval += "CHECK_AFFECTED_ROWS ; ";
        break;
      case mysql_task::AFFECTED_ROWS:
        retval += "AFFECTED_ROWS ; ";
        break;
      case mysql_task::FETCH_ROW:
        retval += "FETCH_ROW ; ";
        break;
      case mysql_task::FINISH:
        retval += "FINISH ; ";
        break;
    }
  }
  return retval;
}

void mysql_connection::_run() {
  std::unique_lock<std::mutex> locker(_result_mutex);
  _conn = mysql_init(NULL);
  if (!_conn) {
    mysql_manager::instance().set_error(::mysql_error(_conn), true);
  }
  else if (!mysql_real_connect(
         _conn,
         _host.c_str(),
         _user.c_str(),
         _pwd.c_str(),
         _name.c_str(),
         _port,
         NULL,
         0)) {
    mysql_manager::instance().set_error(::mysql_error(_conn), true);
  }

  if (_qps > 1)
    mysql_autocommit(_conn, 0);
  else
    mysql_autocommit(_conn, 1);

  _started = true;
  locker.unlock();
  _result_condition.notify_all();

  while (!_finished) {
    std::unique_lock<std::mutex> locker(_list_mutex);
    if (!_tasks_list.empty()) {
      std::shared_ptr<mysql_task> task(_tasks_list.front());
      _tasks_list.pop_front();
      locker.unlock();
      --_tasks_count;
      if (_task_processing_table[task->type])
        (this->*(_task_processing_table[task->type]))(task.get());
      else {
        logging::error(logging::medium)
          << "mysql: Error type not managed...";
      }
    }
    else {
      _tasks_count = 0;
      _tasks_condition.wait(locker);
    }
  }
}

/******************************************************************************/
/*                    Methods executed by the main thread                     */
/******************************************************************************/

mysql_connection::mysql_connection(database_config const& db_cfg)
  : _conn(NULL),
    _finished(false),
    _host(db_cfg.get_host()),
    _user(db_cfg.get_user()),
    _pwd(db_cfg.get_password()),
    _name(db_cfg.get_name()),
    _port(db_cfg.get_port()),
    _started(false),
    _qps(db_cfg.get_queries_per_transaction()) {

  std::unique_lock<std::mutex> locker(_result_mutex);
  _thread.reset(new std::thread(&mysql_connection::_run, this));
  while (!_started)
    _result_condition.wait(locker);
  if (mysql_manager::instance().is_in_error()) {
    finish();
    _thread->join();
    mysql_error err(mysql_manager::instance().get_error());
    throw exceptions::msg() << err.get_message();
  }
}

mysql_connection::~mysql_connection() {
  for (umap<unsigned int, MYSQL_STMT*>::iterator
         it(_stmt.begin()),
         end(_stmt.end());
       it != end;
       ++it)
    mysql_stmt_close(it->second);

  mysql_close(_conn);
  mysql_thread_end();
  finish();
  _thread->join();
}

void mysql_connection::_push(std::shared_ptr<mysql_task> const& q) {
  std::lock_guard<std::mutex> locker(_list_mutex);
  _tasks_list.push_back(q);
  ++_tasks_count;
  _tasks_condition.notify_all();
}

void mysql_connection::check_affected_rows(
                     std::string const& message,
                     int statement_id) {
  _push(std::make_shared<mysql_task_check_affected_rows>(message, statement_id));
}

/**
 *  This method is used from the main thread to execute synchronously a query.
 *
 *  @param query The SQL query
 *  @param error_msg The error message to return in case of error.
 *  @throw an exception in case of error.
 */
int mysql_connection::get_affected_rows(int statement_id) {
  std::promise<int> promise;
  _push(std::make_shared<mysql_task_affected_rows>(&promise, statement_id));
  return promise.get_future().get();
}

int mysql_connection::get_last_insert_id() {
  std::promise<int> promise;
  _push(std::make_shared<mysql_task_last_insert_id>(&promise));
  return promise.get_future().get();;
}

/**
 *  This method finishes to send current tasks and then commits. The commited variable
 *  is then incremented of the queries committed count.
 *  This function is called by mysql::commit whom goal is to commit on each of the connections.
 *  So, this last method waits all the commits to be done ; the semaphore is there for that
 *  purpose.
 *
 *  @param[out] promise This promise is set when count == 0
 *  @param count The integer counting how many queries are committed.
 */
void mysql_connection::commit(std::promise<bool>* promise, std::atomic_int& count) {
  _push(std::make_shared<mysql_task_commit>(promise, count));
}

void mysql_connection::run_statement(mysql_stmt& stmt,
                     std::promise<mysql_result>* p,
                     std::string const& error_msg, bool fatal) {
  _push(std::make_shared<mysql_task_statement>(stmt, p, error_msg, fatal));
}

void mysql_connection::prepare_query(int stmt_id, std::string const& query) {
  _push(std::make_shared<mysql_task_prepare>(stmt_id, query));
}

/**
 *  This method is used from the main thread to execute asynchronously a query.
 *  No exception is thrown in case of error since this query is made asynchronously.
 *
 *  @param query The SQL query
 *  @param error_msg The error message to return in case of error.
 *  @param p A pointer to a promise.
 */
void mysql_connection::run_query(
                     std::string const& query,
                     std::promise<mysql_result>* p,
                     std::string const& error_msg, bool fatal) {
  _push(std::make_shared<mysql_task_run>(query, error_msg, fatal, p));
}

void mysql_connection::finish() {
  _push(std::make_shared<mysql_task_finish>());
}

bool mysql_connection::fetch_row(mysql_result& result) {
  std::promise<bool> promise;
  _push(std::make_shared<mysql_task_fetch>(&result, &promise));
  return promise.get_future().get();
}

bool mysql_connection::match_config(database_config const& db_cfg) const {
  std::lock_guard<std::mutex> lock(_cfg_mutex);
  return db_cfg.get_host() == _host
         && db_cfg.get_user() == _user
         && db_cfg.get_password() == _pwd
         && db_cfg.get_name() == _name
         && db_cfg.get_port() == _port
         && db_cfg.get_queries_per_transaction() == _qps;
}

int mysql_connection::get_tasks_count() const {
  return _tasks_count;
}
