project(examples)

file(GLOB EXAMPLE_SRC "./*.cpp")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}")

foreach(src ${EXAMPLE_SRC})
    get_filename_component(src_bin ${src} NAME_WE)
    add_executable(${src_bin} ${src})
    target_link_libraries(${src_bin} PUBLIC stk)
endforeach()
