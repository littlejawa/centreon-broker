/*
** Copyright 2009-2013,2019 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#include <cstdlib>
#include <cstring>
#include <ctime>
#include <memory>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/config/applier/state.hh"
#include "com/centreon/broker/neb/callbacks.hh"
#include "com/centreon/broker/neb/events.hh"
#include "com/centreon/broker/neb/initial.hh"
#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/engine/broker.hh"
#include "com/centreon/engine/common.hh"
#include "com/centreon/engine/configuration/applier/state.hh"
#include "com/centreon/engine/downtime_manager.hh"
#include "com/centreon/engine/globals.hh"
#include "com/centreon/engine/hostgroup.hh"
#include "com/centreon/engine/nebcallbacks.hh"
#include "com/centreon/engine/nebstructs.hh"
#include "com/centreon/engine/objects.hh"
#include "com/centreon/engine/servicegroup.hh"

// Internal Nagios host list.
extern "C" {
  extern hostdependency* hostdependency_list;
  extern servicedependency* servicedependency_list;
}

using namespace com::centreon::broker;

// NEB module list.
extern "C" {
  nebmodule* neb_module_list;
}

/**************************************
*                                     *
*          Static Functions           *
*                                     *
**************************************/

/**
 *  Send to the global publisher the list of custom variables.
 */
static void send_custom_variables_list() {
  // Start log message.
  logging::info(logging::medium)
    << "init: beginning custom variables dump";

  // Iterate through all hosts.
  for (umap<std::string, com::centreon::shared_ptr<com::centreon::engine::host> >::const_iterator
         it_hst(com::centreon::engine::configuration::applier::state::instance().hosts().begin()),
         end_hst(com::centreon::engine::configuration::applier::state::instance().hosts().end());
       it_hst != end_hst;
       ++it_hst)
    // Send all custom variables.
    for (com::centreon::engine::customvar_set::const_iterator
           it_var(it_hst->second->get_customvars().begin()),
           end_var(it_hst->second->get_customvars().end());
         it_var != end_var;
         ++it_var) {
      // Fill callback struct.
      nebstruct_custom_variable_data nscvd;
      memset(&nscvd, 0, sizeof(nscvd));
      nscvd.type = NEBTYPE_HOSTCUSTOMVARIABLE_ADD;
      nscvd.timestamp.tv_sec = time(NULL);
      nscvd.var_name = it_var->second.get_name().c_str();
      nscvd.var_value = it_var->second.get_value().c_str();
      nscvd.object_ptr = it_hst->second.get();

      // Callback.
      neb::callback_custom_variable(
             NEBCALLBACK_CUSTOM_VARIABLE_DATA,
             &nscvd);
    }

  // Iterate through all services.
  for (umap<std::pair<std::string, std::string>, com::centreon::shared_ptr<com::centreon::engine::service> >::const_iterator
         it_svc(com::centreon::engine::configuration::applier::state::instance().services().begin()),
         end_svc(com::centreon::engine::configuration::applier::state::instance().services().end());
       it_svc != end_svc;
       ++it_svc)
    // Send all custom variables.
    for (com::centreon::engine::customvar_set::const_iterator
           it_var(it_svc->second->get_customvars().begin()),
           end_var(it_svc->second->get_customvars().end());
         it_var != end_var;
         ++it_var) {
      // Fill callback struct.
      nebstruct_custom_variable_data nscvd;
      memset(&nscvd, 0, sizeof(nscvd));
      nscvd.type = NEBTYPE_SERVICECUSTOMVARIABLE_ADD;
      nscvd.timestamp.tv_sec = time(NULL);
      nscvd.var_name = it_var->second.get_name().c_str();
      nscvd.var_value = it_var->second.get_value().c_str();
      nscvd.object_ptr = it_svc->second.get();

      // Callback.
      neb::callback_custom_variable(
             NEBCALLBACK_CUSTOM_VARIABLE_DATA,
             &nscvd);
    }

  // End log message.
  logging::info(logging::medium)
    << "init: end of custom variables dump";

  return ;
}

/**
 *  Send to the global publisher the list of downtimes.
 */
