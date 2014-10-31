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

class LoopGuardInterface {
public:
	enum RunningState {
		STATE_STOPPED,
		STATE_STARTING,
		STATE_RUNNING
	};
	virtual void reportStateChange_(RunningState s) = 0;
	virtual void reportStopped_();
};

namespace policy {
#ifdef UTIL_USE_BOOST_THREAD
	class BoostConditionVarAndMutex {
	public:
		typedef boost::unique_lock<boost::mutex> GuardType;

		GuardType lock() {
			return GuardType(mut_);
		}
		void wait(GuardType & g) {
			stateCond_.wait(g);
		}

		void notify() {
			stateCond_.notify_all();
		}
	private:
		boost::mutex mut_;
		boost::condition_variable stateCond_;

	};
	typedef BoostConditionVarAndMutex DefaultCondVarPolicy;
#else
	class VPRConditionVarAndMutex {
	public:
		typedef vpr::Guard<vpr::CondVar> GuardType;

		GuardType lock() {
			return GuardType(stateCond_);
		}
		void wait(GuardType &) {
			stateCond_.wait();
		}

		void notify() {
			stateCond_.broadcast();
		}
	private:
		vpr::CondVar stateCond_;
	};
	typedef VPRConditionVarAndMutex DefaultCondVarPolicy;
#endif
}// end of namespace policy

template<typename CondVarPolicy = policy::DefaultCondVarPolicy>
class RunLoopManager : public StartingInterface,
    public LoopInterface,
    public ShutdownInterface,
	private LoopGuardInterface {
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
	/// @name LoopGuardInterface
	/// @{
	virtual void reportStateChange_(RunningState s);
	virtual void reportStopped_();
	/// @}

    /// protected by condition variable
	volatile LoopGuardInterface::RunningState currentState_;
    

    /// One-way signalling flag from outside to the runloop.
    volatile bool shouldStop_;
	CondVarPolicy condVarWrapper_;
	typedef typename CondVarPolicy::GuardType GuardType;

    friend class LoopGuard;
};

/// @brief RAII class to signal loop start and end.
class LoopGuard {
public:
    enum StartTime {
        REPORT_START_IMMEDIATELY,
        DELAY_REPORTING_START
    };
    LoopGuard(LoopGuardInterface &mgr, StartTime t = REPORT_START_IMMEDIATELY);
    ~LoopGuard();

private:
	LoopGuardInterface &mgr_;
};

inline LoopGuard::LoopGuard(LoopGuardInterface &mgr, LoopGuard::StartTime t)
    : mgr_(mgr) {
	mgr_.reportStateChange_(LoopGuardInterface::STATE_STARTING);
    if (t == REPORT_START_IMMEDIATELY) {
		mgr_.reportStateChange_(LoopGuardInterface::STATE_RUNNING);
    }
}

inline LoopGuard::~LoopGuard() {
    mgr_.reportStopped_();
}

template<typename CondVarPolicy>
inline void RunLoopManager<CondVarPolicy>::reportRunning() {
	reportStateChange_(LoopGuardInterface::STATE_RUNNING);
}

template<typename CondVarPolicy>
inline void RunLoopManager<CondVarPolicy>::reportStateChange_(typename RunLoopManager::RunningState s) {
    GuardType condGuard(condVarWrapper_.lock());
    currentState_ = s;
	condVarWrapper_.notify();
}

template<typename CondVarPolicy>
inline void RunLoopManager<CondVarPolicy>::reportStopped_() {
    {
		GuardType condGuard(condVarWrapper_.lock());
        shouldStop_ = false;
    }
    reportStateChange_(STATE_STOPPED);
}

template<typename CondVarPolicy>
inline void RunLoopManager<CondVarPolicy>::signalStart() {
    /// @todo how?
}

template<typename CondVarPolicy>
inline void RunLoopManager<CondVarPolicy>::signalAndWaitForStart() {
    signalStart();
    {
		GuardType condGuard(condVarWrapper_.lock());
        while (currentState_ != STATE_RUNNING) {
			condVarWrapper_.wait(condGuard);
        }
    }
}

template<typename CondVarPolicy>
inline bool RunLoopManager<CondVarPolicy>::shouldContinue() {
    return !shouldStop_;
}

template<typename CondVarPolicy>
inline void RunLoopManager<CondVarPolicy>::signalShutdown() {
	GuardType condGuard(condVarWrapper_.lock());
    if (currentState_ != STATE_STOPPED) {
        shouldStop_ = true;
    }
}

template<typename CondVarPolicy>
inline void RunLoopManager<CondVarPolicy>::signalAndWaitForShutdown() {
	GuardType condGuard(condVarWrapper_.lock());

    if (currentState_ != STATE_STOPPED) {
        shouldStop_ = true;
    }

    do {
		condVarWrapper_.wait(condGuard);
    } while (currentState_ != STATE_STOPPED);
}

} // end of namespace util

#endif // INCLUDED_RunLoopManager_h_GUID_cca4d4ff_064a_48bb_44db_b8414fb8d202
