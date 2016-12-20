# CMake cross-platform build system
# 2009-2016 Ryan Pavlik <ryan.pavlik@gmail.com>

cmake_minimum_required(VERSION 3.1.0)

# Set package properties
project(util-headers)

set(CPACK_PACKAGE_VENDOR "Ryan Pavlik")
set(CPACK_PACKAGE_CONTACT "Ryan Pavlik <ryan.pavlik@gmail.com>")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_VERSION
	"${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${CPACK_PACKAGE_VERSION}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${PROJECT_NAME}-${CPACK_PACKAGE_VERSION}-src")

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT AND (NOT ONCE_SET_CMAKE_INSTALL_PREFIX))
	set(ONCE_SET_CMAKE_INSTALL_PREFIX
		true
		CACHE
		INTERNAL
		"Have we set the install prefix yet?"
		FORCE)
	set(CMAKE_INSTALL_PREFIX
		"${CMAKE_CURRENT_SOURCE_DIR}/installed-headers"
		CACHE
		PATH
		"Install path prefix, prepended onto install directories"
		FORCE)
endif()
###
# Perform build configuration of dependencies
###

# Locally-developed modules dist'ed with this app - always have this first.
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(UseBackportedModules)

# Separate module not included with the cmake-modules repo
include(CMakeBoostHelper.cmake)

include(CppcheckTargets)
include(DoxygenTargets)
include(EnableExtraCompilerWarnings)
include(GetGitRevisionDescription)

include(CTest)
include(CreateDashboardScripts)

globally_enable_extra_compiler_warnings()

include(SetDefaultBuildType)
set_default_build_type(RelWithDebInfo)

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

find_package(Threads)

find_package(VRJuggler COMPONENTS vpr)
if(VPR20_FOUND OR VPR22_FOUND)
    set(VPR_FOUND TRUE)
endif()
find_package(GMTL)
find_package(OpenSceneGraph)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS thread date_time chrono system)
set(Boost_USE_STATIC_LIBS OFF)

###
# End build configuration
###

###
# Build the project
###

add_subdirectory(third-party)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}" ${THIRD_PARTY_INCLUDE_DIRS})

add_subdirectory(util)

add_doxygen(Doxyfile NO_PDF)

if(BUILD_TESTING)
	include(BoostTestTargets)
	add_subdirectory(tests)
endif()

create_dashboard_scripts()


#git_describe(VERSION_DESCRIPTION)

message(STATUS "")
message(STATUS "Configuring Git revision and upstream source info into temporary copy of headers.")
message(STATUS "Building the install target will copy these versions into a util directory under")
message(STATUS "CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}")
message(STATUS "")
get_git_head_revision(REFSPEC VERSION_DESCRIPTION)

set(versioninfo "This header is maintained as a part of 'util-headers' - you can always
    find the latest version online at https://github.com/rpavlik/util-headers

    This GUID can help identify the project: d1dbc94e-e863-49cf-bc08-ab4d9f486613

    This copy of the header is from the revision that Git calls
    ${VERSION_DESCRIPTION}
")


find_package(Git QUIET)
if(GIT_FOUND)
	execute_process(COMMAND "${GIT_EXECUTABLE}" log -1 --pretty=format:\"%ci\" HEAD
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
		RESULT_VARIABLE res
		OUTPUT_VARIABLE COMMIT_DATE
		ERROR_QUIET
		OUTPUT_STRIP_TRAILING_WHITESPACE)
	if(res EQUAL 0)
		set(versioninfo "${versioninfo}
    Commit date: ${COMMIT_DATE}")
	endif()
endif()

set(HEADERS_WITH_ISSUES)
foreach(HEADER ${HEADERS} ${UNTESTED_HEADERS})
	set(SRC_FILE "${CMAKE_CURRENT_SOURCE_DIR}/util/${HEADER}")

	# Compute the subdirectory in util, if applicable
	get_filename_component(SRC_DIR "${SRC_FILE}" DIRECTORY)
	file(RELATIVE_PATH REL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/util/" "${SRC_DIR}")

	configure_file("${SRC_FILE}" "${CMAKE_CURRENT_BINARY_DIR}/${HEADER}" @ONLY)
	install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${HEADER}"
		DESTINATION util/${REL_DIR})

	file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/util/${HEADER}" filecontents)
	if(NOT filecontents MATCHES "[@]versioninfo[@]")
		list(APPEND HEADERS_WITH_ISSUES "${HEADER}")
	endif()
endforeach()

if(HEADERS_WITH_ISSUES)
	message(FATAL_ERROR "The following headers lack the \@versioninfo\@ token in their file header comment: ${HEADERS_WITH_ISSUES}")
endif()


###
# Set packaging options (for CPack)
###

# Choose desired package generators
if(APPLE)
	set(CPACK_GENERATOR DragNDrop)
	set(CPACK_SOURCE_GENERATOR ZIP)
elseif(WIN32)
	set(CPACK_SOURCE_GENERATOR ZIP)
else()
	set(CPACK_SOURCE_GENERATOR TARGZ)
endif()

# Include the packaging system now that we have it all set up
include(CPack)

###
# End Packaging
###
