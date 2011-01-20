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

#ifndef EVENTS_HOST_GROUP_MEMBER_HH_
# define EVENTS_HOST_GROUP_MEMBER_HH_

# include <string>
# include "events/group_member.hh"

namespace              events {
  /**
   *  @class host_group_member host_group_member.hh "events/host_group_member.hh"
   *  @brief Member of a host group.
   *
   *  Base class defining that an host is part of a host group.
   *
   *  @see host
   *  @see host_group
   */
  class                host_group_member : public group_member {
   public:
                       host_group_member();
                       host_group_member(host_group_member const& hgm);
    virtual            ~host_group_member();
    host_group_member& operator=(host_group_member const& hgm);
    int                get_type() const;
  };
}

#endif /* !EVENTS_HOST_GROUP_MEMBER_HH_ */
