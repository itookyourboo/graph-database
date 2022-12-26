#include "gen-c_glib/query_svc.h"

#include <thrift/c_glib/server/thrift_simple_server.h>
#include <thrift/c_glib/transport/thrift_server_socket.h>
#include <thrift/c_glib/transport/thrift_buffered_transport_factory.h>
#include <thrift/c_glib/protocol/thrift_binary_protocol_factory.h>
#include <thrift/c_glib/protocol/thrift_protocol_factory.h>
#include <thrift/c_glib/transport/thrift_server_transport.h>
#include <thrift/c_glib/thrift.h>

#include <glib-object.h>
#include <glib.h>

#include <stdio.h>
#include <malloc.h>

G_BEGIN_DECLS

#define TYPE_EXAMPLE_QUERY_SVC_HANDLER                          \
  (example_query_svc_handler_get_type ())
#define EXAMPLE_QUERY_SVC_HANDLER(obj)                          \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj),                           \
                               TYPE_EXAMPLE_QUERY_SVC_HANDLER,  \
                               ExampleQuerySvcHandler))
#define EXAMPLE_QUERY_SVC_HANDLER_CLASS(c)                      \
  (G_TYPE_CHECK_CLASS_CAST ((c),                                \
                            TYPE_EXAMPLE_QUERY_SVC_HANDLER,     \
                            ExampleQuerySvcHandlerClass))
#define IS_EXAMPLE_QUERY_SVC_HANDLER(obj)                       \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),                           \
                               TYPE_EXAMPLE_QUERY_SVC_HANDLER))
#define IS_EXAMPLE_QUERY_SVC_HANDLER_CLASS(c)                   \
  (G_TYPE_CHECK_CLASS_TYPE ((c),                                \
                            TYPE_EXAMPLE_QUERY_SVC_HANDLER))
#define EXAMPLE_QUERY_SVC_HANDLER_GET_CLASS(obj)                \
  (G_TYPE_INSTANCE_GET_CLASS ((obj),                            \
                              TYPE_EXAMPLE_QUERY_SVC_HANDLER,   \
                              ExampleQuerySvcHandlerClass))

struct _ExampleQuerySvcHandler {
    querySvcHandler parent_instance;
};
typedef struct _ExampleQuerySvcHandler ExampleQuerySvcHandler;

struct _ExampleQuerySvcHandlerClass {
    querySvcHandlerClass parent_class;
};
typedef struct _ExampleQuerySvcHandlerClass ExampleQuerySvcHandlerClass;

GType example_query_svc_handler_get_type(void);

G_END_DECLS

G_DEFINE_TYPE(ExampleQuerySvcHandler,
              example_query_svc_handler,
        TYPE_QUERY_SVC_HANDLER

)

static gboolean
example_query_svc_handler_execute(querySvcIf *iface,
                                  QueryResult **_return,
                                  const Query *query,
                                  GError **error) {
    THRIFT_UNUSED_VAR(iface);
    THRIFT_UNUSED_VAR(error);

    g_assert(*_return != NULL);

    printf("Server received query type: %d, from client\n", query->type);

    g_object_set(
            *_return,
            "type", QUERY_RESULT_TYPE_RESULT_NONE,
            "message", "200, Query executed",
            NULL
    );

    return TRUE;
}

static void
example_query_svc_handler_init(ExampleQuerySvcHandler *self) {
}

static void
example_query_svc_handler_class_init(ExampleQuerySvcHandlerClass *klass) {
    querySvcHandlerClass *query_svc_handler_class =
            QUERY_SVC_HANDLER_CLASS (klass);

    query_svc_handler_class->execute = example_query_svc_handler_execute;
}

int
main(void) {
    querySvcHandler *handler;
    querySvcProcessor *processor;

    ThriftServerTransport *server_transport;
    ThriftTransportFactory *transport_factory;
    ThriftProtocolFactory *protocol_factory;
    ThriftServer *server;

    GError *error = NULL;

#if (!GLIB_CHECK_VERSION(2, 36, 0))
    g_type_init ();
#endif

    handler = g_object_new(TYPE_EXAMPLE_QUERY_SVC_HANDLER,
                           NULL);
    processor = g_object_new(TYPE_QUERY_SVC_PROCESSOR,
                             "handler", handler,
                             NULL);

    server_transport = g_object_new(THRIFT_TYPE_SERVER_SOCKET,
                                    "port", 9090,
                                    NULL);
    transport_factory = g_object_new(THRIFT_TYPE_BUFFERED_TRANSPORT_FACTORY,
                                     NULL);
    protocol_factory = g_object_new(THRIFT_TYPE_BINARY_PROTOCOL_FACTORY,
                                    NULL);

    server = g_object_new(THRIFT_TYPE_SIMPLE_SERVER,
                          "processor", processor,
                          "server_transport", server_transport,
                          "input_transport_factory", transport_factory,
                          "output_transport_factory", transport_factory,
                          "input_protocol_factory", protocol_factory,
                          "output_protocol_factory", protocol_factory,
                          NULL);

    thrift_server_serve(server, &error);

    g_object_unref(server);
    g_object_unref(protocol_factory);
    g_object_unref(transport_factory);
    g_object_unref(server_transport);

    g_object_unref(processor);
    g_object_unref(handler);

    return 0;
}