/*
** Copyright 2017-2019 Centreon
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

#ifndef CCE_NOTIFICATIONS_NOTIFIABLE_HH
#  define CCE_NOTIFICATIONS_NOTIFIABLE_HH

#  include <ctime>
#  include <list>
#  include "com/centreon/engine/checks/checkable.hh"
#  include "com/centreon/engine/globals.hh"

CCE_BEGIN()

// Forward declaration
class contact;

namespace                  notifications {
  /**
   *  @class notifiable notifiable.hh "com/centreon/engine/notifications/notifiable.hh"
   *  @brief Hold the current state of an object that can trigger notifications.
   *
   */
  class                    notifiable : public checks::checkable {
   public:
    typedef void (notifiable::* macro_builder)(struct nagios_macros&);
    enum                   acknowledgement_type {
      ACKNOWLEDGEMENT_NONE,
      ACKNOWLEDGEMENT_NORMAL,
      ACKNOWLEDGEMENT_STICKY
    };
    enum                   action_on {
      ON_NONE = 0,
      ON_RECOVERY = (1 << 0),
      ON_UP = (1 << 0),
      ON_OK = (1 << 0),
      ON_DOWN = (1 << 1),
      ON_WARNING = (1 << 1),
      ON_CRITICAL = (1 << 2),
      ON_UNREACHABLE = (1 << 2),
      ON_UNKNOWN = (1 << 3),
      ON_FLAPPING = (1 << 4),
      ON_DOWNTIME = (1 << 5)
    };
    enum                   notification_type {
      PROBLEM = 0,
      RECOVERY = 1,
      ACKNOWLEDGEMENT = 2,
      FLAPPINGSTART = 3,
      FLAPPINGSTOP = 4,
      FLAPPINGDISABLED = 5,
      DOWNTIMESTART = 6,
      DOWNTIMESTOP = 7,
      DOWNTIMECANCELLED = 8,
      CUSTOM = 9
    };
    enum                   notifiable_type {
      HOST_NOTIFICATION,
      SERVICE_NOTIFICATION
    };

                           notifiable();
                           notifiable(notifiable const& other);
    virtual                ~notifiable();
    notifiable&              operator=(notifiable const& other);

    // Configuration.
    int                    get_first_notification_delay() const;
    void                   set_first_notification_delay(int delay);
    timeperiod*            get_notification_period() const;
    void                   set_notification_period(timeperiod* tperiod);
    long                   get_notification_interval() const;
    void                   set_notification_interval(long interval);
    bool                   get_notifications_enabled() const;
    void                   set_notifications_enabled(bool enable);
    bool                   get_notify_on(action_on state) const;
    void                   set_notify_on(action_on state, bool enable);
    bool                   is_state_notification_enabled(
                             int state) const;
    int                    get_recovery_notification_delay() const;
    void                   set_recovery_notification_delay(int delay);

    // Runtime.
    int                    get_current_notification_id() const;
    void                   set_current_notification_id(int id);
    int                    get_current_notification_number() const;
    void                   set_current_notification_number(int number);
    time_t                 get_first_notification() const;
    void                   set_first_notification(time_t first_notification);
    time_t                 get_last_notification() const;
    void                   set_last_notification(
                             time_t last_notification);
    time_t                 get_next_notification() const;
    void                   set_next_notification(
                             time_t next_notification);
    bool                   get_no_more_notifications() const;
    void                   set_no_more_notifications(bool no_more);
    void                   notify(
                             notification_type type,
                             std::string const& author = "",
                             std::string const& comment = "",
                             int options = 0);

    // Contacts / contact groups.
    void                   add_contact(engine::contact* user);
    void                   add_contactgroup(engine::contactgroup* cg);
    void                   clear_contacts();
    void                   clear_contactgroups();
    bool                   contains_contact(
                             std::string const& username) const;
    umap<std::string, engine::contact*> const&
                           get_contacts() const;
    umap<std::string, engine::contactgroup*> const&
                           get_contactgroups() const;

    // Acknowledgement.
    int                    get_acknowledgement_timeout() const;
    void                   set_acknowledgement_timeout(int timeout);
    acknowledgement_type   get_acknowledgement_type() const;
    bool                   is_acknowledged() const;
    void                   set_acknowledged(acknowledgement_type type);
    time_t                 get_last_acknowledgement() const;
    void                   set_last_acknowledgement(
                             time_t last_acknowledgement);
    void                   schedule_acknowledgement_expiration();
    void                   update_acknowledgement_on_state_change();

    // Downtime.
    int                    get_pending_flex_downtime() const;
    void                   inc_pending_flex_downtime();
    void                   dec_pending_flex_downtime();
    int                    get_scheduled_downtime_depth() const;
    void                   inc_scheduled_downtime_depth();
    void                   dec_scheduled_downtime_depth();

    // XXX
    void                   delete_acknowledgement_comments();
    void                   delete_all_comments();
    void                   check_pending_flex_downtime();
    bool                   should_be_escalated() const;
    virtual std::string    get_info() const = 0;
    void                   set_flapping_comment_id(unsigned int id);
    unsigned int           get_flapping_comment_id() const;

   protected:
    virtual void           _checkable_macro_builder(
                             nagios_macros& mac) = 0;

   private:
    void                   _internal_copy(notifiable const& other);
    bool                   _is_notification_viable(int type, int options);

    macro_builder          _get_macro_builder(
                             notification_type type) const;

    void                   _problem_macro_builder(nagios_macros& mac);
    void                   _recovery_macro_builder(nagios_macros& mac);

    static macro_builder   _macro_builder[];
    static std::string const
                           _notification_string[];

    int                    _acknowledgement_timeout;
    umap<std::string, engine::contact*>
                           _contacts;
    umap<std::string, engine::contactgroup*>
                           _contact_groups;
    acknowledgement_type   _current_acknowledgement;
    int                    _current_notification_id;
    int                    _current_notification_number;
    bool                   _escalate_notification;
    time_t                 _first_notification;
    int                    _first_notification_delay;
    time_t                 _last_acknowledgement;
    time_t                 _last_notification;
    time_t                 _next_notification;
    bool                   _no_more_notifications;
    long                   _notification_interval;
    timeperiod*            _notification_period;
    bool                   _notifications_enabled;
    int                    _notified_states;
    int                    _pending_flex_downtime;
    int                    _recovery_notification_delay;
    int                    _scheduled_downtime_depth;
    unsigned int           _flapping_comment_id;
  };
}

CCE_END()

#endif // !CCE_NOTIFICATIONS_NOTIFIABLE_HH
