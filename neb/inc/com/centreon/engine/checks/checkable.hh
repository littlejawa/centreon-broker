/*
** Copyright 2017-2018 Centreon
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

#ifndef CCE_CHECKS_CHECKABLE_HH
#  define CCE_CHECKS_CHECKABLE_HH

#  include "com/centreon/engine/namespace.hh"
#  include "com/centreon/engine/objects/timeperiod.hh"

CCE_BEGIN()

  // Forward declarations
  namespace            commands {
    class command;
  }

namespace              checks {
  /**
   *  @class checkable checkable.hh "com/centreon/engine/checks/checkable.hh"
   *  @brief Object executing checks.
   *
   */
  class                checkable {
   public:
                       checkable();
                       checkable(checkable const& other);
    virtual            ~checkable();
    checkable&         operator=(checkable const& other);

    // Check configuration.
    bool               get_active_checks_enabled() const;
    void               set_active_checks_enabled(bool enable);
    commands::command* get_check_command() const;
    void               set_check_command(commands::command* cmd);
    std::string const& get_check_command_args() const;
    void               set_check_command_args(std::string const& args);
    timeperiod*        get_check_period() const;
    void               set_check_period(timeperiod* tp);
    int                get_max_attempts() const;
    void               set_max_attempts(int attempts);
    int                get_normal_check_interval() const;
    void               set_normal_check_interval(int interval);
    bool               get_ocp_enabled() const;
    void               set_ocp_enabled(bool enable);
    bool               get_passive_checks_enabled() const;
    void               set_passive_checks_enabled(bool enable);
    bool               get_process_perfdata() const;
    void               set_process_perfdata(bool process);
    int                get_retry_check_interval() const;
    void               set_retry_check_interval(int interval);
    std::string const& get_timezone() const;
    void               set_timezone(std::string const& timezone);

    // Check runtime.
    int                get_check_options() const;
    void               set_check_options(int options);
    int                get_check_type() const;
    void               set_check_type(int type);
    int                get_current_attempt() const;
    void               set_current_attempt(int attempt);
    bool               get_executing() const;
    void               set_executing(bool executing);
    double             get_execution_time() const;
    void               set_execution_time(double execution_time);
    virtual bool       is_host() const = 0;
    time_t             get_last_check() const;
    void               set_last_check(time_t last_check);
    double             get_latency() const;
    void               set_latency(double latency);
    std::string const& get_long_output() const;
    void               set_long_output(std::string const& output);
    time_t             get_next_check() const;
    void               set_next_check(time_t next_check);
    std::string const& get_output() const;
    void               set_output(std::string const& output);
    std::string const& get_perfdata() const;
    void               set_perfdata(std::string const& perfdata);
    bool               get_should_be_scheduled() const;
    void               set_should_be_scheduled(bool schedule);

    // State runtime.
    int                get_current_event_id() const;
    void               set_current_event_id(int id);
    int                get_current_problem_id() const;
    void               set_current_problem_id(int id);
    int                get_current_state() const;
    void               set_current_state(int state);
    int                get_current_state_type() const;
    void               set_current_state_type(int type);
    bool               get_has_been_checked() const;
    void               set_has_been_checked(bool checked);
    bool               get_host_problem_at_last_check() const;
    void               set_host_problem_at_last_check(bool problem);
    int                get_last_event_id() const;
    void               set_last_event_id(int id);
    int                get_last_hard_state() const;
    void               set_last_hard_state(int state);
    time_t             get_last_hard_state_change() const;
    void               set_last_hard_state_change(time_t last_change);
    int                get_last_problem_id() const;
    void               set_last_problem_id(int id);
    int                get_last_state() const;
    void               set_last_state(int state);
    time_t             get_last_state_change() const;
    void               set_last_state_change(time_t last_change);
    int                get_modified_attributes() const;
    void               set_modified_attributes(int attributes);
    double             get_percent_state_change() const;
    void               set_percent_state_change(double change);

    // Event handler.
    commands::command* get_event_handler() const;
    void               set_event_handler(commands::command* cmd);
    std::string const& get_event_handler_args() const;
    void               set_event_handler_args(std::string const& args);
    bool               get_event_handler_enabled() const;
    void               set_event_handler_enabled(bool enable);

    // Flap detection.
    static int const   historical_state_entries = 21;
    bool               get_flap_detection_enabled() const;
    void               set_flap_detection_enabled(bool enable);
    bool               get_flapping() const;
    void               set_flapping(bool flapping);
    double             get_low_flap_threshold() const;
    void               set_low_flap_threshold(double threshold);
    double             get_high_flap_threshold() const;
    void               set_high_flap_threshold(double threshold);
    void               add_historical_state(int state);
    int                get_historical_state(int index) const;

    // Freshness checks.
    bool               get_being_freshened() const;
    void               set_being_freshened(bool freshened);
    bool               get_freshness_checks_enabled() const;
    void               set_freshness_checks_enabled(bool enable);
    int                get_freshness_threshold() const;
    void               set_freshness_threshold(int threshold);

   private:
    void               _internal_copy(checkable const& other);

    bool               _active_checks_enabled;
    bool               _being_freshened;
    commands::command* _check_command;
    std::string        _check_command_args;
    int                _check_options;
    timeperiod*        _check_period;
    int                _check_type;
    int                _current_attempt;
    int                _current_event_id;
    int                _current_problem_id;
    int                _current_state;
    int                _current_state_type;
    commands::command* _event_handler;
    std::string        _event_handler_args;
    bool               _event_handler_enabled;
    bool               _executing;
    double             _execution_time;
    bool               _flap_detection_enabled;
    bool               _flapping;
    bool               _freshness_checks_enabled;
    int                _freshness_threshold;
    bool               _has_been_checked;
    double             _high_flap_threshold;
    int                _historical_states[historical_state_entries];
    int                _historical_state_index;
    bool               _host_problem_at_last_check;
    time_t             _last_check;
    int                _last_event_id;
    int                _last_hard_state;
    time_t             _last_hard_state_change;
    int                _last_problem_id;
    int                _last_state;
    time_t             _last_state_change;
    double             _latency;
    std::string        _long_output;
    double             _low_flap_threshold;
    int                _max_attempts;
    int                _modified_attributes;
    time_t             _next_check;
    int                _normal_check_interval;
    bool               _ocp_enabled;
    std::string        _output;
    bool               _passive_checks_enabled;
    double             _percent_state_change;
    std::string        _perfdata;
    bool               _process_perfdata;
    int                _retry_check_interval;
    bool               _should_be_scheduled;
    std::string        _timezone;
  };
}

CCE_END()

#endif // !CCE_CHECKS_CHECKABLE_HH
