/** @file
	@brief Header

	Define UTIL_EIGEN_TIE_UNIQUE_ASSIGN_ASSERT or UTIL_EIGEN_TIE_UNIQUE_ASSIGN_EXCEPTION
	to enable a debug-mode run-time	check that all tied variables are unique
	when a tie is assigned to. This is fairly computationally expensive.
	(std::set construction from iterators used to count unique pointers,
	n log n for a tie of dimension n)

	@date 2011

	@versioninfo@

	@author
	Ryan Pavlik
	<rpavlik@iastate.edu> and <abiryan@ryand.net>
	http://academic.cleardefinition.com/
	Iowa State University Virtual Reality Applications Center
	Human-Computer Interaction Graduate Program
*/

//          Copyright Iowa State University 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#ifndef INCLUDED_EigenTie_h_GUID_6c867047_6869_440c_8724_0d7733c6c7cd
#define INCLUDED_EigenTie_h_GUID_6c867047_6869_440c_8724_0d7733c6c7cd

// Internal Includes
// - none

// Library/third-party includes
#include <Eigen/Core>

// Standard includes
// - none
#if (defined(UTIL_EIGEN_TIE_UNIQUE_ASSIGN_ASSERT) || defined(UTIL_EIGEN_TIE_UNIQUE_ASSIGN_EXCEPTION)) && !defined(NDEBUG)
#	define UTIL_EIGEN_TIE_UNIQUE_ASSIGN_DO_CHECK
#	include <set>
namespace util {
	namespace {
		/// Utility function used when one of the unique assign checks is enabled.
		/// @relates TieVector
		void checkSameDimensions(int uniqueElts, int tieDimension);
	}
}
#endif


namespace util {

/// @addtogroup Math Math Utilities
/// @{
	namespace detail {
		// forward declaration
		template<int _Dim, typename _Scalar>
		class TieVector;

		// forward declaration
		template<int _Dim, typename _Scalar1, typename _Scalar2>
		bool operator==(TieVector<_Dim, _Scalar1> const& lhs, TieVector<_Dim, _Scalar2> const& rhs);

		/// Template class used to implement tying scalar variables or variable references
		/// together to interoperate (as lvalue or value) with vector math classes in Eigen.
		/// Construct these using util::TieVector() template functions.
		template<int _Dim = 3, typename _Scalar = double>
		class TieVector {
			public:
				typedef _Scalar Scalar;
				enum {
					Dim = _Dim
				};
				typedef Eigen::Matrix<_Scalar, _Dim, 1> Base;
			private:
				Scalar * _data[Dim];

				/// Internal method used for setting a tie vector from an eigen vector
				template<typename OtherDerived>
				EIGEN_STRONG_INLINE TieVector & _set(::Eigen::MatrixBase<OtherDerived> const& other) {
					EIGEN_STATIC_ASSERT_VECTOR_SPECIFIC_SIZE(OtherDerived, int(Dim));

#if defined(UTIL_EIGEN_TIE_UNIQUE_ASSIGN_DO_CHECK)
					std::set<Scalar *> uniqueElements(_data, _data + Dim);
					checkSameDimensions(static_cast<int>(uniqueElements.size()), Dim);
#endif

					for (int i = 0; i < Dim; ++i) {
						*(_data[i]) = other[i];
					}
					return *this;
				}

				template<int _D, typename _S>
				friend class TieVector;

				/// Private constructor used for chained syntax
				TieVector(TieVector < Dim - 1, Scalar > const& prev, Scalar & newVal) {
					for (int i = 0; i < Dim - 1; ++i) {
						_data[i] = prev._data[i];
					}
					_data[Dim - 1] = &newVal;
				}

			public:
#define EIGEN_TIE_STATIC_ASSERT_DIMENSION(_NUMARGS) assert(Dim == _NUMARGS && "YOU_DID_NOT_PASS_THE_CORRECT_AMOUNT_OF_ARGUMENTS_TO_TIE_TOGETHER")
//#define EIGEN_TIE_STATIC_ASSERT_DIMENSION(_NUMARGS) EIGEN_STATIC_ASSERT( (int(Dim) == int(_NUMARGS)), YOU_DID_NOT_PASS_THE_CORRECT_AMOUNT_OF_ARGUMENTS_TO_TIE_TOGETHER)
#define EIGEN_TIE_INITIALIZE_ELEMENT(_ELT) _data[_ELT] = &x ## _ELT

