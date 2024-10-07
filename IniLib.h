/*
MIT License

Copyright (c) 2024

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

/**
 * @def INILIB_API
 * @brief API visibility specifier for INILIB.
 * This macro is used to control symbol visibility when compiling as
 * a static or dynamic library.
 */
#ifdef INILIB_STATIC
#define INILIB_API 
#else
#ifdef INILIB_EXPORTS
#define INILIB_API __declspec(dllexport)
#else
#define INILIB_API  __declspec(dllimport)
#endif
#endif

#include <string>
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <type_traits>
#include "IniValueConvert.h"

namespace IniLib {

    /**
     * @class IniFileException
     * @brief Exception class for INI file errors
     */
    class IniFileException : public std::runtime_error {
    public:
        /**
         * @brief Constructor for IniFileException
         * @param message Exception message
         */
        explicit IniFileException(const std::string& message) : std::runtime_error(message) {}
    };

    // Forward declaration of classes
    class IniFile;
    class IniSection;

    /**
     * @class IniValue
     * @brief Wrapper class for handling values stored in the INI file.
     *
     * IniValue encapsulates a vector of strings and provides utilities to
     * manage, retrieve, and append INI key values.
     */
    class IniValue {
    public:
        /// @brief Default constructor
        IniValue() = default;

        /// @brief Constructor initializing from a vector of strings
        IniValue(const std::vector<std::string>& values) : values(values) {}

        /// @brief Constructor initializing from an initializer list
        IniValue(std::initializer_list<std::string> values) : values(values) {}

        /// @brief Constructor initializing from a single string
        IniValue(const std::string& value) : values({ value }) {}

        /// @brief Constructor initializing from an array of characters
        IniValue(const char value[]) : values({value}) {}

        /**
         * @brief Returns the length of the underlying vector
         * @return size_t Number of elements in the vector
         */
        size_t length() const { return values.size(); }

        /**
         * @brief Checks if the IniValue represents a vector of values
         * @return true if the value contains more than one entry, false otherwise
         */
        bool isVector() const { return values.size() > 1; }

        /**
         * @brief Returns the vector of strings
         * @return std::vector<std::string> The underlying vector of values
         */
        std::vector<std::string> getVector() const { return values; }

        /**
         * @brief Returns a string representation of the value
         *
         * If the vector is empty, it returns an empty string.
         * If it has only one element, it returns that element.
         * Otherwise, it concatenates all elements into a comma-separated string.
         *
         * @return std::string String representation of the value
         */
        std::string getString() const;

        /**
         * @brief Appends a string to the value
         * @param value String to be appended to the vector
         */
        void append(const std::string& value);

        /**
         * @brief Clears the underlying vector
         */
        void clear();

        /**
         * @brief Accesses the underlying vector by index
         * @param index Index of the element to access
         * @return std::string& Reference to the element at the specified index
         * @throws IniFileException if the index is out of bounds
         */
        std::string& operator[](size_t index);

        /**
         * @brief Const version of subscript operator
         * @param index Index of the element to access
         * @return const std::string& Reference to the element at the specified index
         * @throws IniFileException if the index is out of bounds
         */
        const std::string& operator[](size_t index) const;

        /**
         * @brief Returns a single value of type T from the first entry in the vector
         * @tparam T The type to convert the string value to.
         * @return T The value of type T.
         * @throws IniValueConvertException if conversion fails.
         */
        template<typename T>
        T getAs() const {
            if (values.empty()) {
                throw IniValueConvertException("IniValue is empty");
            }
            return IniValueConvert<T>::decode(values[0]);
        }

        /**
         * @brief Returns a vector of values of type T from the internal vector.
         * @tparam T The type to convert the string values to.
         * @return std::vector<T> The vector of values of type T.
         * @throws IniValueConvertException if conversion fails.
         */
        template<typename T>
        std::vector<T> getVectorAs() const {
            std::vector<T> result;
            for (const std::string& str : values) {
                result.push_back(IniValueConvert<T>::decode(str));
            }
            return result;
        }

        /**
         * @brief Returns a C-style array of values of type T.
         * @tparam T The type to convert the string values to.
         * @return T* A pointer to a dynamically allocated C-style array.
         * @throws IniValueConvertException if conversion fails.
         */
        template<typename T>
        T* getArrayAs() const {
            std::vector<T> vec = getVectorAs<T>();
            T* array = new T[vec.size()];
            std::copy(vec.begin(), vec.end(), array);
            return array;
        }

