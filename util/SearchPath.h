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
#ifndef INCLUDED_SearchPath_h_GUID_7a79b983_9d8b_4185_80ab_77146a676bdf
#define INCLUDED_SearchPath_h_GUID_7a79b983_9d8b_4185_80ab_77146a676bdf

// Internal Includes
// - none

// Library/third-party includes
#include <boost/tokenizer.hpp>

// Standard includes
#include <string>
#include <deque>
#include <iostream>
#include <sstream>
#include <cassert>
#include <stdexcept>

namespace util {

/// @addtogroup DataStructures Data Structures
/// @{
	namespace SearchPath {

		/** @brief Class representing an element in a search path, as a
			filename template..

			This uses the generalized concept of a search path as used
			by Lua: a "template" filename that may contain a character that
			is replaced to form a potential filename. Lua uses "?" as the default
			character for substitution, so it will be used here as well by example.
			However, it is never actually stored, so you may use whatever character
			suits your needs.

			Note that such a template is a strict superset of the functionality
			offered by a directory entry in a path list like LD_LIBRARY_PATH,
			as well as an entry (jar file) in a Java classpath.
		*/
		class FilenameTemplate {
			public:
				/// @name Constructors
				/// @{

				/// @brief default constructor - corresponds to "?"
				FilenameTemplate()
					: _hasPlaceholder(true) {
				}

				/** @brief Named constructor for a filename template corresponding
					to a directory.

					This normalizes the element to end in / if it does not already.
				*/
				static FilenameTemplate createFromDirectory(std::string const& elt);

				/** @brief Named constructor for a filename template in the Lua style:
					either a fixed value or containing a placeholder value (which splits the
					element into a prefix and suffix).
				*/
				static FilenameTemplate createFromTemplate(std::string const& elt,
				        const char placeholderChar = DEFAULT_PLACEHOLDER);

				/** @brief Constructor from a prefix and suffix
				*/
				FilenameTemplate(std::string const & pre, std::string const & suff)
					: _prefix(pre)
					, _suffix(suff)
					, _hasPlaceholder(true) {
				}

				/// @}

				/// @name Accessors
				/// @{

				/// @brief Get prefix: the part before the placeholder
				std::string const& getPrefix() const;

				/// @brief Get suffix: the part after the placeholder if applicable.
				std::string const& getSuffix() const;

				/// @brief Is there a placeholder between the prefix and suffix?
				///
				/// If not, the suffix is mandated to be empty.
				bool hasPlaceholder() const;

				/// @brief Is this is a fixed path rather than a template? (Opposite of hasPlaceholder())
				bool isFixedPath() const {
					return !hasPlaceholder();
				}

				/** @brief Does this represent a directory?

					That is, does it have a placeholder, no suffix, and a trailing
					slash in the prefix?
				*/
				bool isDirectory() const;

				/// @brief Combine prefix, placeholder or value of your choice
				/// (if this element has a placeholder), and suffix
				std::string getStringWithSubstitution(std::string const& substitution = std::string(1, DEFAULT_PLACEHOLDER)) const;

				/** @brief Get the directory

					Same functionality as "getPrefix", but throws an exception
					if this isn't actually a directory element (if it has a placeholder)
				*/
				std::string getDirectory() const;

				/// @}

				/// @name Mutators
				/// @{

				/// @brief Set prefix
				void setPrefix(std::string const & s);

				/// @brief Set suffix (only if this element has a placeholder!
				void setSuffix(std::string const & s);
				/// @}

				/// @name Search Path Manipulation
				/// @{

				/// @brief A list of filename templates, also known as a search path.
				typedef std::deque<FilenameTemplate> List;

				/** @brief Given a "lua-style" search path string, split it into a list of elements.

					Empty elements are dropped.
				*/
				static List splitListOfFilenameTemplates(std::string const & input, const char delimiter = DEFAULT_DELIMITER, const char placeholderChar = DEFAULT_PLACEHOLDER);
				/** @brief Given a search path string of directories, split it into a list of elements.

					Empty elements are dropped.
				*/
				static List splitListOfDirectories(std::string const & input, const char delimiter = DEFAULT_DELIMITER);

