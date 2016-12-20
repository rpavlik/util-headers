add_custom_target(headercompile)


if(VRJUGGLER_FOUND)
	include_directories(${VRJUGGLER_INCLUDE_DIRS})
	set(LIBRARIES_BlockingInvokeFunctorVPR ${VRJUGGLER_LIBRARIES})
else()
	list(REMOVE_ITEM HEADERS BlockingInvokeFunctorVPR.h)
endif()

if(OPENSCENEGRAPH_FOUND)
	include_directories(${OPENSCENEGRAPH_INCLUDE_DIRS})
	if(GMTL_FOUND)
		include_directories(${GMTL_INCLUDE_DIRS})
	else()
		list(REMOVE_ITEM HEADERS gmtlToOsgMatrix.h)
	endif()
else()
	list(REMOVE_ITEM HEADERS osgFindNamedNode.h)
endif()

if(Boost_THREAD_LIBRARY AND CMAKE_THREAD_LIBS_INIT)
	set(LIBRARIES_RunLoopManagerBoost
		${Boost_THREAD_LIBRARY}
		${Boost_DATE_TIME_LIBRARY}
		${Boost_SYSTEM_LIBRARY}
		${Boost_CHRONO_LIBRARY}
        ${CMAKE_THREAD_LIBS_INIT})
else()
	list(REMOVE_ITEM HEADERS RunLoopManagerBoost.h)
endif()

if(VPR20_FOUND)
	set(LIBRARIES_RunLoopManagerVPR ${VPR20_LIBRARIES})
elseif(VPR22_FOUND)
	set(LIBRARIES_RunLoopManagerVPR ${VPR22_LIBRARIES})
else()
	list(REMOVE_ITEM HEADERS RunLoopManagerVPR.h)
endif()

foreach(HEADER ${HEADERS})
	string(REPLACE ".h" "" SHORTNAME "${HEADER}")
	string(MAKE_C_IDENTIFIER "${SHORTNAME}" SHORTNAME)
	set(stem "test_headercompile_${SHORTNAME}")
	configure_file(test_headercompile.cpp.in "${CMAKE_CURRENT_BINARY_DIR}/${stem}.cpp" @ONLY)

	add_executable(${stem} "${CMAKE_CURRENT_BINARY_DIR}/${stem}.cpp")
	if(LIBRARIES_${SHORTNAME})
		target_link_libraries(${stem} ${LIBRARIES_${SHORTNAME}})
	endif()
	if(CXX_STANDARD_${SHORTNAME})
		set_property(TARGET ${stem} CXX_STANDARD ${CXX_STANDARD_${SHORTNAME}})
	endif()
	add_test(${stem}_executes ${stem})
	add_dependencies(headercompile ${stem})
endforeach()
