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

#ifndef INCLUDED_LastErrorToString_h_GUID_DE8D6748_B808_4C8A_497E_F92350D11D41
#define INCLUDED_LastErrorToString_h_GUID_DE8D6748_B808_4C8A_497E_F92350D11D41

// Internal Includes
#include <util/w32/UniqueWinObj.h>
#include <util/w32/Win32Utilities.h>
#include <util/w32/WindowsHeader.h>

// Library/third-party includes
// - none

// Standard includes
#include <string>

namespace util {
namespace w32 {

    static inline std::string lastErrorToString(DWORD err) {
        using LocalFreeStringType =
            UniqueWinObj<LPSTR, unique_win_obj::LocalFreeDeleter,
                         unique_win_obj::NullptrValue>;
        LocalFreeStringType buf;

        auto chars = ::FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            reinterpret_cast<LPSTR>(
                Attach(buf).getAddressOf()), //< yes, that's a
                                             //< pointer-to-a-pointer being cast
            //< to just a pointer. That's the Windows API.
            0, nullptr);
        if (chars != 0) {
            return std::string(buf.get());
        }
        /// There was an error, ironically. Return an empty string.
        return "Error converting last error to string: " +
               dwordToHex(::GetLastError());
    }
}
}
#endif // INCLUDED_LastErrorToString_h_GUID_DE8D6748_B808_4C8A_497E_F92350D11D41
