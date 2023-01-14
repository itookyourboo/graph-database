cmake_minimum_required(VERSION 3.24)
project(low_level_programming_task3 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_COMPILER "gcc")

set(SERVER_EXE server_exe)
set(CLIENT_EXE client_exe)

set(LIB_DB db_lib)
set(LIB_DB_DIR db_lib)
set(LIB_PARSER parser_lib)
set(LIB_PARSER_DIR parser_lib)

set(THRIFT_LIB thrift_c_glib)
set(THRIFT_GEN gen-c_glib)

add_library(${LIB_DB} STATIC
        ${LIB_DB_DIR}/main.c
        ${LIB_DB_DIR}/main.h
        ${LIB_DB_DIR}/database/mem.h
        ${LIB_DB_DIR}/database/model/block_model.c
        ${LIB_DB_DIR}/database/model/block_model.h
        ${LIB_DB_DIR}/database/model/string_model.c
        ${LIB_DB_DIR}/database/model/string_model.h
        ${LIB_DB_DIR}/database/connection.h
        ${LIB_DB_DIR}/database/connection.c
        ${LIB_DB_DIR}/database/buffer_manager.h
        ${LIB_DB_DIR}/database/buffer_manager.c
        ${LIB_DB_DIR}/database/database_manager.h
        ${LIB_DB_DIR}/database/database_manager.c
        ${LIB_DB_DIR}/database/serialize/string_serialize.h
        ${LIB_DB_DIR}/database/serialize/string_serialize.c
        ${LIB_DB_DIR}/database/serialize/block_serialize.h
        ${LIB_DB_DIR}/database/serialize/block_serialize.c
        ${LIB_DB_DIR}/database/serialize/util.h
        ${LIB_DB_DIR}/database/visualize.h
        ${LIB_DB_DIR}/database/visualize.c
        ${LIB_DB_DIR}/database/model/schema_model.c
        ${LIB_DB_DIR}/database/model/schema_model.h
        ${LIB_DB_DIR}/database/model/attribute_model.c
        ${LIB_DB_DIR}/database/model/attribute_model.h
        ${LIB_DB_DIR}/database/model/data_type_model.h
        ${LIB_DB_DIR}/database/serialize/schema_serialize.c
        ${LIB_DB_DIR}/database/serialize/schema_serialize.h
        ${LIB_DB_DIR}/database/serialize/attribute_serialize.c
        ${LIB_DB_DIR}/database/serialize/attribute_serialize.h
        ${LIB_DB_DIR}/database/model/database_model.h
        ${LIB_DB_DIR}/database/model/database_model.c
        ${LIB_DB_DIR}/database/data_structure/block_heap.c
        ${LIB_DB_DIR}/database/data_structure/block_heap.h
        ${LIB_DB_DIR}/database/data_structure/block_node.c
        ${LIB_DB_DIR}/database/data_structure/block_node.h
        ${LIB_DB_DIR}/database/model/node_model.c
        ${LIB_DB_DIR}/database/model/node_model.h
        ${LIB_DB_DIR}/database/model/value_model.c
        ${LIB_DB_DIR}/database/model/value_model.h
        ${LIB_DB_DIR}/database/serialize/value_serialize.c
        ${LIB_DB_DIR}/database/serialize/value_serialize.h
        ${LIB_DB_DIR}/database/serialize/node_serialize.c
        ${LIB_DB_DIR}/database/serialize/node_serialize.h
        ${LIB_DB_DIR}/database/model/models.h
        ${LIB_DB_DIR}/database/serialize/serializers.h
        ${LIB_DB_DIR}/database/condition.h
        ${LIB_DB_DIR}/database/comparation.h
        ${LIB_DB_DIR}/database/serialize/util.c
        ${LIB_DB_DIR}/database/data_manipulation/crud_util.h
        ${LIB_DB_DIR}/database/data_manipulation/attribute_crud.c
        ${LIB_DB_DIR}/database/data_manipulation/attribute_crud.h
        ${LIB_DB_DIR}/database/data_manipulation/node_crud.c
        ${LIB_DB_DIR}/database/data_manipulation/node_crud.h
        ${LIB_DB_DIR}/database/data_manipulation/schema_crud.c
        ${LIB_DB_DIR}/database/data_manipulation/schema_crud.h
        ${LIB_DB_DIR}/database/data_manipulation/value_crud.c
        ${LIB_DB_DIR}/database/data_manipulation/value_crud.h
        ${LIB_DB_DIR}/database/data_manipulation/string_crud.c
        ${LIB_DB_DIR}/database/data_manipulation/string_crud.h
        ${LIB_DB_DIR}/database/data_manipulation/cruds.h
        ${LIB_DB_DIR}/database/model/link_model.c
        ${LIB_DB_DIR}/database/model/link_model.h
        ${LIB_DB_DIR}/database/serialize/link_serialize.c
        ${LIB_DB_DIR}/database/serialize/link_serialize.h
        ${LIB_DB_DIR}/database/data_manipulation/link_crud.c
        ${LIB_DB_DIR}/database/data_manipulation/link_crud.h
        ${LIB_DB_DIR}/database/query_manager.c
        ${LIB_DB_DIR}/database/query_manager.h
        ${LIB_DB_DIR}/database/model/result_model.h
        ${LIB_DB_DIR}/database/model/result_model.c
        ${LIB_DB_DIR}/database/data_structure/vector.h
        ${LIB_DB_DIR}/database/data_structure/generator.c
        ${LIB_DB_DIR}/database/data_structure/generator.h
        ${LIB_DB_DIR}/database/data_structure/vector_abstract.h
        ${LIB_DB_DIR}/database/data_structure/vector.c
        )

