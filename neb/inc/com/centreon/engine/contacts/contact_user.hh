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

#ifndef CCE_CONTACTS_CONTACT_INDIVIDUAL_HH
#  define CCE_CONTACTS_CONTACT_INDIVIDUAL_HH

#  include <string>
#  include "com/centreon/engine/contacts/contact_generic.hh"

CCE_BEGIN()

namespace           contacts {

  /**
   *  @class contact_user contact_user.hh "com/centreon/engine/notifications/contact_user.hh"
   *  @brief Object representing user contacts.
   */
  class               contact_user : public contact_generic {
    public:
                      contact_user();
                      contact_user(std::string const& name);
                      contact_user(contact_user const& other);
     contact_user&    operator=(contact_user const& other);
     virtual          ~contact_user();
     void             fill_contact_users(
                        std::list<shared_ptr<contact_user> >& lst);
     std::string const&
                      get_name() const;

    private:
     bool             _lt(contact_generic const& other) const;

     std::string      _name;
  };
}

CCE_END()

#endif // !CCE_CONTACTS_CONTACT_INDIVIDUAL_HH
