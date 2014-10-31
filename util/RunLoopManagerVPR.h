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
    RunLoopManagerVPR()
        : currentState_(STATE_STOPPED) {}

    /// @name StartingInterface
    /// @{
    /*void signalStart();*/
    void signalAndWaitForStart();
    /// @}

    /// @name LoopInterface
    /// @{
    void reportRunning();
    /*bool shouldContinue();*/
    /// @}

    /// @name ShutdownInterface
    /// @{
    void signalShutdown();
    void signalAndWaitForShutdown();
    /// @}


	/// @name LoopGuardInterface
	/// @{
	void reportStateChange_(RunningState s);
	void reportStopped_();
	/// @}
private:
    vpr::CondVar stateCond_;
    
    typedef vpr::Guard<vpr::CondVar> Lock;
    
    /// protected by condition variable
	volatile LoopGuardInterface::RunningState currentState_;
    

    /// One-way signalling flag from outside to the runloop.
    volatile bool shouldStop_;

};

inline void RunLoopManagerVPR::reportRunning() {
	reportStateChange_(LoopGuardInterface::STATE_RUNNING);
}

inline void RunLoopManagerVPR::reportStateChange_(RunLoopManagerBase::RunningState s) {
    {
        Lock condGuard(stateCond_);
        currentState_ = s;
    }
	stateCond_.broadcast();
}


inline void RunLoopManagerVPR::reportStopped_() {
    {
        Lock condGuard(stateCond_);
        setShouldStop_(false);
    }
    reportStateChange_(STATE_STOPPED);
}

inline void RunLoopManagerVPR::signalAndWaitForStart() {
    signalStart();
    {
		Lock condGuard(stateCond_);
        while (currentState_ != STATE_RUNNING) {
			stateCont_.wait(condGuard);
        }
    }
}

inline void RunLoopManagerVPR::signalShutdown() {
	Lock condGuard(stateCond_);
    if (currentState_ != STATE_STOPPED) {
        setShouldStop_(true);
    }
}


inline void RunLoopManagerVPR::signalAndWaitForShutdown() {
	Lock condGuard(stateCond_);

    if (currentState_ != STATE_STOPPED) {
        setShouldStop_(true);
    }

    do {
		stateCont_.wait(condGuard);
    } while (currentState_ != STATE_STOPPED);
}

} // end of namespace util

#endif // INCLUDED_RunLoopManagerVPR_h_GUID_fdb74ed3_ce09_429f_b76d_877a8c0a4f91

