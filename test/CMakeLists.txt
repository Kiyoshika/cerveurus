add_executable(param_parser param_parser.c)
target_include_directories(param_parser PUBLIC ${Cerveurus_SOURCE_DIR}/include)
target_link_libraries(param_parser cerveurus)
add_test(NAME param_parser COMMAND param_parser)
