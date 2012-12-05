/*
** Copyright 2011-2012 Merethis
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

#include "com/centreon/broker/exceptions/msg.hh"
#include "temporary_endpoint.hh"
#include "temporary_stream.hh"

using namespace com::centreon::broker;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
temporary_endpoint::temporary_endpoint()
  : io::endpoint(false) {

}

/**
 *  Copy constructor.
 *
 *  @param[in] se Object to copy.
 */
temporary_endpoint::temporary_endpoint(temporary_endpoint const& se)
  : io::endpoint(se) {

}

/**
 *  Destructor.
 */
temporary_endpoint::~temporary_endpoint() {
  this->close();
}

/**
 *  Assignment operator.
 *
 *  @param[in] se Object to copy.
 *
 *  @return This object.
 */
temporary_endpoint& temporary_endpoint::operator=(temporary_endpoint const& se) {
  if (&se != this)
    com::centreon::broker::io::endpoint::operator=(se);
  return (*this);
}

/**
 *  Clone endpoint.
 */
com::centreon::broker::io::endpoint* temporary_endpoint::clone() const {
  return (new temporary_endpoint(*this));
}

/**
 *  Close endpoint.
 */
void temporary_endpoint::close() {
  return ;
}

/**
 *  Open endpoint.
 *
 *  @return New temporary_stream.
 */
misc::shared_ptr<io::stream> temporary_endpoint::open() {
  return (misc::shared_ptr<io::stream>(new temporary_stream));
}