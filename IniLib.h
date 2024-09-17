#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <stdexcept>

namespace IniLib {

    // Exception class for INI file errors
    class IniFileException : public std::runtime_error {
    public:
        explicit IniFileException(const std::string& message) : std::runtime_error(message) {}
    };

    // Forward declaration of classes
    class IniFile;
    class IniSection;

    // IniValue wrapper class
    class IniValue {
    public:
        // Constructors
        IniValue() = default;
        IniValue(const std::vector<std::string>& values) : values(values) {}
        IniValue(std::initializer_list<std::string> values) : values(values) {}
        IniValue(const std::string& value) : values({ value }) {}
        IniValue(const char value[]) : values({value}) {}

        // Getters
        size_t length() const { return values.size(); }
        bool isVector() const { return values.size() > 1; }

        std::vector<std::string> getVector() const { return values; }
        std::string getString() const;

        // Append a value
        void append(const std::string& value);

        // Clear the value
        void clear();

        // Subscript operator with boundary check
        std::string& operator[](size_t index);
        const std::string& operator[](size_t index) const;

    private:
        std::vector<std::string> values;

        // Joins a vector of strings into a single string, separated by a delimiter
        static std::string join(const std::vector<std::string>& vec, const std::string& delimiter);
    };


    class IniSection {
    public:
        using KeyValueMap = std::unordered_map<std::string, IniValue>;

        // Get functions returning IniValue
        IniValue get(const std::string& key, const IniValue& defaultValue = IniValue()) const;

        // Set functions accepting IniValue
        void set(const std::string& key, const IniValue& value);

        // Removes a key by name
        bool removeKey(const std::string& key);

        // Clears all keys from the section
        void clear();

        // Checks if a key exists
        bool hasKey(const std::string& key) const;

        // Returns the number of keys in the section
        size_t keyCount() const;

        // Overload [] to access keys by name
        IniValue& operator[](const std::string& key);
        const IniValue& operator[](const std::string& key) const;

    private:
        KeyValueMap keyValues;

        friend class IniFile; // Allow IniFile to access private members
    };

    class IniFile {
    public:
        // Loads an INI file from a given path
        bool load(const std::string& filename);

        // Saves the current INI configuration to a file
        bool save(const std::string& filename) const;
        
        // Get functions returning IniValue
        IniValue get(const std::string& section, const std::string& key, const IniValue& defaultValue = IniValue()) const;

        // Set functions accepting IniValue
        void set(const std::string& section, const std::string& key, const IniValue& value);

        // Removes a section or a key
        bool removeSection(const std::string& section);
        bool removeKey(const std::string& section, const std::string& key);

        // Clears the entire INI instance
        void clear();

        // Clears a specific section
        void clearSection(const std::string& section);

        // Checks if a section or a key exists
        bool hasSection(const std::string& section) const;
        bool hasKey(const std::string& section, const std::string& key) const;

        // Counts the number of sections or keys in a section
        size_t sectionCount() const;
        size_t keyCount(const std::string& section) const;

        // Overload [] to access sections by name
        IniSection& operator[](const std::string& section);
        const IniSection& operator[](const std::string& section) const;

        // Adds a section, returns true if the section is newly created, false if it already exists
        bool addSection(const std::string& section);

    private:
        std::unordered_map<std::string, IniSection> sections;

        friend class IniSection; // Allow IniSection to access private members

        // Trims whitespace from both ends of a string
        static std::string trim(const std::string& str);

        // Converts a string to lowercase
        static std::string toLower(const std::string& str);

        // Removes comments from a line
        static std::string removeComment(const std::string& str);

        // Splits a string by a delimiter and trims each part
        static std::vector<std::string> split(const std::string& str, char delimiter);
    };

} // namespace IniLib
