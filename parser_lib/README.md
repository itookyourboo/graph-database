# Низкоуровневое программирование. ЛР №2

## Задание

Использовать средство синтаксического анализа по выбору, реализовать модуль для разбора некоторого
достаточного подмножества языка запросов **Gremlin**. Должна быть обеспечена возможность описания команд 
создания, выборки, модификации и удаления элементов данных.

## Установка и запуск

### Необходимые реквизиты

- gcc
- flex
- bison

### Запуск

```shell
$ make
$ ./parser
```

## Структура проекта

Лексический анализ (`lexer.l`) происходит с помощью `flex`. Символы превращаются в токены. 
Некоторые токены преобразуются в `enum`'ы или `Value`:  

```c
typedef enum {
    CMP_GT,
    CMP_LT,
    CMP_GE,
    CMP_LE,
    CMP_EQ,
    CMP_NEQ,
    CMP_CONTAINS
} Comparation, CMP;

typedef enum {
    LT_TO,
    LT_FROM,
    LT_BOTH,

    LT_ANY,
} LinkType;

typedef enum {
    VT_INTEGER,
    VT_STRING,
    VT_FLOATING,
    VT_BOOLEAN
} ValueType;

typedef struct {
    ValueType vt;
    union {
        int integer;
        char *string;
        float floating;
        bool boolean;
    };
} Value;
```

Синтаксический анализатор (`parser.y`), написанный на `bison`, принимает токены 
и строит синтаксическое дерево с помощью вспомогательных структур данных, 
описанных в `db_model.h` и `query.h`.

В конце синтаксического анализа все сводится к структуре `Query`.

```c
typedef struct {
    QueryType type;
    union {
        SchemaCreateQuery schema_create_query;
        SchemaGetQuery schema_get_query;
        SchemaDeleteQuery schema_delete_query;

        NodeCreateQuery node_create_query;
        NodeUpdateQuery node_update_query;
        NodeDeleteQuery node_delete_query;
        NodeMatchQuery node_match_query;

        LinkCreateQuery link_create_query;
        LinkDeleteQuery link_delete_query;

        MatchQuery match_query;
    };
} Query;
```

В `db_model.h` описаны стержневые структуры самой ДБ

```c
typedef struct Property {
    char *field;
    Value *value;
    struct Property *next;
} Property;

typedef struct Attribute {
    char *name;
    ValueType vt;
    struct Attribute *next;
} Attribute;

typedef struct {
    char *name;
    Attribute *attributes;
} Schema;

typedef struct {
    char *name;
    Property *properties;
} Node;

typedef struct {
    char *name;
    LinkType type;
    char *first;
    char *second;
} Link;
```

В `query.h` описаны вспомогательные структуры для построения дерева п
ри синтаксическом анализе, например, `Predicate` (обработка логических выражений):

```c
typedef enum {
    PT_TERM,
    PT_OR,
    PT_AND
} PredicateType;

typedef struct Predicate {
    PredicateType type;
    union {
        ConditionCompare *term;
        struct Predicate *and;
        struct Predicate *or;
    };
    struct Predicate *next;
} Predicate;
```

Для вывода дерева используется `printer`, принимающий на вход структуру `Query`. 

## Запросы

### Создание схемы

```
> g.addS("students")
    .propertyKey("id").Int()
    .propertyKey("name").Text()
    .propertyKey("score").Float()
    .propertyKey("will_be_expelled").Bool();
```
```
{
    query_type: SCHEMA_CREATE,
    result_type: RESULT_NONE,
    query: {
        schema: {
            name: students,
            attrs: [
                {
                    name: id,
                    type: INTEGER
                },
                {
                    name: name,
                    type: STRING
                },
                {
                    name: score,
                    type: FLOATING
                },
                {
                    name: will_be_expelled,
                    type: BOOLEAN
                }
            ]
        }
    }
}
```

### Получение схемы

```
> g.S("students");
```
```
{
    query_type: SCHEMA_GET,
    result_type: RESULT_SCHEMA,
    query: {
        schema_name: students
    }
}
```

### Удаление схемы

```
> g.S("students").drop();
```
```
{
    query_type: SCHEMA_DELETE,
    result_type: RESULT_NONE,
    query: {
        schema_name: students
    }
}
```

### Создание вершины

```
> g.addV("students")
    .property("id", 1)
    .property("name", "Alex")
    .property("score", 4.8)
    .property("will_be_expelled", false)
    .as("alex");
```
```
{
    query_type: NODE_CREATE,
    result_type: RESULT_NONE,
    query: {
        schema_name: students,
        node: {
            name: alex,
            props: [
                {
                    name: will_be_expelled,
                    value: {
                        type: BOOLEAN,
                        value: false
                    }
                },
                {
                    name: score,
                    value: {
                        type: FLOATING,
                        value: 4.800000
                    }
                },
                {
                    name: name,
                    value: {
                        type: STRING,
                        value: Alex
                    }
                },
                {
                    name: id,
                    value: {
                        type: INTEGER,
                        value: 1
                    }
                }
            ]
        }
    }
}
```

