/*
** Copyright 2009-2011 MERETHIS
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
*/

#ifndef CCB_EXCEPTIONS_RETVAL_HH_
# define CCB_EXCEPTIONS_RETVAL_HH_

# include "exceptions/basic.hh"

namespace         com {
  namespace       centreon {
    namespace     broker {
      namespace   exceptions {
        /**
         *  @class retval retval.hh "exceptions/retval.hh"
         *  @brief Exceptions with return value.
         *
         *  This kind of exception embed a return value along with an
         *  error message.
         */
        class     retval : public basic {
         private:
          int     _retval;

         public:
                  retval(int r = 0) throw ();
                  retval(retval const& r) throw ();
          virtual ~retval() throw ();
          retval& operator=(retval const& r) throw ();
          retval& operator<<(bool b) throw ();
          retval& operator<<(double d) throw ();
          retval& operator<<(int i) throw ();
          retval& operator<<(unsigned int i) throw ();
          retval& operator<<(char const* str) throw ();
          retval& operator<<(time_t t) throw ();
          int     get_retval() const throw ();
          void    set_retval(int r) throw ();
        };
      }
    }
  }
}

#endif /* !CCB_EXCEPTIONS_RETVAL_HH_ */
