/*
** Copyright 2018 Centreon
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

#ifndef CCE_DOWNTIME_MANAGER_HH
#  define CCE_DOWNTIME_MANAGER_HH

#  include <ctime>
#  include <string>
#  include "com/centreon/engine/namespace.hh"
#  include "com/centreon/engine/notifications/notifiable.hh"
#  include "com/centreon/unordered_hash.hh"

CCE_BEGIN()

/**
 *  @class downtime_manager downtime_manager.hh "com/centreon/engine/downtime_manager.hh"
 *  @brief Handle downtime creation, deletion, triggering, ...
 *
 *  In the Engine application, downtimes should not be created directly
 *  but through a downtime_manager instance.
 */
class                      downtime_manager {
 public:
  enum                     propagation {
    DOWNTIME_PROPAGATE_NONE,
    DOWNTIME_PROPAGATE_SIMPLE,
    DOWNTIME_PROPAGATE_TRIGGERED
  };

  // Singleton.
  static void              load();
  static downtime_manager& instance();
  static void              unload();

  // Downtime management.
  umap<unsigned long, downtime> const&
                           get_downtimes() const;
  unsigned long            get_next_downtime_id() const;
  void                     set_next_downtime_id(unsigned long id);
  unsigned long            schedule(
                             notifications::notifiable* target,
                             time_t entry_time,
                             std::string const& author,
                             std::string const& comment,
                             time_t start_time,
                             time_t end_time,
                             bool fixed,
                             unsigned long duration,
                             unsigned long triggered_by = 0,
                             propagation propagate = DOWNTIME_PROPAGATE_NONE,
                             unsigned long id = 0);
  void                     start(unsigned long id);
  void                     stop(unsigned long id);
  void                     unschedule(unsigned long id);

 private:
                           downtime_manager();
                           downtime_manager(downtime_manager const& other);
                           ~downtime_manager();
  downtime_manager&        operator=(downtime_manager const& other);
  unsigned long            _use_next_downtime_id();

  umap<unsigned long, downtime>
                           _downtimes;
  static downtime_manager* _instance;
  unsigned long            _next_downtime_id;
};

CCE_END()

#endif // !CCE_DOWNTIME_MANAGER_HH