### Создание ребра

```
> g.addE("LOVES").from("alex", "boris"); 
```

```
{
    query_type: LINK_CREATE,
    result_type: RESULT_NONE,
    query: {
        link: {
            name: LOVES,
            type: LT_FROM,
            first: alex,
            second: boris,
        }
    }
}
```

```
g.addE("FRIENDS").both("alex", "boris");
```

```
{
    query_type: LINK_CREATE,
    result_type: RESULT_NONE,
    query: {
        link: {
            name: FRIENDS,
            type: LT_BOTH,
            first: alex,
            second: boris,
        }
    }
}
```

### Удаление ребра

```
> g
    .V("people").has("id", ge(1921))
    .in("LIKES")
    .V("people").has("age", lt(10))
    .dropE();
```

```
{
    query_type: LINK_DELETE,
    result_type: RESULT_NONE,
    query: {
        match_query: {
            node_first_condition: {
                is_null: false,
                schema: people,
                predicates: [
                    {
                        type: PT_TERM,
                        term: {
                            left: {
                                type: CMPT_FIELD,
                                field_name: id
                            },
                            compare: {
                                cmp: CMP_GE,
                                with: {
                                    type: CMPT_VALUE,
                                    value: {
                                        type: INTEGER,
                                        value: 1921
                                    }
                                }
                            }
                        }
                    }
                ]
            },
            node_second_condition: {
                is_null: false,
                schema: people,
                predicates: [
                    {
                        type: PT_TERM,
                        term: {
                            left: {
                                type: CMPT_FIELD,
                                field_name: age
                            },
                            compare: {
                                cmp: CMP_LT,
                                with: {
                                    type: CMPT_VALUE,
                                    value: {
                                        type: INTEGER,
                                        value: 10
                                    }
                                }
                            }
                        }
                    }
                ]
            },
            node_cross_condition: (null),
            link_condition: {
                is_null: false,
                link_type: LT_FROM,
                link_name: LIKES
            }
        }
    }
}
```

### Получение вершин

#### Запрос с простым условием

```
> g.V("students").has("score", gt(4.5));
```

```
{
    query_type: NODE_MATCH,
    result_type: RESULT_GENERATOR,
    query: {
        node_condition: {
            is_null: false,
            schema: students,
            predicates: [
                {
                    type: PT_TERM,
                    term: {
                        left: {
                            type: CMPT_FIELD,
                            field_name: 
                        },
                        compare: {
                            cmp: CMP_GT,
                            with: {
                                type: CMPT_VALUE,
                                value: {
                                    type: FLOATING,
                                    value: 4.500000
                                }
                            }
                        }
                    }
                }
            ]
        }
    }
}
```

#### Запрос со сложным условием

```
> g.V("students").and(
    has("score", gt(4.5)),
    or(
        has("will_be_expelled", eq(true)),
        has("name", contains("alex"))
    )
);
```

```
{
    query_type: NODE_MATCH,
    result_type: RESULT_GENERATOR,
    query: {
        node_condition: {
            is_null: false,
            schema: students,
            predicates: [
                {
                    type: PT_AND,
                    and: [
                        {
                            type: PT_TERM,
                            term: {
                                left: {
                                    type: CMPT_FIELD,
                                    field_name: 
                                },
                                compare: {
                                    cmp: CMP_GT,
                                    with: {
                                        type: CMPT_VALUE,
                                        value: {
                                            type: FLOATING,
                                            value: 4.500000
                                        }
                                    }
                                }
                            }
                        },
                        {
                            type: PT_OR,
                            or: [
                                {
                                    type: PT_TERM,
                                    term: {
                                        left: {
                                            type: CMPT_FIELD,
                                            field_name: 
                                        },
                                        compare: {
                                            cmp: CMP_EQ,
                                            with: {
                                                type: CMPT_VALUE,
                                                value: {
                                                    type: BOOLEAN,
                                                    value: true
                                                }
                                            }
                                        }
                                    }
                                },
                                {
                                    type: PT_TERM,
                                    term: {
                                        left: {
                                            type: CMPT_FIELD,
                                            field_name: 
                                        },
                                        compare: {
                                            cmp: CMP_CONTAINS,
                                            with: {
                                                type: CMPT_VALUE,
                                                value: {
                                                    type: STRING,
                                                    value: alex
                                                }
                                            }
                                        }
                                    }
                                }
                            ]
                        }
                    ]
                }
            ]
        }
    }
}
```

### Удаление вершин

```
> g.V("students").has("score", gt(4.5)).drop();
```