				/** @brief Given a list of elements, produce a "lua-style" search path string

					If this is a "directories-style" search path (no suffixes on any)
					instead of a Lua-style search path, use FilenameTemplate::listOfDirectoriesToString().
				*/
				static std::string listOfFilenameTemplatesToString(List const& input, const char delimiter = DEFAULT_DELIMITER, std::string const& placeholder = std::string(1, DEFAULT_PLACEHOLDER));
				/** @brief Given a list of elements, produce a search path string of directories. Throws if not all elements are directories.

					If this is a "Lua-style" search path that you want to contain placeholders,
					use listOfPathTemplatesToString.
				*/
				static std::string listOfDirectoriesToString(List const& input, const char delimiter = DEFAULT_DELIMITER);

				/** @brief Template function for applying a functor to
					every filename template in a list.
				*/
				template<typename T>
				static void forEachFilenameTemplate(List const& input, T functor);

				/** @brief Template function for applying a functor to
					every filename template that contains a placeholder
					(i.e. is not a complete filename already) in a list.
				*/
				template<typename T>
				static void forEachFilenameTemplateWithPlaceholder(List const& input, T functor);

				/** @brief Template function for applying a functor to
					every directory name in a list of filename templates,
					passing only the directory name.
				*/
				template<typename T>
				static void forEachDirectory(List const& input, T functor);
				/// @}

				/// @brief Default placeholder item
				static const char DEFAULT_PLACEHOLDER = '?';

				/// @brief Default path list delimiter
				static const char DEFAULT_DELIMITER = ';';
			private:
				/// @brief Private constructor used by named constructors.
				FilenameTemplate(std::string const & pre, std::string const & suff, bool hasPlaceholder)
					: _prefix(pre)
					, _suffix(suff)
					, _hasPlaceholder(hasPlaceholder) {
					assert(_suffix.empty() || _hasPlaceholder);
				}
				static bool _endsInPathSeparator(std::string const & s);
				std::string _prefix;
				std::string _suffix;
				bool _hasPlaceholder;
		};

		/// @brief Equality comparison operator for filename templates.
		inline bool operator==(FilenameTemplate const& lhs, FilenameTemplate const& rhs) {
			return (lhs.getPrefix() == rhs.getPrefix()) &&
			       (lhs.getSuffix() == rhs.getSuffix()) &&
			       (lhs.hasPlaceholder() == rhs.hasPlaceholder());
		}

		/// @brief Output operator for filename templates.
		inline std::ostream & operator<<(std::ostream & os, FilenameTemplate const& rhs) {
			os << rhs.getStringWithSubstitution();
			return os;
		}

		inline FilenameTemplate::List FilenameTemplate::splitListOfFilenameTemplates(std::string const & input, const char delimiter, const char placeholderChar) {
			FilenameTemplate::List ret;
			typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
			std::string const delimString(1, delimiter);
			boost::char_separator<char> sep(delimString.c_str());
			tokenizer tokens(input, sep);
			for (tokenizer::const_iterator it = tokens.begin(), end = tokens.end(); it != end; ++it) {
				ret.push_back(FilenameTemplate::createFromTemplate(*it, placeholderChar));
			}
			return ret;
		}

		inline FilenameTemplate::List FilenameTemplate::splitListOfDirectories(std::string const & input, const char delimiter) {
			FilenameTemplate::List ret;
			typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
			std::string const delimString(1, delimiter);
			boost::char_separator<char> sep(delimString.c_str());
			tokenizer tokens(input, sep);
			for (tokenizer::const_iterator it = tokens.begin(), end = tokens.end(); it != end; ++it) {
				ret.push_back(FilenameTemplate::createFromDirectory(*it));
			}
			return ret;

		}

		inline std::string FilenameTemplate::listOfFilenameTemplatesToString(FilenameTemplate::List const& input, const char delimiter, std::string const& placeholder) {
			std::ostringstream os;
			for (size_t i = 0, n = input.size(); i < n; ++i) {
				os << input[i].getStringWithSubstitution(placeholder);
				if (i + 1 < n) {
					os << delimiter;
				}
			}
			return os.str();

		}

