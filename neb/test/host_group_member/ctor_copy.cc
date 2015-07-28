/*
** Copyright 2012 Merethis
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

#include "com/centreon/broker/neb/host_group_member.hh"
#include "test/randomize.hh"

using namespace com::centreon::broker;

/**
 *  Check host_group_member's copy constructor.
 *
 *  @return 0 on success.
 */
int main() {
  // Initialization.
  randomize_init();

  // Object #1.
  neb::host_group_member hgrpmmbr1;
  std::vector<randval> randvals1;
  randomize(hgrpmmbr1, &randvals1);

  // Object #2.
  neb::host_group_member hgrpmmbr2(hgrpmmbr1);

  // Reset object #1.
  std::vector <randval> randvals2;
  randomize(hgrpmmbr1, &randvals2);

  // Compare objects with expected results.
  int retval((hgrpmmbr1 != randvals2) || (hgrpmmbr2 != randvals1));

  // Cleanup.
  randomize_cleanup();

  return (retval);
}