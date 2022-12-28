%{
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <malloc.h>
#include "db_model.h"
#include "query.h"
#include "printer.h"

#ifdef YYDEBUG
    yydebug = 1;
#endif

#define YYERROR_VERBOSE 1

extern int yylineno;
extern int column;
extern char *lineptr;

void yyerror(Query *q, const char *s);
%}

%union {
    int integer;
    float floating;
    char *string;
    bool boolean;

    Value *value;

    ValueType vt;
    CMP cmp;

    Predicate *predicate;
    PredicateType pt;

    Comparable *comparable;
    Compare *compare;
    ConditionCompare *condition_cmp;

    LinkType link_type;
    Link *link;

    Attribute *attribute;
    Property *property;
    Node *node;

    NodeCondition *condition_node;
    LinkCondition *condition_link;
    MatchCondition *condition_match;

    Query *query;
}

%token FLOATING INTEGER BOOLEAN STRING
%token ID DOT COMMA
%token L R

%token g
%token V E S
%token has where
%token and or
%token gt lt ge le eq neq contains
%token in out
%token addV property as addE both from to
%token addS propertyKey Text Int Float Bool
%token drop dropE
%token COL

%type <floating> FLOATING;
%type <integer> INTEGER;
%type <string> STRING
%type <boolean> BOOLEAN;

%type <value> term

%type <condition_cmp> f_has
%type <predicate> f_and f_or cond_term cond_terms

%type <compare> f_cmp
%type <cmp> _cmp gt lt ge le eq neq contains
%type <comparable> _cmp_value_arg _cmp_string_arg _has_field_arg

%type <string> f_propertyKey f_as f_addV f_addE f_addS

%type <vt> property_type Text Int Float Bool
%type <link_type> both to from _link_dir
%type <link_type> in out
%type <link> _link_init

%type <property> f_add_property f_add_properties
%type <attribute> f_add_property_key f_add_property_keys

%type <query> create_schema get_schema drop_schema get_all_schemas
%type <query> create_node update_nodes delete_nodes match_nodes
%type <query> create_link delete_link

%type <condition_node> match_nodes_condition match_cross_condition match_nodes_condition_dot
%type <condition_node> match_nodes_condition_with_schema match_nodes_init
%type <condition_link> match_link_condition
%type <condition_match> _match_cond _match_cond_dot _match_cond_link_second_cross _match_cond_link_second _match_cond_link

%type <query> _schemas _nodes _links _match
%type <query> _query query input

%parse-param { Query *q }

%start input

%%

input
: query
| input query
;

query
: _query COL {
    *q = *$1;
    $$ = q;
}
;

_query
: _schemas
| _nodes
| _links
| _match
;

_schemas
: create_schema
| get_schema
| get_all_schemas
| drop_schema
;

_nodes
: create_node
| delete_nodes
| update_nodes
| match_nodes
;

_links
: create_link
| delete_link
;

create_link
: g DOT f_addE DOT _link_init {
    $5->name = $3;

    Query *q = create_query(LINK_CREATE);
    q->link_create_query = (LinkCreateQuery) { $5 };
    $$ = q;
}
;

delete_link
: _match_cond_dot dropE L R {
    Query *q = create_query(LINK_DELETE);
    q->link_delete_query = (LinkDeleteQuery) { $1 };
    $$ = q;
}
;

_match
: _match_cond {
    Query *q = create_query(MATCH);
    q->match_query = (MatchQuery) { $1 };
    $$ = q;
};

get_schema
: g DOT S L STRING R {
    Query *q = create_query(SCHEMA_GET);
    q->schema_get_query = (SchemaGetQuery) { $5 };
    $$ = q;
}
;

get_all_schemas
: g DOT S L R {
    $$ = create_query(SCHEMA_GET_ALL);
}
;

create_schema
: g DOT f_addS DOT f_add_property_keys {
    Schema *schema = create_schema();
    schema->name = $3;
    schema->attributes = $5;

    Query *q = create_query(SCHEMA_CREATE);
    q->schema_create_query = (SchemaCreateQuery) { schema };
    $$ = q;
}
;

drop_schema
: g DOT S L STRING R DOT f_drop {
    Query *q = create_query(SCHEMA_DELETE);
    q->schema_delete_query = (SchemaDeleteQuery) { $5 };
    $$ = q;
}
;

f_add_property_keys
: f_add_property_key DOT f_add_property_keys {
    $1->next = $3;
    $$ = $1;
}
| f_add_property_key
;

f_add_property_key
: f_propertyKey DOT property_type L R {
    Attribute *attr = create_attribute();
    attr->name = $1;
    attr->vt = $3;
    $$ = attr;
}
;

property_type
: Text
| Int
| Float
| Bool
;

create_node
: g DOT f_addV DOT f_add_properties DOT f_as {
    Node *n = create_node();
    n->name = $7;
    n->properties = $5;

    Query *q = create_query(NODE_CREATE);
    q->node_create_query = (NodeCreateQuery) { $3, n };
    $$ = q;
}
| g DOT f_addV DOT f_add_properties {
    Node *n = create_node();
    n->properties = $5;

    Query *q = create_query(NODE_CREATE);
    q->node_create_query = (NodeCreateQuery) { $3, n };
    $$ = q;
}
;

f_add_property
: property L STRING COMMA term R {
    Property *p = create_property();
    p->field = $3;
    p->value = $5;
    $$ = p;
}
;

