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

    if (LTB_ENABLE_TESTING)
        add_executable(
                Test${target}
                ${ARGN}
        )
        target_link_libraries(
                Test${target}
                PRIVATE
                doctest::doctest
                $<TARGET_NAME_IF_EXISTS:ltb-dev-settings>
        )
        add_test(
                NAME Test${target}
                COMMAND $<TARGET_FILE:Test${target}>
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

    if (LTB_ENABLE_TESTING)
        add_executable(
                Test${target}
                ${ARGN}
                ${LTB_TEST_MAIN}
        )
        target_link_libraries(
                Test${target}
                PRIVATE
                $<$<BOOL:${LTB_TEST_MAIN}>:doctest::doctest>
                $<$<NOT:$<BOOL:${LTB_TEST_MAIN}>>:doctest_with_main>
                $<TARGET_NAME_IF_EXISTS:ltb-dev-settings>
        )
        add_test(
                NAME Test${target}
                COMMAND $<TARGET_FILE:Test${target}>
        )
    endif ()
endfunction()

function(ltb_target_compile_definitions target)
    target_compile_definitions(${target} ${ARGN})
    if (TARGET Test${target})
        target_compile_definitions(Test${target} ${ARGN})
    endif ()
endfunction()

function(ltb_target_compile_features target)
    target_compile_features(${target} ${ARGN})
    if (TARGET Test${target})
        target_compile_features(Test${target} ${ARGN})
    endif ()
endfunction()

function(ltb_target_compile_options target)
    target_compile_options(${target} ${ARGN})
    if (TARGET Test${target})
        target_compile_options(Test${target} ${ARGN})
    endif ()
endfunction()

function(ltb_target_include_directories target)
    target_include_directories(${target} ${ARGN})
    if (TARGET Test${target})
        target_include_directories(Test${target} ${ARGN})
    endif ()
endfunction()

function(ltb_target_link_libraries target)
    target_link_libraries(${target} ${ARGN})
    if (TARGET Test${target})
        target_link_libraries(Test${target} ${ARGN})
    endif ()
endfunction()

function(ltb_target_sources target)
    target_sources(${target} ${ARGN})
    if (TARGET Test${target})
        target_sources(Test${target} ${ARGN})
    endif ()
endfunction()
