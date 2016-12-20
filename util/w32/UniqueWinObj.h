/** @file
    @brief Header providing a unique_ptr-like wrapper for Windows objects (like
   handles, etc.) with policy-driven behavior.

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

#ifndef INCLUDED_UniqueWinObj_h_GUID_BEFD0CC7_1581_4C98_2E74_F5BFEC61C05B
#define INCLUDED_UniqueWinObj_h_GUID_BEFD0CC7_1581_4C98_2E74_F5BFEC61C05B

#undef UTIL_UNIQUE_WIN_OBJ_VERBOSE

// Internal Includes
#include <util/w32/WindowsHeader.h>

// Library/third-party includes
// - none

// Standard includes
#include <type_traits>
#include <utility>

#ifdef UTIL_UNIQUE_WIN_OBJ_VERBOSE
#include <cstdint>
#include <iostream>
#include <util/ValToHex.h>
#endif

namespace util {
namespace w32 {
    namespace unique_win_obj {
        template <typename Derived> struct BaseSingleInvalidValuePolicy {
            /// default is that only one value is invalid.
            template <typename T> static bool valid(T const &v) {
                return v != Derived::get();
            }
        };
        /// UniqueWinObj InvalidValuePolicy implementation: where
        /// INVALID_HANDLE_VALUE means invalid.
        struct InvalidHandleValue
            : BaseSingleInvalidValuePolicy<InvalidHandleValue> {
            static HANDLE get() { return INVALID_HANDLE_VALUE; }

            /// here we claim that both nullptr and the canonical
            /// INVALID_HANDLE_VALID
            template <typename T> static bool valid(T const &v) {
                return v != get() && v != nullptr;
            }
        };

        /// UniqueWinObj InvalidValuePolicy implementation: where
        /// INVALID_HANDLE_VALUE is the canonical invalid
        /// representation, but nullptr/0 is also considered invalid.
        struct InvalidHandleOrNullptr : InvalidHandleValue {
            using base = InvalidHandleValue;
            /// here we claim that both nullptr and the canonical
            /// INVALID_HANDLE_VALID
            template <typename T> static bool valid(T const &v) {
                return v != base::get() && v != nullptr;
            }
        };

        /// UniqueWinObj InvalidValuePolicy implementation: where nullptr means
        /// invalid.
        struct NullptrValue : BaseSingleInvalidValuePolicy<NullptrValue> {
            static std::nullptr_t get() { return nullptr; }
        };

        /// UniqueWinObj DeleterPolicy implementation: uses CloseHandle.
        struct CloseHandleDeleter {
            static void apply(HANDLE h) { ::CloseHandle(h); }
        };

        /// UniqueWinObj DeleterPolicy implementation: uses LocalFree for
        /// LocalAlloc MEM_FIXED.
        /// @todo needs different version for windows runtime which lacks
        /// LocalFree - see MSDN docs for FormatMessage.
        struct LocalFreeDeleter {
            template <typename T> static void apply(T &&v) {
                ::LocalFree(std::forward<T>(v));
            }
        };

        template <typename ContainedType> struct DefaultContainedTypeTraits {
            using value_type = ContainedType;
            using return_type = ContainedType;
            using param_type = ContainedType;
            using address_type = typename std::add_pointer<ContainedType>::type;
            static address_type getAddress(value_type &v) { return &v; }
        };

        template <typename ContainedType> struct ArrayContainedTypeTraits {
            using value_type = ContainedType;
            using return_type = ContainedType;
            using param_type = ContainedType;
            using address_type = ContainedType;
            static address_type getAddress(value_type &v) { return v; }
        };

        template <typename ContainedType, typename DeleterPolicy,
                  typename InvalidValuePolicy,
                  typename ContainedTypeTraits =
                      DefaultContainedTypeTraits<ContainedType>>
        class UniqueWinObj {
        public:
            using type = UniqueWinObj<ContainedType, DeleterPolicy,
                                      InvalidValuePolicy, ContainedTypeTraits>;
            using traits = ContainedTypeTraits;
            using value_type = typename ContainedTypeTraits::value_type;
            using return_type = typename ContainedTypeTraits::return_type;
            using param_type = typename ContainedTypeTraits::param_type;
            using validity_policy = InvalidValuePolicy;

            /// Default constructor (empty/invalid)
            UniqueWinObj() : v_(invalid_value()) {}

            /// Explicit constructor taking ownership
            explicit UniqueWinObj(param_type v) : v_(v) {}

            /// non-copyable
            UniqueWinObj(type const &) = delete;

            /// non-copy-assignable
            UniqueWinObj &operator=(type const &) = delete;
#if 0
            /// move constructor
            UniqueWinObj(type&& other) : v_(other.release()) {}
#endif

            /// move construction from a different InvalidValuePolicy - converts
            /// foreign invalid values to our internal
            /// invalid values.
            template <typename OtherInvalidValuePolicy>
            UniqueWinObj(UniqueWinObj<ContainedType, DeleterPolicy,
                                      OtherInvalidValuePolicy,
                                      ContainedTypeTraits> &&other)
                : v_(other.valid() ? other.release() : invalid_value()) {}
#if 0
            /// move assignment
            UniqueWinObj& operator=(type&& other) {
                if (this == &other) {
                    // avoid self-move-assignment
                    return *this;
                }
                // clear self
                reset();
                using namespace std;
                // swap with other
                swap(v_, other.v_);
                return *this;
            }
#endif
            /// move assignment from a different InvalidValuePolicy - converts
            /// foreign invalid values to our internal
            /// invalid values.
            template <typename OtherInvalidValuePolicy>
            UniqueWinObj &operator=(UniqueWinObj<ContainedType, DeleterPolicy,
                                                 OtherInvalidValuePolicy,
                                                 ContainedTypeTraits> &&other) {

                if (static_cast<void *>(this) == static_cast<void *>(&other)) {
                    // avoid self-move-assignment
                    return *this;
                }
                // clear self
                reset();
                if (other.valid()) {
                    v_ = other.release();
                }
                // if not valid, leaves our default invalid value - allows
                // handling assignment from other invalid value
                // policies.
                return *this;
            }

            /// Destructor
            ~UniqueWinObj() {
                if (valid()) {
                    call_deleter(v_);
                }
            }
            /// gets validity
            bool valid() const { return InvalidValuePolicy::valid(v_); }
            /// gets validity
            explicit operator bool() const { return valid(); }

            /// Access contained value
            return_type get() const { return v_; }

            /// Get the contained value and remove it from the control of this
            /// container.
            value_type release() {
                value_type ret = v_;
                v_ = invalid_value();
                return ret;
            }

            /// Resets to the given value, releasing/deleting previous object if
            /// applicable.
            bool reset(param_type newVal = type::invalid_value()) {
                if (valid() && (newVal != v_)) {
                    call_deleter(v_);
                }
                v_ = newVal;
                return valid();
            }

            /// Helper to wrap usage of InvalidValuePolicy
            static value_type invalid_value() {
                return InvalidValuePolicy::get();
            }

        private:
            /// Helper to wrap usage of DeleterPolicy
            static void call_deleter(value_type v) { DeleterPolicy::apply(v); }
            value_type v_;
        };

        /// Helper class that permits assigning to UniqueWinObj without
        /// overloading operator&
        template <typename WinObjType> class UniqueWinObjAssignmentWrapperBase {
        public:
            using type = UniqueWinObjAssignmentWrapperBase<WinObjType>;
            using wrapped_type = WinObjType;
            using traits = typename wrapped_type::traits;
            using value_type = typename wrapped_type::value_type;
            using return_type = typename wrapped_type::return_type;
            using param_type = typename wrapped_type::param_type;
            using address_type = typename traits::address_type;

            /// constructor
            explicit UniqueWinObjAssignmentWrapperBase(WinObjType &obj)
                : obj_(obj) {
#ifdef UTIL_UNIQUE_WIN_OBJ_VERBOSE
                outputInstanceAddr();
                std::cout << " constructed" << std::endl;
#endif
            }

            /// destructor - sets the wrapped object to the received value
            ~UniqueWinObjAssignmentWrapperBase() {
#ifdef UTIL_UNIQUE_WIN_OBJ_VERBOSE
                outputInstanceAddr();
                std::cout << " ~UniqueWinObjAssignmentWrapperBase(), active = "
                          << active_;
                std::cout << ", holding "
                          << valToHex(reinterpret_cast<std::uintptr_t>(v_));
                std::cout << std::endl;
#endif
                if (active_) {
                    obj_.reset(v_);
                }
            }

            /// non-copy-constructible
            UniqueWinObjAssignmentWrapperBase(type const &) = delete;

            /// Move construction
            UniqueWinObjAssignmentWrapperBase(type &&other)
                : v_(other.v_), obj_(other.obj_), active_(other.active_) {
#ifdef UTIL_UNIQUE_WIN_OBJ_VERBOSE
                outputInstanceAddr();
                std::cout << " UniqueWinObjAssignmentWrapperBase in move "
                             "construction from ";
                other.outputInstanceAddr();
                std::cout << std::endl;
#endif

                /// de-activate the moved-from wrapper.
                other.active_ = false;
            }

            /// non-copy-assignable
            type &operator=(type const &) = delete;

            /// Conversion to pointer type: returns the address of our inner
            /// temporary storage.
            operator address_type() {
#ifdef UTIL_UNIQUE_WIN_OBJ_VERBOSE
                outputInstanceAddr();
                std::cout << " UniqueWinObjAssignmentWrapperBase conversion to "
                             "address_type"
                          << std::endl;
#endif
                return traits::getAddress(v_);
            }

            address_type getAddressOf() {
#ifdef UTIL_UNIQUE_WIN_OBJ_VERBOSE
                outputInstanceAddr();
                std::cout << " UniqueWinObjAssignmentWrapperBase conversion to "
                             "address_type"
                          << std::endl;
#endif
                return traits::getAddress(v_);
            }

        protected:
#ifdef UTIL_UNIQUE_WIN_OBJ_VERBOSE
            void outputInstanceAddr() const {
                std::cout << valToHex(reinterpret_cast<std::uintptr_t>(this));
            }
#endif
            value_type v_ = wrapped_type::invalid_value();

        private:
            WinObjType &obj_;
            bool active_ = true;
        };

        // tag dispatching for choosing which overloads to provide for a given
        // object assignment wrapper
        struct DefaultAssignmentWrapperTag;
        struct ArrayAssignmentWrapperTag;
        template <typename WinObjType> struct AssignmentWrapperDispatch {
            using wrapped_type = WinObjType;
            using value_type = typename wrapped_type::value_type;
            template <bool Condition, typename T, typename U>
            using conditional_t =
                typename std::conditional<Condition, T, U>::type;
            using type = conditional_t<std::is_pointer<value_type>::value,
                                       ArrayAssignmentWrapperTag,
                                       DefaultAssignmentWrapperTag>;
        };

        template <typename WinObjType,
                  typename Tag =
                      typename AssignmentWrapperDispatch<WinObjType>::type>
        class UniqueWinObjAssignmentWrapper;

        /// Default conversion op: just to address (provided by base class)
        template <typename WinObjType>
        class UniqueWinObjAssignmentWrapper<WinObjType,
                                            DefaultAssignmentWrapperTag>
            : public UniqueWinObjAssignmentWrapperBase<WinObjType> {
        public:
            using type =
                UniqueWinObjAssignmentWrapper<WinObjType,
                                              DefaultAssignmentWrapperTag>;
            using Base = UniqueWinObjAssignmentWrapperBase<WinObjType>;
            using wrapped_type = WinObjType;
            using traits = typename wrapped_type::traits;
            /// constructor
            explicit UniqueWinObjAssignmentWrapper(WinObjType &obj)
                : Base(obj) {}

            /// Move construction
            UniqueWinObjAssignmentWrapper(type &&other)
                : Base(std::move(other)) {}
        };

        /// Arrays get default conversion (to address) as well as value access.
        template <typename WinObjType>
        class UniqueWinObjAssignmentWrapper<WinObjType,
                                            ArrayAssignmentWrapperTag>
            : public UniqueWinObjAssignmentWrapperBase<WinObjType> {
        public:
            using type =
                UniqueWinObjAssignmentWrapper<WinObjType,
                                              ArrayAssignmentWrapperTag>;
            using Base = UniqueWinObjAssignmentWrapperBase<WinObjType>;
            using wrapped_type = WinObjType;
            using value_type = typename wrapped_type::value_type;
            /// constructor
            explicit UniqueWinObjAssignmentWrapper(WinObjType &obj)
                : Base(obj) {}

            /// Move construction
            UniqueWinObjAssignmentWrapper(type &&other)
                : Base(std::move(other)) {}

            /// conversion to value_type for arrays.
            operator value_type() {
#ifdef UTIL_UNIQUE_WIN_OBJ_VERBOSE
                outputInstanceAddr();
                std::cout << " UniqueWinObjAssignmentWrapper (Array) "
                             "conversion operator returning value_type"
                          << std::endl;
#endif
                return Base::v_;
            }
        };

        /// Helper function, to use instead of operator&, when passing a
        /// UniqueWinObj as an output parameter
        template <typename WinObjType>
        inline UniqueWinObjAssignmentWrapper<WinObjType>
        Attach(WinObjType &obj) {
            return UniqueWinObjAssignmentWrapper<WinObjType>(obj);
        }

        template <typename DeleterPolicy = CloseHandleDeleter,
                  typename InvalidValuePolicy = InvalidHandleValue>
        using UniqueHandle =
            UniqueWinObj<HANDLE, DeleterPolicy, InvalidValuePolicy>;

    } // namespace unique_win_obj
    using unique_win_obj::UniqueWinObj;
    using unique_win_obj::UniqueHandle;
    using UniqueHandleForPipes = UniqueHandle<>;
    using UniqueProcessHandle = UniqueHandle<unique_win_obj::CloseHandleDeleter,
                                             unique_win_obj::NullptrValue>;
    template <typename T>
    using UniqueLocalAllocStruct =
        UniqueWinObj<T *, unique_win_obj::LocalFreeDeleter,
                     unique_win_obj::NullptrValue>;
} // namespace w32
} // namespace util

#endif // INCLUDED_UniqueWinObj_h_GUID_BEFD0CC7_1581_4C98_2E74_F5BFEC61C05B
