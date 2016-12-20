/**
	@author
	Ryan Pavlik ( <rpavlik@iastate.edu> http://academic.cleardefinition.com/ ),
	Iowa State University
	Virtual Reality Applications Center and
	Human-Computer Interaction Graduate Program
*/

#define BOOST_TEST_MODULE WithHistory

// Internal Includes
#include <util/WithHistory.h>

// Library/third-party includes
#include <BoostTestTargetConfig.h>

// Standard includes
#include <vector>

using namespace boost::unit_test;
using namespace util;

// be able to print int vectors
namespace std {
static inline ostream &operator<<(ostream &os, const vector<int> &val) {
	for (unsigned int i = 0; i < val.size(); ++i) {
		os << val[i] << ", ";
	}
	return os;
}
} // namespace std

BOOST_AUTO_TEST_CASE(DefaultConstructionPOD) { WithHistory<int> a; }

BOOST_AUTO_TEST_CASE(ValueConstructionPOD) {
	WithHistory<int> a(5);
	BOOST_CHECK_EQUAL(a.current(), 5);
}

BOOST_AUTO_TEST_CASE(ValuePODSwap) {
	WithHistory<int> a(5);
	BOOST_CHECK_EQUAL(a.current(), 5);
	a.swap();
	BOOST_CHECK_EQUAL(a.previous(), 5);
}

BOOST_AUTO_TEST_CASE(ValuePODAdvanceByCopy) {
	WithHistory<int> a(5);
	BOOST_CHECK_EQUAL(a.current(), 5);
	a.advanceByCopy();
	BOOST_CHECK_EQUAL(a.current(), 5);
	BOOST_CHECK_EQUAL(a.previous(), 5);
}

BOOST_AUTO_TEST_CASE(DualValueConstructionPOD) {
	WithHistory<int> a(5, 10);
	BOOST_CHECK_EQUAL(a.current(), 5);
	BOOST_CHECK_EQUAL(a.previous(), 10);
}

BOOST_AUTO_TEST_CASE(CopyFromDefaultConstructionPOD) {
	WithHistory<int> a;
	WithHistory<int> b(a);
}

BOOST_AUTO_TEST_CASE(CopyFromValueConstructionPOD) {
	WithHistory<int> a(5);
	WithHistory<int> b(a);
	BOOST_CHECK_EQUAL(b.current(), 5);
}

BOOST_AUTO_TEST_CASE(CopyFromDualValueConstructionPOD) {
	WithHistory<int> a(5, 10);
	WithHistory<int> b(a);
	BOOST_CHECK_EQUAL(b.current(), 5);
	BOOST_CHECK_EQUAL(b.previous(), 10);
}

BOOST_AUTO_TEST_CASE(DualValuePODSwap) {
	WithHistory<int> a(5, 10);
	a.swap();
	BOOST_CHECK_EQUAL(a.current(), 10);
	BOOST_CHECK_EQUAL(a.previous(), 5);
}

BOOST_AUTO_TEST_CASE(DualValuePODAdvanceByCopy) {
	WithHistory<int> a(5, 10);
	a.advanceByCopy();
	BOOST_CHECK_EQUAL(a.current(), 5);
	BOOST_CHECK_EQUAL(a.previous(), 5);
}

BOOST_AUTO_TEST_CASE(DefaultConstructionVector) {
	WithHistory<std::vector<int> > a;
	BOOST_CHECK_EQUAL(a.current().size(), 0);
	BOOST_CHECK_EQUAL(a.previous().size(), 0);
}

BOOST_AUTO_TEST_CASE(ValueConstructionVector) {
	std::vector<int> aVal(5, 0);
	WithHistory<std::vector<int> > a(aVal);
	BOOST_CHECK_EQUAL(a.current().size(), 5);
	BOOST_CHECK_EQUAL(a.current(), aVal);
	BOOST_CHECK_EQUAL(a.previous().size(), 0);
}

