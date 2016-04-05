
set(UNIT_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/alpha.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/convert.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/packer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/rgb.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/unit.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/unpack.cpp
    )

add_executable(tests ${UNIT_SOURCES} ${LIBRARY_SOURCES})
target_link_libraries(tests gtest)
