/*
** Copyright 2014 Merethis
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

#include "com/centreon/broker/bam/service_listener.hh"

using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
service_listener::service_listener() {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
service_listener::service_listener(service_listener const& right) {
  (void)right;
}

/**
 *  Destructor.
 */
service_listener::~service_listener() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
service_listener& service_listener::operator=(
                                      service_listener const& right) {
  (void)right;
  return (*this);
}