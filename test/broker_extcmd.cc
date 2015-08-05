/*
** Copyright 2015 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
*/

#include <cstdio>
#include <QLocalSocket>
#include <sstream>
#include <unistd.h>
#include "com/centreon/broker/exceptions/msg.hh"
#include "test/broker_extcmd.hh"

using namespace com::centreon::broker;

/**
 *  Default constructor.
 */
broker_extcmd::broker_extcmd() {
  set_file(tmpnam(NULL));
}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
broker_extcmd::broker_extcmd(broker_extcmd const& other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
broker_extcmd::~broker_extcmd() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
broker_extcmd& broker_extcmd::operator=(broker_extcmd const& other) {
  if (this != &other)
    _internal_copy(other);
  return (*this);
}

/**
 *  Execute an external command.
 *
 *  @param[in] query         Query to execute.
 *  @param[in] wait_command  Wait for command execute ?
 *
 *  @return True if command is pending.
 */
bool broker_extcmd::execute(
                      std::string const& query,
                      bool wait_command) {
  // Connect to socket.
  QLocalSocket sockt;
  sockt.connectToServer(_file.c_str());
  if (!sockt.waitForConnected())
    throw (exceptions::msg() << "could not connect to socket: "
           << sockt.errorString());

  // Write query and read result.
  {
    std::ostringstream oss;
    oss << query << "\n";
    _write(sockt, oss.str());
  }
  unsigned int id;
  bool pending;
  std::string msg;
  _read(sockt, id, pending, msg, query);
  std::string status_cmd;
  {
    std::ostringstream oss;
    oss << "STATUS;" << id << "\n";
    status_cmd = oss.str();
  }
  while (wait_command && pending) {
    ::usleep(100000);
    _write(sockt, status_cmd);
    _read(sockt, id, pending, msg, query);
  } while (wait_command && pending);

  // Close socket.
  sockt.close();

  return (pending);
}

/**
 *  Get command file.
 *
 *  @return Command file.
 */
std::string const& broker_extcmd::get_file() const throw () {
  return (_file);
}

/**
 *  Set command file.
 *
 *  @param[in] file  Command file.
 */
void broker_extcmd::set_file(std::string const& file) {
  _file = file;
  return ;
}

/**
 *  Copy internal data members.
 *
 *  @param[in] other  Object to copy.
 */
void broker_extcmd::_internal_copy(broker_extcmd const& other) {
  _file = other._file;
  return ;
}

/**
 *  Read result from socket.
 *
 *  @param[in,out] sockt    Socket object.
 *  @param[out]    id       Command ID.
 *  @param[out]    pending  Set to true if query is still pending.
 *  @param[out]    msg      Status message.
 *  @param[in]     query    Initial query.
 */
void broker_extcmd::_read(
                      QLocalSocket& sockt,
                      unsigned int& id,
                      bool& pending,
                      std::string& msg,
                      std::string const& query) {
  char buffer[1000];
  sockt.waitForReadyRead();
  sockt.readLine(buffer, sizeof(buffer));
  if (buffer[0] != '\0')
    buffer[strlen(buffer) - 1] = '\0';
  char* limit1(strchr(buffer, ' '));
  if (!limit1)
    throw (exceptions::msg() << "invalid result format (query was '"
           << query << "': " << buffer);
  char* limit2(strchr(limit1 + 1, ' '));
  if (!limit2)
    throw (exceptions::msg() << "invalid result format (query was '"
           << query << "': " << buffer);
  *limit1 = '\0';
  *limit2 = '\0';
  id = strtoul(buffer, NULL, 0);
  unsigned int code(strtoul(limit1 + 1, NULL, 0));
  msg = limit2 + 1;
  if ((code != 0) && (code != 1))
    throw (exceptions::msg() << "command execution failed: " << msg);
  pending = (code == 1);
  return ;
}

/**
 *  Write query to socket.
 *
 *  @param[in,out] sockt  Socket object.
 *  @param[in]     query  Query to send.
 */
void broker_extcmd::_write(
                      QLocalSocket& sockt,
                      std::string const& query) {
  char const* buffer(query.c_str());
  int remaining(query.size());
  while (remaining > 0) {
    int wb(sockt.write(buffer, remaining));
    if (wb <= 0)
      throw (exceptions::msg() << "cannot write query '"
             << query << "' to socket '" << _file << "': "
             << sockt.errorString());
    buffer += wb;
    remaining -= wb;
  }
  return ;
}
