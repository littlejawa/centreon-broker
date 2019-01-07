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

#ifndef CCE_CONTACTS_CONTACT_GROUP_HH
#  define CCE_CONTACTS_CONTACT_GROUP_HH

#  include "com/centreon/engine/contacts/contact_generic.hh"

CCE_BEGIN()

namespace             contacts {

  class               contact_user;
  /**
   *  @class contact contact.hh "com/centreon/engine/contacts/contact.hh"
   *  @brief Object representing contacts groups.
   *
   */
  class               contact_group : public contact_generic {
    public:
                      contact_group();
                      contact_group(contact_group const& other);
    contact_group&    operator=(contact_group const& other);
     virtual          ~contact_group();
     void             fill_contact_users(
                        std::list<shared_ptr<contact_user> >& lst);

    private:
     bool             _lt(contact_generic const& other) const = 0;

     std::list<shared_ptr<contact_generic> >
                      _contacts;
  };
}

CCE_END()

#endif // !CCE_CONTACTS_CONTACT_GROUP_HH