f_add_properties
: f_add_property
| f_add_properties DOT f_add_property {
    $3->next = $1;
    $$ = $3;
}
;

_match_cond_link
: match_nodes_condition_dot match_link_condition {
    MatchCondition *c = create_match_condition();
    c->node_first_condition = $1;
    c->link_condition = $2;
    $$ = c;
}
;

_match_cond_link_second
: _match_cond_link DOT match_nodes_condition_with_schema {
    $1->node_second_condition = $3;
    $$ = $1;
}
;

_match_cond_link_second_cross
: _match_cond_link_second DOT match_cross_condition {
    $1->node_cross_condition = $3;
    $$ = $1;
}
;

_match_cond
: _match_cond_link_second_cross
| _match_cond_link_second
| _match_cond_link
;

_match_cond_dot
: _match_cond_link_second_cross DOT
| _match_cond_link_second DOT
| _match_cond_link DOT
;

match_link_condition
: _link_dir L STRING R {
    LinkCondition *c = create_link_condition();
    c->link_type = $1;
    c->link_name = $3;
    $$ = c;
}
;

match_cross_condition
: where L cond_terms R {
    NodeCondition *n = create_node_condition();
    n->predicates = $3;
    $$ = n;
}
;

match_nodes_condition_with_schema
: V L STRING R DOT cond_term {
    NodeCondition *n = create_node_condition();
    n->schema = $3;
    n->predicates = $6;
    $$ = n;
}
;

match_nodes_condition
: match_nodes_init DOT cond_term {
    $1->predicates = $3;
    $$ = $1;
}
| match_nodes_init
;

match_nodes_condition_dot
: match_nodes_init DOT cond_term DOT {
    $1->predicates = $3;
    $$ = $1;
}
| match_nodes_init DOT
;

match_nodes_init
: g DOT V L STRING R {
    NodeCondition *n = create_node_condition();
    n->is_null = false;
    n->schema = $5;
    $$ = n;
}
| g DOT V L R {
    NodeCondition *n = create_node_condition();
    n->is_null = true;
    $$ = n;
}
;

match_nodes
: match_nodes_condition {
    Query *q = create_query(NODE_MATCH);
    q->node_match_query = (NodeMatchQuery) {$1};
    $$ = q;
}
;

delete_nodes
: match_nodes_condition_dot f_drop {
    Query *q = create_query(NODE_DELETE);
    q->node_delete_query = (NodeDeleteQuery) {$1};
    $$ = q;
}
;

update_nodes
: match_nodes_condition_dot f_add_properties {
    Query *q = create_query(NODE_UPDATE);
    q->node_update_query = (NodeUpdateQuery) {$1, $2};
    $$ = q;
}
;

term
: INTEGER {
    $$ = create_integer($1);
}
| BOOLEAN {
    $$ = create_boolean($1);
}
| FLOATING {
    $$ = create_floating($1);
}
| STRING {
    $$ = create_string($1);
}
;

cond_term
: f_has {
    Predicate *p = create_predicate(PT_TERM);
    p->term = $1;
    $$ = p;
}
| f_and {
    Predicate *p = create_predicate(PT_AND);
    p->and = $1;
    $$ = p;
}
| f_or {
    Predicate *p = create_predicate(PT_OR);
    p->or = $1;
    $$ = p;
}
;

cond_terms
: cond_term
| cond_terms COMMA cond_term {
    $1->next = $3;
    $$ = $1;
}
;

_has_field_arg
: STRING {
    Comparable *c = create_comparable(CMPT_FIELD);
    c->field_name = $1;
    $$ = c;
}
;

f_has
: has L _has_field_arg COMMA f_cmp R {
    $$ = create_condition($3, $5);
}
;

f_and
: and L cond_terms R {
    $$ = $3;
}
;

f_or
: or L cond_terms R {
    $$ = $3;
}
;

_cmp_value_arg
: L term R {
    Comparable *c = create_comparable(CMPT_VALUE);
    c->value = $2;
    $$ = c;
}
;

_cmp_string_arg
: L STRING R {
    Comparable *c = create_comparable(CMPT_VALUE);
    c->value = create_string($2);
    $$ = c;
}
;

_cmp
: gt
| lt
| ge
| le
| eq
| neq
;

f_cmp
: _cmp _cmp_value_arg {
    $$ = create_compare($1, $2);
}
| contains _cmp_string_arg {
    $$ = create_compare($1, $2);
}
;

f_addS
: addS L STRING R {
    $$ = $3;
}
;

f_propertyKey
: propertyKey L STRING R {
    $$ = $3;
}
;

f_as
: as L STRING R {
    $$ = $3;
}
;

f_addV
: addV L STRING R {
    $$ = $3;
}
;

f_addE
: addE L STRING R {
    $$ = $3;
}
;

_link_dir
: both
| to
| from
| in
| out
;

_link_init
: _link_dir L STRING COMMA STRING R {
    $$ = create_link($1, $3, $5);
}
;

f_drop
: drop L R
;

%%

void yyerror(Query *q, const char *str)
{
    fprintf(stderr, "error: %s in line %d, column %d\n", str, yylineno, column);
    fprintf(stderr, "%s", lineptr);
    for(int i = 0; i < column - 1; i++)
        fprintf(stderr, "_");
    fprintf(stderr, "^\n");
}
