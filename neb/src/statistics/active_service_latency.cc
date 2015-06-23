/*
** Copyright 2013,2015 Merethis
**
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

#include <iomanip>
#include <sstream>
#include "com/centreon/broker/config/applier/state.hh"
#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/broker/neb/statistics/active_service_latency.hh"
#include "com/centreon/broker/neb/statistics/compute_value.hh"
#include "com/centreon/engine/globals.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::neb;
using namespace com::centreon::broker::neb::statistics;

/**
 *  Default constructor.
 */
active_service_latency::active_service_latency()
  : plugin("active_service_latency") {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
active_service_latency::active_service_latency(active_service_latency const& right)
 : plugin(right) {}

/**
 *  Destructor.
 */
active_service_latency::~active_service_latency() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
active_service_latency& active_service_latency::operator=(active_service_latency const& right) {
  plugin::operator=(right);
  return (*this);
}

/**
 *  Get statistics.
 *
 *  @param[out] output   The output return by the plugin.
 *  @param[out] perfdata The perf data return by the plugin.
 */
void active_service_latency::run(
              std::string& output,
	      std::string& perfdata) {
  compute_value<double> cv;
  for (service* s(service_list); s; s = s->next)
    if (s->check_type == SERVICE_CHECK_ACTIVE)
      cv << s->latency;

  if (cv.size()) {
    // Output.
    std::ostringstream oss;
    oss << "Engine " << config::applier::state::instance().poller_name()
        << " has an average active service latency of "
        << std::fixed << std::setprecision(2) << cv.avg() << "s";
    output = oss.str();

    // Perfdata.
    oss.str("");
    oss << "avg=" << cv.avg() << "s min=" << cv.min()
        << "s max=" << cv.max() << "s";
    perfdata = oss.str();
  }
  else {
    // Output.
    output = "No active service to compute active service "
      "latency on " + config::applier::state::instance().poller_name();
  }
  return ;
}