static void send_downtimes_list() {
  // Start log message.
  logging::info(logging::medium) << "init: beginning downtimes dump";

  // Iterate through all downtimes.
  for (umap<unsigned long, com::centreon::engine::downtime>::const_iterator
         it(com::centreon::engine::downtime_manager::instance().get_downtimes().begin()),
         end(com::centreon::engine::downtime_manager::instance().get_downtimes().end());
       it != end;
       ++it) {
    // Fill callback struct.
    nebstruct_downtime_data nsdd;
    memset(&nsdd, 0, sizeof(nsdd));
    nsdd.type = NEBTYPE_DOWNTIME_ADD;
    nsdd.timestamp.tv_sec = time(NULL);
    nsdd.downtime_type = it->second.get_type();
    // XXX
    // nsdd.host_name = dt->host_name;
    // nsdd.service_description = dt->service_description;
    nsdd.entry_time = it->second.get_entry_time();
    nsdd.author_name = it->second.get_author().c_str();
    nsdd.comment_data = it->second.get_comment().c_str();
    nsdd.start_time = it->second.get_start_time();
    nsdd.end_time = it->second.get_end_time();
    nsdd.fixed = it->second.get_fixed();
    nsdd.duration = it->second.get_duration();
    nsdd.triggered_by = it->second.get_triggered_by();
    nsdd.downtime_id = it->second.get_id();
    nsdd.object_ptr = const_cast<void*>(static_cast<void const*>(&it->second));

    // Callback.
    neb::callback_downtime(NEBCALLBACK_DOWNTIME_DATA, &nsdd);
  }

  // End log message.
  logging::info(logging::medium) << "init: end of downtimes dump";

  return ;
}

/**
 *  Send to the global publisher the list of host dependencies within Nagios.
 */
static void send_host_dependencies_list() {
  // Start log message.
  logging::info(logging::medium)
    << "init: beginning host dependencies dump";

  try {
    // Loop through all dependencies.
    for (hostdependency* hd(hostdependency_list); hd; hd = hd->next) {
      // Fill callback struct.
      nebstruct_adaptive_dependency_data nsadd;
      memset(&nsadd, 0, sizeof(nsadd));
      nsadd.type = NEBTYPE_HOSTDEPENDENCY_ADD;
      nsadd.flags = NEBFLAG_NONE;
      nsadd.attr = NEBATTR_NONE;
      nsadd.timestamp.tv_sec = time(NULL);
      nsadd.object_ptr = hd;

      // Callback.
      neb::callback_dependency(
             NEBCALLBACK_ADAPTIVE_DEPENDENCY_DATA,
             &nsadd);
    }
  }
  catch (std::exception const& e) {
    logging::error(logging::high)
      << "init: error occurred while dumping host dependencies: "
      << e.what();
  }
  catch (...) {
    logging::error(logging::high)
      << "init: unknown error occurred while dumping host dependencies";
  }

  // End log message.
  logging::info(logging::medium)
    << "init: end of host dependencies dump";

  return ;
}

/**
 *  Send to the global publisher the list of host groups within Engine.
 */
static void send_host_group_list() {
  // Start log message.
  logging::info(logging::medium)
    << "init: beginning host group dump";

  // Loop through all host groups.
  for (umap<std::string, com::centreon::shared_ptr<com::centreon::engine::hostgroup> >::const_iterator
         it_grp(com::centreon::engine::configuration::applier::state::instance().hostgroups().begin()),
         end_grp(com::centreon::engine::configuration::applier::state::instance().hostgroups().end());
       it_grp != end_grp;
       ++it_grp) {
    // Fill callback struct.
    nebstruct_group_data nsgd;
    memset(&nsgd, 0, sizeof(nsgd));
    nsgd.type = NEBTYPE_HOSTGROUP_ADD;
    nsgd.object_ptr = it_grp->second.get();

    // Callback.
    neb::callback_group(NEBCALLBACK_GROUP_DATA, &nsgd);

    // Dump host group members.
    for (umap<std::string, com::centreon::engine::host*>::const_iterator
           it_hst(it_grp->second->get_members().begin()),
           end_hst(it_grp->second->get_members().end());
         it_hst != end_hst;
         ++it_hst) {
      // Fill callback struct.
      nebstruct_group_member_data nsgmd;
      memset(&nsgmd, 0, sizeof(nsgmd));
      nsgmd.type = NEBTYPE_HOSTGROUPMEMBER_ADD;
      nsgmd.object_ptr = it_hst->second;
      nsgmd.group_ptr = it_grp->second.get();

      // Callback.
      neb::callback_group_member(NEBCALLBACK_GROUP_MEMBER_DATA, &nsgmd);
    }
  }

  // End log message.
  logging::info(logging::medium)
    << "init: end of host group dump";

  return ;
}

