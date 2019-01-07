/*
** Copyright 2011-2013 Merethis
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

#ifndef CCE_OBJECTS_TOOL_HH
#  define CCE_OBJECTS_TOOL_HH

#  include <cstring>
#  include <sstream>
#  include "com/centreon/unordered_hash.hh"
#  include "com/centreon/engine/namespace.hh"

CCE_BEGIN()

template<typename T>
inline std::string      chkobj(T const* obj) throw () {
  if (!obj)
    return ("\"NULL\"");
  std::ostringstream oss;
  oss << *obj;
  return (oss.str());
}

template<typename T>
inline std::string      chkobj(umap<std::string, T> const& obj) throw () {
  std::ostringstream oss;
  typename umap<std::string, T>::const_iterator it(obj.begin()), end(obj.end());
  if (it != end) {
    oss << it->first;
  }
  for (; it != end; ++it)
    oss << ", " << it->first;

  return oss.str();
}

template<typename T>
inline std::string      chkobj(umap<std::pair<std::string, std::string>, T> const& obj) throw () {
  std::ostringstream oss;
  typename umap<std::pair<std::string, std::string>, T>::const_iterator it(obj.begin()), end(obj.end());
  if (it != end) {
    oss << '(' << it->first.first << ", " << it->first.second << ')';
  }
  for (; it != end; ++it)
    oss << ", (" << it->first.first << ", " << it->first.second << ')';

  return oss.str();
}

inline bool             is_equal(
                          char const* str1,
                          char const* str2) throw () {
  return (str1 == str2 || (str1 && str2 && !strcmp(str1, str2)));
}

inline bool             is_equal(
                          char* const* tab1,
                          char* const* tab2,
                          unsigned int size) throw () {
  for (unsigned int i(0); i < size; ++i)
    if (!is_equal(tab1[i], tab2[i]))
      return (false);
  return (true);
}

template<typename T>
inline bool             is_equal(
                          T const* tab1,
                          T const* tab2,
                          unsigned int size) throw () {
  for (unsigned int i(0); i < size; ++i)
    if (tab1[i] != tab2[i])
      return (false);
  return (true);
}

template<typename T>
inline bool             is_equal(
                          T const* obj1,
                          T const* obj2) throw () {
  return (obj1 == obj2 || (obj1 && obj2 && *obj1 == *obj2));
}

CCE_END()

#endif // !CCE_OBJECTS_TOOL_HH
