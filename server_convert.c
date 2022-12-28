#include "server_convert.h"

Attribute *convert_attribute(I_Attribute *i_attribute) {
    gchar *name;
    I_ValueType vt;
    g_object_get(i_attribute,
                 "name", &name,
                 "vt", &vt,
                 NULL);

    Attribute *attribute = attribute_init();
    String *string = string_init();
    string->size = strlen(name);
    string->content = g_strdup(name);
    attribute->name = string;
    attribute->data_type = (int) vt;

    return attribute;
}

Attribute **convert_attributes(GPtrArray *array) {
    Attribute **attributes = malloc(array->len * sizeof(Attribute));
    for (size_t i = 0; i < array->len; i++)
        attributes[i] = convert_attribute(array->pdata[i]);
    return attributes;
}

Schema *convert_schema(I_Schema *i_schema) {
    gchar *name;
    GPtrArray *array;

    Schema *schema = schema_init();
    g_object_get(i_schema,
                 "name", &name,
                 "attributes", &array,
                 NULL);
    String *string = string_init();
    string->size = strlen(name);
    string->content = g_strdup(name);

    schema->name = string;
    schema->attr_count = array->len;
    schema->attributes = convert_attributes(array);
    return schema;
}

SchemaCreateQuery convert_schema_create_query(I_SchemaCreateQuery *query) {
    I_Schema *schema;
    g_object_get(query,
                 "schema", &schema,
                 NULL);
    return (SchemaCreateQuery){.schema = convert_schema(schema)};
}

Query convert_query(I_Query *query) {
    I_uQuery *u_query;
    g_object_get(query,
                 "query", &u_query,
                 NULL);

    I_SchemaCreateQuery *schema_create_query;

    Query result = {0};
    g_object_get(query,
                 "type", &result.type,
                 NULL);
    switch (result.type) {
        case SCHEMA_CREATE:
            g_object_get(u_query,
                         "schema_create_query", &schema_create_query,
                         NULL);
            result.schema_create_query = convert_schema_create_query(schema_create_query);
            break;
        case SCHEMA_GET:
            break;
        case SCHEMA_GET_ALL:
            break;
        case SCHEMA_DELETE:
            break;
        case NODE_CREATE:
            break;
        case NODE_UPDATE:
            break;
        case NODE_DELETE:
            break;
        case NODE_MATCH:
            break;
        case LINK_CREATE:
            break;
        case LINK_UPDATE:
            break;
        case LINK_DELETE:
            break;
        case MATCH:
            break;
    }
    return result;
}