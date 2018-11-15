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
#include <iostream>
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/mysql_thread.hh"

using namespace com::centreon::broker;

const int STR_SIZE = 200;

void (mysql_thread::* const mysql_thread::_task_processing_table[])(mysql_task* task) = {
  &mysql_thread::_run,
  &mysql_thread::_commit,
  &mysql_thread::_prepare,
  &mysql_thread::_statement,
  &mysql_thread::_statement_on_condition,
  &mysql_thread::_get_last_insert_id_sync,
  &mysql_thread::_check_affected_rows,
  &mysql_thread::_get_affected_rows_sync,
  &mysql_thread::_fetch_row_sync,
  &mysql_thread::_finish,
};

/******************************************************************************/
/*                      Methods executed by this thread                       */
/******************************************************************************/

void mysql_thread::_run(mysql_task* t) {
  mysql_task_run* task(static_cast<mysql_task_run*>(t));
  logging::debug(logging::low)
    << "mysql: run query: "
    << task->query.c_str();
  std::cout << "mysql: run query: "
    << task->query.c_str();
  if (mysql_query(_conn, task->query.c_str())) {
    logging::debug(logging::low)
      << "mysql: run query failed: "
      << ::mysql_error(_conn);
    std::cout << "run query in error...: " << ::mysql_error(_conn) << std::endl;
    if (task->fatal) {
      if (task->promise) {
        exceptions::msg e;
        e << ::mysql_error(_conn);
        task->promise->set_exception(
                         std::make_exception_ptr<exceptions::msg>(e));
      }
      else if (!_error.is_active()) {
        _error = mysql_error(::mysql_error(_conn), true);
      }
    }
    else {
      logging::error(logging::medium) << task->error_msg
        << "could not execute query: " << ::mysql_error(_conn) << " (" << task->query << ")";
    }
  }
  else if (task->promise) {
    /* All is good here */
    task->promise->set_value(mysql_result(mysql_store_result(_conn)));
  }
}

void mysql_thread::_commit(mysql_task* t) {
  mysql_task_commit* task(static_cast<mysql_task_commit*>(t));
  if (mysql_commit(_conn)) {
    std::cout << "commit queries: " << ::mysql_error(_conn) << std::endl;
    logging::error(logging::medium)
      << "could not commit queries: " << ::mysql_error(_conn);
    ++task->count;
  }
  task->sem.release();
}

void mysql_thread::_prepare(mysql_task* t) {
  mysql_task_prepare* task(static_cast<mysql_task_prepare*>(t));
  if (_stmt[task->id]) {
    logging::info(logging::low)
      << "mysql: Statement already prepared: "
      << task->id << " ( " << task->query << " )";
    return ;
  }
  logging::debug(logging::low)
    << "mysql: prepare query: "
    << task->id << " ( " << task->query << " )";
  MYSQL_STMT* stmt(mysql_stmt_init(_conn));
  if (!stmt)
    _error = mysql_error("statement initialization failed: insuffisant memory", true);
  else {
    if (mysql_stmt_prepare(stmt, task->query.c_str(), task->query.size())) {
      logging::debug(logging::low)
        << "mysql: prepare failed ("
        << ::mysql_stmt_error(stmt);
      std::ostringstream oss;
      oss << "statement preparation failed ("
          << mysql_stmt_error(stmt) << ")";
      _error = mysql_error(oss.str().c_str(), true);
    }
    else
      _stmt[task->id] = stmt;
  }
}

