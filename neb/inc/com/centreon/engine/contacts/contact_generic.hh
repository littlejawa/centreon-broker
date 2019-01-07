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

#ifndef CCE_CONTACTS_CONTACT_GENERIC_HH
#  define CCE_CONTACTS_CONTACT_GENERIC_HH

#  include <list>
#  include "com/centreon/engine/namespace.hh"
#  include "com/centreon/shared_ptr.hh"

CCE_BEGIN()
namespace             contacts {

  // Forward declaration
  class               contact_user;
  class               contact_group;

  /**
   *  @class contact_generic contact_generic.hh "com/centreon/engine/notifications/contact_generic.hh"
   *  @brief Object representing individual contacts or contacts groups.
   *
   */
  class               contact_generic {
   public:
                      contact_generic();
                      contact_generic(contact_generic const& other);
    contact_generic&  operator=(contact_generic const& other);
    bool              operator<(contact_generic const& other) const;
    virtual           ~contact_generic();
    virtual void      fill_contact_users(
                        std::list<shared_ptr<contact_user> >& lst) = 0;
   private:
    virtual bool      _lt(contact_generic const& other) const = 0;
  };
}

CCE_END()

#endif // !CCE_CONTACTS_CONTACT_GENERIC_HH
