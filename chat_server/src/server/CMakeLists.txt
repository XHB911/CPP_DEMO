aux_source_directory(. SRC_LIST)
aux_source_directory(./db DB_LIST)
aux_source_directory(./model MODEL_LIST)

add_executable(chat_server ${SRC_LIST} ${DB_LIST} ${MODEL_LIST})

target_link_libraries(chat_server muduo_net muduo_base mysqlclient pthread)