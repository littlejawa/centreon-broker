/*
** Copyright 2019 Centreon
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

#ifndef CCE_NOTIFICATIONS_NOTIFIER_HH
#  define CCE_NOTIFICATIONS_NOTIFIER_HH

#  include "com/centreon/engine/namespace.hh"

CCE_BEGIN()

namespace notifications {
  // Forward declaration.
  class notifiable;

  /**
   *  @class notifier notifier.hh "com/centreon/engine/notifications/notifier.hh"
   *  @brief Send notifications on behalf of a notifiable object.
   */
  class  notifier {
   public:
    void notify_problem(notifiable& source);
    void notify_recovery(notifiable& source);
  };
}

CCE_END()

#endif // !CCE_NOTIFICATIONS_NOTIFIER_HH
