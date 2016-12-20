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

#ifndef INCLUDED_Win32Utilities_h_GUID_C1332DA8_3583_4596_0ED7_A0E7AC039F50
#define INCLUDED_Win32Utilities_h_GUID_C1332DA8_3583_4596_0ED7_A0E7AC039F50

// Internal Includes
#include <util/ValToHex.h>
#include <util/w32/WindowsHeader.h>

// Library/third-party includes
// - none

// Standard includes
#include <string>

namespace util {
namespace w32 {

    static inline bool bool_cast(BOOL winBool) { return winBool == TRUE; }

    static inline std::string dwordToHex(DWORD val) {
        return ::util::valToHex(val);
    }

} // namespace w32
} // namespace util
#endif