        /**
         * @brief Templated assignment operator for a single value of type T.
         *
         * This operator uses IniValueConvert<T>::encode to convert the value to a string
         * and assigns it to the internal vector, clearing any previous contents.
         *
         * @tparam T The type of the value to be assigned.
         * @param value The value to assign.
         * @return IniValue& A reference to the modified IniValue object.
         */
        template<typename T>
        IniValue& operator=(const T& value) {
            values.clear();
            values.push_back(IniValueConvert<T>::encode(value));
            return *this;
        }

        /**
         * @brief Templated assignment operator for an array of values of type T.
         *
         * This operator uses IniValueConvert<T>::encode to convert each value in the array
         * to a string and assigns them to the internal vector, clearing any previous contents.
         *
         * @tparam T The type of the values in the array.
         * @tparam N The size of the array.
         * @param arr The array of values to assign.
         * @return IniValue& A reference to the modified IniValue object.
         */
        template<typename T, size_t N>
        IniValue& operator=(T(&arr)[N]) {
            values.clear();

            //check if T is a const char * (string literal)
            if (std::is_same<T, const char>::value)
            {
                values.push_back(IniValueConvert<const char*>::encode((const char*)arr));
            }
            else
            {
                for (size_t i = 0; i < N; ++i) {
                    values.push_back(IniValueConvert<T>::encode(arr[i]));
                }
            }
            return *this;
        }

        /**
         * @brief Templated assignment operator for an initializer_list of values of type T.
         *
         * This operator uses IniValueConvert<T>::encode to convert each value in the initializer_list
         * to a string and assigns them to the internal vector, clearing any previous contents.
         *
         * @tparam T The type of the values in the initializer_list.
         * @param list The initializer_list of values to assign.
         * @return IniValue& A reference to the modified IniValue object.
         */
        template<typename T>
        IniValue& operator=(std::initializer_list<T> list) {
            values.clear();
            for (const T& value : list) {
                values.push_back(IniValueConvert<T>::encode(value));
            }
            return *this;
        }

        /**
         * @brief Templated assignment operator for a vector of values of type T.
         *
         * This operator uses IniValueConvert<T>::encode to convert each value in the vector
         * to a string and assigns them to the internal vector, clearing any previous contents.
         *
         * @tparam T The type of the values in the vector.
         * @param vec The vector of values to assign.
         * @return IniValue& A reference to the modified IniValue object.
         */
        template<typename T>
        IniValue& operator=(const std::vector<T>& vec) {
            values.clear();
            for (const T& value : vec) {
                values.push_back(IniValueConvert<T>::encode(value));
            }
            return *this;
        }

    private:
        std::vector<std::string> values; ///< The underlying vector of values

        /**
         * @brief Joins a vector of strings into a single string, separated by a delimiter
         * @param vec Vector of strings to join
         * @param delimiter Delimiter to separate each element
         * @return std::string The concatenated string
         */
        static std::string join(const std::vector<std::string>& vec, const std::string& delimiter);
    };

    /**
     * @class IniSection
     * @brief Class representing an INI section containing multiple keys.
     */
    class IniSection {
    public:
        using KeyValueMap = std::unordered_map<std::string, IniValue>; ///< Map of key-value pairs in the section

        /**
         * @brief Retrieves a value for a given key
         * @param key The key to look for
         * @param defaultValue Value to return if the key is not found
         * @return IniValue The value associated with the key, or the defaultValue if not found
         */
        IniValue get(const std::string& key, const IniValue& defaultValue = IniValue()) const;

        /**
         * @brief Sets a value for a given key
         * @param key The key to set the value for
         * @param value The value to set
         */
        void set(const std::string& key, const IniValue& value);

        /**
         * @brief Removes a key from the section
         * @param key The key to remove
         * @return true if the key was removed, false otherwise
         */
        bool removeKey(const std::string& key);

        /**
         * @brief Clears all key-value pairs in the section
         */
        void clear();

        /**
         * @brief Checks if a key exists in the section
         * @param key The key to check for
         * @return true if the key exists, false otherwise
         */
        bool hasKey(const std::string& key) const;

        /**
         * @brief Returns the number of keys in the section
         * @return size_t The number of keys in the section
         */
        size_t keyCount() const;

        /**
         * @brief Accesses a value by key, creates the key if it doesn't exist
         * @param key The key to access
         * @return IniValue& Reference to the value associated with the key
         */
        IniValue& operator[](const std::string& key);

