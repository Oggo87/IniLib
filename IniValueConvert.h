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

#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <typeinfo>
#include <iomanip>

namespace IniLib {

    /**
     * @class IniValueConvertException
     * @brief Exception thrown when conversion fails in IniValueConvert.
     */
    class IniValueConvertException : public std::runtime_error {
    public:
        /**
         * @brief Constructor for IniValueConvertException
         * @param message Exception message
         */
        explicit IniValueConvertException(const std::string& message) : std::runtime_error(message) {}
    };

    /**
     * @class IniValueConvert
     * @brief Templated class for converting between strings and various types.
     *
     * Provides default implementations of `decode` and `encode` that throw
     * `IniValueConvertException`. Specialized implementations are provided for
     * common types.
     */
    template<typename T>
    class IniValueConvert {
    public:
        /**
         * @brief Decodes a string into type T.
         * @param value The string to decode.
         * @return T The decoded value of type T.
         * @throws IniValueConvertException if the conversion is not implemented.
         */
        static T decode(const std::string& value) {
            throw IniValueConvertException("Decode not implemented for type: " + std::string(typeid(T).name()));
        }

        /**
         * @brief Encodes a value of type T into a string.
         * @param value The value to encode.
         * @return std::string The encoded string.
         * @throws IniValueConvertException if the conversion is not implemented.
         */
        static std::string encode(const T& value) {
            throw IniValueConvertException("Encode not implemented for type: " + std::string(typeid(T).name()));
        }
    };

    // Specialized template implementations for specific types

    template<>
    class IniValueConvert<bool> {
    public:
        /**
         * @brief Decodes a string into a boolean.
         * @param value The string to decode.
         * @return bool The decoded boolean value.
         * @throws IniValueConvertException if the value is not a valid boolean.
         */
        static bool decode(const std::string& value) {
            if (value == "true" || value == "1") return true;
            if (value == "false" || value == "0") return false;
            throw IniValueConvertException("Invalid boolean value: " + value);
        }

        /**
         * @brief Encodes a boolean into a string.
         * @param value The boolean value to encode.
         * @return std::string The encoded string.
         */
        static std::string encode(const bool& value) {
            return value ? "true" : "false";
        }
    };

    template<>
    class IniValueConvert<char> {
    public:
        /**
         * @brief Decodes a string into a char.
         * @param value The string to decode.
         * @return char The decoded char value.
         * @throws IniValueConvertException if the string length is not 1.
         */
        static char decode(const std::string& value) {
            if (value.length() != 1) throw IniValueConvertException("Invalid char value: " + value);
            return value[0];
        }

        /**
         * @brief Encodes a char into a string.
         * @param value The char value to encode.
         * @return std::string The encoded string.
         */
        static std::string encode(const char& value) {
            return std::string(1, value);
        }
    };

    template<>
    class IniValueConvert<short> {
    public:
        /**
         * @brief Decodes a string into a short.
         * @param value The string to decode.
         * @return short The decoded short value.
         * @throws IniValueConvertException if the conversion fails.
         */
        static short decode(const std::string& value) {
            try {
                size_t idx;
                int result = std::stoi(value, &idx, 0); // Detect decimal or hexadecimal
                if (idx != value.size()) throw std::invalid_argument("extra characters");
                return static_cast<short>(result);
            }
            catch (...) {
                throw IniValueConvertException("Invalid short value: " + value);
            }
        }

        /**
         * @brief Encodes a short into a string.
         * @param value The short value to encode.
         * @return std::string The encoded string.
         */
        static std::string encode(const short& value) {
            return std::to_string(value);
        }
    };

    template<>
    class IniValueConvert<int> {
    public:
        /**
         * @brief Decodes a string into an int (supports decimal and hexadecimal).
         * @param value The string to decode.
         * @return int The decoded int value.
         * @throws IniValueConvertException if the conversion fails.
         */
        static int decode(const std::string& value) {
            try {
                size_t idx;
                int result = std::stoi(value, &idx, 0); // Detect decimal or hexadecimal
                if (idx != value.size()) throw std::invalid_argument("extra characters");
                return result;
            }
            catch (...) {
                throw IniValueConvertException("Invalid int value: " + value);
            }
        }

        /**
         * @brief Encodes an int into a string.
         * @param value The int value to encode.
         * @return std::string The encoded string.
         */
        static std::string encode(const int& value) {
            return std::to_string(value);
        }
    };

    template<>
    class IniValueConvert<long> {
    public:
        /**
         * @brief Decodes a string into a long (supports decimal and hexadecimal).
         * @param value The string to decode.
         * @return long The decoded long value.
         * @throws IniValueConvertException if the conversion fails.
         */
        static long decode(const std::string& value) {
            try {
                size_t idx;
                long result = std::stol(value, &idx, 0); // Detect decimal or hexadecimal
                if (idx != value.size()) throw std::invalid_argument("extra characters");
                return result;
            }
            catch (...) {
                throw IniValueConvertException("Invalid long value: " + value);
            }
        }

        /**
         * @brief Encodes a long into a string.
         * @param value The long value to encode.
         * @return std::string The encoded string.
         */
        static std::string encode(const long& value) {
            return std::to_string(value);
        }
    };

    template<>
    class IniValueConvert<float> {
    public:
        /**
         * @brief Decodes a string into a float.
         * @param value The string to decode.
         * @return float The decoded float value.
         * @throws IniValueConvertException if the conversion fails.
         */
        static float decode(const std::string& value) {
            try {
                return std::stof(value);
            }
            catch (...) {
                throw IniValueConvertException("Invalid float value: " + value);
            }
        }

        /**
         * @brief Encodes a float into a string.
         * @param value The float value to encode.
         * @return std::string The encoded string.
         */
        static std::string encode(const float& value) {
            return std::to_string(value);
        }
    };

    template<>
    class IniValueConvert<double> {
    public:
        /**
         * @brief Decodes a string into a double.
         * @param value The string to decode.
         * @return double The decoded double value.
         * @throws IniValueConvertException if the conversion fails.
         */
        static double decode(const std::string& value) {
            try {
                return std::stod(value);
            }
            catch (...) {
                throw IniValueConvertException("Invalid double value: " + value);
            }
        }

        /**
         * @brief Encodes a double into a string.
         * @param value The double value to encode.
         * @return std::string The encoded string.
         */
        static std::string encode(const double& value) {
            return std::to_string(value);
        }
    };

    template<>
    class IniValueConvert<std::string> {
    public:
        /**
         * @brief Decodes a string into a string (no conversion needed).
         * @param value The string to decode.
         * @return std::string The decoded string.
         */
        static std::string decode(const std::string& value) {
            return value;
        }

        /**
         * @brief Encodes a string into a string (no conversion needed).
         * @param value The string value to encode.
         * @return std::string The encoded string.
         */
        static std::string encode(const std::string& value) {
            return value;
        }
    };

    template<>
    class IniValueConvert<const char*> {
    public:
        /**
         * @brief Decodes a string into a const char* (returns the c_str of the string).
         * @param value The string to decode.
         * @return const char* The decoded C-style string.
         */
        static const char* decode(const std::string& value) {
            return value.c_str();
        }

        /**
         * @brief Encodes a const char* into a string.
         * @param value The C-style string to encode.
         * @return std::string The encoded string.
         */
        static std::string encode(const char* value) {
            return std::string(value);
        }
    };

} // namespace IniLib
