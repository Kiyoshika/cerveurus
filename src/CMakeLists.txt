add_library(cervparams Parameters.c)
target_include_directories(cervparams PUBLIC ${Cerveurus_SOURCE_DIR}/include)

add_library(cervresponse Response.c)
target_include_directories(cervresponse PUBLIC ${Cerveurus_SOURCE_DIR}/include)

add_library(cervroutes Routes.c)
target_include_directories(cervroutes PUBLIC ${Cerveurus_SOURCE_DIR}/include)

# throw all other libraries under libcerveurus
add_library(cerveurus HTTP_Server.c)
target_include_directories(cerveurus PUBLIC ${Cerveurus_SOURCE_DIR}/include)
target_link_libraries(cerveurus
	cervparams
	cervresponse
	cervroutes)


add_executable(main main.c)
target_include_directories(main PUBLIC ${Cerveurus_SOURCE_DIR}/include)
target_link_libraries(main cerveurus)