        /**
         * @brief Const version of subscript operator
         * @param key The key to access
         * @return const IniValue& Reference to the value associated with the key
         * @throws IniFileException if the key doesn't exist
         */
        const IniValue& operator[](const std::string& key) const;

    private:
        KeyValueMap keyValues; ///< Map of keys and values

        friend class IniFile;  ///< Allow IniFile to access private members
    };

    /**
     * @class IniFile
     * @brief Class representing an INI file with multiple sections.
     */
    class IniFile {
    public:
        /**
         * @brief Loads an INI file from a given path
         * @param filename The path of the INI file to load
         * @return true if the file was successfully loaded, false otherwise
         */
        bool load(const std::string& filename);

        /**
         * @brief Saves the current INI configuration to a file
         * @param filename The path of the file to save to
         * @return true if the file was successfully saved, false otherwise
         */
        bool save(const std::string& filename) const;

        /**
         * @brief Retrieves a value for a given section and key
         * @param section The section to look in
         * @param key The key to look for
         * @param defaultValue Value to return if the key is not found
         * @return IniValue The value associated with the key, or the defaultValue if not found
         */
        IniValue get(const std::string& section, const std::string& key, const IniValue& defaultValue = IniValue()) const;

        /**
         * @brief Sets a value for a given section and key
         * @param section The section to set the value for
         * @param key The key to set the value for
         * @param value The value to set
         */
        void set(const std::string& section, const std::string& key, const IniValue& value);

        /**
         * @brief Adds a section, returns true if the section is newly created, false if it already exists
         * @param section The section to add
         * @return true if the section was newly created, false otherwise
         */
        bool addSection(const std::string& section);

        /**
         * @brief Removes a section
         * @param section The section to remove
         * @return true if the section was removed, false otherwise
         */
        bool removeSection(const std::string& section);

        /**
         * @brief Removes a key from a section
         * @param section The section containing the key
         * @param key The key to remove
         * @return true if the key was removed, false otherwise
         */
        bool removeKey(const std::string& section, const std::string& key);

        /**
         * @brief Clears all sections from the INI file
         */
        void clear();

        /**
         * @brief Clears all keys in a specific section
         * @param section The section to clear
         */
        void clearSection(const std::string& section);

        /**
         * @brief Checks if a section exists
         * @param section The section to check for
         * @return true if the section exists, false otherwise
         */
        bool hasSection(const std::string& section) const;

        /**
         * @brief Checks if a key exists in a specific section
         * @param section The section containing the key
         * @param key The key to check for
         * @return true if the key exists, false otherwise
         */
        bool hasKey(const std::string& section, const std::string& key) const;

        /**
         * @brief Returns the number of sections in the INI file
         * @return size_t The number of sections
         */
        size_t sectionCount() const;

        /**
         * @brief Returns the number of keys in a section
         * @param section The section to count keys in
         * @return size_t The number of keys in the section
         */
        size_t keyCount(const std::string& section) const;

        /**
         * @brief Accesses a section by name, creates the section if it doesn't exist
         * @param section The section to access
         * @return IniSection& Reference to the section
         */
        IniSection& operator[](const std::string& section);

        /**
         * @brief Const version of subscript operator
         * @param section The section to access
         * @return const IniSection& Reference to the section
         * @throws IniFileException if the section doesn't exist
         */
        const IniSection& operator[](const std::string& section) const;

    private:
        std::unordered_map<std::string, IniSection> sections; ///< Map of section names and sections

        friend class IniSection; ///< Allow IniSection to access private members

        /**
         * @brief Trims whitespace from both ends of a string
         * @param str The string to trim
         * @return std::string The trimmed string
         */
        static std::string trim(const std::string& str);

        /**
         * @brief Converts a string to lowercase
         * @param str The string to convert
         * @return std::string The lowercase version of the string
         */
        static std::string toLower(const std::string& str);

        /**
         * @brief Removes comments from a line
         * @param str The line to process
         * @return std::string The line with comments removed
         */
        static std::string removeComment(const std::string& str);

        /**
         * @brief Splits a string by a delimiter and trims each part
         * @param str The string to split
         * @param delimiter The delimiter to split by
         * @return std::vector<std::string> The list of substrings
         */
        static std::vector<std::string> split(const std::string& str, char delimiter);
    };

} // namespace IniLib
