# Creates unit test executable target and runs it.
# We require the target name to link against it, or depend on it building
function(add_unit_tests TARGET)

if(CATKIN_ENABLE_TESTING)
    # Add gtest based cpp test target and link libraries
    set(TEST_BINARY "unit_tests")
    if(EXISTS ${CMAKE_SOURCE_DIR}/test/${TEST_BINARY}.cpp)
        catkin_add_executable_with_gtest(${TEST_BINARY} test/${TEST_BINARY}.cpp)        
        # Only link target if it is a library
        get_target_property(target_type ${TARGET} TYPE)
        if(NOT target_type STREQUAL "EXECUTABLE")
            target_link_libraries(${TEST_BINARY} ${TARGET})
        endif()
        
        # Link to all libs that target links to
        get_target_property(TARGET_LIBS ${TARGET} LINK_LIBRARIES)
        if(TARGET_LIBS)
            target_link_libraries(${TEST_BINARY} ${TARGET_LIBS})
        endif()
        add_custom_target(test_${PROJECT_NAME} ALL DEPENDS ${TEST_BINARY} ${TARGET}
                          COMMAND ${CATKIN_DEVEL_PREFIX}/lib/${PROJECT_NAME}/unit_tests --gtest_output=xml:unit_test.xml 1>&2)
    else()
        message("Unable to find unit test source file. You should do something about that")
    endif()
endif()

endfunction()