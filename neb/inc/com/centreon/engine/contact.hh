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

#ifndef CCE_CONTACT_HH
#  define CCE_CONTACT_HH

#  include <list>
#  include <vector>
#  include "com/centreon/engine/customvar.hh"
#  include "com/centreon/engine/namespace.hh"
#  include "com/centreon/engine/notifications/notifiable.hh"
#  include "com/centreon/engine/objects/timeperiod.hh"

CCE_BEGIN()

// Forward declaration
namespace commands {
  class command;
}

class contactgroup;

/**
 *  @class contact contact.hh "com/centreon/engine/contact.hh
 *  @brief Object representing a contact user
 *
 */
class                           contact {
 public:
                                contact();
  virtual                       ~contact();

  // Base properties.
  std::string const&            get_address(int index) const;
  std::vector<std::string> const&
                                get_addresses() const;
  void                          set_addresses(
                                  std::vector<std::string> const& addresses);
  std::string const&            get_alias() const;
  void                          set_alias(std::string const& alias);
  bool                          get_can_submit_commands() const;
  void                          set_can_submit_commands(bool can_submit);
  std::string const&            get_email() const;
  void                          set_email(std::string const& email);
  unsigned long                 get_modified_attributes() const;
  void                          set_modified_attributes(unsigned long attr);
  std::string const&            get_name() const;
  void                          set_name(std::string const& name);
  std::string const&            get_pager() const;
  void                          set_pager(std::string const& pager);
  bool                          get_retain_status_information() const;
  void                          set_retain_status_information(bool retain);
  bool                          get_retain_nonstatus_information() const;
  void                          set_retain_nonstatus_information(bool retain);
  std::string const&            get_timezone() const;
  void                          set_timezone(std::string const& timezone);

  // Host notification properties.
  void                          add_host_notification_command(
                                  commands::command* cmd,
                                  std::string const& args);
  void                          clear_host_notification_commands();
  std::list<std::pair<commands::command*, std::string> > const&
                                get_host_notification_commands() const;
  bool                          get_host_notifications_enabled() const;
  void                          set_host_notifications_enabled(
                                  bool enable);
  bool                          get_host_notify_on(
                                  notifications::notifiable::action_on state) const;
  void                          set_host_notify_on(
                                  notifications::notifiable::action_on state,
                                  bool enabled);
  timeperiod_struct*            get_host_notification_period() const;
  void                          set_host_notification_period(
                                  timeperiod* tp);
  time_t                        get_last_host_notification() const;
  void                          set_last_host_notification(time_t t);
  unsigned long                 get_modified_host_attributes() const;
  void                          set_modified_host_attributes(
                                  unsigned long attr);

  // Service notification properties.
  void                          add_service_notification_command(
                                  commands::command* cmd,
                                  std::string const& args);
  void                          clear_service_notification_commands();
  std::list<std::pair<commands::command*, std::string> > const&
                                get_service_notification_commands() const;
  bool                          get_service_notifications_enabled() const;
  void                          set_service_notifications_enabled(
                                  bool enable);
  bool                          get_service_notify_on(
                                  notifications::notifiable::action_on state) const;
  void                          set_service_notify_on(
                                  notifications::notifiable::action_on state,
                                  bool enable);
  timeperiod_struct*            get_service_notification_period() const;
  void                          set_service_notification_period(
                                  timeperiod* tp);
  time_t                        get_last_service_notification() const;
  void                          set_last_service_notification(time_t t);
  unsigned long                 get_modified_service_attributes() const;
  void                          set_modified_service_attributes(
                                  unsigned long attr);

  // Contact groups.
  void                          add_contactgroup(contactgroup* cg);
  void                          clear_contactgroups();
  std::list<contactgroup*> const&
                                get_contactgroups() const;

  // Custom variables.
  void                          add_custom_variable(
                                  char const* varname,
                                  char const* varvalue);
  void                          clear_custom_variables();
  customvar_set const&          get_customvars() const;
  void                          set_customvar(customvar const& var);

 private:
                                contact(contact const& other);
  contact&                      operator=(contact const& other);

  std::vector<std::string>      _addresses;
  std::string                   _alias;
  bool                          _can_submit_commands;
  std::list<contactgroup*>      _contact_groups;
  std::string                   _email;
  std::list<std::pair<commands::command*, std::string> >
                                _host_notification_commands;
  timeperiod*                   _host_notification_period;
  bool                          _host_notifications_enabled;
  unsigned int                  _host_notified_states;
  std::string                   _name;
  time_t                        _last_host_notification;
  time_t                        _last_service_notification;
  unsigned long                 _modified_attributes;
  unsigned long                 _modified_host_attributes;
  unsigned long                 _modified_service_attributes;
  std::string                   _pager;
  bool                          _retain_nonstatus_information;
  bool                          _retain_status_information;
  std::list<std::pair<commands::command*, std::string> >
                                _service_notification_commands;
  timeperiod*                   _service_notification_period;
  bool                          _service_notifications_enabled;
  unsigned int                  _service_notified_states;
  std::string                   _timezone;
  customvar_set                 _vars;
};

CCE_END()

using com::centreon::engine::contact;

#endif // !CCE_CONTACT_HH
