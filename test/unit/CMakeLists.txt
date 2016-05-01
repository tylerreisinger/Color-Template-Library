
set(UNIT_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Alpha.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Color.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ColorCast.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Hsl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Hsv.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Packer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Rgb.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/StreamPacker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ToHsv.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/unit.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Unpacker.cpp
    )

add_executable(tests ${UNIT_SOURCES} ${LIBRARY_SOURCES})
target_link_libraries(tests gtest)