```
{
    query_type: NODE_DELETE,
    result_type: RESULT_NONE,
    query: {
        node_condition: {
            is_null: false,
            schema: students,
            predicates: [
                {
                    type: PT_TERM,
                    term: {
                        left: {
                            type: CMPT_FIELD,
                            field_name: 
                        },
                        compare: {
                            cmp: CMP_GT,
                            with: {
                                type: CMPT_VALUE,
                                value: {
                                    type: FLOATING,
                                    value: 4.500000
                                }
                            }
                        }
                    }
                }
            ]
        }
    }
}
```

### Модификация вершин

```
> g.V("students").has("score", gt(4.5))
    .property("score", 5.0)
    .property("will_be_expelled", false);
```

```
{
    query_type: NODE_UPDATE,
    result_type: RESULT_NONE,
    query: {
        node_condition: {
            is_null: false,
            schema: students,
            predicates: [
                {
                    type: PT_TERM,
                    term: {
                        left: {
                            type: CMPT_FIELD,
                            field_name: 
                        },
                        compare: {
                            cmp: CMP_GT,
                            with: {
                                type: CMPT_VALUE,
                                value: {
                                    type: FLOATING,
                                    value: 4.500000
                                }
                            }
                        }
                    }
                }
            ]
        },
        properties: [
            {
                name: score,
                value: {
                    type: FLOATING,
                    value: 5.000000
                }
            },
            {
                name: will_be_expelled,
                value: {
                    type: BOOLEAN,
                    value: false
                }
            }
        ]
    }
}
```

### Полный поиск

```
> g
    .V("s1").has("f1", ge(1))
    .in("LINK")
    .V("s2").has("f2", lt(2))
    .where(
        has("s1.f1", gt("s2.f2"))
    );
```

```
{
    query_type: MATCH,
    result_type: RESULT_GENERATOR,
    query: {
        match_query: {
            node_first_condition: {
                is_null: false,
                schema: s1,
                predicates: [
                    {
                        type: PT_TERM,
                        term: {
                            left: {
                                type: CMPT_FIELD,
                                field_name: f1
                            },
                            compare: {
                                cmp: CMP_GE,
                                with: {
                                    type: CMPT_VALUE,
                                    value: {
                                        type: INTEGER,
                                        value: 1
                                    }
                                }
                            }
                        }
                    }
                ]
            },
            node_second_condition: {
                is_null: false,
                schema: s2,
                predicates: [
                    {
                        type: PT_TERM,
                        term: {
                            left: {
                                type: CMPT_FIELD,
                                field_name: f2
                            },
                            compare: {
                                cmp: CMP_LT,
                                with: {
                                    type: CMPT_VALUE,
                                    value: {
                                        type: INTEGER,
                                        value: 2
                                    }
                                }
                            }
                        }
                    }
                ]
            },
            node_cross_condition: {
                is_null: false,
                schema: (null),
                predicates: [
                    {
                        type: PT_TERM,
                        term: {
                            left: {
                                type: CMPT_FIELD,
                                field_name: s1.f1
                            },
                            compare: {
                                cmp: CMP_GT,
                                with: {
                                    type: CMPT_VALUE,
                                    value: {
                                        type: STRING,
                                        value: s2.f2
                                    }
                                }
                            }
                        }
                    }
                ]
            },
            link_condition: {
                is_null: false,
                link_type: LT_FROM,
                link_name: LINK
            }
        }
    }
}
```

### Вывод синтаксической ошибки

```
> g.V("schema").has("age", 5).drop();
```
```
error: syntax error in line 2, column 26
g.V("schema").has("age", 5).drop();
_________________________^
```

```
> g.V("Schema").has("age", almost_equal(5));
```
```
Unknown token: almost_equal
error: syntax error in line 1, column 38
g.V("Schema").has("age", almost_equal(5));
_____________________________________^
```

### Потребление оперативной памяти

После исполнения запроса дерево удаляется из оперативной памяти с помощью `free_query`, который освобождает вложенные структуры, например `free_node` или `free_predicate`.

```shell
$ query='g.V("students").has("score", gt(4.5)).property("score", 5.0).property("will_be_expelled", false);\n'
$ pid=`pidof ./parser`
$ pmap $pid | grep total
 total             2816K
$ echo -e $query > /proc/$pid/fd/0
$ pmap $pid | grep total
 total             2816K
$ echo -e $query > /proc/$pid/fd/0
$ pmap $pid | grep total
 total             2816K
$ echo -e $query > /proc/$pid/fd/0
$ pmap $pid | grep total
 total             2816K
```

## Выводы

В процессе работы я познакомился с такими мощными инструментами как `flex` и `bison`.
Столкнулся с немалым количеством сложностей, среди которых не совсем понятная структура языка запросов `Gremlin`,
а также различные тонкости реализации грамматики (привет shift/reduce conflict).

Было трудно, но интересно :)