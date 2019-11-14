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

#ifndef DATASTAX_ENTERPRISE_INTERNAL_GRAPH_HPP
#define DATASTAX_ENTERPRISE_INTERNAL_GRAPH_HPP

#include "dse.h"

#include "allocated.hpp"
#include "dse_line_string.hpp"
#include "dse_polygon.hpp"
#include "json.hpp"

#include "external.hpp"
#include <scoped_ptr.hpp>

#define DSE_GRAPH_OPTION_LANGUAGE_KEY "graph-language"
#define DSE_GRAPH_OPTION_SOURCE_KEY "graph-source"
#define DSE_GRAPH_OPTION_NAME_KEY "graph-name"
#define DSE_GRAPH_OPTION_READ_CONSISTENCY_KEY "graph-read-consistency"
#define DSE_GRAPH_OPTION_WRITE_CONSISTENCY_KEY "graph-write-consistency"
#define DSE_GRAPH_REQUEST_TIMEOUT "request-timeout"

#define DSE_GRAPH_DEFAULT_LANGUAGE "gremlin-groovy"
#define DSE_GRAPH_DEFAULT_SOURCE "g"
#define DSE_GRAPH_ANALYTICS_SOURCE "a"

#define DSE_LOOKUP_ANALYTICS_GRAPH_SERVER "CALL DseClientTool.getAnalyticsGraphServer()"

namespace datastax { namespace internal { namespace enterprise {

class GraphStatement;

class GraphOptions : public Allocated {
public:
  GraphOptions()
      : payload_(cass_custom_payload_new())
      , read_consistency_(CASS_CONSISTENCY_UNKNOWN)
      , write_consistency_(CASS_CONSISTENCY_UNKNOWN)
      , request_timeout_ms_(0) {
    set_graph_language(DSE_GRAPH_DEFAULT_LANGUAGE);
    set_graph_source(DSE_GRAPH_DEFAULT_SOURCE);
  }

  ~GraphOptions() { cass_custom_payload_free(payload_); }

  CassCustomPayload* payload() const { return payload_; }

  GraphOptions* clone() const;

  void set_graph_language(const datastax::String& graph_language) {
    cass_custom_payload_set_n(
        payload_, DSE_GRAPH_OPTION_LANGUAGE_KEY, sizeof(DSE_GRAPH_OPTION_LANGUAGE_KEY) - 1,
        reinterpret_cast<const cass_byte_t*>(graph_language.data()), graph_language.size());
    graph_language_ = graph_language;
  }

  const datastax::String graph_source() const { return graph_source_; }

  void set_graph_source(const datastax::String& graph_source) {
    cass_custom_payload_set_n(
        payload_, DSE_GRAPH_OPTION_SOURCE_KEY, sizeof(DSE_GRAPH_OPTION_SOURCE_KEY) - 1,
        reinterpret_cast<const cass_byte_t*>(graph_source.data()), graph_source.size());
    graph_source_ = graph_source;
  }

  void set_graph_name(const datastax::String& graph_name) {
    cass_custom_payload_set_n(
        payload_, DSE_GRAPH_OPTION_NAME_KEY, sizeof(DSE_GRAPH_OPTION_NAME_KEY) - 1,
        reinterpret_cast<const cass_byte_t*>(graph_name.data()), graph_name.size());
    graph_name_ = graph_name;
  }

  void set_graph_read_consistency(CassConsistency consistency) {
    const char* name = cass_consistency_string(consistency);
    cass_custom_payload_set_n(payload_, DSE_GRAPH_OPTION_READ_CONSISTENCY_KEY,
                              sizeof(DSE_GRAPH_OPTION_READ_CONSISTENCY_KEY) - 1,
                              reinterpret_cast<const cass_byte_t*>(name), strlen(name));
    read_consistency_ = consistency;
  }

  void set_graph_write_consistency(CassConsistency consistency) {
    const char* name = cass_consistency_string(consistency);
    cass_custom_payload_set_n(payload_, DSE_GRAPH_OPTION_WRITE_CONSISTENCY_KEY,
                              sizeof(DSE_GRAPH_OPTION_WRITE_CONSISTENCY_KEY) - 1,
                              reinterpret_cast<const cass_byte_t*>(name), strlen(name));
    write_consistency_ = consistency;
  }

  int64_t request_timeout_ms() const { return request_timeout_ms_; }

