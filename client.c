#include "parser_lib/query.h"
#include "parser_lib/printer.h"
#include "parser_lib/main.h"
#include "client_convert.h"

#include "gen-c_glib/query_svc.h"

#include <thrift/c_glib/transport/thrift_transport.h>
#include <thrift/c_glib/transport/thrift_socket.h>
#include <thrift/c_glib/transport/thrift_buffered_transport.h>
#include <thrift/c_glib/protocol/thrift_binary_protocol.h>

#include <glib-object.h>
#include <glib.h>

#include <stdio.h>

#define BUF_SIZE 1024

int
main(void) {
    ThriftSocket *socket;
    ThriftTransport *transport;
    ThriftProtocol *protocol;

    querySvcIf *client;

    gboolean success;
    gchar *message = NULL;
    I_QueryResult *result = NULL;
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

    char *buffer = malloc(BUF_SIZE * sizeof(char));
    buffer[0] = '\0';

    while (1) {
        printf("> ");
        size_t i = 0;
        char c;
        while ((c = getchar()) != EOF) {
            buffer[i++] = c;
            if (c == ';')
                break;
        }
        buffer[i] = '\0';
        if (strcmp(buffer, "exit;") == 0)
            break;

        Query q = get_query(buffer);
        print_query(q);
        I_Query *i_query = convert_query(&q);

        result = g_object_new(TYPE_I__QUERY_RESULT, NULL);
        success = query_svc_if_execute(client, &result, i_query, &error);
        if (success) {
            g_object_get(result, "message", &message, NULL);
            puts(message);
        } else {
            fprintf(stderr, "Client caught an exception: %s\n", error->message);
            g_clear_error(&error);
        }
    }
    free(buffer);

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