/**
 *  Send to the global publisher the list of hosts within Nagios.
 */
static void send_host_list() {
  // Start log message.
  logging::info(logging::medium) << "init: beginning host dump";

  // Loop through all hosts.
  for (umap<std::string, com::centreon::shared_ptr<com::centreon::engine::host> >::const_iterator
         it(com::centreon::engine::configuration::applier::state::instance().hosts().begin()),
         end(com::centreon::engine::configuration::applier::state::instance().hosts().end());
       it != end;
       ++it) {
    // Fill callback struct.
    nebstruct_adaptive_host_data nsahd;
    memset(&nsahd, 0, sizeof(nsahd));
    nsahd.type = NEBTYPE_HOST_ADD;
    nsahd.command_type = CMD_NONE;
    nsahd.modified_attribute = MODATTR_ALL;
    nsahd.modified_attributes = MODATTR_ALL;
    nsahd.object_ptr = it->second.get();

    // Callback.
    neb::callback_host(NEBCALLBACK_ADAPTIVE_HOST_DATA, &nsahd);
  }

  // End log message.
  logging::info(logging::medium) << "init: end of host dump";

  return ;
}

/**
 *  Send to the global publisher the list of host parents within Nagios.
 */
static void send_host_parents_list() {
  // Start log message.
  logging::info(logging::medium) << "init: beginning host parents dump";

  try {
    // Loop through all hosts.
    for (umap<std::string, com::centreon::shared_ptr<com::centreon::engine::host> >::const_iterator
           it_hst(com::centreon::engine::configuration::applier::state::instance().hosts().begin()),
           end_hst(com::centreon::engine::configuration::applier::state::instance().hosts().end());
         it_hst != end_hst;
         ++it_hst)
      // Loop through all parents.
      for (std::list<com::centreon::engine::host*>::const_iterator
             it_parent(it_hst->second->get_parents().begin()),
             end_parent(it_hst->second->get_parents().end());
           it_parent != end_parent;
           ++it_parent) {
        // Fill callback struct.
        nebstruct_relation_data nsrd;
        memset(&nsrd, 0, sizeof(nsrd));
        nsrd.type = NEBTYPE_PARENT_ADD;
        nsrd.flags = NEBFLAG_NONE;
        nsrd.attr = NEBATTR_NONE;
        nsrd.timestamp.tv_sec = time(NULL);
        nsrd.hst = *it_parent;
        nsrd.dep_hst = it_hst->second.get();

        // Callback.
        neb::callback_relation(NEBTYPE_PARENT_ADD, &nsrd);
      }
  }
  catch (std::exception const& e) {
    logging::error(logging::high)
      << "init: error occurred while dumping host parents: "
      << e.what();
  }
  catch (...) {
    logging::error(logging::high)
      << "init: unknown error occurred while dumping host parents";
  }

  // End log message.
  logging::info(logging::medium) << "init: end of host parents dump";

  return ;
}

/**
 *  Send to the global publisher the list of modules loaded by Engine.
 */
static void send_module_list() {
  // Start log message.
  logging::info(logging::medium)
    << "init: beginning modules dump";

  // Browse module list.
  for (nebmodule* nm(neb_module_list); nm; nm = nm->next)
    if (nm->filename) {
      // Fill callback struct.
      nebstruct_module_data nsmd;
      memset(&nsmd, 0, sizeof(nsmd));
      nsmd.module = nm->filename;
      nsmd.args = nm->args;
      nsmd.type = NEBTYPE_MODULE_ADD;

      // Callback.
      neb::callback_module(NEBTYPE_MODULE_ADD, &nsmd);
    }

  // End log message.
  logging::info(logging::medium) << "init: end of modules dump";

  return ;
}

/**
 *  Send to the global publisher the list of service dependencies within
 *  Nagios.
 */
static void send_service_dependencies_list() {
  // Start log message.
  logging::info(logging::medium)
    << "init: beginning service dependencies dump";

  try {
    // Loop through all dependencies.
    for (servicedependency* sd(servicedependency_list);
         sd;
         sd = sd->next) {
      // Fill callback struct.
      nebstruct_adaptive_dependency_data nsadd;
      memset(&nsadd, 0, sizeof(nsadd));
      nsadd.type = NEBTYPE_SERVICEDEPENDENCY_ADD;
      nsadd.flags = NEBFLAG_NONE;
      nsadd.attr = NEBATTR_NONE;
      nsadd.timestamp.tv_sec = time(NULL);
      nsadd.object_ptr = sd;

      // Callback.
      neb::callback_dependency(
             NEBCALLBACK_ADAPTIVE_DEPENDENCY_DATA,
             &nsadd);
    }
  }
  catch (std::exception const& e) {
    logging::error(logging::high)
      << "init: error occurred while dumping service dependencies: "
      << e.what();
  }
  catch (...) {
    logging::error(logging::high) << "init: unknown error occurred "
      << "while dumping service dependencies";
  }

  // End log message.
  logging::info(logging::medium)
    << "init: end of service dependencies dump";

  return ;
}

