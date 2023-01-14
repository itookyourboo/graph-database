enum I_ValueType {
    VT_INTEGER
    VT_STRING
    VT_FLOATING
    VT_BOOLEAN
}

union I_uValue {
    1: i32 integer
    2: string string_
    3: double floating
    4: bool boolean
}

struct I_Value {
    1: I_ValueType type
    2: I_uValue value
}

struct I_Property {
    1: optional string field
    2: I_Value value
}

struct I_Attribute {
    1: string name
    2: I_ValueType vt
}

struct I_Schema {
    1: string name
    2: list<I_Attribute> attributes
}

struct I_Node {
    1: optional string name
    2: list<I_Property> properties
}

enum I_LinkType {
    LT_TO
    LT_FROM
    LT_BOTH

    LT_ANY
}

struct I_Link {
    1: string name
    2: I_LinkType type
}

enum I_Comparation {
    CMP_GT
    CMP_LT
    CMP_GE
    CMP_LE
    CMP_EQ
    CMP_NEQ
    CMP_CONTAINS
}

enum I_ComparableType {
    CMPT_FIELD
    CMPT_VALUE
}

union I_uComparable {
    1: string field_name
    2: I_Value value
}

struct I_Comparable {
    1: I_ComparableType type
    2: I_uComparable comparable
}

struct I_Compare {
    1: I_Comparation cmp
    2: I_Comparable with_
}

struct I_ConditionCompare {
    1: I_Comparable left
    2: I_Compare compare
}

enum I_PredicateType {
    PT_TERM
    PT_OR
    PT_AND
}

union I_uPredicate {
    1: I_ConditionCompare term
    2: list<I_Predicate> and_
    3: list<I_Predicate> or_
}

struct I_Predicate {
    1: I_PredicateType type
    2: I_uPredicate predicate
}

struct I_NodeCondition {
    1: bool is_null
    2: string schema
    3: list<I_Predicate> predicates
}

struct I_LinkCondition {
    1: bool is_null

    2: I_LinkType link_type
    3: string link_name
}

struct I_MatchCondition {
    1: bool is_null

    2: I_NodeCondition node_first_condition
    3: I_NodeCondition node_second_condition
    4: I_NodeCondition node_cross_condition
    5: I_LinkCondition link_condition
}

enum I_QueryType {
    SCHEMA_CREATE
    SCHEMA_GET
    SCHEMA_GET_ALL
    SCHEMA_DELETE

    NODE_CREATE
    NODE_UPDATE
    NODE_DELETE
    NODE_MATCH

    LINK_CREATE
    LINK_DELETE

    MATCH
}

struct I_SchemaCreateQuery {
    1: I_Schema schema
}

struct I_SchemaGetQuery {
    1: string schema_name
}

struct I_SchemaGetAllQuery {}

struct I_SchemaDeleteQuery {
    1: string schema_name
}

struct I_NodeCreateQuery {
    1: string schema_name
    2: I_Node node
}

struct I_NodeUpdateQuery {
    1: I_NodeCondition condition
    2: list<I_Property> properties
}

struct I_NodeDeleteQuery {
    1: I_NodeCondition condition
}

struct I_NodeMatchQuery {
    1: I_NodeCondition condition
}

struct I_LinkCreateQuery {
    1: string link_name
    2: I_LinkType link_type
    3: I_NodeCondition first
    4: I_NodeCondition second
}

struct I_LinkDeleteQuery {
    1: I_MatchCondition condition
}

struct I_MatchQuery {
    1: I_MatchCondition condition
}

union I_uQuery {
    1: I_SchemaCreateQuery schema_create_query
    2: I_SchemaGetQuery schema_get_query
    3: I_SchemaGetAllQuery schema_get_all_query;
    4: I_SchemaDeleteQuery schema_delete_query

    5: I_NodeCreateQuery node_create_query
    6: I_NodeUpdateQuery node_update_query
    7: I_NodeDeleteQuery node_delete_query
    8: I_NodeMatchQuery node_match_query

    9: I_LinkCreateQuery link_create_query
    10: I_LinkDeleteQuery link_delete_query

    11: I_MatchQuery match_query
}

struct I_Query {
    1: I_QueryType type
    2: I_uQuery query
}

enum I_Return {
    RETURN_NOTHING      = 0,
    RETURN_NODE_FIRST   = 1,
    RETURN_NODE_SECOND  = 2,
    RETURN_LINK         = 4,
    RETURN_ALL          = 7,

    RETURN_UNDEF        = 8,
}

struct I_Result {
    1: I_Return return_items
    2: optional I_Node first
    3: optional I_Link link
    4: optional I_Node second
    5: optional I_Schema schema
}

enum I_QueryResultType {
    RESULT_SCHEMA
    RESULT_GENERATOR
    RESULT_NONE
}

union I_uQueryResult {
    1: I_Schema schema
    2: list<I_Result> items
}

struct I_QueryResult {
    1: I_QueryResultType type;
    2: string message;
    3: optional I_uQueryResult result;
}


service querySvc {
   I_QueryResult execute(1: I_Query query)
}
