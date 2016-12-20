/** @file
    @brief Format an integer value as a fully-padded hex string.

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

#ifndef INCLUDED_ValToHex_h_GUID_BB84C322_FA14_46BC_A1E2_321AE086B23E
#define INCLUDED_ValToHex_h_GUID_BB84C322_FA14_46BC_A1E2_321AE086B23E

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
#include <climits>
#include <iomanip>
#include <sstream>
#include <string>

namespace util {
/// @brief Format an integer value as a fully-padded hex string.
template <typename T>
static inline std::string valToHex(T val, bool prefix0x = true) {
    // two hex digits per byte - a hex digit can handle 4 bits.
    static const auto hexDigits = sizeof(T) * CHAR_BIT / 4;
    std::ostringstream os;
    if (prefix0x) {
        os << "0x";
    }
    os << std::setfill('0') << std::setw(hexDigits) << std::hex << val;
    return os.str();
}
} // namespace util

#endif // INCLUDED_ValToHex_h_GUID_BB84C322_FA14_46BC_A1E2_321AE086B23E
