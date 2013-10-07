/*
** Copyright 2009-2012 Merethis
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

#include "com/centreon/broker/neb/custom_variable.hh"

using namespace com::centreon::broker::neb;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
custom_variable::custom_variable() : var_type(0) {
  modified = false;
}

/**
 *  Copy constructor.
 *
 *  @param[in] cv Object to copy.
 */
custom_variable::custom_variable(custom_variable const& cv)
  : custom_variable_status(cv) {
  _internal_copy(cv);
}

/**
 *  Destructor.
 */
custom_variable::~custom_variable() {}

/**
 *  Assignment operator.
 *
 *  @param[in] cv Object to copy.
 *
 *  @return This object.
 */
custom_variable& custom_variable::operator=(custom_variable const& cv) {
  if (this != &cv) {
    custom_variable_status::operator=(cv);
    _internal_copy(cv);
  }
  return (*this);
}

/**
 *  Get the type of this event.
 *
 *  @return The string "com::centreon::broker::neb::custom_variable".
 */
unsigned int custom_variable::type() const {
  return (io::data::data_type(io::data::neb, neb::custom_variable));
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy internal data members.
 *
 *  @param[in] cv Object to copy.
 */
void custom_variable::_internal_copy(custom_variable const& cv) {
  var_type = cv.var_type;
  return ;
}
