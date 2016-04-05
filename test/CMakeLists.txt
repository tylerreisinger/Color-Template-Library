set(TEST_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp 
    )

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/unit)

add_executable(color_test ${LIBRARY_SOURCES} ${TEST_SOURCES})
