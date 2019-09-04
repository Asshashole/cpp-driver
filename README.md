# C/C++ DataStax Enterprise Driver

A driver built specifically for the [DataStax Enterprise][DSE] (DSE). It builds
on the [DataStax C/C++ driver for Apache Cassandra] and includes specific
features for DSE.

This software can be used solely with DataStax Enterprise. See the [License
section](#licence) below.

## Getting the Driver

Binary versions of the driver, available for multiple operating systems and
multiple architectures, can be obtained from our [download server].

## Features

* [DSE authentication]
  * Plaintext/DSE
  * LDAP
  * GSSAPI (Kerberos)
* [DSE geospatial types]
* [DSE graph integration]
* DSE [proxy authentication][DSE Proxy Authentication] and
  [execution][DSE Proxy Execution]
* [DSE DateRange]
* Support for [DataStax Constellation] Cloud Data Platform

## Compatibility

The DataStax C/C++ driver currently supports DataStax Enterprise 4.8+ and its
graph integration only supports DataStax Enterprise 5.0+.

__Disclaimer__: DataStax products do not support big-endian systems.

## Documentation

* [Home]
* [API]
* [Features]
* [Building and Testing]
* [Migration]

## Getting Help

* JIRA: https://datastax-oss.atlassian.net/browse/CPP (Assign "Component/s" field set to "DSE")
* Mailing List: https://groups.google.com/a/lists.datastax.com/forum/#!forum/cpp-driver-user

## Feedback Requested

**Help us focus our efforts!** [Provide your input] on the DSE C/C++ Driver
Platform and Runtime Survey (we kept it short).

## Examples

Examples for using the driver can be found in our [examples repository].

### A Simple Example

Connecting the driver is is the same as the core C/C++ driver except that
`dse.h` should be included instead of `cassandra.h` which is automatically
included by `dse.h`.

```c
/* Include the DSE driver */
#include <dse.h>

int main() {
  /* Setup and connect to cluster */
  CassFuture* connect_future = NULL;
  CassCluster* cluster = cass_cluster_new();
  CassSession* session = cass_session_new();

  /* Add contact points */
  cass_cluster_set_contact_points(cluster, "127.0.0.1");

  /* Provide the cluster object as configuration to connect the session */
  connect_future = cass_session_connect(session, cluster);

  if (cass_future_error_code(connect_future) == CASS_OK) {

    /* Run queries here */

    /* Close the session */
    close_future = cass_session_close(session);
    cass_future_wait(close_future);
    cass_future_free(close_future);
  } else {
    /* Handle error */
    const char* message;
    size_t message_length;
    cass_future_error_message(connect_future, &message, &message_length);
    fprintf(stderr, "Unable to connect: '%.*s'\n", (int)message_length,
                                                        message);
  }

  /* Cleanup driver objects */
  cass_future_free(connect_future);
  cass_cluster_free(cluster);
  cass_session_free(session);

  return 0;
}
```

## License

&copy; DataStax, Inc.

The full license terms are available at
http://www.datastax.com/terms/datastax-dse-driver-license-terms

[DSE]: http://www.datastax.com/products/datastax-enterprise
[DataStax Constellation]: https://constellation.datastax.com
[DataStax C/C++ driver for Apache Cassandra]: https://github.com/datastax/cpp-driver
[download server]: http://downloads.datastax.com/cpp-driver
[GitHub]: https://github.com/datastax/cpp-dse-driver
[Home]: http://docs.datastax.com/en/developer/cpp-driver-dse/latest
[API]: http://docs.datastax.com/en/developer/cpp-driver-dse/latest/api
[Features]: http://docs.datastax.com/en/developer/cpp-driver-dse/latest/features
[Building and Testing]: http://docs.datastax.com/en/developer/cpp-driver-dse/latest/building
[Migration]: http://docs.datastax.com/en/developer/cpp-driver-dse/latest/getting-started
[Provide your input]: http://goo.gl/forms/ihKC5uEQr6
[examples repository]: https://github.com/datastax/cpp-dse-driver-examples
[DSE authentication]: http://docs.datastax.com/en/developer/cpp-driver-dse/latest/features/authentication
[DSE geospatial types]: http://docs.datastax.com/en/developer/cpp-driver-dse/latest/features/geotypes
[DSE graph integration]: http://docs.datastax.com/en/developer/cpp-driver-dse/latest/features/graph
[DSE Proxy Authentication]: http://docs.datastax.com/en/developer/cpp-driver-dse/latest/features/authentication/#proxy-authentication
[DSE Proxy Execution]: http://docs.datastax.com/en/developer/cpp-driver-dse/latest/features/authentication/#proxy-execution
[DSE DateRange]: https://github.com/datastax/cpp-dse-driver-examples/blob/master/dse/examples/date_range/date_range.c
