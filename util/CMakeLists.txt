# 2009-2010 Ryan Pavlik <rpavlik@iastate.edu>
# http://academic.cleardefinition.com/
# Iowa State University HCI Graduate Program/VRAC

set(UTIL_HEADERS
	BoostAssertMsg.h
	SizeGenerator.h
	Stride.h)

set(DATASTRUCTURES_HEADERS
	BlockingInvokeFunctor.h
	BlockingInvokeFunctorVPR.h
	booststdint.h
	CountedUniqueValues.h
	FusionMapToTemplate.h
	LockFreeBuffer.h
	RangedInt.h
	ReceiveBuffer.h
	RunLoopManager.h
	RunLoopManagerBoost.h
	RunLoopManagerStd.h
	RunLoopManagerVPR.h
	SearchPath.h
	Set2.h
	SplitMap.h
	TypeId.h
	ValueToTemplate.h
	ValueToTemplatePolicy.h
	VectorSimulator.h
	WithHistory.h)

set(METAPROGRAMMING_HEADERS
	MPLApplyAt.h
	MPLFindIndex.h)

set(MATH_HEADERS
	CubeComponents.h
	EigenMatrixSerialize.h
	EigenTie.h
	max_extended.h
	min_extended.h
	RandomFloat.h
	Saturate.h)

set(FREEFUNCTION_HEADERS
	ChangeFileExtension.h
	GetLocalComputerName.h
	gmtlToOsgMatrix.h
	launchByAssociation.h
	osgFindNamedNode.h)

add_custom_target(util
	SOURCES
	Documentation.dox
	${UTIL_HEADERS})

add_custom_target(util-DataStructures
	SOURCES
	${DATASTRUCTURES_HEADERS})

add_custom_target(util-Math
	SOURCES
	${MATH_HEADERS})

add_custom_target(util-Metaprogramming
	SOURCES
	${METAPROGRAMMING_HEADERS})

add_custom_target(util-FreeFunctions
	SOURCES
	${FREEFUNCTION_HEADERS})

###
# Remove headers we can't test from the build list, but not the IDE
###

set(HEADERS ${UTIL_HEADERS} ${DATASTRUCTURES_HEADERS} ${MATH_HEADERS} ${METAPROGRAMMING_HEADERS} ${FREEFUNCTION_HEADERS})

macro(remove_header_tests)
	list(APPEND UNTESTED_HEADERS ${ARGN})
	list(REMOVE_ITEM HEADERS ${ARGN})
endmacro()

if(NOT VPR_FOUND)
	remove_header_tests(
		BlockingInvokeFunctorVPR.h
		RunLoopManagerVPR.h)
endif()

if(NOT (Boost_THREAD_LIBRARY AND CMAKE_THREAD_LIBS_INIT))
	remove_header_tests(RunLoopManagerBoost.h)
endif()

if(MSVC AND NOT MSVC_VERSION LESS 1700)
	# MSVC 2012 and up can support this
else()
	# TODO enable C++11 support in compilers where you have to switch it on,
	# and enable this header then.

	remove_header_tests(RunLoopManagerStd.h)
endif()

if(NOT OPENSCENEGRAPH_FOUND)
	remove_header_tests(osgFindNamedNode.h)
endif()

if(NOT (OPENSCENEGRAPH_FOUND AND GMTL_FOUND))
	remove_header_tests(gmtlToOsgMatrix.h)
endif()


set(HEADERS ${HEADERS} PARENT_SCOPE)
set(UNTESTED_HEADERS ${UNTESTED_HEADERS} PARENT_SCOPE)

###
# Check to ensure we have all headers listed
###
file(GLOB_RECURSE globbed_headers RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.h)
list(REMOVE_ITEM globbed_headers ${HEADERS} ${UNTESTED_HEADERS})
list(LENGTH globbed_headers num_unknown_headers)
if(num_unknown_headers GREATER 0)
	message(FATAL_ERROR "The following headers are present in the file system, but missing in the build system: ${globbed_headers}")
endif()
