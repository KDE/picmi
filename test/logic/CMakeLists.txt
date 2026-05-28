include_directories(
    ${CMAKE_SOURCE_DIR}/src/logic
    ${CMAKE_SOURCE_DIR}
)

set(PICMI_LOGIC_TESTS
    streaks_test
    board_test
    boardmap_test
    boardstate_test
    elapsedtime_test
    picmi_test
    levelloader_test
)

foreach(t ${PICMI_LOGIC_TESTS})
    add_executable(${t} ${t}.cpp)
    add_test(NAME ${t} COMMAND ${t})
    ecm_mark_as_test(${t})
    target_link_libraries(${t}
        picmi_logic
        Qt6::Test
        Qt6::Core
    )
endforeach()

# streaks_test has a separate header; add it explicitly.
target_sources(streaks_test PRIVATE streaks_test.h)

# vim:set ts=4 sw=4 et:
