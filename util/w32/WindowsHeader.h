/** @file
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

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#define UTILHEADERS_WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#define UTILHEADERS_NOMINMAX
#endif

#ifndef NOSERVICE
#define NOSERVICE
#define UTILHEADERS_NOSERVICE
#endif

#ifndef NOMCX
#define NOMCX
#define UTILHEADERS_NOMCX
#endif

#ifndef NOIME
#define NOIME
#define UTILHEADERS_NOIME
#endif

#ifndef STRICT
#define STRICT
#define UTILHEADERS_STRICT
#endif

#include <windows.h>

#ifdef UTILHEADERS_WIN32_LEAN_AND_MEAN
#undef UTILHEADERS_WIN32_LEAN_AND_MEAN
#undef WIN32_LEAN_AND_MEAN
#endif

#ifdef UTILHEADERS_NOMINMAX
#undef UTILHEADERS_NOMINMAX
#undef NOMINMAX
#endif

#ifdef UTILHEADERS_NOSERVICE
#undef UTILHEADERS_NOSERVICE
#undef NOSERVICE
#endif

#ifdef UTILHEADERS_NOMCX
#undef UTILHEADERS_NOMCX
#undef NOMCX
#endif

#ifdef UTILHEADERS_NOIME
#undef UTILHEADERS_NOIME
#undef NOIME
#endif

#ifdef UTILHEADERS_STRICT
#undef UTILHEADERS_STRICT
#undef STRICT
#endif

#endif // _WIN32

#endif // INCLUDED_WindowsHeader_h_GUID_223FABF1_C10A_4DBD_D0A3_71CDC510929F
