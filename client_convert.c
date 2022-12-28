#include "client_convert.h"

I_Attribute *convert_attribute(Attribute *attribute) {
    I_Attribute *result = g_object_new(TYPE_I__ATTRIBUTE,
                                       "name", g_strdup(attribute->name),
                                       "vt", attribute->vt,
                                       NULL);
    return result;
}

GPtrArray *convert_attributes(Attribute *attributes) {
    GPtrArray *array = g_ptr_array_new();
    while (attributes) {
        g_ptr_array_add(array, convert_attribute(attributes));
        attributes = attributes->next;
    }
    return array;
}

I_Schema *convert_schema(Schema *schema) {
    I_Schema *result = g_object_new(TYPE_I__SCHEMA,
                                    "name", g_strdup(schema->name),
                                    "attributes", convert_attributes(schema->attributes),
                                    NULL);
    return result;
}

I_SchemaCreateQuery *convert_schema_create_query(SchemaCreateQuery query) {
    I_SchemaCreateQuery *result = g_object_new(
            TYPE_I__SCHEMA_CREATE_QUERY,
            "schema", convert_schema(query.schema),
            NULL);
    return result;
}

I_Query *convert_query(Query *query) {
    I_Query *result = g_object_new(TYPE_I__QUERY, NULL);
    I_uQuery *u_query = g_object_new(TYPE_I_U_QUERY, NULL);
    g_object_set(
            result,
            "type", query->type,
            "query", u_query,
            NULL);

    switch (query->type) {
        case SCHEMA_CREATE:
            g_object_set(u_query,
                         "schema_create_query",
                         convert_schema_create_query(query->schema_create_query),
                         NULL);
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
        case LINK_DELETE:
            break;
        case MATCH:
            break;
    }
    return result;
}
