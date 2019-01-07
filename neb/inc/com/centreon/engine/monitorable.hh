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

#ifndef CCE_MONITORABLE_HH
#  define CCE_MONITORABLE_HH

#  include <map>
#  include <string>
#  include "com/centreon/engine/customvar.hh"
#  include "com/centreon/engine/namespace.hh"
#  include "com/centreon/engine/notifications/notifier.hh"

CCE_BEGIN()

/**
 *  @class monitorable monitorable.hh "com/centreon/engine/monitorable.hh"
 *  @brief Monitorable node.
 *
 *  Monitorable nodes are checked and notified of problems.
 */
class                  monitorable : public notifications::notifier {
 public:
                       monitorable();
                       monitorable(monitorable const& other);
  virtual              ~monitorable();
  monitorable&         operator=(monitorable const& other);
  std::string const&   get_action_url() const;
  void                 set_action_url(std::string const& action_url);
  void                 clear_customvars();
  customvar_set const& get_customvars() const;
  void                 set_customvar(customvar const& var);
  std::string const&   get_display_name() const;
  void                 set_display_name(std::string const& display);
  std::string const&   get_icon_image() const;
  void                 set_icon_image(std::string const& image);
  std::string const&   get_icon_image_alt() const;
  void                 set_icon_image_alt(std::string const& image);
  unsigned int         get_id() const;
  void                 set_id(unsigned int id);
  int                  get_initial_state() const;
  void                 set_initial_state(int state);
  std::string const&   get_notes() const;
  void                 set_notes(std::string const& notes);
  std::string const&   get_notes_url() const;
  void                 set_notes_url(std::string const& notes_url);
  bool                 get_retain_nonstate_info() const;
  void                 set_retain_nonstate_info(bool retain);
  bool                 get_retain_state_info() const;
  void                 set_retain_state_info(bool retain);

 private:
  void                 _internal_copy(monitorable const& other);

  std::string          _action_url;
  std::string          _display_name;
  std::string          _icon_image;
  std::string          _icon_image_alt;
  unsigned int         _id;
  int                  _initial_state;
  std::string          _notes;
  std::string          _notes_url;
  bool                 _retain_nonstate_info;
  bool                 _retain_state_info;
  customvar_set        _vars;
};

CCE_END()

#endif // !CCE_MONITORABLE_HH
