set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(
    source_list
    "include/Parallel.h"
    "include/Parallel/LockFreeDequeue.h"
    "src/Parallel/Parallel.cpp"
)

add_library(
	Parallel STATIC
	${source_list}
)

function(assign_source_group)
    foreach(_source IN ITEMS ${ARGN})
        if (IS_ABSOLUTE "${_source}")
            file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source}")
        else()
            set(_source_rel "${_source}")
        endif()
        get_filename_component(_source_path "${_source_rel}" PATH)
        string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
        source_group("${_source_path_msvc}" FILES "${_source}")
    endforeach()
endfunction(assign_source_group)

assign_source_group(${source_list})



target_include_directories(Parallel PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_compile_definitions(Parallel PUBLIC PARALLEL_VERSION_MAJOR=0
                                           PARALLEL_VERSION_MINOR=1
                                           PARALLEL_VERSION_PATCH=0
                                           "$<$<CONFIG:DEBUG>:DEBUG_BUILD>")