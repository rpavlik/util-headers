# 2009-2010 Ryan Pavlik <rpavlik@iastate.edu>
# http://academic.cleardefinition.com/
# Iowa State University HCI Graduate Program/VRAC

set(UTIL_HEADERS
	BoostAssertMsg.h
	SizeGenerator.h
	Stride.h)

set(DATASTRUCTURES_HEADERS
	BlockingInvokeFunctor.h
	booststdint.h
	CountedUniqueValues.h
	FusionMapToTemplate.h
	LockFreeBuffer.h
	RangedInt.h
	ReceiveBuffer.h
	RunLoopManager.h
	SearchPath.h
	Set2.h
	SplitMap.h
	TypeId.h
	ValueToTemplate.h
	ValueToTemplatePolicy.h
	VectorSimulator.h
	WithHistory.h)

if(VPR_FOUND)
	list(APPEND DATASTRUCTURES_HEADERS
		BlockingInvokeFunctorVPR.h
		RunLoopManagerVPR.h)
else()
	list(APPEND UNTESTED_HEADERS
		BlockingInvokeFunctorVPR.h
		RunLoopManagerVPR.h)
endif()
if(Boost_THREAD_LIBRARY AND CMAKE_THREAD_LIBS_INIT)
	list(APPEND DATASTRUCTURES_HEADERS
		RunLoopManagerBoost.h)
else()
	list(APPEND UNTESTED_HEADERS
		RunLoopManagerBoost.h)
endif()

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
	launchByAssociation.h)

if(OPENSCENEGRAPH_FOUND)
	list(APPEND FREEFUNCTION_HEADERS
		osgFindNamedNode.h)
else()
	list(APPEND UNTESTED_HEADERS
		osgFindNamedNode.h)
endif()

if(OPENSCENEGRAPH_FOUND AND GMTL_FOUND)
	list(APPEND FREEFUNCTION_HEADERS
		gmtlToOsgMatrix.h)
else()
	list(APPEND UNTESTED_HEADERS
		gmtlToOsgMatrix.h)
endif()


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

set(HEADERS ${UTIL_HEADERS} ${DATASTRUCTURES_HEADERS} ${MATH_HEADERS} ${METAPROGRAMMING_HEADERS} ${FREEFUNCTION_HEADERS})
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
