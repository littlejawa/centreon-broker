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

#ifndef CCE_CONTACTGROUP_HH
#  define CCE_CONTACTGROUP_HH

#  include <list>
#  include "com/centreon/engine/namespace.hh"
#  include "com/centreon/shared_ptr.hh"
#  include "com/centreon/unordered_hash.hh"

CCE_BEGIN()

// Forward declaration
class contact;

namespace configuration {
  class contactgroup;
}

  /**
   *  @class contactgroup contactgroup.hh "com/centreon/engine/contactgroup.hh
   *  @brief Object representing a contactgroup user
   *
   */
class                          contactgroup {
 public:
                               contactgroup();
                               contactgroup(
                                 configuration::contactgroup const& obj);
  virtual                      ~contactgroup();
  contactgroup&                operator=(contactgroup const& other);
  bool                         operator<(contactgroup const& other) const;
  bool                         operator!=(contactgroup const& other) const;
  std::string const&           get_name() const;
  std::string const&           get_alias() const;
  void                         set_alias(std::string const& alias);
  void                         add_member(contact* cntct);
  void                         clear_members();
  umap<std::string, contact*> const&
                               get_members() const;
  bool                         has_member(std::string const& name) const;

  bool                         contains_illegal_object_chars() const;

 private:
  std::string                  _alias;
  umap<std::string, contact*>  _members;
  std::string                  _name;
};

CCE_END()

using com::centreon::engine::contactgroup;

typedef umap<std::string, com::centreon::shared_ptr<contactgroup> > contactgroup_map;

#endif // !CCE_CONTACTGROUP_HH
