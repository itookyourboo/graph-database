# Низкоуровневое программирование. ЛР №3

## Задание

На базе данного транспортного формата описать схему протокола обмена информацией и воспользоваться существующей библиотекой по выбору для реализации модуля, обеспечивающего его функционирование. Протокол должен включать представление информации о командах создания, выборки, модификации и удаления данных в соответствии с данной формой, и результатах их выполнения.

Используя созданные в результате выполнения заданий модули, разработать в виде консольного приложения две программы: клиентскую и серверную части. Серверная часть – получающая по сети запросы и операции описанного формата и последовательно выполняющая их над файлом данных с помощью модуля из первого задания. Имя фала данных для работы получать с аргументами командной строки, создавать новый в случае его отсутствия. Клиентская часть – в цикле получающая на стандартный ввод текст команд, извлекающая из него информацию о запрашиваемой операции с помощью модуля из второго задания и пересылающая её на сервер с помощью модуля для обмена информацией, получающая ответ и выводящая его в человеко-понятном виде в стандартный вывод.

### Вариант
Apache Thrift

## Установка и запуск

### Необходимые реквизиты

- gcc
- flex
- bison
- glib-2.0
- gobjects-2.0
- thrift & thrift_c_glib

### Сборка и запуск

```shell
$ export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib/
$ thrift --gen c_glib query.thrift
$ cmake --build cmake-build-debug -j 6
```

```shell
$ ./cmake-build-debug/server_exe
$ ./cmake-build-debug/client_exe
```

## Примеры запросов

### Схемы

#### Создание

```
> g.addS("students")
    .propertyKey("id").Int()
    .propertyKey("name").Text()
    .propertyKey("score").Float()
    .propertyKey("will_be_expelled").Bool();
Result code: 0 (RC_OK)
> g.addS("simple").propertyKey("id").Int();
Result code: 0 (RC_OK)
```

#### Получение всех

```
> g.S();
Result code: 0 (RC_OK)
0. Schema: students
- id: I__VALUE_TYPE_VT_INTEGER
- name: I__VALUE_TYPE_VT_STRING
- score: I__VALUE_TYPE_VT_FLOATING
- will_be_expelled: I__VALUE_TYPE_VT_BOOLEAN
1. Schema: simple
- id: I__VALUE_TYPE_VT_INTEGER
```

#### Получение схемы по имени

```
> g.S("students");
Result code: 0 (RC_OK)
Schema: students
- id: I__VALUE_TYPE_VT_INTEGER
- name: I__VALUE_TYPE_VT_STRING
- score: I__VALUE_TYPE_VT_FLOATING
- will_be_expelled: I__VALUE_TYPE_VT_BOOLEAN
```

#### Удаление схемы

```
> g.S("simple").drop();
Result code: 0 (RC_OK)
> g.S();
Result code: 0 (RC_OK)
0. Schema: students
- id: I__VALUE_TYPE_VT_INTEGER
- name: I__VALUE_TYPE_VT_STRING
- score: I__VALUE_TYPE_VT_FLOATING
- will_be_expelled: I__VALUE_TYPE_VT_BOOLEAN
```

### Вершины

#### Создание

```
> g.addV("students")
    .property("id", 1)
    .property("name", "Alex")
    .property("score", 4.8)
    .property("will_be_expelled", false);
Result code: 0 (RC_OK)
```

#### Получение

```
> g.V("students");
Result code: 0 (RC_OK)
0. [         1      Alex  4.800000     false]
1. [         2     Boris  4.900000     false]
2. [         3     Lyoha  3.100000      true]
3. [         4     Kolya  4.200000      true]
```

```
> g.V("students").has("score", gt(4));
Result code: 0 (RC_OK)
0. [         1      Alex  4.800000     false]
1. [         2     Boris  4.900000     false]
2. [         4     Kolya  4.200000      true]
> g.V("students").and(
    has("will_be_expelled", eq(true)),
    has("score", gt(4))
);
Result code: 0 (RC_OK)
0. [         4     Kolya  4.200000      true]
```

#### Обновление

```
> g.V("students")
    .has("score", gt(4))
    .property("score", 5);
> g.V("students");
Result code: 0 (RC_OK)
0. [         1      Alex  5.000000     false]
1. [         2     Boris  5.000000     false]
2. [         3     Lyoha  3.100000      true]
3. [         4     Kolya  5.000000      true]
```

#### Удаление

```
> g.V("students").has("will_be_expelled", eq(true));
Result code: 0 (RC_OK)
0. [         3     Lyoha  3.100000      true]
1. [         4     Kolya  5.000000      true]

> g.V("students").has("will_be_expelled", eq(true)).drop();
Result code: 0 (RC_OK)
> g.V("students");
Result code: 0 (RC_OK)
0. [         1      Alex  5.000000     false]
1. [         2     Boris  5.000000     false]
```

### Ребра

#### Создание

```
> g.addE("FRIENDS").both(
    g.V("students").has("id", eq(1)), 
    g.V("students").has("id", eq(2))
);
```

#### Получение

#### Удаление

