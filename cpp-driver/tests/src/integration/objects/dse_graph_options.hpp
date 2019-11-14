/*
  Copyright (c) DataStax, Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/

#ifndef __TEST_DSE_GRAPH_OPTIONS_HPP__
#define __TEST_DSE_GRAPH_OPTIONS_HPP__
#include "dse.h"

#include "objects/object_base.hpp"

#include <gtest/gtest.h>

namespace test { namespace driver { namespace dse {

/**
 * Wrapped DSE graph options object
 */
class GraphOptions : public Object<DseGraphOptions, dse_graph_options_free> {
public:
  /**
   * Create the empty DSE graph options object
   */
  GraphOptions()
      : Object<DseGraphOptions, dse_graph_options_free>(dse_graph_options_new()) {}

  /**
   * Create the DSE graph options object from the native driver DSE graph
   * options object
   *
   * @param options Native driver object
   */
  GraphOptions(DseGraphOptions* options)
      : Object<DseGraphOptions, dse_graph_options_free>(options) {}

  /**
   * Create the DSE graph options object from the shared reference
   *
   * @param options Shared reference
   */
  GraphOptions(Ptr options)
      : Object<DseGraphOptions, dse_graph_options_free>(options) {}

  /**
   * Set the language to use when applied to a DSE graph statement
   *
   * @param language Language to apply
   */
  void set_language(const std::string& language) {
    ASSERT_EQ(CASS_OK, dse_graph_options_set_graph_language(get(), language.c_str()));
  }

  /**
   * Set the graph name to use when applied to a DSE graph statement
   *
   * @param name Graph name to apply
   */
  void set_name(const std::string& name) {
    ASSERT_EQ(CASS_OK, dse_graph_options_set_graph_name(get(), name.c_str()));
  }

  /**
   * Set the read consistency used by graph queries
   *
   * @param consistency Consistency to apply
   */
  void set_read_consistency(const CassConsistency consistency) {
    ASSERT_EQ(CASS_OK, dse_graph_options_set_read_consistency(get(), consistency));
  }

  /**
   * Set the traversal source to use when applied to a DSE graph statement
   *
   * @param source Traversal source to apply
   */
  void set_source(const std::string& source) {
    ASSERT_EQ(CASS_OK, dse_graph_options_set_graph_source(get(), source.c_str()));
  }

  /**
   * Set the graph timeout to use when applied to a DSE graph statement
   *
   * @param timemout_ms Graph timeout (in milliseconds) to apply
   */
  void set_timeout(const cass_int64_t timemout_ms) {
    ASSERT_EQ(CASS_OK, dse_graph_options_set_request_timeout(get(), timemout_ms));
  }

  /**
   * Set the write consistency used by graph queries
   *
   * @param consistency Consistency to apply
   */
  void set_write_consistency(const CassConsistency consistency) {
    ASSERT_EQ(CASS_OK, dse_graph_options_set_write_consistency(get(), consistency));
  }
};

}}} // namespace test::driver::dse

#endif // __TEST_DSE_GRAPH_OPTIONS_HPP__
