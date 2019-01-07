/*
** Copyright 1999-2010      Ethan Galstad
** Copyright 2011-2013,2018 Centreon
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

#ifndef CCE_MACROS_GRAB_HH
#  define CCE_MACROS_GRAB_HH

#  include <iomanip>
#  include <sstream>
#  include <time.h>
#  include "com/centreon/engine/common.hh"
#  include "com/centreon/engine/macros/process.hh"
#  include "com/centreon/engine/namespace.hh"
#  include "com/centreon/engine/string.hh"

CCE_BEGIN()

namespace  macros {
  /**
   *  @class grabber grab.hh "com/centreon/engine/macros/grab.hh"
   *  @brief Grab macro value.
   *
   *  Functor used internally to grab macro values.
   */
  template <typename T>
  class                 grabber {
   public:
    virtual             ~grabber() {}
    virtual char const* operator()(T& t, nagios_macros* mac) const = 0;
  };

  /**
   *  @class member_grabber grab.hh "com/centreon/engine/macros/grab.hh"
   *  @brief Grab class member macro value.
   *
   *  Implement the grabber interface to grab class members macro values.
   */
  template <typename T, typename U>
  class                 member_grabber : public grabber<T> {
   public:
                        member_grabber(U (T::* member)() const)
    : _member(member) {}
                        ~member_grabber() {}
    char const*         operator()(T& t, nagios_macros* mac) const {
      (void)mac;
      return (string::dup((t.*_member)()));
    }

   private:
    U (T::*             _member)() const;
  };

  /**
   *  @class function_grabber grab.hh "com/centreon/engine/macros/grab.hh"
   *  @brief Wrap function call.
   *
   *  Implements the grabber interface to wrap a function call.
   */
  template <typename T>
  class            function_grabber : public grabber<T> {
   public:
                   function_grabber(
                     char const* (* function)(T&, nagios_macros*))
    : _function(function) {}
                   ~function_grabber() {}
    char const*    operator()(T& t, nagios_macros* mac) const {
      return (_function(t, mac));
    }

   private:
    char const* (* _function)(T&, nagios_macros*);
  };

  /**
   *  Extract double.
   *
   *  @param[in] t   Host object.
   *  @param[in] mac Unused.
   *
   *  @return Newly allocated string with value as a fixed point string.
   */
  template <typename T, double (T::* member), unsigned int precision>
  char*    get_double(T& t, nagios_macros* mac) {
    (void)mac;
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision)
        << t.*member;
    return (string::dup(oss.str()));
  }

  /**
   *  Extract duration.
   *
   *  @param[in] t   Base object.
   *  @param[in] mac Unused.
   *
   *  @return Duration in a newly allocated string.
   */
  template <typename T>
  char const* get_duration(T& t, nagios_macros* mac) {
    (void)mac;

    // Get duration.
    time_t now(time(NULL));
    unsigned long duration(now - t.get_last_state_change());

    // Break down duration.
    unsigned int days(duration / (24 * 60 * 60));
    duration %= (24 * 60 * 60);
    unsigned int hours(duration / (60 * 60));
    duration %= (60 * 60);
    unsigned int minutes(duration / 60);
    duration %= 60;

    // Stringify duration.
    std::ostringstream oss;
    oss << days << "d "
        << hours << "h "
        << minutes << "m "
        << duration << "s";
    return (string::dup(oss.str()));
  }

  /**
   *  Extract duration in seconds.
   *
   *  @param[in] t   Base object.
   *  @param[in] mac Unused.
   *
   *  @return Duration in second in a newly allocated string.
   */
  template <typename T>
  char const* get_duration_sec(T& t, nagios_macros* mac) {
    (void)mac;

    // Get duration.
    time_t now(time(NULL));
    unsigned long duration(now - t.get_last_state_change());
    return (string::dup(duration));
  }

  /**
   *  Copy macro.
   *
   *  @param[in] t   Unused.
   *  @param[in] mac Macro array.
   *
   *  @return Copy of the requested macro.
   */
  template <typename T, unsigned int macro_id>
  char const* get_macro_copy(T& t, nagios_macros* mac) {
    (void)t;
    return (string::dup(mac->x[macro_id] ? mac->x[macro_id] : ""));
  }

  /**
   *  Recursively process macros.
   *
   *  @param[in] hst Host object.
   *  @param[in] mac Unused.
   *
   *  @return Newly allocated string with macros processed.
   */
  template <typename T, char* (T::* member), unsigned int options>
  char* get_recursive(T& t, nagios_macros* mac) {
    (void)mac;

    // Get copy of string with macros processed.
    char* buffer(NULL);
    process_macros_r(mac, t.*member, &buffer, options);
    return (buffer);
  }

  /**
   *  Get action URL.
   *
   *  @param[in] t    Base object.
   *  @param[in] mac  Macros.
   *
   *  @return Newly allocated and encoded action URL as string.
   */
  template <typename T>
  char const* get_action_url(T& t, nagios_macros* mac) {
    char* buffer(NULL);
    process_macros_r(
      mac,
      t.get_action_url().c_str(),
      &buffer,
      URL_ENCODE_MACRO_CHARS);
    return (buffer);
  }

  /**
   *  Get notes.
   *
   *  @param[in] t    Base object.
   *  @param[in] mac  Macros.
   *
   *  @return Newly allocated notes as string.
   */
  template <typename T>
  char const* get_notes(T& t, nagios_macros* mac) {
    char* buffer(NULL);
    process_macros_r(
      mac,
      t.get_notes().c_str(),
      &buffer,
      0);
    return (buffer);
  }

  /**
   *  Get notes URL.
   *
   *  @param[in] t    Base object.
   *  @param[in] mac  Macros.
   *
   *  @return Newly allocated and encoded notes URL as string.
   */
  template <typename T>
  char const* get_notes_url(T& t, nagios_macros* mac) {
    char* buffer(NULL);
    process_macros_r(
      mac,
      t.get_notes_url().c_str(),
      &buffer,
      URL_ENCODE_MACRO_CHARS);
    return (buffer);
  }

  /**
   *  Extract state type.
   *
   *  @param[in] t    Base object.
   *  @param[in] mac  Unused.
   *
   *  @return Newly allocated state type as a string.
   */
  template <typename T>
  char const* get_state_type(T& t, nagios_macros* mac) {
    (void)mac;
    return (string::dup((t.get_current_state_type() == HARD_STATE)
                      ? "HARD"
                      : "SOFT"));
  }
}

CCE_END()

#endif // !CCE_MACROS_GRAB_HH
