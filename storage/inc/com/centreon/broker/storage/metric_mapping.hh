/*
** Copyright 2015 Centreon
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

#ifndef CCB_STORAGE_METRIC_MAPPING_HH
#  define CCB_STORAGE_METRIC_MAPPING_HH

#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/io/event_info.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/timestamp.hh"
#  include "com/centreon/broker/mapping/entry.hh"

CCB_BEGIN()

namespace          storage {
  /**
   *  @class metric_mapping metric_mapping.hh "com/centreon/broker/storage/metric_mapping.hh"
   *  @brief Information about a metric stored in the database.
   *
   *  Used to provide more informations about the mapping of
   *  the metrics to status.
   */
  class            metric_mapping : public io::data {
  public:
                   metric_mapping();
                   metric_mapping(metric_mapping const& s);
                   ~metric_mapping();
    metric_mapping&
                   operator=(metric_mapping const& s);
    unsigned int   type() const;
    static unsigned int
                   static_type();

    unsigned int   index_id;
    unsigned int   metric_id;

    static mapping::entry const
                   entries[];
    static io::event_info::event_operations const
                   operations;

  private:
    void           _internal_copy(metric_mapping const& s);
  };
}

CCB_END()

#endif // !CCB_STORAGE_METRIC_MAPPING_HH
