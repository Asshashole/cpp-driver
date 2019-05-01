/*
  Copyright (c) DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#ifndef __TEST_DSE_CLUSTER_HPP__
#define __TEST_DSE_CLUSTER_HPP__
#include "objects/cluster.hpp"

#include "dse.h"

namespace test { namespace driver { namespace dse {

/**
 * Wrapped cluster object (builder) for DSE extras
 */
class Cluster : public driver::Cluster {
public:
  /**
   * Create the DSE cluster for the builder object
   */
  Cluster()
      : driver::Cluster(cass_cluster_new_dse()){};

  /**
   * Create the DSE cluster for the builder object
   *
   * @param cluster Already defined cluster object to utilize
   */
  Cluster(CassCluster* cluster)
      : driver::Cluster(cluster) {}

  /**
   * Create the DSE cluster object from a shared reference
   *
   * @param cluster Shared reference
   */
  Cluster(Ptr cluster)
      : driver::Cluster(cluster) {}

  /**
   * Create the DSE cluster object from a wrapped cluster
   *
   * @param cluster Wrapped cluster object
   */
  Cluster(driver::Cluster cluster)
      : driver::Cluster(cluster) {}

  /**
   * Destroy the DSE cluster
   */
  virtual ~Cluster(){};

  /**
   * Build/Create the DSE cluster
   *
   * @return DSE cluster object
   */
  static Cluster build() { return Cluster(); }

  /**
   * Enable GSSAPI/SASL authentication
   *
   * @param service Name of the service
   * @param principal Principal for the server
   * @return DSE cluster object
   */
  Cluster& with_gssapi_authenticator(const std::string& service, const std::string& principal) {
    EXPECT_EQ(CASS_OK,
              cass_cluster_set_dse_gssapi_authenticator(get(), service.c_str(), principal.c_str()));
    return *this;
  }

  /**
   * Enable GSSAPI/SASL authentication proxy
   *
   * @param service Name of the service
   * @param principal Principal for the server
   * @param authorization_id Authorization ID
   * @return DSE cluster object
   */
  Cluster& with_gssapi_authenticator_proxy(const std::string& service, const std::string& principal,
                                           const std::string& authorization_id) {
    EXPECT_EQ(CASS_OK, cass_cluster_set_dse_gssapi_authenticator_proxy(
                           get(), service.c_str(), principal.c_str(), authorization_id.c_str()));
    return *this;
  }

  /**
   * Enable plain text authentication
   *
   * @param username Username to authenticate
   * @param password Password for username
   * @return DSE cluster object
   */
  Cluster& with_plaintext_authenticator(const std::string& username, const std::string& password) {
    EXPECT_EQ(CASS_OK, cass_cluster_set_dse_plaintext_authenticator(get(), username.c_str(),
                                                                    password.c_str()));
    return *this;
  }

  /**
   * Enable plain text authentication proxy
   *
   * @param username Username to authenticate
   * @param password Password for username
   * @param authorization_id Authorization ID
   * @return DSE cluster object
   */
  Cluster& with_plaintext_authenticator_proxy(const std::string& username,
                                              const std::string& password,
                                              const std::string& authorization_id) {
    EXPECT_EQ(CASS_OK, cass_cluster_set_dse_plaintext_authenticator_proxy(
                           get(), username.c_str(), password.c_str(), authorization_id.c_str()));
    return *this;
  }
};

}}} // namespace test::driver::dse

#endif // __TEST_DSE_CLUSTER_HPP__