void mysql_thread::_statement(mysql_task* t) {
  mysql_task_statement* task(static_cast<mysql_task_statement*>(t));
  _previous = false;
  logging::debug(logging::low)
    << "mysql: execute statement: "
    << task->statement_id;
  MYSQL_STMT* stmt(_stmt[task->statement_id]);
  if (!stmt) {
    logging::debug(logging::low)
      << "mysql: no statement to execute";
    if (!_error.is_active())
      _error = mysql_error("statement not prepared", true);
    return ;
  }
  if (task->bind.get() && mysql_stmt_bind_param(stmt, const_cast<MYSQL_BIND*>(task->bind->get_bind()))) {
    logging::debug(logging::low)
      << "mysql: statement binding failed ("
      << mysql_stmt_error(stmt) << ")";
    std::cout << "ERROR in BINDING: " << mysql_stmt_error(stmt) << std::endl;
    if (task->fatal) {
      if (task->promise) {
        exceptions::msg e;
        e << mysql_stmt_error(stmt);
        task->promise->set_exception(
                         std::make_exception_ptr<exceptions::msg>(e));
      }
      else if (!_error.is_active())
      _error = mysql_error(mysql_stmt_error(stmt), task->fatal);
    }
    else {
      logging::error(logging::medium)
        << "mysql: Error while binding values in statement: "
        << mysql_stmt_error(stmt);
    }
  }
  else if (mysql_stmt_execute(stmt)) {
    logging::debug(logging::low)
      << "mysql: statement execution failed ("
      << mysql_stmt_error(stmt) << ")";
    if (task->fatal) {
      if (task->promise) {
        exceptions::msg e;
        e << mysql_stmt_error(stmt);
        task->promise->set_exception(
                         std::make_exception_ptr<exceptions::msg>(e));
      }
      else if (!_error.is_active()) {
        std::cout << "FATAL ERROR: " << mysql_stmt_error(stmt) << std::endl;
        _error = mysql_error(mysql_stmt_error(stmt), task->fatal);
      }
    }
    else {
      std::cout << "ERROR in STATEMENT: " << mysql_stmt_error(stmt) << std::endl;
      logging::error(logging::medium)
        << "mysql: Error while sending prepared query: "
        << mysql_stmt_error(stmt)
        << " (" << task->error_msg << ")";
    }
  }
  else {
    _previous = true;

    if (task->promise) {
      mysql_result res(task->statement_id);
      MYSQL_STMT* stmt(_stmt[task->statement_id]);
      MYSQL_RES* prepare_meta_result(mysql_stmt_result_metadata(stmt));
      if (prepare_meta_result == NULL) {
        exceptions::msg e;
        e << mysql_stmt_error(stmt);
        task->promise->set_exception(
                         std::make_exception_ptr<exceptions::msg>(e));
      }
      else {
        int size(mysql_num_fields(prepare_meta_result));
        std::unique_ptr<mysql_bind> bind(new mysql_bind(size, STR_SIZE));

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
        res.set_bind(bind);
        task->promise->set_value(std::move(res));
      }
    }
  }
}

void mysql_thread::_statement_on_condition(mysql_task* t) {
  mysql_task_statement_on_condition* task(static_cast<mysql_task_statement_on_condition*>(t));
  switch (task->condition) {
    case mysql_task::ON_ERROR:
      if (_error.is_active()) {
        _error.clear();
        _statement(t);
      }
      break;
    case mysql_task::IF_PREVIOUS:
      if (_previous) {
        _statement(t);
      }
      break;
    case mysql_task::IF_NOT_PREVIOUS:
      if (!_previous) {
        _statement(t);
      }
      break;
  }
}

/**
 *  Run a query synchronously. The result is stored in _result and if an error
 *  occurs, it is stored in _error_msg.
 *  This function locks the _result_mutex and wakes up threads waiting on
 *  _result_condition.
 *
 *  @param task         The task to realize, it contains a query.
 */
void mysql_thread::_get_last_insert_id_sync(mysql_task* t) {
  mysql_task_last_insert_id* task(static_cast<mysql_task_last_insert_id*>(t));
  QMutexLocker locker(&_result_mutex);
  *task->id = mysql_insert_id(_conn);
  _result_condition.wakeAll();
}

