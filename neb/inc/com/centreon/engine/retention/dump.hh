/*
** Copyright 2011-2013,2017 Centreon
**
** This file is part of Centreon Engine.
**
** Centreon Engine is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Engine is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Engine. If not, see
** <http://www.gnu.org/licenses/>.
*/

#ifndef CCE_RETENTION_DUMP_HH
#  define CCE_RETENTION_DUMP_HH

#  include <ostream>
#  include "com/centreon/engine/customvar.hh"
#  include "com/centreon/engine/host.hh"
#  include "com/centreon/engine/namespace.hh"
#  include "com/centreon/engine/service.hh"


CCE_BEGIN()

// Forward declaration.
class comment;
class contact;
class downtime;

namespace         retention {
  namespace       dump {
    std::ostream& comment(std::ostream& os, comment const& obj);
    std::ostream& comments(std::ostream& os);
    std::ostream& contact(std::ostream& os, contact const& obj);
    std::ostream& contacts(std::ostream& os);
    std::ostream& customvariables(std::ostream& os, customvar_set const& obj);
    std::ostream& downtime(std::ostream& os, downtime const& obj);
    std::ostream& downtimes(std::ostream& os);
    std::ostream& header(std::ostream& os);
    std::ostream& host(std::ostream& os, ::host const& obj);
    std::ostream& hosts(std::ostream& os);
    std::ostream& info(std::ostream& os);
    std::ostream& program(std::ostream& os);
    bool          save(std::string const& path);
    std::ostream& service(std::ostream& os, ::service const& obj);
    std::ostream& services(std::ostream& os);
  }
}
CCE_END()

#endif // !CCE_RETENTION_DUMP_HH
