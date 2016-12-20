#!/usr/bin/env lua
--[[
Script to generate an include header wrapping windows.h, defining
symbols to make the include minimal, and undefining to initial state
when done. -- Ryan Pavlik

Copyright 2015 Sensics, Inc.
Distributed under the Boost Software License, Version 1.0.
   (See accompanying file LICENSE_1_0.txt or copy at
         http://www.boost.org/LICENSE_1_0.txt)

]]

symbols = {"WIN32_LEAN_AND_MEAN", "NOMINMAX", "NOSERVICE", "NOMCX", "NOIME", "STRICT"}
template = {
    before = {
        "#ifndef ",
        "\n#define ",
        "\n#define UTILHEADERS_",
        "\n#endif\n"
    };
    after = {
        "#ifdef UTILHEADERS_",
        "\n#undef UTILHEADERS_",
        "\n#undef ",
        "\n#endif\n"
    }
}

print [[/** @file
    @brief Header to minimally include windows.h

    Generated in util-headers project by WindowsHeader_generator.lua

    @versioninfo@

    @date 2015-2016

    @author
    Ryan Pavlik
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

/*
// Copyright 2015-2016 Sensics, Inc.
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef INCLUDED_WindowsHeader_h_GUID_223FABF1_C10A_4DBD_D0A3_71CDC510929F
#define INCLUDED_WindowsHeader_h_GUID_223FABF1_C10A_4DBD_D0A3_71CDC510929F

#ifdef _WIN32
]]

for _, symbol in ipairs(symbols) do
    print(table.concat(template.before, symbol))
end

print [[
#include <windows.h>
]]


for _, symbol in ipairs(symbols) do
    print(table.concat(template.after, symbol))
end

print [[
#endif // _WIN32

#endif // INCLUDED_WindowsHeader_h_GUID_223FABF1_C10A_4DBD_D0A3_71CDC510929F]]
