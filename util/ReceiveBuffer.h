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
#ifndef INCLUDED_ReceiveBuffer_h_GUID_2d1681f0_0ffe_495d_8ec9_fd730b801721
#define INCLUDED_ReceiveBuffer_h_GUID_2d1681f0_0ffe_495d_8ec9_fd730b801721

// Internal Includes
#include "VectorSimulator.h"
#include <util/booststdint.h>

// Library/third-party includes
#include <boost/array.hpp>
#include <boost/integer.hpp>
#include <boost/assert.hpp>
#include <util/BoostAssertMsg.h>

// Standard includes
#include <algorithm>
#include <cmath>

namespace util {

	/// @brief A receive buffer class, designed for piecemeal or bulk back-insertion/removal,
	/// and front access/removal.
	///
	/// To minimize the number of times the buffer contents must be shifted
	/// internally in the wrapped container, suggest setting SIZE to twice
	/// your maximum message size.
	template<std::size_t SIZE, typename Values = stdint::uint8_t>
	class ReceiveBuffer : public vector_simulator<ReceiveBuffer<SIZE, Values>, Values, typename boost::uint_value_t< SIZE >::least> {
		public:
			typedef ReceiveBuffer<SIZE, Values> type;
			typedef vector_simulator<type, Values, typename boost::uint_value_t< SIZE >::least> base_type;

			typedef Values value_type;
			typedef value_type & reference;
			typedef value_type const & const_reference;
			typedef typename boost::uint_value_t< SIZE >::least size_type;
			typedef boost::array<value_type, SIZE> wrapped_type;
			typedef typename wrapped_type::iterator iterator;
			typedef typename wrapped_type::const_iterator const_iterator;

			enum {
				CAPACITY = SIZE
			};

			/// @brief Default constructor
			ReceiveBuffer()
				: _begin(0)
				, _pastEnd(0)
				, _contents() {
			}

			/// @brief Copy constructor
			ReceiveBuffer(type const& other)
				: _begin(0)
				, _pastEnd(other.size())
				, _contents() {
				idealContentsCopy(other, *this);
			}

			/// @brief Copy from array
			ReceiveBuffer(value_type * v, size_type len)
				: _begin(0)
				, _pastEnd(len)
				, _contents() {
				std::copy(v, v + len, _contents.begin());
			}

			/// @brief Copy from array
			template<typename InputIterator>
			ReceiveBuffer(InputIterator first, InputIterator last)
				: _begin(0)
				, _pastEnd(0)
				, _contents() {
				push_back(first, last);
			}

			/// @brief Assignment operator from another buffer.
			///
			/// Performs an ideal copy. Invalidates iterators.
			/// Self-assignment performs a clean-up (invalidating iterators)
			type & operator=(type const& other) {
				if (this == &other) {
					slide_contents_forward(); /// A cleanup externally-nearly-no-op
				} else {
					idealContentsCopy(other, *this);
				}
				return *this;
			}

			/// @brief Is the buffer empty?
			bool empty() const {
				return _begin == _pastEnd;
			}

			/// @brief Number of elements currently in buffer
			size_type size() const {
				return _pastEnd - _begin;
			}

			/// @brief Max size is fixed by type declaration
			static size_type max_size() {
				return CAPACITY;
			}

			/// @brief Direct access to (read-only) data
			const value_type * data() const {
				return &_contents[adjusted_index(0)];
			}

			/// @brief Element reference access operator
			///
			/// @note Does not forcibly check bounds!
			reference operator[](size_type i) {
				BOOST_ASSERT_MSG(i < size(), "out of range");
				return _contents[adjusted_index(i)];
			}

			/// @brief Element const reference access operator
			///
			/// @note Does not forcibly check bounds!
			const_reference operator[](size_type i) const {
				BOOST_ASSERT_MSG(i < size(), "out of range");
				return _contents[adjusted_index(i)];
			}

			/// @brief Reset begin and end so the buffer is empty.
			///
			/// @note Does not call destructors!
			void clear() {
				_begin = 0;
				_pastEnd = 0;
			}

			/// @brief Single element push back
			///
			/// @note May invalidate iterators!
			/// @note Does not forcibly check bounds!
			void push_back(const_reference x) {
				increment_pastEnd();
				(*this)[size() - 1] = x;
			}

			/// @brief Range push back
			///
			/// @note May invalidate iterators!
			/// @note Does not forcibly check bounds!
			template<typename InputIterator>
			void push_back(InputIterator input_begin, InputIterator input_end) {
				ensure_space(input_end - input_begin);
				std::copy(input_begin, input_end, end());
				_pastEnd += input_end - input_begin;

				verify_invariants(); // just because I'm a little nervous
			}

			/// @brief External Buffer Function Capability - pass a functor
			/// that takes an iterator and a max count, and returns number
			/// of bytes buffered.
			///
			/// This method will ensure available space, then call the functor
			/// to perform the buffering.
			///
			/// @note May invalidate iterators!
			template<typename Functor>
			size_type bufferFromExternalFunctorRef(Functor & f, size_type n) {
				n = std::min<size_type>(n, max_size() - size());
				ensure_space(n);
				size_type actual = f(_contents.begin() + _pastEnd, n);
				_pastEnd += actual;
				verify_invariants(); // just because I'm a little nervous
				return actual;
			}

