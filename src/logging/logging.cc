/*
** Copyright 2009-2011 MERETHIS
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
**
** For more information: contact@centreon.com
*/

#include "logging/defines.hh"
#include "logging/internal.hh"
#include "logging/logging.hh"

using namespace logging;

/**************************************
*                                     *
*          Internal Objects           *
*                                     *
**************************************/

// List of registered backends.
std::map<backend*, std::pair<unsigned int, level> > logging::backends;

/**************************************
*                                     *
*           Global Objects            *
*                                     *
**************************************/

logger      logging::config(CONFIG);
#ifdef NDEBUG
void_logger logging::debug;
#else
logger      logging::debug(DEBUG);
#endif /* NDEBUG */
logger      logging::error(ERROR);
logger      logging::info(INFO);

/**
 *  Clear the list of logging objects.
 */
void logging::clear() {
  for (std::map<backend*, std::pair<unsigned int, level> >::iterator
         it = backends.begin(), end = backends.end();
       it != end;
       ++it)
    delete (it->first);
  backends.clear();
}

/**
 *  @brief Add or remove a log backend.
 *
 *  If either types or min_priority is 0, the backend will be removed.
 *
 *  @param[in] b            Backend.
 *  @param[in] types        Log types to store on this backend. Bitwise OR of
 *                          multiple logging::type.
 *  @param[in] min_priority Minimal priority of messages to be logged.
 */
void logging::log_on(backend* b,
                     unsigned int types,
                     level min_priority) {
  if (types && min_priority) {
    backends[b].first = types;
    backends[b].second = min_priority;
    }
  else {
    delete b;
    backends.erase(b);
  }
  return ;
}
