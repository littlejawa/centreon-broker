#include <iostream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/mysql_manager.hh"

using namespace com::centreon::broker;

mysql_manager mysql_manager::_singleton;

mysql_manager& mysql_manager::instance() {
  return _singleton;
}

mysql_manager::mysql_manager()
  : _current_thread(0),
    _version(mysql::v2),
    _count_ref(0) {
  if (mysql_library_init(0, nullptr, nullptr))
    throw exceptions::msg()
      << "mysql_manager: unable to initialize the MySQL connector";
}

mysql_manager::~mysql_manager() {
  mysql_library_end();
}

std::vector<std::shared_ptr<mysql_connection>> mysql_manager::get_connections(
                      database_config const& db_cfg) {
  std::vector<std::shared_ptr<mysql_connection>> retval;
  int connection_count(db_cfg.get_connections_count());
  int current_connection(0);

  //std::lock_guard<std::mutex> lock(_cfg_mutex);
  for (std::shared_ptr<mysql_connection> c : _connection) {
    // Is this thread matching what the configuration needs?
    if (c->match_config(db_cfg)) {
      // Yes
      retval.push_back(c);
      ++current_connection;
      if (current_connection > connection_count)
        return retval;
    }
  }

  // We are still missing threads in the configuration to return
  for ( ; current_connection < connection_count; ++current_connection) {
    std::shared_ptr<mysql_connection> c(std::make_shared<mysql_connection>(db_cfg));
    _connection.push_back(c);
    retval.push_back(c);
  }

  return retval;
}

bool mysql_manager::is_in_error() const {
  std::lock_guard<std::mutex> locker(_err_mutex);
  return _error.is_active();
}

com::centreon::broker::mysql_error mysql_manager::get_error() {
  std::lock_guard<std::mutex> locker(_err_mutex);
  return std::move(_error);
}

void mysql_manager::set_error(std::string const& message, bool fatal) {
  std::lock_guard<std::mutex> locker(_err_mutex);
  if (!_error.is_active())
    _error = mysql_error(message.c_str(), fatal);
}

void mysql_manager::clear_error() {
  std::lock_guard<std::mutex> locker(_err_mutex);
  _error.clear();
}
