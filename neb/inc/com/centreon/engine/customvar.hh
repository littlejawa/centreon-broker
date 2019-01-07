/*
** Copyright 2017 Centreon
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

#ifndef CCE_CUSTOMVAR_HH
#  define CCE_CUSTOMVAR_HH

#  include <string>
#  include "com/centreon/engine/namespace.hh"
#  include "com/centreon/unordered_hash.hh"

CCE_BEGIN()

/**
 *  @class customvar customvar.hh "com/centreon/engine/customvar.hh"
 *  @brief Custom variable.
 *
 *  Represents and manipulate a custom variable.
 */
class                customvar {
 public:
                     customvar(
                       std::string const& name = "",
                       std::string const& value = "",
                       bool modified = false);
                     customvar(customvar const& other);
                     ~customvar();
  customvar&         operator=(customvar const& other);
  bool               get_modified() const;
  std::string const& get_name() const;
  void               set_name(std::string const& name);
  std::string const& get_value() const;
  void               set_modified(bool modified);
  void               set_value(std::string const& value);

 private:
  bool               _modified;
  std::string        _name;
  std::string        _value;
};

typedef umap<std::string, customvar> customvar_set;

CCE_END()

#endif // !CCE_CUSTOMVAR_HH
