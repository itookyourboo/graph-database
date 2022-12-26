#include "gen-c_glib/query_svc.h"

#include <thrift/c_glib/transport/thrift_transport.h>
#include <thrift/c_glib/transport/thrift_socket.h>
#include <thrift/c_glib/transport/thrift_buffered_transport.h>
#include <thrift/c_glib/protocol/thrift_binary_protocol.h>

#include <glib-object.h>
#include <glib.h>

#include <stdio.h>

int
main(void) {
    ThriftSocket *socket;
    ThriftTransport *transport;
    ThriftProtocol *protocol;

    querySvcIf *client;

    gboolean success;
    gchar *message = NULL;
    QueryResult *result = NULL;
    GError *error = NULL;

#if (!GLIB_CHECK_VERSION(2, 36, 0))
    g_type_init ();
#endif

    socket = g_object_new(THRIFT_TYPE_SOCKET,
                          "hostname", "localhost",
                          "port", 9090,
                          NULL);
    transport = g_object_new(THRIFT_TYPE_BUFFERED_TRANSPORT,
                             "transport", socket,
                             NULL);
    protocol = g_object_new(THRIFT_TYPE_BINARY_PROTOCOL,
                            "transport", transport,
                            NULL);

    thrift_transport_open(transport, &error);

    client = g_object_new(TYPE_QUERY_SVC_CLIENT,
                          "input_protocol", protocol,
                          "output_protocol", protocol,
                          NULL);

    Query *query = g_object_new(TYPE_QUERY, NULL);
    uQuery *u_query = g_object_new(TYPE_U_QUERY, NULL);
    SchemaGetQuery *schema_get_query = g_object_new(TYPE_SCHEMA_GET_QUERY, NULL);
    g_object_set(
            schema_get_query,
            "schema_name", "schema",
            NULL
    );
    g_object_set(
            u_query,
            "schema_get_query", schema_get_query,
            NULL
    );
    g_object_set(
            query,
            "type", QUERY_TYPE_SCHEMA_GET,
            "query", u_query,
            NULL
    );

    result = g_object_new(TYPE_QUERY_RESULT, NULL);
    success = query_svc_if_execute(client, &result, query, &error);
    if (success) {
        g_object_get(result, "message", &message, NULL);
        puts(message);
    } else {
        fprintf(stderr, "Client caught an exception: %s\n", error->message);
        g_clear_error(&error);
    }

    g_object_unref(schema_get_query);
    g_object_unref(u_query);
    g_object_unref(query);
    g_object_unref(result);

    thrift_transport_close(transport, &error);
    success = success && (error == NULL);

    g_clear_error(&error);

    g_object_unref(client);

    g_object_unref(protocol);
    g_object_unref(transport);
    g_object_unref(socket);

    return (success ? 0 : 1);
}