void mysql_thread::_check_affected_rows(mysql_task* t) {
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

void mysql_thread::_get_affected_rows_sync(mysql_task* t) {
  mysql_task_affected_rows* task(static_cast<mysql_task_affected_rows*>(t));
  QMutexLocker locker(&_result_mutex);
  if (task->statement_id)
    *task->count = mysql_stmt_affected_rows(_stmt[task->statement_id]);
  else
    *task->count = mysql_affected_rows(_conn);
  _result_condition.wakeAll();
}

//void mysql_thread::_get_result_sync(mysql_task* t) {
//  mysql_task_result* task(static_cast<mysql_task_result*>(t));
//  QMutexLocker locker(&_result_mutex);
//  if (!_error.is_active() || !_error.is_fatal()) {
//    int stmt_id(task->result->get_statement_id());
//    if (stmt_id) {
//      MYSQL_STMT* stmt(_stmt[stmt_id]);
//      MYSQL_RES* prepare_meta_result(mysql_stmt_result_metadata(stmt));
//      if (prepare_meta_result == NULL) {
//        _error = mysql_error(mysql_stmt_error(stmt), true);
//      }
//      else {
//        int size(mysql_num_fields(prepare_meta_result));
//        std::unique_ptr<mysql_bind> bind(new mysql_bind(size, STR_SIZE));
//
//        if (mysql_stmt_bind_result(stmt, bind->get_bind()))
//          _error = mysql_error(mysql_stmt_error(stmt), true);
//        else {
//          if (mysql_stmt_store_result(stmt))
//            _error = mysql_error(mysql_stmt_error(stmt), true);
//
//          // Here, we have the first row.
//          task->result->set(prepare_meta_result);
//          bind->set_empty(true);
//        }
//        task->result->set_bind(bind);
//      }
//    }
//    else
//      task->result->set(mysql_store_result(_conn));
//  }
//  _result_condition.wakeAll();
//}

void mysql_thread::_fetch_row_sync(mysql_task* t) {
  mysql_task_fetch* task(static_cast<mysql_task_fetch*>(t));
  QMutexLocker locker(&_result_mutex);
  int stmt_id(task->result->get_statement_id());
  if (stmt_id) {
    MYSQL_STMT* stmt(_stmt[stmt_id]);
    task->result->get_bind()->set_empty(mysql_stmt_fetch(stmt));
  }
  else
    task->result->set_row(mysql_fetch_row(task->result->get()));
  _result_condition.wakeAll();
}

void mysql_thread::_finish(mysql_task* t) {
  _finished = true;
}

void mysql_thread::run() {
  QMutexLocker locker(&_result_mutex);
  _conn = mysql_init(NULL);
  if (!_conn) {
    _error = mysql_error(::mysql_error(_conn), true);
    _finished = true;
    // FIXME DBR
    // "mysql: Unable to initialize the MySQL client connector: "
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
    std::cout << "mysql_thread::run real connect failed: "<< ::mysql_error(_conn) << std::endl;
    _error = mysql_error(::mysql_error(_conn), true);
    _finished = true;
//    throw exceptions::msg()
//      << "mysql: The connection to '"
//      << _db_cfg.get_name() << ":" << _db_cfg.get_port()
//      << "' MySQL database failed: "
//      << ::mysql_error(_conn);
  }

  if (_qps > 1)
    mysql_autocommit(_conn, 0);
  else
    mysql_autocommit(_conn, 1);

  locker.unlock();
  _result_condition.wakeAll();

  while (!_finished) {
    QMutexLocker locker(&_list_mutex);
    if (!_tasks_list.empty()) {
      std::shared_ptr<mysql_task> task(_tasks_list.front());
      _tasks_list.pop_front();
      locker.unlock();
      if (_task_processing_table[task->type])
        (this->*(_task_processing_table[task->type]))(task.get());
      else {
        std::cout << "ERROR: run DEFAULT SITUATION with type = " << task->type << std::endl;
        logging::error(logging::medium)
          << "mysql: Error type not managed...";
      }
    }
    else
      _tasks_condition.wait(locker.mutex());
  }
  std::cout << "run return" << std::endl;
}

/******************************************************************************/
/*                    Methods executed by the main thread                     */
/******************************************************************************/

mysql_thread::mysql_thread(database_config const& db_cfg)
  : _conn(NULL),
    _finished(false),
    _previous(false),
    _host(db_cfg.get_host()),
    _user(db_cfg.get_user()),
    _pwd(db_cfg.get_password()),
    _name(db_cfg.get_name()),
    _port(db_cfg.get_port()),
    _qps(db_cfg.get_queries_per_transaction()) {

  std::cout << "mysql_thread start thread" << std::endl;
  QMutexLocker locker(&_result_mutex);
  start();
  std::cout << "mysql_thread start WAIT thread" << std::endl;
  _result_condition.wait(locker.mutex());
  std::cout << "mysql_thread wait for start... => GO" << std::endl;
  if (_error.is_active())
    throw exceptions::msg()
      << _error.get_message();
}

mysql_thread::~mysql_thread() {
  for (umap<unsigned int, MYSQL_STMT*>::iterator
         it(_stmt.begin()),
         end(_stmt.end());
       it != end;
       ++it)
    mysql_stmt_close(it->second);

  mysql_close(_conn);
  mysql_thread_end();
}

void mysql_thread::_push(std::shared_ptr<mysql_task> const& q) {
  QMutexLocker locker(&_list_mutex);
  _tasks_list.push_back(q);
  _tasks_condition.wakeAll();
}

void mysql_thread::check_affected_rows(
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
int mysql_thread::get_affected_rows(int statement_id) {
  QMutexLocker locker(&_result_mutex);
  int retval;
  _push(std::make_shared<mysql_task_affected_rows>(&retval, statement_id));
  _result_condition.wait(locker.mutex());
  return retval;
}

int mysql_thread::get_last_insert_id() {
  QMutexLocker locker(&_result_mutex);
  int retval;
  _push(std::make_shared<mysql_task_last_insert_id>(&retval));
  _result_condition.wait(locker.mutex());
  return retval;
}

/**
 *  This method finishes to send current tasks and then commits. The commited variable
 *  is then incremented of the queries committed count.
 *  This function is called by mysql::commit whom goal is to commit on each of the connections.
 *  So, this last method waits all the commits to be done ; the semaphore is there for that
 *  purpose.
 *
 *  @param sem The semaphore used to synchronize commits from various threads.
 *  @param count The integer counting how many queries are committed.
 */
void mysql_thread::commit(QSemaphore& sem, std::atomic_int& count) {
  _push(std::make_shared<mysql_task_commit>(sem, count));
}

void mysql_thread::run_statement(mysql_stmt& stmt,
                     std::promise<mysql_result>* p,
                     std::string const& error_msg, bool fatal) {
  _push(std::make_shared<mysql_task_statement>(stmt, p, error_msg, fatal));
}

void mysql_thread::run_statement_on_condition(
                     mysql_stmt& stmt,
                     std::promise<mysql_result>* promise,
                     mysql_task::condition condition,
                     std::string const& error_msg, bool fatal) {
  _push(std::make_shared<mysql_task_statement_on_condition>(
               stmt,
               promise,
               condition,
               error_msg,
               fatal));
}

void mysql_thread::prepare_query(int stmt_id, std::string const& query) {
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
void mysql_thread::run_query(
                     std::string const& query,
                     std::promise<mysql_result>* p,
                     std::string const& error_msg, bool fatal) {
  _push(std::make_shared<mysql_task_run>(query, error_msg, fatal, p));
}

void mysql_thread::finish() {
  std::cout << "mysql_thread finish" << std::endl;
  _push(std::make_shared<mysql_task_finish>());
}

//mysql_result mysql_thread::get_result(int statement_id) {
//  QMutexLocker locker(&_result_mutex);
//  mysql_result retval(statement_id);
//  _push(std::make_shared<mysql_task_result>(&retval));
//  _result_condition.wait(locker.mutex());
//  if (_error.is_active() && _error.is_fatal())
//    throw exceptions::msg() << _error.get_message();
//  return retval;
//}

bool mysql_thread::fetch_row(mysql_result& result) {
  QMutexLocker locker(&_result_mutex);
  _push(std::make_shared<mysql_task_fetch>(&result));
  _result_condition.wait(locker.mutex());
  return !result.is_empty();
}

com::centreon::broker::mysql_error mysql_thread::get_error() {
  QMutexLocker locker(&_result_mutex);
  mysql_error retval(_error);
  _error.clear();
  return retval;
}
