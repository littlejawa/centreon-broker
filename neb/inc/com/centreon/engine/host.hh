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

#ifndef CCE_HOST_HH
#  define CCE_HOST_HH

#  include <list>
#  include "com/centreon/engine/monitorable.hh"
#  include "com/centreon/engine/namespace.hh"

CCE_BEGIN()

// Forward declarations.
class service;
class hostgroup;

/**
 *  @class host host.hh "com/centreon/engine/host.hh"
 *  @brief This class represents a host.
 *
 *  A host is checkable and also a notifiable.
 */
class                        host : public monitorable {
 public:
                             host();
                             host(host const& other);
                             ~host();
  host&                      operator=(host const& other);

  // Configuration.
  std::string const&         get_address() const;
  void                       set_address(std::string const& address);
  std::string const&         get_alias() const;
  void                       set_alias(std::string const& alias);
  int                        get_circular_path_checked() const;
  void                       set_circular_path_checked(int check_level);
  virtual bool               is_host() const;
  std::string                get_info() const;
  std::string const&         get_name() const;
  void                       set_name(std::string const& name);
  bool                       get_stalk_on_down() const;
  void                       set_stalk_on_down(bool stalk);
  bool                       get_stalk_on_unreachable() const;
  void                       set_stalk_on_unreachable(bool stalk);
  bool                       get_stalk_on_up() const;
  void                       set_stalk_on_up(bool stalk);
  std::string const&         get_statusmap_image() const;
  void                       set_statusmap_image(std::string const& image);
  std::string const&         get_vrml_image() const;
  void                       set_vrml_image(std::string const& image);

  // (Useless) coordinates.
  bool                       get_have_2d_coords() const;
  void                       set_have_2d_coords(bool has_coords);
  bool                       get_have_3d_coords() const;
  void                       set_have_3d_coords(bool has_coords);
  int                        get_x_2d() const;
  void                       set_x_2d(int x);
  int                        get_y_2d() const;
  void                       set_y_2d(int y);
  int                        get_x_3d() const;
  void                       set_x_3d(int x);
  int                        get_y_3d() const;
  void                       set_y_3d(int y);
  int                        get_z_3d() const;
  void                       set_z_3d(int z);

  // Links with other objects.
  void                       add_child(host* hst);
  void                       clear_children();
  std::list<host*> const&    get_children() const;
  void                       add_group(hostgroup* hg);
  void                       clear_groups();
  std::list<hostgroup*> const&
                             get_groups() const;
  void                       add_parent(host* hst);
  void                       clear_parents();
  std::list<host*> const&    get_parents() const;
  void                       add_service(service* svc);
  void                       clear_services();
  std::list<service*> const& get_services() const;
  int                        get_total_service_check_interval() const;

  // State runtime.
  time_t                     get_last_time_down() const;
  void                       set_last_time_down(time_t last_time);
  time_t                     get_last_time_unreachable() const;
  void                       set_last_time_unreachable(time_t last_time);
  time_t                     get_last_time_up() const;
  void                       set_last_time_up(time_t last_time);
  bool                       get_should_reschedule_current_check() const;
  void                       set_should_reschedule_current_check(
                               bool reschedule);

  // Flap detection.
  bool                       get_flap_detection_on_up() const;
  void                       set_flap_detection_on_up(bool detection);
  bool                       get_flap_detection_on_down() const;
  void                       set_flap_detection_on_down(bool detection);
  bool                       get_flap_detection_on_unreachable() const;
  void                       set_flap_detection_on_unreachable(bool detection);
  time_t                     get_last_historical_state_update() const;
  void                       set_last_historical_state_update(
                               time_t last_update);

 protected:
  void                       _checkable_macro_builder(nagios_macros& mac);

 private:
  void                       _internal_copy(host const& other);

  std::string                _address;
  std::string                _alias;
  std::list<host*>           _children;
  int                        _circular_path_checked;
  bool                       _flap_detection_on_down;
  bool                       _flap_detection_on_unreachable;
  bool                       _flap_detection_on_up;
  std::list<hostgroup*>
                             _groups;
  bool                       _have_2d_coords;
  bool                       _have_3d_coords;
  time_t                     _last_historical_state_update;
  time_t                     _last_time_down;
  time_t                     _last_time_unreachable;
  time_t                     _last_time_up;
  std::string                _name;
  std::list<host*>           _parents;
  std::list<service*>        _services;
  bool                       _should_reschedule_current_check;
  bool                       _stalk_on_down;
  bool                       _stalk_on_unreachable;
  bool                       _stalk_on_up;
  std::string                _statusmap_image;
  int                        _total_service_check_interval;
  std::string                _vrml_image;
  int                        _x_2d;
  int                        _y_2d;
  int                        _x_3d;
  int                        _y_3d;
  int                        _z_3d;
};

CCE_END()

using com::centreon::engine::host;

typedef std::list<host*> host_set;

#endif // !CCE_HOST_HH