				TieVector(Scalar & x0) {
					EIGEN_TIE_STATIC_ASSERT_DIMENSION(1);
					EIGEN_TIE_INITIALIZE_ELEMENT(0);
				}

				TieVector(Scalar & x0, Scalar & x1) {
					EIGEN_TIE_STATIC_ASSERT_DIMENSION(2);
					EIGEN_TIE_INITIALIZE_ELEMENT(0);
					EIGEN_TIE_INITIALIZE_ELEMENT(1);
				}

				TieVector(Scalar & x0, Scalar & x1, Scalar & x2) {
					EIGEN_TIE_STATIC_ASSERT_DIMENSION(3);
					EIGEN_TIE_INITIALIZE_ELEMENT(0);
					EIGEN_TIE_INITIALIZE_ELEMENT(1);
					EIGEN_TIE_INITIALIZE_ELEMENT(2);
				}

				TieVector(Scalar & x0, Scalar & x1, Scalar & x2, Scalar & x3) {
					EIGEN_TIE_STATIC_ASSERT_DIMENSION(4);
					EIGEN_TIE_INITIALIZE_ELEMENT(0);
					EIGEN_TIE_INITIALIZE_ELEMENT(1);
					EIGEN_TIE_INITIALIZE_ELEMENT(2);
					EIGEN_TIE_INITIALIZE_ELEMENT(3);
				}

#undef EIGEN_TIE_INITIALIZE_ELEMENT
#undef EIGEN_TIE_STATIC_ASSERT_DIMENSION

				/// Assignment operator from another equal-sized tie vector
				/// assigns the values, not the tie references, by converting
				/// to an Eigen object and setting. This avoids aliasing problems.
				EIGEN_STRONG_INLINE TieVector& operator=(const TieVector& other) {
					if (this == &other) {
						return *this;
					}
					// Forcing conversion to Eigen::Vector to avoid aliasing problems
					return _set(other.convert());
				}

				/// Assignment operator from an Eigen object - allows you to
				/// assign any Eigen expression result or value to a TieVector.
				template<typename OtherDerived>
				EIGEN_STRONG_INLINE TieVector & operator=(const ::Eigen::MatrixBase <OtherDerived>& other) {
					return _set(other);
				}

				/// Returns a copy of the TieVector current value as a fixed-size Eigen::Vector
				EIGEN_STRONG_INLINE Base convert() const {
					Base temp;
					for (int i = 0; i < Dim; ++i) {
						temp[i] = *(_data[i]);
					}
					return temp;
				}

				/// Conversion operator to a fixed-size Eigen::Vector
				EIGEN_STRONG_INLINE operator Base() const {
					return convert();
				}

				/// Coefficient value accessor
				EIGEN_STRONG_INLINE Scalar coeff(int i) const {
					return *(_data[i]);
				}

				/// Operator allowing you to make arbitrary sizes of tie vectors
				/// with syntax like TieVector(x)(y)(z) = Eigen::Vector3d...
				EIGEN_STRONG_INLINE TieVector < Dim + 1, Scalar > operator()(Scalar & newVal) const {
					return TieVector < Dim + 1, Scalar > (*this, newVal);
				}
		};

		/// Stream output operator for TieVector that forwards to Eigen's stream output operators
		/// @relates TieVector
		template<typename StreamType, int _Dim, typename _Scalar>
		EIGEN_STRONG_INLINE StreamType & operator<<(StreamType & os, TieVector<_Dim, _Scalar> const& val) {
			os << val.convert();
			return os;
		}

		/// Equality comparison operator for two TieVector objects.
		/// @relates TieVector
		template<int _Dim, typename _Scalar1, typename _Scalar2>
		EIGEN_STRONG_INLINE bool operator==(TieVector<_Dim, _Scalar1> const& lhs, TieVector<_Dim, _Scalar2> const& rhs) {
			for (int i = 0; i < _Dim; ++i) {
				if (lhs.coeff(i) != rhs.coeff(i)) {
					return false;
				}
			}
			return true;
		}

