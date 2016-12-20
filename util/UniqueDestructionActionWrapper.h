/** @file
    @brief Header

    @versioninfo@

    @date 2016

    @author
    Ryan Pavlik
    <ryan.pavlik@gmail.com>
    <http://ryanpavlik.com>
*/

// Copyright 2016 Sensics, Inc.
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef INCLUDED_UniqueDestructionActionWrapper_h_GUID_B8BF43A0_9DD7_449D_BE66_A7E3E98BB685
#define INCLUDED_UniqueDestructionActionWrapper_h_GUID_B8BF43A0_9DD7_449D_BE66_A7E3E98BB685

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
#include <utility>

namespace util {
/// Executes a functor on destruction, with cancel ability and move semantics.
///
/// Provide as type input a class, suitable to inherit from, with a nullary
/// operator() that performs the cleanup
/// operation.
///
/// Essentially an extensible/reusable version of the class returned from
/// `finally()`
template <typename Wrapped>
class UniqueDestructionActionWrapper : public Wrapped {
public:
    // using derived_type = Derived;
    using type = UniqueDestructionActionWrapper<Wrapped>;
    using wrapped_type = Wrapped;
    using Base = Wrapped;

    /// Default-constructible in inactive state.
    UniqueDestructionActionWrapper() : active_(false) {}

    /// non-copyable
    UniqueDestructionActionWrapper(type const &) = delete;
    UniqueDestructionActionWrapper &operator=(type const &) = delete;

    /// move-constructible
    UniqueDestructionActionWrapper(type &&other)
        : Base(std::move(other)), active_(other.active_) {
        other.cancel();
    }

    /// move-assignable
    UniqueDestructionActionWrapper &operator=(type &&other) {
        if (this == *other) {
            /// self assign
            return *this;
        }
        reset();
        base() = std::move(other.base());
        active_ = other.active_;
        other.cancel();
        return *this;
    }

    ~UniqueDestructionActionWrapper() { reset(); }

    explicit operator bool() const { return active_; }

    /// Perform the cleanup action
    void reset() {
        if (*this) {
            (*this)();
            cancel();
        }
    }

    /// Disable the cleanup action.
    void cancel() { active_ = false; }

    /// Named, perfect-forwarding constructor.
    template <typename... Args> static type construct(Args &&... args) {
        /// The initial dummy argument here keeps the constructor private and
        /// keeps the perfect forwarder from being
        /// overzealous in its overloading.
        return type(static_cast<DummyArg *>(nullptr),
                    std::forward<Args>(args)...);
    }

private:
    /// Wraps the cleanup action and keeps it out of the public interface of the
    /// wrapped object.
    void operator()() { Base::operator()(); }

    Base &base() { return *static_cast<Base *>(this); }

    Base const &base() const { return *static_cast<Base const *>(this); }
    /// Used to exclude the perfect-forwarding constructor from overload
    /// resolution.
    struct DummyArg;
    /// Perfect forwarding constructor invoked from construct()
    template <typename... Args>
    UniqueDestructionActionWrapper(DummyArg *, Args &&... args)
        : Base(std::forward<Args>(args)...) {}
    bool active_ = true;
};
} // namespace util
#endif // INCLUDED_UniqueDestructionActionWrapper_h_GUID_B8BF43A0_9DD7_449D_BE66_A7E3E98BB685