		inline std::string FilenameTemplate::listOfDirectoriesToString(FilenameTemplate::List const& input, const char delimiter) {
			std::ostringstream os;
			for (size_t i = 0, n = input.size(); i < n; ++i) {
				os << input[i].getDirectory();
				if (i + 1 < n) {
					os << delimiter;
				}
			}
			return os.str();
		}

		template<typename T>
		inline void FilenameTemplate::forEachFilenameTemplate(FilenameTemplate::List const& input, T functor) {
			for (size_t i = 0, n = input.size(); i < n; ++i) {
				functor(input[i]);
			}
		}

		template<typename T>
		inline void FilenameTemplate::forEachFilenameTemplateWithPlaceholder(FilenameTemplate::List const& input, T functor) {
			for (size_t i = 0, n = input.size(); i < n; ++i) {
				if (input[i].hasPlaceholder()) {
					functor(input[i]);
				}
			}
		}

		template<typename T>
		inline void FilenameTemplate::forEachDirectory(FilenameTemplate::List const& input, T functor)  {
			for (size_t i = 0, n = input.size(); i < n; ++i) {
				if (input[i].isDirectory()) {
					functor(input[i].getDirectory());
				}
			}
		}

		inline FilenameTemplate FilenameTemplate::createFromDirectory(std::string const& elt) {
			if (_endsInPathSeparator(elt)) {
				return FilenameTemplate(elt, "", true);
			} else {
				return FilenameTemplate(elt + "/", "", true);
			}
		}

		inline FilenameTemplate FilenameTemplate::createFromTemplate(std::string const& elt,
		        const char placeholderChar) {
			std::size_t placeholder = elt.find(placeholderChar);
			if (placeholder == std::string::npos) {
				// Handle case 1 and 4
				// Didn't find a placeholder so treat the whole thing as a fixed value.
				return FilenameTemplate(elt, "", false);
			}
			// Found a placeholder: split into prefix and suffix
			// We're in case 2 or 3
			if (placeholder + 1 < elt.size()) {
				// We actually have a suffix: case 3
				return FilenameTemplate(elt.substr(0, placeholder), elt.substr(placeholder + 1), true);
			}
			// case 2
			return FilenameTemplate(elt.substr(0, placeholder), "", true);
		}

		inline std::string const& FilenameTemplate::getPrefix() const {
			return _prefix;
		}

		inline std::string const& FilenameTemplate::getSuffix() const {
			return _suffix;
		}

		inline bool FilenameTemplate::hasPlaceholder() const {
			return _hasPlaceholder;
		}

		inline bool FilenameTemplate::isDirectory() const {
			if (!_hasPlaceholder) {
				// It's a fixed string
				return false;
			}
			return _suffix.empty() && _endsInPathSeparator(_prefix);
		}

		inline std::string FilenameTemplate::getStringWithSubstitution(std::string const& substitution) const {
			if (_hasPlaceholder) {
				return _prefix + substitution + _suffix;
			} else {
				return _prefix;
			}
		}

		std::string FilenameTemplate::getDirectory() const {
			if (!isDirectory()) {
				throw std::logic_error("This filename template isn't a directory!");
			}
			return _prefix;
		}

		inline void FilenameTemplate::setPrefix(std::string const& s) {
			/// @todo Check for placeholder in this value and handle it?
			/// Setting the prefix to something like /bla/?/ would be weird.
			_prefix = s;
		}

		inline void FilenameTemplate::setSuffix(std::string const& s) {
			if (!_hasPlaceholder) {
				throw std::logic_error("Can't set the suffix of a filename template without a placeholder!");
			}
			/// @todo Check for placeholder in this value and handle it?
			/// Setting the prefix to something like /bla/?/ would be weird.
			_suffix = s;
		}

		inline bool FilenameTemplate::_endsInPathSeparator(std::string const& s) {
			if (s.empty()) {
				return false;
			}

			return (s.at(s.size() - 1) == '/')
#ifdef _WIN32
			       || (s.at(s.size() - 1) == '\\')
#endif
			       ;
		}
	} // end of namespace SearchPath

/// @}

} // end of namespace util

#endif // INCLUDED_SearchPath_h_GUID_7a79b983_9d8b_4185_80ab_77146a676bdf
