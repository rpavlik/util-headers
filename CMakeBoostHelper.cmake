# Help CMake find recent Boost MSVC binaries without manual configuration.
#
# Original Author:
# 2014-2016 Ryan Pavlik <ryan.pavlik@gmail.com>
#
# Copyright Sensics, Inc. 2014-2016.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

if(MSVC)
    if(NOT DEFINED Boost_USE_MULTITHREADED)
        set(Boost_USE_MULTITHREADED ON) # Most common ABI
    endif()
endif()

# Function exists solely to introduce a new scope for CMAKE_MODULE_PATH
function(_boosthelper_includehelper)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")
    include(GetMSVCVersion)
endfunction()

if(MSVC AND (NOT Boost_INCLUDE_DIR OR NOT Boost_LIBRARY_DIR))
    _boosthelper_includehelper()
    get_msvc_major_minor_version(_vs_major _vs_minor)
    if(CMAKE_CXX_SIZEOF_DATA_PTR EQUAL 8)
        set(_libdir "lib64-msvc-${_vs_major}.${_vs_minor}")
    else()
        set(_libdir "lib32-msvc-${_vs_major}.${_vs_minor}")
    endif()

    set(_haslibs)
    if(EXISTS "c:/local")
        # Get all versions, newest first.
        file(GLOB _possibilities "c:/local/boost*")
        list(REVERSE _possibilities)
        foreach(DIR ${_possibilities})
            if(EXISTS "${DIR}/${_libdir}")
                list(APPEND _haslibs "${DIR}")
            endif()
        endforeach()
        if(_haslibs)
            list(APPEND CMAKE_PREFIX_PATH ${_haslibs})
            find_package(Boost QUIET)
            if(Boost_FOUND AND NOT Boost_LIBRARY_DIR)
                set(BOOST_ROOT "${Boost_INCLUDE_DIR}" CACHE PATH "")
                set(BOOST_LIBRARYDIR "${Boost_INCLUDE_DIR}/${_libdir}" CACHE PATH "")
            endif()
        endif()
    endif()
endif()
