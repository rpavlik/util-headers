/** @file
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

#ifndef INCLUDED_RunLoopManagerVPR_h_GUID_fdb74ed3_ce09_429f_b76d_877a8c0a4f91
#define INCLUDED_RunLoopManagerVPR_h_GUID_fdb74ed3_ce09_429f_b76d_877a8c0a4f91

// Internal Includes
#include "RunLoopManager.h"

// Library/third-party includes
#include <vpr/Sync/CondVar.h>
#include <vpr/Sync/Guard.h>

// Standard includes
// - none

namespace util {

	class RunLoopManagerVPR : public RunLoopManagerBase {
		public:
			RunLoopManagerVPR() : currentState_(STATE_STOPPED) {}

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
			vpr::CondVar stateCond_;

			typedef vpr::Guard<vpr::CondVar> Lock;

			/// protected by condition variable
			volatile RunningState currentState_;
	};

	inline void RunLoopManagerVPR::signalStart() {
		Lock condGuard(stateCond_);
		setShouldStop_(false);
	}

	inline void
	RunLoopManagerVPR::reportStateChange_(RunLoopManagerBase::RunningState s) {
		{
			Lock condGuard(stateCond_);
			currentState_ = s;
		}
		stateCond_.broadcast();
	}

	inline void RunLoopManagerVPR::signalAndWaitForStart() {
		signalStart();
		{
			Lock condGuard(stateCond_);
			while (currentState_ != STATE_RUNNING) {
				stateCond_.wait(condGuard);
			}
		}
	}

	inline void RunLoopManagerVPR::signalShutdown() {
		Lock condGuard(stateCond_);
		setShouldStop_(true);
	}

	inline void RunLoopManagerVPR::signalAndWaitForShutdown() {
		Lock condGuard(stateCond_);
		setShouldStop_(true);

		while (currentState_ != STATE_STOPPED) {
			stateCond_.wait(condGuard);
		}
	}

} // end of namespace util

#endif // INCLUDED_RunLoopManagerVPR_h_GUID_fdb74ed3_ce09_429f_b76d_877a8c0a4f91
