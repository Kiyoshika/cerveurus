add_library(cervsarray SortedArray.c)
target_include_directories(cervsarray PUBLIC ${Cerveurus_SOURCE_DIR}/include)

add_library(cervresponse Response.c)
target_include_directories(cervresponse PUBLIC ${Cerveurus_SOURCE_DIR}/include)

add_library(cervroutes Routes.c)
target_include_directories(cervroutes PUBLIC ${Cerveurus_SOURCE_DIR}/include)

add_library(cervsql SQL.c)
target_include_directories(cervsql PUBLIC ${Cerveurus_SOURCE_DIR}/include)
target_link_libraries(cervsql cervresponse)

# throw all other libraries under libcerveurus
add_library(cerveurus HTTP_Server.c)
target_include_directories(cerveurus PUBLIC ${Cerveurus_SOURCE_DIR}/include)
target_link_libraries(cerveurus
	cervsarray
	cervresponse
	cervroutes
	cervsql)


add_executable(main main.c)
target_include_directories(main PUBLIC ${Cerveurus_SOURCE_DIR}/include)
target_link_libraries(main cerveurus)