BOOST_AUTO_TEST_CASE(ValueVectorSwap) {
	std::vector<int> aVal(5, 0);
	WithHistory<std::vector<int> > a(aVal);
	a.swap();
	BOOST_CHECK_EQUAL(a.previous().size(), 5);
	BOOST_CHECK_EQUAL(a.previous(), aVal);
	BOOST_CHECK_EQUAL(a.current().size(), 0);
}

BOOST_AUTO_TEST_CASE(ValueVectorAdvanceByCopy) {
	std::vector<int> aVal(5, 0);
	WithHistory<std::vector<int> > a(aVal);
	a.advanceByCopy();
	BOOST_CHECK_EQUAL(a.current().size(), 5);
	BOOST_CHECK_EQUAL(a.current(), aVal);
	BOOST_CHECK_EQUAL(a.previous().size(), 5);
	BOOST_CHECK_EQUAL(a.previous(), aVal);
}

BOOST_AUTO_TEST_CASE(DualValueConstructionVector) {
	std::vector<int> aVal(5, 0);
	std::vector<int> bVal(3, 0);
	WithHistory<std::vector<int> > a(aVal, bVal);
	BOOST_CHECK_EQUAL(a.current().size(), 5);
	BOOST_CHECK_EQUAL(a.current(), aVal);
	BOOST_CHECK_EQUAL(a.previous().size(), 3);
	BOOST_CHECK_EQUAL(a.previous(), bVal);
}

BOOST_AUTO_TEST_CASE(DualValueVectorSwap) {
	std::vector<int> aVal(5, 0);
	std::vector<int> bVal(3, 0);
	WithHistory<std::vector<int> > a(aVal, bVal);
	a.swap();
	BOOST_CHECK_EQUAL(a.current().size(), 3);
	BOOST_CHECK_EQUAL(a.current(), bVal);
	BOOST_CHECK_EQUAL(a.previous().size(), 5);
	BOOST_CHECK_EQUAL(a.previous(), aVal);
}

BOOST_AUTO_TEST_CASE(DualValueVectorAdvanceByCopy) {
	std::vector<int> aVal(5, 0);
	std::vector<int> bVal(3, 0);
	WithHistory<std::vector<int> > a(aVal, bVal);
	a.advanceByCopy();
	BOOST_CHECK_EQUAL(a.current().size(), 5);
	BOOST_CHECK_EQUAL(a.current(), aVal);
	BOOST_CHECK_EQUAL(a.previous().size(), 5);
	BOOST_CHECK_EQUAL(a.previous(), aVal);
}

BOOST_AUTO_TEST_CASE(CopyFromDefaultConstructionVector) {
	WithHistory<std::vector<int> > a;
	WithHistory<std::vector<int> > b(a);
	BOOST_CHECK_EQUAL(b.current().size(), 0);
	BOOST_CHECK_EQUAL(b.previous().size(), 0);
}

BOOST_AUTO_TEST_CASE(CopyFromValueConstructionVector) {
	std::vector<int> aVal(5, 0);
	WithHistory<std::vector<int> > a(aVal);
	WithHistory<std::vector<int> > b(a);
	BOOST_CHECK_EQUAL(b.current().size(), 5);
	BOOST_CHECK_EQUAL(b.current(), aVal);
	BOOST_CHECK_EQUAL(b.previous().size(), 0);
}

BOOST_AUTO_TEST_CASE(CopyFromDualValueConstructionVector) {
	std::vector<int> aVal(5, 0);
	std::vector<int> bVal(3, 0);
	WithHistory<std::vector<int> > a(aVal, bVal);
	WithHistory<std::vector<int> > b(a);
	BOOST_CHECK_EQUAL(b.current().size(), 5);
	BOOST_CHECK_EQUAL(b.current(), aVal);
	BOOST_CHECK_EQUAL(b.previous().size(), 3);
	BOOST_CHECK_EQUAL(b.previous(), bVal);
}
