# CMake cross-platform build system
# 2009-2010 Ryan Pavlik <rpavlik@iastate.edu>
# http://academic.cleardefinition.com/
# Iowa State University HCI Graduate Program/VRAC

# Script list-tests.sh will extract all test names defined with BOOST_AUTO_TEST_CASE
# in the given file

add_boost_test(Saturate
	SOURCES
	Saturate.cpp
	TESTS
	TwoParamNoSaturateZeros
	ThreeParamNoSaturateZeros
	TwoParam1DNoSaturate
	TwoParam1DNoSaturateNeg
	TwoParam1DSaturate
	TwoParam1DSaturateNeg
	ThreeParam1DNoSaturate
	ThreeParam1DNoSaturateNeg
	ThreeParam1DSaturate
	ThreeParam1DSaturateNeg
	TwoParam3DNoSaturate
	TwoParam3DNoSaturateNeg
	TwoParam3DSaturate
	TwoParam3DSaturateNeg
	ThreeParam3DNoSaturate
	ThreeParam3DNoSaturateNeg
	ThreeParam3DSaturate
	ThreeParam3DSaturateNeg)

add_boost_test(Set2
	SOURCES
	Set2.cpp
	TESTS
	DefaultConstruction
	ConstructionSmallerFirst
	ConstructionSmallerSecond
	ConstructionEqual
	CopyConstruction
	AssignmentOperator
	AssignmentOperatorBothWays
	AssignmentOperatorSelfAssign
	MutatorSmallerFirst
	MutatorSmallerSecond
	MutatorBothEqual
	MutatorNoChange
	MutatorNoEffectiveChange
	ComparisonFirstsUnequal
	ComparisonFirstsEqual
	ComparisonBothEqual
	VectorCompatibility
	VectorSort
	VectorSortNoOp
	MapCompatibility
	MapUpdate)

add_boost_test(CountedUniqueValues
	SOURCES
	CountedUniqueValues.cpp
	TESTS
	DefaultConstruction
	IncrementalCounts
	SimpleRetrieve
	ValueIdentity)

add_boost_test(CubeComponents
	SOURCES
	CubeComponents.cpp
	TESTS
	VertexConstructionDefault
	VertexConstructionBitset
	VertexConstructionInt
	VertexUniqueGeneration
	FaceConstructionDefault
	FaceConstructionInt
	FaceIdentity
	FaceVertexAccess
	FaceVertexIdentity)

add_boost_test(EigenTie
	SOURCES
	EigenTie.cpp
	TESTS
	ThreeZerosAssign
	ThreeOnesAssign
	ThreeZerosTieTieAssign
	ThreeOnesTieTieAssign
	ThreeZerosChainedAssign
	ThreeOnesChainedAssign
	AliasingAcrossAssign
	NoThrowForRHSNotUnique
	ThrowsTwoNotUnique
	ThrowsThreeNotUnique)

add_boost_test(RangedInt
	SOURCES
	RangedInt.cpp
	TESTS
	InRangeConstructionDefaults
	OutOfRangeConstructionDefaults
	URUMath
	URRMath
	UURMath
	RRUMath
	RRRMath
	InRangeConstructionNotChecked
	OutOfRangeConstructionNotChecked
	SameRangeCrossChecking)

if(NOT MSVC)
	add_boost_test(FusionMapToTemplate
		SOURCES
		FusionMapToTemplate.cpp
		TESTS
		SummingFunc
		KeywiseFunc)
endif()

add_boost_test(ValueToTemplate
	SOURCES
	ValueToTemplate.cpp
	TESTS
	AllBools
	BoolsAndInts
	RangedInts)

add_boost_test(WithHistory
	SOURCES
	WithHistory.cpp
	TESTS
	DefaultConstructionPOD
	ValueConstructionPOD
	ValuePODSwap
	ValuePODAdvanceByCopy
	DualValueConstructionPOD
	CopyFromDefaultConstructionPOD
	CopyFromValueConstructionPOD
	CopyFromDualValueConstructionPOD
	DualValuePODSwap
	DualValuePODAdvanceByCopy
	DefaultConstructionVector
	ValueConstructionVector
	ValueVectorSwap
	ValueVectorAdvanceByCopy
	DualValueConstructionVector
	DualValueVectorSwap
	DualValueVectorAdvanceByCopy
	CopyFromDefaultConstructionVector
	CopyFromValueConstructionVector
	CopyFromDualValueConstructionVector)

add_boost_test(SearchPath
	SOURCES
	SearchPath.cpp
	TESTS
	ElementDefaultConstruction
	ElementConstructionString1
	ElementConstructionString2
	ElementConstructionString3
	ElementConstructionString4
	ElementConstructionString5
	ElementGetStringWithSubstitution1
	ElementGetStringWithSubstitution2
	ElementGetStringWithSubstitution3
	ElementGetStringWithSubstitution4
	ElementGetStringWithSubstitution5
	ElementConstructionTwoString1
	ElementConstructionTwoString2
	ElementConstructionTwoString3
	ElementPrefixMutator
	ElementSuffixMutator
	ElementPrefixAndSuffixMutator
	LuaListEmpty
	LuaListJustDelimiter
	LuaListOneElement
	LuaListOneElementWithTrailing
	LuaListTwoElements
	DirectoryListEmpty
	DirectoryListJustDelimiter
	DirectoryListOneElement
	DirectoryListOneElementWithTrailing
	DirectoryListTwoElements)

if(NOT MSVC)
	# TODO why is this broken on MSVC?
	add_boost_test(MPLApplyAt
		SOURCES
		MPLApplyAt.cpp
		TESTS
		NumberGetter
		NumberGetterBigger
		ActualTypes)
endif()

add_boost_test(ReceiveBuffer
	SOURCES
	ReceiveBuffer.cpp
	TESTS
	ConstructionDefault
	ConstructionCopy
	PushBackSingle
	CopyPopFront
	CopyPopBack
	Slide
	SlideAppend
	EraseFront
	EraseTwoFront
	EraseBack
	EraseTwoBack)

add_boost_test(TypeId
	SOURCES
	TypeId.cpp
	TESTS
	DefaultConstructor
	TypeinfoConstructor
	MPLIdentityConstructor
	StaticTemplateConstructor
	FixtureConstruction
	EqualityComparison
	ConstructorEquality
	EmptyMethod
	GetMethod
	GetPointerMethod
	NameMethod
	Ordering
	Vector
	Set
	Map
	TransitivityOfOrderingAndEquality)

if(Boost_THREAD_LIBRARY AND CMAKE_THREAD_LIBS_INIT)
	add_boost_test(RunLoopManagerBoost
		SOURCES
		RunLoopManagerBoost.cpp
		RunLoopManager_common.h
		LIBRARIES
		${Boost_THREAD_LIBRARY}
		${Boost_DATE_TIME_LIBRARY}
		${Boost_SYSTEM_LIBRARY}
		${Boost_CHRONO_LIBRARY}
        ${CMAKE_THREAD_LIBS_INIT})
endif()

if(NOT MSVC)
	# TODO why is this broken on MSVC?
	find_package(Boost COMPONENTS serialization)
	if(Boost_SERIALIZATION_LIBRARY)
		add_boost_test(EigenMatrixSerialize
			SOURCES
			EigenMatrixSerialize.cpp
			LIBRARIES ${Boost_SERIALIZATION_LIBRARY}
			TESTS
			IdentityRoundTrip
			ConstantVecRoundTrip)
	endif()
endif()

add_subdirectory(cleanbuild)
