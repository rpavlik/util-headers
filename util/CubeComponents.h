/** @file
	@brief Header

	@versioninfo@

	@date 2011

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
#ifndef INCLUDED_CubeComponents_h_GUID_eea925df_f01f_4e08_b4db_e9c2800b49a6
#define INCLUDED_CubeComponents_h_GUID_eea925df_f01f_4e08_b4db_e9c2800b49a6

// Internal Includes
// - none

// Library/third-party includes
#include <Eigen/Core>

// Standard includes
#include <bitset>
#include <cassert>
#include <stdexcept>
#include <iosfwd>

namespace util {

	namespace {
	} // end of unnamed namespace

	namespace CubeComponents {

		namespace detail {
			/// @internal
			/// @brief interface for inserting into an output stream
			class Outputable {
				private:
					/// Method to insert a human-readable description
					/// into a std::ostream
					virtual void streamTo(std::ostream & os) const = 0;
					friend std::ostream & operator<<(std::ostream & os, Outputable const& val);
			};

			/// @internal
			/// @brief operator<< for inserting into an output stream
			inline std::ostream & operator<<(std::ostream & os, Outputable const& val) {
				val.streamTo(os);
				return os;
			}

			inline std::bitset<3> constructBitset(bool bit0, bool bit1, bool bit2) {
				return (std::bitset<3>(bit0)) | (std::bitset<3>(bit1) << 1) | (std::bitset<3>(bit2) << 2);
			}

			inline std::bitset<3> bitsetInsert(std::bitset<2> const & input, bool val, size_t location) {
				switch (location) {
					case 0:
						return constructBitset(val, input[0], input[1]);
						break;

					case 1:
						return constructBitset(input[0], val, input[1]);
						break;

					case 2:
						return constructBitset(input[0], input[1], val);
						break;

					default:
						throw std::out_of_range("Location to insert bit is out of range!");
				}
			}
		} // end of namespace detail

		template<typename _VecType = Eigen::Vector3d>
		struct Cube {
				typedef unsigned char BitIDType;
				typedef unsigned char IDType;
				typedef bool BitValueType;
				typedef _VecType VectorType;

				class Vertex;
				class Face;
				class FaceVertex;

				/** @brief Representation of a vertex of a 2x2x2 cube centered at the origin

					A set of 3 bits is used to represent a vertex, where each bit
					corresponds to a dimension (in {x, y, z}), and values are mapped
					0 -> -1 and 1 -> 1. A vertex's neighbors vary in one bit from the
					its own value.
				*/
				class Vertex : public detail::Outputable {
					public:
						typedef Cube CubeType;
						typedef std::bitset<3> BitsetType;
						static const IDType COUNT = 8;

						/// Default constructor: constructs vertex 0
						Vertex() {}

						/// Constructor from a bitset
						explicit Vertex(BitsetType const& val)
							: _v(val) {
						}

						/// Constructor from a vertex ID in range 0, 1, ..., 7
						explicit Vertex(IDType val)
							: _v(val) {
							if (val >= COUNT) {
								throw std::out_of_range("Vertex index specified is out of range  {0, 1, ..., 7} !");
							}
						}

						/// Get the coordinates of this vertex in a 2x2x2 cube centered
						/// on the origin. (All components will be +-1)
						VectorType get() const {
							return VectorType(_v[0], _v[1], _v[2]) * 2 - VectorType::Constant(1);
						}

						/// Get vertex ID in range 0, 1, ..., 7
						IDType getID() const {
							return _v.to_ulong();
						}

						/// Get bitset corresponding to this vertex
						BitsetType const& getBitset() const {
							return _v;
						}

						/// Get vertex neighbor m, where m is in 0, 1, 2
						Vertex getNeighbor(IDType m) {
							assert(m < 3);
							return Vertex(BitsetType(_v).flip(m));
						}

						/// Compare vertex equality based on ID
						bool operator==(Vertex const& other) const {
							return _v == other._v;
						}

						/// Compare vertex inequality based on ID
						bool operator!=(Vertex const& other) const {
							return _v != other._v;
						}


					private:
						void streamTo(std::ostream & os) const {
							os << "Vertex " << getID();
						}

						BitsetType _v;
				};

				/** @brief Representation of a face of a 2x2x2 cube centered at the origin.

					A face consists of a bit index to fix and a value to fix it at. The
					remaining two bits indicate the particular vertex in that face.
				*/
				class Face : public detail::Outputable {
					public:
						static const IDType COUNT = 6;
						/// Default constructor: constructs face 0
						Face()
							: _fixedBit(0)
							, _bitval(false) {
						}

						/// Constructor from a face ID in range 0, 1, ..., 5
						explicit Face(IDType j)
							: _fixedBit(j % 3)
							, _bitval(j / 3) {
							if (j >= COUNT) {
								throw std::out_of_range("Face index specified is out of range {0, 1, ..., 5} !");
							}
						}

						/// Constructor from a bit ID to fix (in 0, 1, 2) and a value for that bit (bool)
						Face(BitIDType fixedBit, BitValueType bitval)
							: _fixedBit(fixedBit)
							, _bitval(bitval) {
							if (fixedBit >= 3) {
								throw std::out_of_range("Face fixed bit index specified is out of range {0, 1, 2} !");
							}
						}

						/// Get coordinates of the center of this face
						VectorType getCenter() const {
							return VectorType(_fixedBit == 0 ? _bitval * 2 - 1 : 0,
							                  _fixedBit == 1 ? _bitval * 2 - 1 : 0,
							                  _fixedBit == 2 ? _bitval * 2 - 1 : 0);
						}

						/// Get outward-pointing normal of this face
						VectorType getNormal() const {
							return getCenter(); /// @todo center same as normal for centered 2x2x2 cube?
						}

						/// Get the ID of the bit that is fixed for this face
						BitIDType getFixedBitIndex() const {
							return _fixedBit;
						}

						/// Get the value assigned to the fixed bit for this face
						BitValueType getFixedBitValue() const {
							return _bitval;
						}

						/// Get face ID in range 0, 1, ..., 5
						IDType getID() const {
							return _bitval * 3 + _fixedBit;
						}

						/// Get one of the vertices of this face, with id in 0, 1, 2, 3
						FaceVertex getFaceVertex(IDType k) {
							return FaceVertex(*this, k);
						}

						/// Comparison operator based on face ID
						bool operator==(Face const& other) const {
							return getID() == other.getID();
						}

					private:
						/// Method to insert a human-readable description
						/// into a std::ostream
						void streamTo(std::ostream & os) const {
							os << "Face " << getID();
						}

						BitIDType _fixedBit;
						BitValueType _bitval;
				};

				/** @brief Representation of a vertex of a cube face.

					Convertible to a cube vertex, but also usable as a face vertex
					since it has only two neighbors on the same face.
				*/
				class FaceVertex : public detail::Outputable {
					public:
						typedef std::bitset<2> BitsetType;

						static const IDType COUNT = 4;

						/// Constructor from a face and an ID in {0, 1, 2, 3}
						FaceVertex(Face const& f, IDType k)
							: _fixedBit(f.getFixedBitIndex())
							, _bitval(f.getFixedBitValue())
							, _vertexID(k) {
							if (k >= COUNT) {
								throw std::out_of_range("Face vertex index specified is out of range {0, 1, 2, 3} !");
							}
						}

						/// Get the face this vertex belongs to
						Face getFace() const {
							return Face(_fixedBit, _bitval);
						}

						/// Get the ID in {0, 1, 2, 3}
						IDType getID() const {
							return _vertexID;
						}

						/// Get vertex's neighbor on this face, given n in {0, 1}
						FaceVertex getNeighbor(IDType n) const {
							if (n >= 2) {
								throw std::out_of_range("Face vertex neighbor index specified is out of range {0, 1} !");
							}
							return FaceVertex(getFace(), BitsetType(_vertexID).flip(n).to_ulong());
						}

						/// Conversion operator to (Cube) Vertex
						operator Vertex() const {
							return Vertex(detail::bitsetInsert(BitsetType(_vertexID), _bitval, _fixedBit));
						}

						/// Explicit method for converting to cube Vertex
						Vertex getCubeVertex() const {
							return operator Vertex();
						}


					private:
						/// Method to insert a human-readable description
						/// into a std::ostream
						void streamTo(std::ostream & os) const {
							os << "Face vertex " << _vertexID << " on face " << getFace().getID();
						}

						BitIDType _fixedBit;
						BitValueType _bitval;
						IDType _vertexID;
				};
				struct FaceEdge;
				struct Edge;
		}; // end of class Cube
	} // end of namespace CubeComponents

	using CubeComponents::Cube;

} // end of namespace util

#endif // INCLUDED_CubeComponents_h_GUID_eea925df_f01f_4e08_b4db_e9c2800b49a6