  void set_request_timeout_ms(int64_t timeout_ms);

private:
  CassCustomPayload* payload_;
  datastax::String graph_language_;
  datastax::String graph_name_;
  datastax::String graph_source_;
  CassConsistency read_consistency_;
  CassConsistency write_consistency_;
  int64_t request_timeout_ms_;
};

class GraphWriter
    : public Allocated
    , private json::Writer<json::StringBuffer> {
public:
  GraphWriter()
      : json::Writer<json::StringBuffer>(buffer_) {}

  const char* data() const { return buffer_.GetString(); }
  size_t length() const { return buffer_.GetSize(); }

  bool is_complete() const { return IsComplete(); }

  void add_null() { Null(); }
  void add_bool(cass_bool_t value) { Bool(value != cass_false); }
  void add_int32(cass_int32_t value) { Int(value); }
  void add_int64(cass_int64_t value) { Int64(value); }
  void add_double(cass_double_t value) { Double(value); }

  void add_string(const char* string, size_t length) {
    String(string, static_cast<datastax::rapidjson::SizeType>(length));
  }

  void add_key(const char* key, size_t length) {
    Key(key, static_cast<datastax::rapidjson::SizeType>(length));
  }

  void add_point(cass_double_t x, cass_double_t y);

  void add_line_string(const LineString* line_string) { String(line_string->to_wkt().c_str()); }

  void add_polygon(const Polygon* polygon) { String(polygon->to_wkt().c_str()); }

  void add_writer(const GraphWriter* writer, datastax::rapidjson::Type type) {
    size_t length = writer->buffer_.GetSize();
    Prefix(type);
    memcpy(os_->Push(length), writer->buffer_.GetString(), length);
  }

  void reset() {
    buffer_.Clear();
    Reset(buffer_);
  }

protected:
  void start_object() { StartObject(); }
  void end_object() { EndObject(); }

  void start_array() { StartArray(); }
  void end_array() { EndArray(); }

private:
  json::StringBuffer buffer_;
};

class GraphObject : public GraphWriter {
public:
  GraphObject() { start_object(); }

  void reset() {
    GraphWriter::reset();
    start_object();
  }

  void finish() {
    if (!is_complete()) end_object();
  }
};

class GraphArray : public GraphWriter {
public:
  GraphArray() { start_array(); }

  void reset() {
    GraphWriter::reset();
    start_array();
  }

  void finish() {
    if (!is_complete()) end_array();
  }
};

class GraphStatement : public Allocated {
public:
  GraphStatement(const char* query, size_t length, const GraphOptions* options)
      : query_(query, length)
      , wrapped_(cass_statement_new_n(query, length, 0)) {
    if (options != NULL) {
      cass_statement_set_custom_payload(wrapped_, options->payload());
      cass_statement_set_request_timeout(wrapped_,
                                         static_cast<cass_uint64_t>(options->request_timeout_ms()));
      graph_source_ = options->graph_source();
    } else {
      GraphOptions default_options;
      cass_statement_set_custom_payload(wrapped_, default_options.payload());
      cass_statement_set_request_timeout(
          wrapped_, static_cast<cass_uint64_t>(default_options.request_timeout_ms()));
      graph_source_ = default_options.graph_source();
    }
  }

  ~GraphStatement() { cass_statement_free(wrapped_); }

  const datastax::String graph_source() const { return graph_source_; }

  const CassStatement* wrapped() const { return wrapped_; }

  CassError bind_values(const GraphObject* values) {
    if (values != NULL) {
      cass_statement_reset_parameters(wrapped_, 1);
      return cass_statement_bind_string_n(wrapped_, 0, values->data(), values->length());
    } else {
      cass_statement_reset_parameters(wrapped_, 0);
      return CASS_OK;
    }
  }

  CassError set_timestamp(int64_t timestamp) {
    return cass_statement_set_timestamp(wrapped_, timestamp);
  }

private:
  datastax::String query_;
  datastax::String graph_source_;
  CassStatement* wrapped_;
};

typedef json::Value GraphResult;

class GraphResultSet : public Allocated {
public:
  GraphResultSet(const CassResult* result)
      : rows_(cass_iterator_from_result(result))
      , result_(result) {}

  ~GraphResultSet() {
    cass_iterator_free(rows_);
    cass_result_free(result_);
  }

  size_t count() const { return cass_result_row_count(result_); }

  const GraphResult* next();

private:
  json::Document document_;
  datastax::String json_;
  CassIterator* rows_;
  const CassResult* result_;
};

}}} // namespace datastax::internal::enterprise

EXTERNAL_TYPE(datastax::internal::enterprise::GraphOptions, DseGraphOptions)
EXTERNAL_TYPE(datastax::internal::enterprise::GraphStatement, DseGraphStatement)
EXTERNAL_TYPE(datastax::internal::enterprise::GraphArray, DseGraphArray)
EXTERNAL_TYPE(datastax::internal::enterprise::GraphObject, DseGraphObject)
EXTERNAL_TYPE(datastax::internal::enterprise::GraphResultSet, DseGraphResultSet)
EXTERNAL_TYPE(datastax::internal::enterprise::GraphResult, DseGraphResult)

#endif
