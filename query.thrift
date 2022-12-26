enum ValueType {
    VT_INTEGER
    VT_STRING
    VT_FLOATING
    VT_BOOLEAN
}

union uValue {
    1: i32 integer
    2: string string_
    3: double floating
    4: bool boolean
}

struct Value {
    1: ValueType type
    2: uValue value
}

struct Property {
    1: string field
    2: Value value
}

struct Attribute {
    1: string name
    2: ValueType vt
}

struct Schema {
    1: string name
    2: list<Attribute> attributes
}

struct Node {
    1: string name
    2: list<Property> properties
}

enum LinkType {
    LT_TO
    LT_FROM
    LT_BOTH

    LT_ANY
}

struct Link {
    1: string name
    2: LinkType type
    3: string first
    4: string second
}

enum Comparation {
    CMP_GT
    CMP_LT
    CMP_GE
    CMP_LE
    CMP_EQ
    CMP_NEQ
    CMP_CONTAINS
}

enum ComparableType {
    CMPT_FIELD
    CMPT_VALUE
}

union uComparable {
    1: string field_name
    2: Value value
}

struct Comparable {
    1: ComparableType type
    2: uComparable comparable
}

struct Compare {
    1: Comparation cmp
    2: Comparable with_
}

struct ConditionCompare {
    1: Comparable left
    2: Compare compare
}

enum PredicateType {
    PT_TERM
    PT_OR
    PT_AND
}

union uPredicate {
    1: ConditionCompare term
    2: list<Predicate> and_
    3: list<Predicate> or_
}

struct Predicate {
    1: PredicateType type
    2: uPredicate predicate
}

struct NodeCondition {
    1: bool is_null
    2: string schema
    3: list<Predicate> predicates
}

struct LinkCondition {
    1: bool is_null

    2: LinkType link_type
    3: string link_name
}

struct MatchCondition {
    1: bool is_null

    2: NodeCondition node_first_condition
    3: NodeCondition node_second_condition
    4: NodeCondition node_cross_condition
    5: LinkCondition link_condition
}

enum QueryType {
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

struct SchemaCreateQuery {
    1: Schema schema
} 

struct SchemaGetQuery {
    1: string schema_name
}

struct SchemaDeleteQuery {
    1: string schema_name
}

struct NodeCreateQuery {
    1: string schema_name
    2: Node node
}

struct NodeUpdateQuery {
    1: NodeCondition condition
    2: list<Property> properties
}

struct NodeDeleteQuery {
    1: NodeCondition condition
}

struct NodeMatchQuery {
    1: NodeCondition condition
}

struct LinkCreateQuery {
    1: Link link
}

struct LinkDeleteQuery {
    1: MatchCondition condition
}

struct MatchQuery {
    1: MatchCondition condition
}

union uQuery {
    1: SchemaCreateQuery schema_create_query
    2: SchemaGetQuery schema_get_query
    3: SchemaDeleteQuery schema_delete_query

    4: NodeCreateQuery node_create_query
    5: NodeUpdateQuery node_update_query
    6: NodeDeleteQuery node_delete_query
    7: NodeMatchQuery node_match_query

    8: LinkCreateQuery link_create_query
    9: LinkDeleteQuery link_delete_query

    10: MatchQuery match_query
}

struct Query {
    1: QueryType type
    2: uQuery query
}

enum Return {
    RETURN_NOTHING      = 0,
    RETURN_NODE_FIRST   = 1,
    RETURN_NODE_SECOND  = 2,
    RETURN_LINK         = 4,
    RETURN_ALL          = 7,

    RETURN_UNDEF        = 8,
}

struct Result {
    1: Return return_items
    2: optional Node first
    3: optional Link link
    4: optional Node second
}

enum QueryResultType {
    RESULT_SCHEMA
    RESULT_GENERATOR
    RESULT_NONE
}

union uQueryResult {
    1: list<Schema> schema
    2: list<Result> items
}

struct QueryResult {
    1: QueryResultType type;
    2: string message;
    3: optional uQueryResult result;
}


service querySvc {
   QueryResult execute(1: Query query)
}
