##########################################################################################
# Copyright (c) 2022 Logan Barnes - All Rights Reserved
##########################################################################################

function(ltb_add_executable target)
    add_executable(
            ${target}
            ${ARGN}
    )
    target_link_libraries(
            ${target}
            PRIVATE
            doctest::doctest
            $<TARGET_NAME_IF_EXISTS:ltb-dev-settings>
    )
    target_compile_definitions(
            ${target}
            PRIVATE
            DOCTEST_CONFIG_DISABLE
    )

    if (BUILD_TESTING)
        add_executable(
                test-${target}
                ${ARGN}
        )
        target_link_libraries(
                test-${target}
                PRIVATE
                doctest::doctest
                $<TARGET_NAME_IF_EXISTS:ltb-dev-settings>
        )
        add_test(
                NAME test-${target}
                COMMAND $<TARGET_FILE:test-${target}>
        )
    endif ()
endfunction()

function(ltb_add_library target)
    add_library(
            ${target}
            ${ARGN}
    )
    target_link_libraries(
            ${target}
            PRIVATE
            doctest::doctest
            $<TARGET_NAME_IF_EXISTS:ltb-dev-settings>
    )
    target_compile_definitions(
            ${target}
            PRIVATE DOCTEST_CONFIG_DISABLE
    )

    if (BUILD_TESTING)
        add_executable(
                test-${target}
                ${ARGN}
                ${LTB_TEST_MAIN}
        )
        target_link_libraries(
                test-${target}
                PRIVATE
                $<$<BOOL:${LTB_TEST_MAIN}>:doctest::doctest>
                $<$<NOT:$<BOOL:${LTB_TEST_MAIN}>>:doctest_with_main>
                $<TARGET_NAME_IF_EXISTS:ltb-dev-settings>
        )
        add_test(
                NAME test-${target}
                COMMAND $<TARGET_FILE:test-${target}>
        )
    endif ()
endfunction()

function(ltb_target_compile_definitions target)
    target_compile_definitions(${target} ${ARGN})
    if (TARGET test-${target})
        target_compile_definitions(test-${target} ${ARGN})
    endif ()
endfunction()

function(ltb_target_compile_features target)
    target_compile_features(${target} ${ARGN})
    if (TARGET test-${target})
        target_compile_features(test-${target} ${ARGN})
    endif ()
endfunction()

function(ltb_target_compile_options target)
    target_compile_options(${target} ${ARGN})
    if (TARGET test-${target})
        target_compile_options(test-${target} ${ARGN})
    endif ()
endfunction()

function(ltb_target_include_directories target)
    target_include_directories(${target} ${ARGN})
    if (TARGET test-${target})
        target_include_directories(test-${target} ${ARGN})
    endif ()
endfunction()

function(ltb_target_link_libraries target)
    target_link_libraries(${target} ${ARGN})
    if (TARGET test-${target})
        target_link_libraries(test-${target} ${ARGN})
    endif ()
endfunction()

function(ltb_target_sources target)
    target_sources(${target} ${ARGN})
    if (TARGET test-${target})
        target_sources(test-${target} ${ARGN})
    endif ()
endfunction()
