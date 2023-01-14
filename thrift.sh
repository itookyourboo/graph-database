thrift --gen c_glib -r ./query.thrift
sed -i -e 's/\"I__VALUE_TYPE_/\"/g' -e 's/\"I__QUERY_TYPE_/\"/g' ./gen-c_glib/query_types.c
