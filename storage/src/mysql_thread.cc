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
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/storage/mysql_thread.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::storage;

/******************************************************************************/
/*                      Methods executed by this thread                       */
/******************************************************************************/

void mysql_thread::_run(mysql_task_run* task) {
  if (mysql_query(_conn, task->query.c_str())) {
    std::cout << "run query failed: " << mysql_error(_conn) << std::endl;
    logging::error(logging::medium) << task->error_msg
      << "could not execute query: " << mysql_error(_conn) << " (" << task->query << ")";
  }
  else if (task->fn) {
    // FIXME DBR: we need a way to send an error to the main thread
    int ret(task->fn(_conn));
    logging::info(logging::medium)
      << "storage: callback returned " << ret;
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
void mysql_thread::_run_sync(mysql_task_run_sync* task) {
  QMutexLocker locker(&_result_mutex);
  if (mysql_query(_conn, task->query.c_str()))
    _error_msg = mysql_error(_conn);
  else
    _result = mysql_store_result(_conn);

  _result_condition.wakeAll();
}

void mysql_thread::_commit(mysql_task_commit* task) {
  if (mysql_commit(_conn)) {
    std::cout << "commit queries: " << mysql_error(_conn) << std::endl;
    logging::error(logging::medium)
      << "could not commit queries: " << mysql_error(_conn);
    task->count.fetchAndAddRelease(1);
  }
  task->sem.release();
  std::cout << "sem release from thread : available = " << task->sem.available() << std::endl;
}

void mysql_thread::_prepare(mysql_task_prepare* task) {
  MYSQL_STMT* stmt(mysql_stmt_init(_conn));
  if (!stmt) {
    logging::error(logging::medium)
      << "storage: Could not initialize statement";
  }
  else {
    if (mysql_stmt_prepare(stmt, task->query.c_str(), task->query.size())) {
      logging::error(logging::medium)
        << "storage: Could not prepare statement";
    }
    _stmt.push_back(stmt);
  }
}

void mysql_thread::_statement(mysql_task_statement* task) {
  if (mysql_stmt_bind_param(_stmt[task->statement_id], const_cast<MYSQL_BIND*>(task->bind.get_bind()))) {
    logging::error(logging::medium)
      << "storage: Error while binding values in statement: "
      << mysql_stmt_error(_stmt[task->statement_id]);
  }
  if (mysql_stmt_execute(_stmt[task->statement_id])) {
    logging::error(logging::medium)
      << "storage: Error while sending prepared query: "
      << mysql_stmt_error(_stmt[task->statement_id]);
  }
}

void mysql_thread::run() {
  while (!_finished) {
    std::cout << "run mutex lock" << std::endl;
    QMutexLocker locker(&_list_mutex);
    std::cout << "run mutex locked" << std::endl;
    if (!_tasks_list.empty()) {
      std::cout << "new task" << std::endl;
      misc::shared_ptr<mysql_task> task(_tasks_list.front());
      _tasks_list.pop_front();
      std::cout << "unlock mutex" << std::endl;
      locker.unlock();
      std::cout << "mutex unlocked" << std::endl;
      switch (task->type) {
       case mysql_task::RUN:
        std::cout << "run RUN" << std::endl;
        _run(static_cast<mysql_task_run*>(task.data()));
        break;
       case mysql_task::RUN_SYNC:
        std::cout << "run RUN SYNC" << std::endl;
        _run_sync(static_cast<mysql_task_run_sync*>(task.data()));
        break;
       case mysql_task::COMMIT:
        std::cout << "run COMMIT" << std::endl;
        _commit(static_cast<mysql_task_commit*>(task.data()));
        break;
       case mysql_task::PREPARE:
        std::cout << "run PREPARE" << std::endl;
        _prepare(static_cast<mysql_task_prepare*>(task.data()));
        break;
       case mysql_task::STATEMENT:
        std::cout << "run STATEMENT" << std::endl;
        _statement(static_cast<mysql_task_statement*>(task.data()));
        break;
       case mysql_task::FINISH:
        std::cout << "run FINISH" << std::endl;
        _finished = true;
        break;
       default:
        std::cout << "ERROR: run DEFAULT SITUATION with type = " << task->type << std::endl;
        logging::error(logging::medium)
          << "storage: Error type not managed...";
        break;
      }
    }
    else {
      std::cout << "run wait for condition on queries or finish" << std::endl;
      _tasks_condition.wait(locker.mutex());
      std::cout << "run condition realized" << std::endl;
    }
  }
  std::cout << "run return" << std::endl;
}

/******************************************************************************/
/*                    Methods executed by the main thread                     */
/******************************************************************************/

mysql_thread::mysql_thread(database_config const& db_cfg)
  : _conn(mysql_init(NULL)),
    _finished(false) {
  std::cout << "mysql_thread constructor" << std::endl;
  if (!_conn) {
    std::cout << "mysql_thread throw exception" << std::endl;
    throw exceptions::msg()
      << "storage: Unable to initialize the MySQL client connector: "
      << mysql_error(_conn);
  }

  std::cout << "mysql_thread real connect..." << std::endl;
  if (!mysql_real_connect(
         _conn,
         db_cfg.get_host().c_str(),
         db_cfg.get_user().c_str(),
         db_cfg.get_password().c_str(),
         db_cfg.get_name().c_str(),
         db_cfg.get_port(),
         NULL,
         0)) {
    std::cout << "mysql_thread constructor real connect failed" << std::endl;
    std::cout << "mysql_thread throw exception" << std::endl;
    throw exceptions::msg()
      << "storage: The connection to '"
      << db_cfg.get_name() << ":" << db_cfg.get_port()
      << "' MySQL database failed: "
      << mysql_error(_conn);
  }
  if (db_cfg.get_queries_per_transaction() > 1)
    mysql_autocommit(_conn, 0);
  else
    mysql_autocommit(_conn, 1);

  std::cout << "mysql_thread start thread..." << std::endl;
  start();
  std::cout << "mysql_thread return" << std::endl;
}

mysql_thread::~mysql_thread() {
  std::cout << "destructor" << std::endl;
  for (std::vector<MYSQL_STMT*>::iterator
         it(_stmt.begin()),
         end(_stmt.end());
       it != end;
       ++it) {
    mysql_stmt_close(*it);
  }
  mysql_close(_conn);
  mysql_thread_end();
  std::cout << "destructor return" << std::endl;
}

void mysql_thread::_push(misc::shared_ptr<mysql_task> const& q) {
  QMutexLocker locker(&_list_mutex);
  _tasks_list.push_back(q);
  _tasks_condition.wakeAll();
}

/**
 *  This method is used from the main thread to execute asynchronously a query.
 *  No exception is thrown in case of error since this query is made asynchronously.
 *
 *  @param query The SQL query
 *  @param error_msg The error message to return in case of error.
 */
void mysql_thread::run_query(std::string const& query, std::string const& error_msg) {
  _push(misc::shared_ptr<mysql_task>(new mysql_task_run(query, error_msg)));
}

/**
 *  This method is used from the main thread to execute synchronously a query.
 *
 *  @param query The SQL query
 *  @param error_msg The error message to return in case of error.
 *  @throw an exception in case of error.
 */
void mysql_thread::run_query_sync(std::string const& query, std::string const& error_msg) {
  QMutexLocker locker(&_result_mutex);
  _push(misc::shared_ptr<mysql_task>(new mysql_task_run_sync(query)));
  _result_condition.wait(locker.mutex());
  if (!_error_msg.empty()) {
    exceptions::msg e;
    if (!error_msg.empty())
      e << error_msg << ": ";
    e << "could not execute query: "
      << _error_msg << " (" << query << ")";
    _error_msg = "";
    throw e;
  }
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
void mysql_thread::commit(QSemaphore& sem, QAtomicInt& count) {
  _push(misc::shared_ptr<mysql_task>(new mysql_task_commit(sem, count)));
}

void mysql_thread::run_statement(int statement_id, mysql_bind const& bind) {
  _push(misc::shared_ptr<mysql_task>(new mysql_task_statement(statement_id, bind)));
}

void mysql_thread::prepare_query(std::string const& query) {
  _push(misc::shared_ptr<mysql_task>(new mysql_task_prepare(query)));
}

void mysql_thread::run_query_with_callback(
                     std::string const& query,
                     std::string const& error_msg,
                     mysql_callback fn) {
  _push(misc::shared_ptr<mysql_task>(new mysql_task_run(query, error_msg, fn)));
}

void mysql_thread::finish() {
  std::cout << "mysql_thread finish" << std::endl;
  _push(misc::shared_ptr<mysql_task>(new mysql_task_finish()));
}

mysql_result mysql_thread::get_result() {
  mysql_result retval(_result);
  _result = NULL;
  return retval;
}