include_directories(
    ${CMAKE_SOURCE_DIR}/src/logic
    ${CMAKE_SOURCE_DIR}
)

add_executable(streaks_test streaks_test.cpp streaks_test.h)
add_test(NAME streaks_test COMMAND streaks_test)
ecm_mark_as_test(streaks_test)

target_link_libraries(streaks_test
    picmi_logic
    Qt6::Test
    Qt6::Core
)

# vim:set ts=4 sw=4 et:
