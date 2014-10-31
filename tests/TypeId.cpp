/**
	@author
	Ryan Pavlik ( <rpavlik@iastate.edu> http://academic.cleardefinition.com/ ),
	Iowa State University
	Virtual Reality Applications Center and
	Human-Computer Interaction Graduate Program
*/

#define BOOST_TEST_MODULE TypeId

// Internal Includes
#include <util/TypeId.h>

// Library/third-party includes
#include <BoostTestTargetConfig.h>

// Standard includes
#include <vector>
#include <set>
#include <map>
#include <string>

using namespace boost::unit_test;

using util::TypeId;
typedef std::vector<util::TypeId> TypeIdVector;
typedef std::set<util::TypeId> TypeIdSet;

typedef std::pair<util::TypeId, int> TypeIdPair;
typedef std::map<util::TypeId, int> TypeIdMap;

struct Fixture {
	Fixture() : StringId(typeid(std::string)), IntId(typeid(int)) {}
	TypeId StringId;
	TypeId IntId;
	TypeId EmptyId;
};

BOOST_AUTO_TEST_CASE(DefaultConstructor) {
	BOOST_REQUIRE_NO_THROW(TypeId());
}

BOOST_AUTO_TEST_CASE(TypeinfoConstructor) {
	BOOST_REQUIRE_NO_THROW(TypeId(typeid(int)));
	BOOST_REQUIRE_NO_THROW(TypeId(typeid(std::string)));
}

BOOST_AUTO_TEST_CASE(MPLIdentityConstructor) {
#ifndef _MSC_VER
	/// @todo Why does VS 2013 not like this?
	BOOST_REQUIRE_NO_THROW(TypeId(boost::mpl::identity<int>()));
	BOOST_REQUIRE_NO_THROW(TypeId(boost::mpl::identity<std::string>()));
#endif
}

BOOST_AUTO_TEST_CASE(StaticTemplateConstructor) {
	BOOST_REQUIRE_NO_THROW(TypeId::create<int>());
	BOOST_REQUIRE_NO_THROW(TypeId::create<std::string>());
}

BOOST_AUTO_TEST_CASE(FixtureConstruction) {
	BOOST_REQUIRE_NO_THROW(Fixture());
}

BOOST_AUTO_TEST_CASE(EqualityComparison) {
	BOOST_CHECK_EQUAL(TypeId(), TypeId());
	BOOST_CHECK_EQUAL(TypeId(typeid(int)), TypeId(typeid(int)));
	BOOST_CHECK_EQUAL(TypeId(typeid(std::string)), TypeId(typeid(std::string)));

	BOOST_CHECK_NE(TypeId(), TypeId(typeid(std::string)));
	BOOST_CHECK_NE(TypeId(typeid(int)), TypeId(typeid(std::string)));

	BOOST_CHECK(TypeId(typeid(std::string)) == typeid(std::string));
	BOOST_CHECK(typeid(std::string) == TypeId(typeid(std::string)));

	BOOST_CHECK(TypeId() != typeid(std::string));
	BOOST_CHECK(typeid(std::string) != TypeId());
}

BOOST_AUTO_TEST_CASE(ConstructorEquality) {
	BOOST_CHECK_EQUAL(TypeId(typeid(int)), TypeId::create<int>());
	BOOST_CHECK_EQUAL(TypeId(typeid(std::string)), TypeId::create<std::string>());

	BOOST_CHECK_EQUAL(TypeId(typeid(int)), TypeId(boost::mpl::identity<int>()));
	BOOST_CHECK_EQUAL(TypeId(typeid(std::string)), TypeId(boost::mpl::identity<std::string>()));
}

BOOST_AUTO_TEST_CASE(EmptyMethod) {
	Fixture f;
	BOOST_CHECK(f.EmptyId.empty());
	BOOST_CHECK(!f.IntId.empty());
	BOOST_CHECK(!f.StringId.empty());
}

BOOST_AUTO_TEST_CASE(GetMethod) {
	Fixture f;
	BOOST_CHECK_NO_THROW(f.IntId.get());
	BOOST_CHECK_NO_THROW(f.StringId.get());
}

BOOST_AUTO_TEST_CASE(GetPointerMethod) {
	Fixture f;
	BOOST_CHECK_NO_THROW(f.EmptyId.getPointer());
	BOOST_CHECK_NO_THROW(f.IntId.getPointer());
	BOOST_CHECK_NO_THROW(f.StringId.getPointer());
}

BOOST_AUTO_TEST_CASE(NameMethod) {
	Fixture f;
	/// These are all the guarantees we give - and even that might be too much.
	BOOST_CHECK_EQUAL(std::string(f.EmptyId.name()), "");
	BOOST_CHECK_NE(std::string(f.IntId.name()), "");
	BOOST_CHECK_NE(std::string(f.StringId.name()), "");
}

