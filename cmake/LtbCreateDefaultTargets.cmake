# ##############################################################################
# Copyright (c) 2022 Logan Barnes - All Rights Reserved
# ##############################################################################

#[=======================================================================[.rst:
ltb_create_default_targets
----------------------------

Creates the necessary targets to build a library using doctest.

.. code-block:: cmake

  ltb_create_default_targets(name
                               source_files...
                               )

This function is a bit obscure but is needed to handle
the integration of the unit testing library, doctest.

This function will create 3 targets:

1) ``name`` - This is the main library and can be linked to other targets
   under the ALIAS ``name::name``.

2) ``name_obj`` - This contains the compiled objects and can be ignored.

3) ``name_deps`` - This is the INTERFACE library where dependencies should be
   linked. Dependencies include targets, directories, flags, etc.

If testing is enabled, a 4th target will also be created:

4) ``test_name`` - This is used to run all the doctest test cases and will
   require a main function to compile and run. If no special main function is
   required, ``doctest_with_main`` can be linked to satisfy this requirement.

#]=======================================================================]
function(ltb_create_default_targets name)
    # ##########################################################################
    # Dependencies
    # ##########################################################################
    add_library(${name}_deps INTERFACE)
    target_include_directories(${name}_deps
                               INTERFACE
                                   $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src>
                               )
    target_compile_features(${name}_deps INTERFACE cxx_std_17)
    target_compile_definitions(${name}_deps
                               # Deal with windows warnings and macros.
                               INTERFACE
                                   $<$<PLATFORM_ID:Windows>:_CRT_SECURE_NO_WARNINGS>
                                   $<$<PLATFORM_ID:Windows>:NOMINMAX>
                               )

    # ##########################################################################
    # Compiled Object Files
    # ##########################################################################

    # These object files are used to create both the target library and the
    # testing executable.
    add_library(${name}_objs OBJECT ${ARGN})
    target_link_libraries(${name}_objs
                          PUBLIC ${name}_deps
                          PRIVATE $<TARGET_NAME_IF_EXISTS:ltb_dev_settings>
                                  $<$<BOOL:${LTB_LINK_GCOV}>:gcov>
                          )
    target_compile_definitions(${name}_objs
                               # Remove all the test cases from the code when
                               # testing is disabled
                               PRIVATE
                                   $<$<NOT:$<BOOL:${LTB_ENABLE_TESTING}>>:DOCTEST_CONFIG_DISABLE>
                               )
    set_target_properties(${name}_objs PROPERTIES POSITION_INDEPENDENT_CODE ON
                                                  WINDOWS_EXPORT_ALL_SYMBOLS ON
                          )

    # ##########################################################################
    # Default Library
    # ##########################################################################
    add_library(${name} $<TARGET_OBJECTS:${name}_objs>)
    add_library(${name}::${name} ALIAS ${name})
    target_link_libraries(${name}
                          PUBLIC ${name}_deps
                          PRIVATE $<TARGET_NAME_IF_EXISTS:ltb_dev_settings>
                                  $<$<BOOL:${LTB_LINK_GCOV}>:gcov>
                          )

    # ##########################################################################
    # Testing
    # ##########################################################################
    if(${LTB_ENABLE_TESTING})
        add_executable(test_${name} $<TARGET_OBJECTS:${name}_objs>)
        target_link_libraries(test_${name}
                              PUBLIC ${name}_deps
                              PRIVATE $<TARGET_NAME_IF_EXISTS:ltb_dev_settings>
                                      $<$<BOOL:${LTB_LINK_GCOV}>:gcov>
                              )
        add_test(NAME test_${name} COMMAND $<TARGET_FILE:test_${name}>)
    endif()
endfunction()
