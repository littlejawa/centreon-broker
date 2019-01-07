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

#ifndef CCE_HOSTGROUP_HH
#  define CCE_HOSTGROUP_HH

#  include <list>
#  include "com/centreon/engine/monitorable.hh"
#  include "com/centreon/engine/namespace.hh"

CCE_BEGIN()

/**
 *  @class hostgroup hostgroup.hh "com/centreon/engine/hostgroup.hh"
 *  @brief This class represents a host group.
 *
 *  A hostgroup is a group of hosts.
 */
class                       hostgroup {
 public:
                            hostgroup();
                            hostgroup(hostgroup const& other);
                            ~hostgroup();
  hostgroup&                operator=(hostgroup const& other);
  umap<std::string, host*> const&
                            get_members() const;
  umap<std::string, host*>& get_members();
  std::string const&        get_alias() const;
  void                      set_alias(std::string const& alias);
  std::string const&        get_name() const;
  void                      set_name(std::string const& name);
  std::string const&        get_action_url() const;
  void                      set_action_url(std::string const& action_url);
  std::string const&        get_notes_url() const;
  void                      set_notes_url(std::string const& notes_url);
  std::string const&        get_notes() const;
  void                      set_notes(std::string const& notes);
  unsigned int              get_id() const;
  void                      set_id(unsigned int id);
  void                      add_member(host* hst);
  void                      clear_members();

 private:
  void                      _internal_copy(hostgroup const& other);

  std::string               _action_url;
  std::string               _alias;
  umap<std::string, host*>  _members;
  std::string               _name;
  std::string               _notes;
  std::string               _notes_url;
  unsigned int              _id;
};

CCE_END()

using com::centreon::engine::hostgroup;

typedef umap<std::string, com::centreon::shared_ptr<hostgroup> > hostgroup_map;
typedef std::list<hostgroup*> hostgroup_set;

#endif // !CCE_HOSTGROUP_HH
