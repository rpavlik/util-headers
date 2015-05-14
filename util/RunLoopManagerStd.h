/** @file
    @brief Header

    @versioninfo@
    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/


//               Copyright Sensics, Inc. 2015.
//  Distributed under the Boost Software License, Version 1.0.
//     (See accompanying file LICENSE_1_0.txt or copy at
//           http://www.boost.org/LICENSE_1_0.txt)

#ifndef INCLUDED_RunLoopManagerStd_h_GUID_34945132_5355_45A8_A7D6_073C7C8C235A
#define INCLUDED_RunLoopManagerStd_h_GUID_34945132_5355_45A8_A7D6_073C7C8C235A


// Internal Includes
#include "RunLoopManager.h"

// Library/third-party includes
// - none

// Standard includes
#include <thread>
#include <mutex>

namespace util {

    class RunLoopManagerStd : public RunLoopManagerBase {
    public:
        RunLoopManagerStd() : currentState_(STATE_STOPPED) {}

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
        std::mutex mut_;
        std::condition_variable stateCond_;

        typedef std::unique_lock<std::mutex> Lock;

        /// protected by condition variable
        volatile RunningState currentState_;
    };

    inline void RunLoopManagerStd::signalStart() {
        Lock condGuard(mut_);
        setShouldStop_(false);
    }

    inline void RunLoopManagerStd::signalAndWaitForStart() {
        signalStart();
        {
            Lock condGuard(mut_);
            while (currentState_ != STATE_RUNNING) {
                stateCond_.wait(condGuard);
            }
        }
    }

    inline void RunLoopManagerStd::signalShutdown() {
        Lock condGuard(mut_);
        setShouldStop_(true);
    }

    inline void RunLoopManagerStd::signalAndWaitForShutdown() {
        Lock condGuard(mut_);
        setShouldStop_(true);

        while (currentState_ != STATE_STOPPED) {
            stateCond_.wait(condGuard);
        }
    }

    inline void
        RunLoopManagerStd::reportStateChange_(RunLoopManagerBase::RunningState s) {
            {
                Lock condGuard(mut_);
                currentState_ = s;
            }
            stateCond_.notify_all();
    }

} // end of namespace util

#endif // INCLUDED_RunLoopManagerStd_h_GUID_34945132_5355_45A8_A7D6_073C7C8C235A