/**
 *  Send to the global publisher the list of service groups within Engine.
 */
static void send_service_group_list() {
  // Start log message.
  logging::info(logging::medium)
    << "init: beginning service group dump";

  // Loop through all service groups.
  for (umap<std::string, com::centreon::shared_ptr<com::centreon::engine::servicegroup> >::const_iterator
         it_grp(com::centreon::engine::configuration::applier::state::instance().servicegroups().begin()),
         end_grp(com::centreon::engine::configuration::applier::state::instance().servicegroups().end());
       it_grp != end_grp;
       ++it_grp) {
    // Fill callback struct.
    nebstruct_group_data nsgd;
    memset(&nsgd, 0, sizeof(nsgd));
    nsgd.type = NEBTYPE_SERVICEGROUP_ADD;
    nsgd.object_ptr = it_grp->second.get();

    // Callback.
    neb::callback_group(NEBCALLBACK_GROUP_DATA, &nsgd);

    // Dump service group members.
    for (umap<std::pair<std::string, std::string>, com::centreon::engine::service*>::const_iterator
           it_svc(it_grp->second->get_members().begin()),
           end_svc(it_grp->second->get_members().end());
         it_svc != end_svc;
         ++it_svc) {
      // Fill callback struct.
      nebstruct_group_member_data nsgmd;
      memset(&nsgmd, 0, sizeof(nsgmd));
      nsgmd.type = NEBTYPE_SERVICEGROUPMEMBER_ADD;
      nsgmd.object_ptr = it_svc->second;
      nsgmd.group_ptr = it_grp->second.get();

      // Callback.
      neb::callback_group_member(NEBCALLBACK_GROUP_MEMBER_DATA, &nsgmd);
    }
  }

  // End log message.
  logging::info(logging::medium) << "init: end of service groups dump";

  return ;
}

/**
 *  Send to the global publisher the list of services within Nagios.
 */
static void send_service_list() {
  // Start log message.
  logging::info(logging::medium) << "init: beginning service dump";

  // Loop through all services.
  for (umap<std::pair<std::string, std::string>, com::centreon::shared_ptr<com::centreon::engine::service> >::const_iterator
         it(com::centreon::engine::configuration::applier::state::instance().services().begin()),
         end(com::centreon::engine::configuration::applier::state::instance().services().end());
       it != end;
       ++it) {
    // Fill callback struct.
    nebstruct_adaptive_service_data nsasd;
    memset(&nsasd, 0, sizeof(nsasd));
    nsasd.type = NEBTYPE_SERVICE_ADD;
    nsasd.command_type = CMD_NONE;
    nsasd.modified_attribute = MODATTR_ALL;
    nsasd.modified_attributes = MODATTR_ALL;
    nsasd.object_ptr = it->second.get();

    // Callback.
    neb::callback_service(NEBCALLBACK_ADAPTIVE_SERVICE_DATA, &nsasd);
  }

  // End log message.
  logging::info(logging::medium) << "init: end of services dump";

  return ;
}

/**
 *  Send the instance configuration loaded event.
 */
static void send_instance_configuration() {
  logging::info(logging::medium)
    << "init: sending initial instance configuration loading event";
  misc::shared_ptr<neb::instance_configuration>
    ic(new neb::instance_configuration);
  ic->loaded = true;
  ic->poller_id = config::applier::state::instance().poller_id();
  neb::gl_publisher.write(ic);
}

/**************************************
*                                     *
*          Global Functions           *
*                                     *
**************************************/

/**
 *  Send initial configuration to the global publisher.
 */
void neb::send_initial_configuration() {
  send_host_list();
  send_service_list();
  send_custom_variables_list();
  send_downtimes_list();
  send_host_parents_list();
  send_host_group_list();
  send_service_group_list();
  send_host_dependencies_list();
  send_service_dependencies_list();
  send_module_list();
  send_instance_configuration();
  return ;
}