		/// Equality comparison operator for a TieVector and a vector value in Eigen
		/// @relates TieVector
		template<int _Dim, typename _Scalar, typename Derived>
		EIGEN_STRONG_INLINE bool operator==(TieVector<_Dim, _Scalar> const& tieVal, ::Eigen::MatrixBase<Derived> const& other) {
			return tieVal.convert() == other;
		}

		/// Equality comparison operator for a vector value in Eigen and a TieVector - forwards to equality in other order
		/// @relates TieVector
		template<int _Dim, typename _Scalar, typename Derived>
		EIGEN_STRONG_INLINE bool operator==(::Eigen::MatrixBase<Derived> const& other, TieVector<_Dim, _Scalar> const& tieVal) {
			return tieVal == other;
		}

		/// Inequality comparison operator for two TieVector objects - forwards to equality operator
		/// @relates TieVector
		template<int _Dim, typename _Scalar1, typename _Scalar2>
		EIGEN_STRONG_INLINE bool operator!=(TieVector<_Dim, _Scalar1> const& lhs, TieVector<_Dim, _Scalar2> const& rhs) {
			return !(lhs == rhs);
		}

		/// Inequality comparison operator for a TieVector and a vector value in Eigen - forwards to equality operator
		/// @relates TieVector
		template<int _Dim, typename _Scalar, typename Derived>
		EIGEN_STRONG_INLINE bool operator!=(TieVector<_Dim, _Scalar> const& tieVal, ::Eigen::MatrixBase<Derived> const& other) {
			return !(tieVal == other);
		}

		/// Inequality comparison operator for a vector value in Eigen and a TieVector - forwards to equality operator
		/// @relates TieVector
		template<int _Dim, typename _Scalar, typename Derived>
		EIGEN_STRONG_INLINE bool operator!=(::Eigen::MatrixBase<Derived> const& other, TieVector<_Dim, _Scalar> const& tieVal) {
			return !(tieVal == other);
		}

	} // end of namespace detail

#define EIGEN_TIE_DEFINE_CREATION_FUNCTION( _DIM, _ARGS, _ARGNAMES) \
	template<typename Scalar> \
	EIGEN_STRONG_INLINE detail::TieVector<_DIM, Scalar> TieVector _ARGS { \
		return detail::TieVector<_DIM, Scalar> _ARGNAMES ; \
	} \

	EIGEN_TIE_DEFINE_CREATION_FUNCTION(1,
	                                   (Scalar & x0),
	                                   (x0)
	                                  )

	EIGEN_TIE_DEFINE_CREATION_FUNCTION(2,
	                                   (Scalar & x0, Scalar & x1),
	                                   (x0, x1)
	                                  )

	EIGEN_TIE_DEFINE_CREATION_FUNCTION(3,
	                                   (Scalar & x0, Scalar & x1, Scalar & x2),
	                                   (x0, x1, x2)
	                                  )

	EIGEN_TIE_DEFINE_CREATION_FUNCTION(4,
	                                   (Scalar & x0, Scalar & x1, Scalar & x2, Scalar & x3),
	                                   (x0, x1, x2, x3)
	                                  )

#undef EIGEN_TIE_DEFINE_CREATION_FUNCTION
/// @}

} // end of namespace util

#ifdef UTIL_EIGEN_TIE_UNIQUE_ASSIGN_DO_CHECK

#	if defined(UTIL_EIGEN_TIE_UNIQUE_ASSIGN_EXCEPTION)
#		include <stdexcept>

namespace util {
	namespace {
		inline void checkSameDimensions(int uniqueElts, int tieDimension) {
			if (uniqueElts != tieDimension) {
				throw std::logic_error("A variable appears more than once in a vector tie that is being assigned to!");
			}
		}
	}
}
#	elif defined(UTIL_EIGEN_TIE_UNIQUE_ASSIGN_ASSERT)
#		include <cassert>
namespace util {
	namespace {
		inline void checkSameDimensions(int uniqueElts, int tieDimension) {
			assert(uniqueElts == tieDimension && "A variable should not appear more than once in a vector tie that is being assigned to!");
		}
	}
}
#	endif

#	undef UTIL_EIGEN_TIE_UNIQUE_ASSIGN_DO_CHECK
#endif // UTIL_EIGEN_TIE_UNIQUE_ASSIGN_DO_CHECK

#endif // INCLUDED_EigenTie_h_GUID_6c867047_6869_440c_8724_0d7733c6c7cd
