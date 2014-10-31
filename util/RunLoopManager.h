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

#ifndef INCLUDED_RunLoopManager_h_GUID_cca4d4ff_064a_48bb_44db_b8414fb8d202
#define INCLUDED_RunLoopManager_h_GUID_cca4d4ff_064a_48bb_44db_b8414fb8d202

// Internal Includes
// - none

// Library/third-party includes
#ifdef UTIL_USE_BOOST_THREAD
#include <boost/thread.hpp>
#else
#include <vpr/Sync/CondVar.h>
#include <vpr/Sync/Guard.h>
#endif

// Standard includes
// - none

namespace util {
class StartingInterface {
public:
    virtual void signalStart() = 0;
    virtual void signalAndWaitForStart() = 0;
};

class LoopInterface {
public:
    virtual void reportRunning() = 0;
    virtual bool shouldContinue() = 0;
};

class ShutdownInterface {
public:
    virtual void signalShutdown() = 0;
    virtual void signalAndWaitForShutdown() = 0;
};

class RunLoopManager : public StartingInterface,
    public LoopInterface,
    public ShutdownInterface {
public:
    RunLoopManager()
        : currentState_(STATE_STOPPED)
        , shouldStop_(false) {}
    /// @name StartingInterface
    /// @{
    void signalStart();
    void signalAndWaitForStart();
    /// @}

    /// @name LoopInterface
    /// @{
    void reportRunning();
    bool shouldContinue();
    /// @}

    /// @name ShutdownInterface
    /// @{
    void signalShutdown();
    void signalAndWaitForShutdown();
    /// @}

private:
    enum RunningState {
        STATE_STOPPED,
        STATE_STARTING,
        STATE_RUNNING
    };
    void reportStateChange_(RunningState s);
    void reportStopped_();

    /// protected by condition variable
    volatile RunningState currentState_;
    vpr::CondVar stateCond_;

    /// One-way signalling flag from outside to the runloop.
    volatile bool shouldStop_;

    typedef vpr::Guard<vpr::CondVar> GuardType;

    friend class LoopGuard;
};

/// @brief RAII class to signal loop start and end.
class LoopGuard {
public:
    enum StartTime {
        REPORT_START_IMMEDIATELY,
        DELAY_REPORTING_START
    };
    LoopGuard(RunLoopManager &mgr, StartTime t = REPORT_START_IMMEDIATELY);
    ~LoopGuard();

private:
    RunLoopManager &mgr_;
};

inline LoopGuard::LoopGuard(RunLoopManager &mgr, LoopGuard::StartTime t)
    : mgr_(mgr) {
    mgr_.reportStateChange_(RunLoopManager::STATE_STARTING);
    if (t == REPORT_START_IMMEDIATELY) {
        mgr_.reportStateChange_(RunLoopManager::STATE_RUNNING);
    }
}

inline LoopGuard::~LoopGuard() {
    mgr_.reportStopped_();
}

inline void RunLoopManager::reportRunning() {
    reportStateChange_(STATE_RUNNING);
}

inline void
RunLoopManager::reportStateChange_(RunLoopManager::RunningState s) {
    GuardType condGuard(stateCond_);
    currentState_ = s;
    stateCond_.broadcast();
}

inline void RunLoopManager::reportStopped_() {
    {
        GuardType condGuard(stateCond_);
        shouldStop_ = false;
    }
    reportStateChange_(STATE_STOPPED);
}

inline void RunLoopManager::signalStart() {
    /// @todo how?
}

inline void RunLoopManager::signalAndWaitForStart() {
    signalStart();
    {
        GuardType condGuard(stateCond_);
        while (currentState_ != STATE_RUNNING) {
            stateCond_.wait();
        }
    }
}

inline bool RunLoopManager::shouldContinue() {
    return !shouldStop_;
}

inline void RunLoopManager::signalShutdown() {
    GuardType condGuard(stateCond_);
    if (currentState_ != STATE_STOPPED) {
        shouldStop_ = true;
    }
}

inline void RunLoopManager::signalAndWaitForShutdown() {
    GuardType condGuard(stateCond_);

    if (currentState_ != STATE_STOPPED) {
        shouldStop_ = true;
    }

    do {
        stateCond_.wait();
    } while (currentState_ != STATE_STOPPED);
}

} // end of namespace util

#endif // INCLUDED_RunLoopManager_h_GUID_cca4d4ff_064a_48bb_44db_b8414fb8d202
