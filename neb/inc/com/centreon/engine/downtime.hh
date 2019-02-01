/*
** Copyright 2017-2018 Centreon
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

#ifndef CCE_DOWNTIME_HH
#  define CCE_DOWNTIME_HH

#  include <ctime>
#  include <string>
#  include "com/centreon/engine/namespace.hh"

CCE_BEGIN()

// Forward declarations
namespace notifications {
  class notifiable;
}

/**
 *  @class downtime downtime.hh "com/centreon/engine/downtime.hh"
 *  @brief Service as a host's downtime.
 *
 *  This class represents a downtime.
 */
class                downtime {
 public:
  enum               downtime_type {
    SERVICE_DOWNTIME = 1,
    HOST_DOWNTIME    = 2,
    ANY_DOWNTIME     = 3
  };

                     downtime(
                       unsigned long id = 0,
                       notifications::notifiable* parent = NULL,
                       time_t entry_time = (time_t)0,
                       std::string const& author = "",
                       std::string const& comment_data = "",
                       unsigned long comment_id = 0,
                       time_t start_time = (time_t)0,
                       time_t end_time = (time_t)0,
                       bool fixed = true,
                       unsigned long duration = 0,
                       unsigned long triggered_by = 0);
                     downtime(downtime const& other);
                     ~downtime();
  downtime&          operator=(downtime const& other);

  unsigned long      get_comment_id() const;
  void               set_comment_id(unsigned long comment_id);
  downtime_type      get_type() const;
  unsigned long      get_id() const;
  void               set_id(unsigned long id);
  int                get_incremented_pending_downtime() const;
  void               set_incremented_pending_downtime();
  void               unset_incremented_pending_downtime();
  bool               get_in_effect() const;
  void               set_in_effect(bool in_effect);
  time_t             get_entry_time() const;
  time_t             get_start_time() const;
  time_t             get_end_time() const;
  bool               get_fixed() const;
  unsigned long      get_triggered_by() const;
  unsigned long      get_duration() const;
  std::string const& get_author() const;
  std::string const& get_comment() const;
  notifications::notifiable*
                     get_parent() const;
  void               start();
  void               stop();

 private:
  void               _internal_copy(downtime const& other);

  std::string        _author;
  std::string        _comment_data;
  unsigned long      _comment_id;
  unsigned long      _duration;
  time_t             _end_time;
  time_t             _entry_time;
  bool               _fixed;
  unsigned long      _id;
  bool               _in_effect;
  bool               _incremented_pending_downtime;
  notifications::notifiable*
                     _parent;
  time_t             _start_time;
  unsigned long      _triggered_by;
};

CCE_END()

#endif // !CCE_DOWNTIME_HH
