/** @file
		@brief Header

		@versioninfo@

		@date 2013

		@author
		Ryan Pavlik
		<rpavlik@iastate.edu> and <abiryan@ryand.net>
		http://academic.cleardefinition.com/
		Iowa State University Virtual Reality Applications Center
		Human-Computer Interaction Graduate Program

*/

//           Copyright Iowa State University 2013.
//  Distributed under the Boost Software License, Version 1.0.
//     (See accompanying file LICENSE_1_0.txt or copy at
//           http://www.boost.org/LICENSE_1_0.txt)

#ifndef INCLUDED_RunLoopManagerBoost_h_GUID_50f7b2f1_493e_4395_25ca_df2f010a34bd
#define INCLUDED_RunLoopManagerBoost_h_GUID_50f7b2f1_493e_4395_25ca_df2f010a34bd

// Internal Includes
#include "RunLoopManager.h"

// Library/third-party includes
#include <boost/thread.hpp>

// Standard includes
// - none

namespace util {

class RunLoopManagerBoost : public RunLoopManagerBase {
public:
	RunLoopManagerBoost() : currentState_(STATE_STOPPED) {}

	/// @name StartingInterface
	/// @{
	void signalStart();
	void signalAndWaitForStart();
	/// @}

	/// @name ShutdownInterface
	/// @{
	void signalShutdown();
	void signalAndWaitForShutdown();
	/// @}

private:
	void reportStateChange_(RunningState s);
	boost::mutex mut_;
	boost::condition_variable stateCond_;

	typedef boost::unique_lock<boost::mutex> Lock;

	/// protected by condition variable
	volatile RunningState currentState_;
};

inline void RunLoopManagerBoost::signalStart() {
	{
		Lock condGuard(mut_);
		setShouldStop_(false);
	}
}

inline void
RunLoopManagerBoost::reportStateChange_(RunLoopManagerBase::RunningState s) {
	{
		Lock condGuard(mut_);
		currentState_ = s;
	}
	stateCond_.notify_all();
}

inline void RunLoopManagerBoost::signalAndWaitForStart() {
	signalStart();
	{
		Lock condGuard(mut_);
		while (currentState_ != STATE_RUNNING) {
			stateCond_.wait(condGuard);
		}
	}
}

inline void RunLoopManagerBoost::signalShutdown() {
	Lock condGuard(mut_);
	setShouldStop_(true);
}

inline void RunLoopManagerBoost::signalAndWaitForShutdown() {
	Lock condGuard(mut_);
	setShouldStop_(true);

	while (currentState_ != STATE_STOPPED) {
		stateCond_.wait(condGuard);
	}
}

} // end of namespace util

#endif // INCLUDED_RunLoopManagerBoost_h_GUID_50f7b2f1_493e_4395_25ca_df2f010a34bd