set(LIB_PARSER_OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}")

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

set(LEXER_OUT "${LIB_PARSER_OUT_DIR}/lex.yy.c")
set(PARSER_OUT "${LIB_PARSER_OUT_DIR}/parser.tab.c")

flex_target(LEXER "${LIB_PARSER_DIR}/lexer.l" ${LEXER_OUT} DEFINES_FILE "${LIB_PARSER_OUT_DIR}/lexer.h")
bison_target(PARSER "${LIB_PARSER_DIR}/parser.y" "${PARSER_OUT}" DEFINES_FILE "${LIB_PARSER_OUT_DIR}/parser.tab.h")
add_flex_bison_dependency(LEXER PARSER)

add_library(${LIB_PARSER} STATIC
        ${LIB_PARSER_DIR}/db_model.c
        ${LIB_PARSER_DIR}/db_model.h
        ${LIB_PARSER_DIR}/printer.c
        ${LIB_PARSER_DIR}/printer.h
        ${LIB_PARSER_DIR}/query.c
        ${LIB_PARSER_DIR}/query.h
        ${LIB_PARSER_DIR}/main.h
        ${LIB_PARSER_DIR}/main.c
        ${LEXER_OUT}
        ${PARSER_OUT}
        )
target_include_directories(${LIB_PARSER} PRIVATE ${LIB_PARSER_DIR} ${LIB_PARSER_OUT_DIR})


target_link_libraries(${LIB_DB} m)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED glib-2.0 gobject-2.0)

add_executable(${SERVER_EXE}
        server.c
        server_convert.c
        server_convert.h
        ${THRIFT_GEN}/query_svc.c
        ${THRIFT_GEN}/query_types.c
        ${THRIFT_GEN}/query_svc.h
        ${THRIFT_GEN}/query_types.h
        )
target_include_directories(${SERVER_EXE} PUBLIC ${GLIB_INCLUDE_DIRS})
target_link_libraries(${SERVER_EXE} PUBLIC ${LIB_DB} ${THRIFT_LIB} ${GLIB_LDFLAGS})

add_executable(${CLIENT_EXE}
        client.c
        client_convert.c
        client_convert.h
        ${THRIFT_GEN}/query_svc.c
        ${THRIFT_GEN}/query_types.c
        ${THRIFT_GEN}/query_svc.h
        ${THRIFT_GEN}/query_types.h
        )
target_include_directories(${CLIENT_EXE} PUBLIC ${GLIB_INCLUDE_DIRS})
target_link_libraries(${CLIENT_EXE} PUBLIC ${LIB_PARSER} ${THRIFT_LIB} ${GLIB_LDFLAGS})