			/// @brief Pop back, by default a single element
			///
			/// @note Does not call destructors!
			/// @note Does not forcibly check bounds!
			value_type pop_back(size_type count = 1) {
				value_type ret(base_type::back());
				for (size_type i = 0; i < count; ++i) {
					decrement_pastEnd();
				}
				return ret;
			}

			/// @brief Pop front, by default a single element
			///
			/// @note Does not call destructors!
			/// @note Does not forcibly check bounds!
			value_type pop_front(size_type count = 1) {
				value_type ret(base_type::front());
				for (size_type i = 0; i < count; ++i) {
					increment_begin();
				}
				return ret;
			}

			/// @brief Return an iterator to the beginning of the buffer
			iterator begin() {
				return _contents.begin() + _begin;
			}

			/// @brief Return an const_iterator to the beginning of the buffer
			const_iterator begin() const {
				return _contents.begin() + _begin;
			}

			/// @brief Return an iterator to the end of the buffer
			iterator end() {
				return _contents.begin() + _pastEnd;
			}

			/// @brief Return an const_iterator to the end of the buffer
			const_iterator end() const {
				return _contents.begin() + _pastEnd;
			}

			/// @brief Erase an element - similar to std::vector<>::erase
			///
			/// @note Does not call destructors! Invalidates iterators!
			iterator erase(iterator position) {
				return erase(position, position + 1);
			}

			/// @brief Erase a range - similar to std::vector<>::erase
			///
			/// @note Does not call destructors! Invalidates iterators!
			iterator erase(iterator first, iterator last) {
				size_type startIndex = first - begin();
				size_type endIndex = last - begin();
				BOOST_ASSERT_MSG(first < last, "Iterators in wrong order!");
				size_type len = endIndex - startIndex;
				if (len == 0) {
					return first;
				}
				BOOST_ASSERT_MSG(len <= size(), "Can't erase more than are there");
				if (startIndex == 0) {
					pop_front(len);
				} else if (last == end()) {
					pop_back(len);
				} else {
					// Ick, the slow case.
					BOOST_ASSERT_MSG(0 < startIndex, "Iterator to erase before our beginning");
					BOOST_ASSERT_MSG(endIndex < size(), "Iterator to erase after our end");
					std::copy(last, end(), begin() + startIndex);
					for (size_type i = 0; i < len; ++i) {
						decrement_pastEnd();
					}
				}
				return begin() + startIndex;
			}

			/// @brief Ensure there is room for n more elements to be
			/// added, shifting contents in the container if necessary.
			///
			/// @note May invalidate iterators!
			void ensure_space(size_type n) {
				// If we're empty, may as well be empty at the beginning
				if (empty()) {
					_begin = 0;
					_pastEnd = 0;
				}
				if (_pastEnd + n <= CAPACITY) {
					return;
				} else {
					/// Slide back to the front of the array
					BOOST_ASSERT_MSG(size() + n <= CAPACITY, "Impossible to ensure that much space");
					slide_contents_forward();
				}
			}

		private:
			friend class vector_simulator_access;


			/// @brief Adapt a buffer index into an index in the wrapped container
			size_type adjusted_index(size_type i) {
				return _begin + i;
			}

			/// @brief Copies the whole buffer to the front of the wrapped container
			void slide_contents_forward() {
				idealContentsCopy(*this, *this);
			}

			/// @brief rangecheck used by vector_simulator
			bool rangecheck(size_type i) const {
				return i < size();
			}

			void increment_pastEnd() {
				ensure_space(1);
				_pastEnd++;
				BOOST_ASSERT_MSG(_pastEnd <= CAPACITY, "Consuming more space than possible");
			}

			void decrement_pastEnd() {
				BOOST_ASSERT_MSG(_pastEnd > _begin, "End moved before beginning");
				_pastEnd--;
			}

			void increment_begin() {
				BOOST_ASSERT_MSG(_begin < _pastEnd, "Beginning moved past end");
				_begin++;
			}

			/// @brief Copies the whole buffer of one object to the front of another (which may be the same)
			static void idealContentsCopy(type const& source, type & dest) {
				std::copy(source.begin(), source.end(), dest._contents.begin());
				dest._pastEnd = source.size();
				dest._begin = 0;
			}

			void verify_invariants() const {
				BOOST_ASSERT_MSG(_begin <= _pastEnd, "Beginning moved past end");
				BOOST_ASSERT_MSG(_pastEnd <= CAPACITY, "Consuming more space than possible");
			}

			size_type _begin;
			size_type _pastEnd;
			wrapped_type _contents;


	};

#if 0
	template<typename T, std::size_t N>
	std::size_t hash_value(ReceiveBuffer<N, T> const & buf) {
		return boost::hash_range(buf.begin(), buf.end());
	}
#endif

} // end of namespace util

#endif // INCLUDED_ReceiveBuffer_h_GUID_2d1681f0_0ffe_495d_8ec9_fd730b801721
