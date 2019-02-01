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
#ifndef CCE_COMMENT_HH
#  define CCE_COMMENT_HH

#  include <string>
#  include "com/centreon/engine/namespace.hh"

CCE_BEGIN()

// Forward declarations
namespace notifications {
  class notifiable;
}
class host;
class service;

/**
 *  @class comment comment.hh "com/centreon/engine/comment.hh"
 *  @brief Comment
 *
 *  This class represents a comment.
 */
class           comment {
 public:
   enum         comment_type {
     HOST_COMMENT    = 1,
     SERVICE_COMMENT = 2
   };

   enum         entry_type {
     USER_COMMENT            = 1,
     DOWNTIME_COMMENT        = 2,
     FLAPPING_COMMENT        = 3,
     ACKNOWLEDGEMENT_COMMENT = 4
   };

   enum         source_type {
     COMMENTSOURCE_INTERNAL  = 0,
     COMMENTSOURCE_EXTERNAL  = 1
   };

    static void delete_comment(unsigned long comment_id);
    static void check_for_expired_comment(unsigned long comment_id);

    static comment*
                add_new_comment(
                  comment_type type,
                  entry_type ent_type,
                  notifications::notifiable* parent,
                  time_t entry_time,
                  std::string const& author_name,
                  std::string const& comment_data,
                  int persistent,
                  source_type source,
                  bool expires,
                  time_t expire_time);

                comment(
                  comment_type cmt_type,
                  entry_type ent_type,
                  notifications::notifiable* parent,
                  time_t entry_time,
                  std::string const& author,
                  std::string const& comment_data,
                  bool persistent,
                  bool expires,
                  time_t expire_time,
                  source_type source,
                  unsigned long comment_id = 0);
                comment(comment const& other);
  comment&      operator=(comment const& other);
                ~comment();

  entry_type    get_entry_type() const;
  void          set_entry_type(entry_type ent_type);
  notifications::notifiable*
                get_parent() const;
  std::string   get_host_name() const;
  void          set_host_name(std::string const& host_name);
  std::string   get_service_description() const;
  void          set_service_description(std::string const& service_description);
  time_t        get_entry_time() const;
  void          set_entry_time(time_t ent_time);
  std::string   get_author() const;
  void          set_author(std::string const& author);
  std::string   get_comment_data() const;
  void          set_comment_data(std::string const& comment_data);
  unsigned long get_id() const;
  void          set_id(unsigned long comment_id);
  bool          get_persistent() const;
  void          set_persistent(bool persistent);
  int           get_source() const;
  void          set_source(int source);
  bool          get_expires() const;
  void          set_expires(bool expires);
  time_t        get_expire_time() const;
  void          set_expire_time(time_t time);
  comment_type  get_comment_type() const;
  void          set_comment_type(comment_type comment_type);

 private:
  static unsigned long
                _next_id;

  std::string   _author;
  std::string   _comment_data;
  unsigned long _comment_id;
  comment_type  _comment_type;
  time_t        _entry_time;
  entry_type    _entry_type;
  bool          _expires;
  time_t        _expire_time;
  notifications::notifiable*
                _parent;
  bool          _persistent;
  int           _source;
};

CCE_END()


#endif // !CCE_COMMENT_HH
