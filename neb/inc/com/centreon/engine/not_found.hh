/*
** Copyright 2017 Merethis
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

#ifndef CCE_NOT_FOUND_HH
#  define CCE_NOT_FOUND_HH

#  include "com/centreon/engine/error.hh"
#  include "com/centreon/engine/namespace.hh"

CCE_BEGIN()

/**
 *  @class not_found not_found.hh "com/centreon/engine/not_found.hh"
 *  @brief Not found exception.
 *
 *  This kind of exception is thrown when an element cannot be found.
 */
class        not_found : public error {
 public:
             not_found();
             not_found(
               char const* file,
               char const* function,
               int line);
             not_found(not_found const& other);
  virtual    ~not_found() throw ();
  not_found& operator=(not_found const& other);

  template <typename T>
  not_found& operator<<(T t) throw () {
    error::operator<<(t);
    return (*this);
  }
};

CCE_END()

#  ifdef NDEBUG
#    define not_found_error() com::centreon::engine::not_found()
#  else
#    define not_found_error() com::centreon::engine::not_found(__FILE__, __func__, __LINE__)
#  endif // NDEBUG

#endif // !CCE_NOT_FOUND_HH