BOOST_AUTO_TEST_CASE(Ordering) {
	Fixture f;
	BOOST_CHECK(!(f.EmptyId < f.EmptyId));

	BOOST_CHECK(!(f.IntId < f.IntId));
	BOOST_CHECK(!(typeid(int) < f.IntId));
	BOOST_CHECK(!(f.IntId < typeid(int)));

	BOOST_CHECK(!(f.StringId < f.StringId));
	BOOST_CHECK(!(typeid(std::string) < f.StringId));
	BOOST_CHECK(!(f.StringId < typeid(std::string)));

	BOOST_CHECK((f.IntId < f.StringId) || (f.StringId < f.IntId));
	BOOST_CHECK(!((f.IntId < f.StringId) && (f.StringId < f.IntId)));
}

BOOST_AUTO_TEST_CASE(Vector) {
	BOOST_REQUIRE_NO_THROW(TypeIdVector());
	TypeIdVector vec;
	Fixture f;
	BOOST_REQUIRE_NO_THROW(vec.push_back(f.EmptyId));
	BOOST_REQUIRE_NO_THROW(vec.push_back(f.IntId));
	BOOST_REQUIRE_NO_THROW(vec.push_back(f.StringId));

	BOOST_REQUIRE_EQUAL(vec.size(), 3);
	BOOST_CHECK_EQUAL(vec[0], f.EmptyId);
	BOOST_CHECK_EQUAL(vec[1], f.IntId);
	BOOST_CHECK_EQUAL(vec[2], f.StringId);

	BOOST_CHECK(vec != TypeIdVector());

	BOOST_REQUIRE_NO_THROW(TypeIdVector(vec));
	TypeIdVector v2(vec);
	BOOST_CHECK(vec == v2);
}

BOOST_AUTO_TEST_CASE(Set) {
	BOOST_REQUIRE_NO_THROW(TypeIdSet());
	TypeIdSet set;
	Fixture f;
	BOOST_REQUIRE_NO_THROW(set.insert(f.EmptyId));
	BOOST_REQUIRE_NO_THROW(set.insert(f.IntId));
	BOOST_REQUIRE_NO_THROW(set.insert(f.StringId));

	BOOST_REQUIRE_EQUAL(set.size(), 3);
	BOOST_CHECK(set.find(f.EmptyId) != set.end());
	BOOST_CHECK(set.find(f.IntId) != set.end());
	BOOST_CHECK(set.find(f.StringId) != set.end());

	BOOST_CHECK(set != TypeIdSet());

	BOOST_REQUIRE_NO_THROW(TypeIdSet(set));
	TypeIdSet s2(set);
	BOOST_CHECK(set == s2);

	BOOST_REQUIRE_NO_THROW(set.erase(set.find(f.IntId)));
	BOOST_REQUIRE_EQUAL(set.size(), 2);
	BOOST_CHECK(set.find(f.EmptyId) != set.end());
	BOOST_CHECK(set.find(f.IntId) == set.end());
	BOOST_CHECK(set.find(f.StringId) != set.end());
}

BOOST_AUTO_TEST_CASE(Map) {
	BOOST_REQUIRE_NO_THROW(TypeIdPair());
	BOOST_REQUIRE_NO_THROW(TypeIdMap());
	TypeIdMap map;
	Fixture f;
	BOOST_REQUIRE(map.insert(TypeIdPair(f.EmptyId, 0)).second);
	BOOST_REQUIRE(map.insert(TypeIdPair(f.IntId, 1)).second);
	BOOST_REQUIRE(map.insert(TypeIdPair(f.StringId, 2)).second);

	BOOST_REQUIRE_EQUAL(map.size(), 3);

	BOOST_CHECK_EQUAL(map.at(f.EmptyId), 0);
	BOOST_CHECK_EQUAL(map.at(f.IntId), 1);
	BOOST_CHECK_EQUAL(map.at(f.StringId), 2);

	BOOST_CHECK_EQUAL(map[f.EmptyId], 0);
	BOOST_CHECK_EQUAL(map[f.IntId], 1);
	BOOST_CHECK_EQUAL(map[f.StringId], 2);

	BOOST_CHECK(map != TypeIdMap());

	BOOST_REQUIRE_NO_THROW(TypeIdMap(map));
	TypeIdMap m2(map);
	BOOST_CHECK(map == m2);
}

BOOST_AUTO_TEST_CASE(TransitivityOfOrderingAndEquality) {
	TypeIdVector vec;
	Fixture f;
	BOOST_REQUIRE_NO_THROW(vec.push_back(f.EmptyId));
	BOOST_REQUIRE_NO_THROW(vec.push_back(f.IntId));
	BOOST_REQUIRE_NO_THROW(vec.push_back(f.StringId));

	for (size_t i = 0; i < vec.size(); ++i) {
		for (size_t j = 0; j < vec.size(); ++j) {
			for (size_t k = 0; k < vec.size(); ++k) {
				BOOST_CHECK((vec[i] < vec[j] && vec[j] < vec[k]) ? vec[i] < vec[k] /* test 'implies' */ : true /* fallback if "if" fails */);
				BOOST_CHECK((vec[i] == vec[j] && vec[j] == vec[k]) ? vec[i] == vec[k] /* test 'implies' */ : true /* fallback if "if" fails */);
			}
		}
	}
}
