/*
** Copyright 2011-2012 Centreon
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

#include "com/centreon/broker/config/logger.hh"

using namespace com::centreon::broker;

/**
 *  Check that default configuration for logging object matches
 *  expectations.
 *
 *  @return 0 on success.
 */
int main () {
  // Logger configuration.
  config::logger cfg;

  // Check content.
  return ((!cfg.config())
          || (cfg.debug())
          || (!cfg.error())
          || (cfg.info())
          || (cfg.level() != logging::high)
          || (cfg.max_size() != 10000000000ull)
          || (!cfg.name().isEmpty())
          || (cfg.type() != config::logger::unknown));
}
