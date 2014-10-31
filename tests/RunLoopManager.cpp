/**
	@author
	Ryan Pavlik ( <rpavlik@iastate.edu> http://academic.cleardefinition.com/ ),
	Iowa State University
	Virtual Reality Applications Center and
	Human-Computer Interaction Graduate Program
*/

#define BOOST_TEST_MODULE RunLoopManager

// Internal Includes
#include <util/RunLoopManager.h>

// Library/third-party includes
#include <BoostTestTargetConfig.h>

// Standard includes
#include <iostream>

using namespace boost::unit_test;

using util::RunLoopManager;

BOOST_AUTO_TEST_CASE(ConstructionDefault) {
	BOOST_CHECK_NO_THROW(RunLoopManager());
}
