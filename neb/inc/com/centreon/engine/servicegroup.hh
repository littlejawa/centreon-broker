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

#ifndef CCE_SERVICEGROUP_HH
#  define CCE_SERVICEGROUP_HH

#  include <list>
#  include "com/centreon/engine/namespace.hh"

CCE_BEGIN()

/**
 *  @class servicegroup servicegroup.hh "com/centreon/engine/servicegroup.hh"
 *  @brief This class represents a service group.
 *
 *  A servicegroup is a group of services.
 */
class                       servicegroup {
 public:
                            servicegroup();
                            servicegroup(servicegroup const& other);
                            ~servicegroup();
  servicegroup&                operator=(servicegroup const& other);
  umap<std::pair<std::string, std::string>, service*> const&
                            get_members() const;
  umap<std::pair<std::string, std::string>, service*>&
                            get_members();

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
  void                      add_member(service* svc);
  void                      clear_members();

 private:
  void                      _internal_copy(servicegroup const& other);

  std::string               _action_url;
  std::string               _alias;
  umap<std::pair<std::string, std::string>, service*>
                            _members;
  std::string               _name;
  std::string               _notes;
  std::string               _notes_url;
  unsigned int              _id;
};

CCE_END()

using com::centreon::engine::servicegroup;

typedef umap<std::string, com::centreon::shared_ptr<servicegroup> > servicegroup_map;
typedef std::list<servicegroup*> servicegroup_set;

#endif // !CCE_SERVICEGROUP_HH
