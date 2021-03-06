/** @file
	@brief Header

	@date 2012

	@versioninfo@

	@author
	Ryan Pavlik
	<rpavlik@iastate.edu> and <abiryan@ryand.net>
	http://academic.cleardefinition.com/
	Iowa State University Virtual Reality Applications Center
	Human-Computer Interaction Graduate Program
*/

//          Copyright Iowa State University 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#ifndef INCLUDED_max_extended_h_GUID_437ff403_6ea8_420b_a3a6_e038a1973821
#define INCLUDED_max_extended_h_GUID_437ff403_6ea8_420b_a3a6_e038a1973821

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
#include <algorithm> // for std::max

namespace util {
	template<typename T>
	inline T max(T a1, T a2) {
		return std::max<T>(a1, a2);
	}

	template<typename T>
	inline T max(T a1, T a2, T a3) {
		return max<T>(max<T>(a1, a2), a3);
	}


	template<typename T>
	inline T max(T a1, T a2, T a3, T a4) {
		return max<T>(max<T>(a1, a2, a3), a4);
	}

	template<typename T>
	inline T max(T a1, T a2, T a3, T a4, T a5) {
		return max<T>(max<T>(a1, a2, a3, a4), a5);
	}

	template<typename T>
	inline T max(T a1, T a2, T a3, T a4, T a5, T a6) {
		return max<T>(max<T>(a1, a2, a3, a4, a5), a6);
	}
} // end of namespace util

#endif // INCLUDED_max_extended_h_GUID_437ff403_6ea8_420b_a3a6_e038a1973821
