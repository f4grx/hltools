include_directories(${newhpsdr_SOURCE_DIR}/include)
add_library(newhpsdr STATIC p1dev.c)

add_executable(hpsdr_enum hpsdr_enum.c)
target_link_libraries(hpsdr_enum newhpsdr)

add_executable(hpsdr_dummyrx hpsdr_dummyrx.c)
target_link_libraries(hpsdr_dummyrx newhpsdr)

install(TARGETS newhpsdr hpsdr_enum
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib)

install(DIRECTORY ${newhpsdr_SOURCE_DIR}/include/
	DESTINATION include
	FILES_MATCHING PATTERN "*.h")

