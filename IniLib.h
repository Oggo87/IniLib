#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace IniLib {

    class IniFile; // Forward declaration of IniFile class

    class IniSection {
    public:
        using KeyValueMap = std::unordered_map<std::string, std::vector<std::string>>;

        // Retrieves a value for a given key
        std::vector<std::string> get(const std::string& key, const std::vector<std::string>& defaultValue = {}) const;
        std::string get(const std::string& key, const std::string& defaultValue = "") const;

        // Sets a value for a given key
        void set(const std::string& key, const std::vector<std::string>& values);
        void set(const std::string& key, const std::string& value);

        // Removes a key by name
        bool removeKey(const std::string& key);

        // Clears all keys from the section
        void clear();

        // Checks if a key exists
        bool hasKey(const std::string& key) const;

        // Returns the number of keys in the section
        size_t keyCount() const;

        // Overload [] to access keys by name
        std::vector<std::string>& operator[](const std::string& key);
        const std::vector<std::string>& operator[](const std::string& key) const;

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

        // Gets a value for a given section and key
        std::vector<std::string> get(const std::string& section, const std::string& key, const std::vector<std::string>& defaultValue = {}) const;
        std::string get(const std::string& section, const std::string& key, const std::string& defaultValue = "") const;

        // Sets a value for a given section and key
        void set(const std::string& section, const std::string& key, const std::vector<std::string>& values);
        void set(const std::string& section, const std::string& key, const std::string& value);

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

        // Access a section via overloaded operator []
        IniSection& operator[](const std::string& section);
        const IniSection& operator[](const std::string& section) const;

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

        // Joins a vector of strings into a single string, separated by a delimiter
        static std::string join(const std::vector<std::string>& vec, const std::string& delimiter);
    };

} // namespace IniLib
