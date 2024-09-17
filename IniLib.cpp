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

#include "IniLib.h"
#include <sstream>
#include <fstream>
#include <algorithm>
#include <cctype>

namespace IniLib {

    //IniValue class methods
    std::string IniValue::getString() const {
        if (values.empty()) {
            return "";
        }
        else if (values.size() == 1) {
            return values[0];
        }
        else {
            return join(values, ", ");
        }
    }

    void IniValue::append(const std::string& value) {
        values.push_back(value);
    }

    void IniValue::clear() {
        values.clear();
    }

    std::string& IniValue::operator[](size_t index) {
        if (index >= values.size()) {
            throw IniFileException("Index out of bounds");
        }
        return values[index];
    }

    const std::string& IniValue::operator[](size_t index) const {
        if (index >= values.size()) {
            throw IniFileException("Index out of bounds");
        }
        return values[index];
    }

    std::string IniValue::join(const std::vector<std::string>& vec, const std::string& delimiter) {
        std::ostringstream oss;
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i != 0) oss << delimiter;
            oss << vec[i];
        }
        return oss.str();
    }

    // IniSection class methods
    IniValue IniSection::get(const std::string& key, const IniValue& defaultValue) const {
        auto it = keyValues.find(IniFile::toLower(key));
        return (it != keyValues.end()) ? it->second : defaultValue;
    }

    void IniSection::set(const std::string& key, const IniValue& value) {
        keyValues[IniFile::toLower(key)] = value;
    }

    bool IniSection::removeKey(const std::string& key) {
        return keyValues.erase(IniFile::toLower(key)) > 0;
    }

    void IniSection::clear() {
        keyValues.clear();
    }

    bool IniSection::hasKey(const std::string& key) const {
        return keyValues.find(IniFile::toLower(key)) != keyValues.end();
    }

    size_t IniSection::keyCount() const {
        return keyValues.size();
    }

    IniValue& IniSection::operator[](const std::string& key) {
        return keyValues[IniFile::toLower(key)];
    }

    const IniValue& IniSection::operator[](const std::string& key) const {
        auto it = keyValues.find(IniFile::toLower(key));
        if (it == keyValues.end()) {
            throw IniFileException("Key \"" + key + "\" does not exist in the section.");
        }
        return it->second;
    }

    // IniFile class methods
    bool IniFile::load(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) return false;

        std::string line, currentSection;
        while (std::getline(file, line)) {
            line = trim(removeComment(line));
            if (line.empty()) continue;

            if (line.front() == '[' && line.back() == ']') {
                currentSection = toLower(trim(line.substr(1, line.size() - 2)));
            }
            else {
                auto pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string key = toLower(trim(line.substr(0, pos)));
                    std::string value = trim(line.substr(pos + 1));
                    std::vector<std::string> values = split(value, ',');
                    sections[currentSection].set(key, values);
                }
            }
        }
        return true;
    }

    bool IniFile::save(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file.is_open()) return false;

        for (const auto& sectionPair : sections) {
            file << "[" << sectionPair.first << "]\n";
            const IniSection::KeyValueMap& keyValues = sectionPair.second.keyValues;
            for (const auto& kv : keyValues) {
                file << kv.first << "=" << kv.second.getString() << "\n";
            }
            file << "\n";
        }
        return true;
    }

    IniValue IniFile::get(const std::string& section, const std::string& key, const IniValue& defaultValue) const {
        auto it = sections.find(IniFile::toLower(section));
        return (it != sections.end()) ? it->second.get(key, defaultValue) : defaultValue;
    }

    void IniFile::set(const std::string& section, const std::string& key, const IniValue& value) {
        sections[IniFile::toLower(section)].set(key, value);
    }

    bool IniFile::removeSection(const std::string& section) {
        return sections.erase(toLower(section)) > 0;
    }

    bool IniFile::removeKey(const std::string& section, const std::string& key) {
        auto secIt = sections.find(toLower(section));
        if (secIt != sections.end()) {
            return secIt->second.removeKey(key);
        }
        return false;
    }

    void IniFile::clear() {
        sections.clear();
    }

    void IniFile::clearSection(const std::string& section) {
        sections[toLower(section)].clear();
    }

    bool IniFile::hasSection(const std::string& section) const {
        return sections.find(toLower(section)) != sections.end();
    }

    bool IniFile::hasKey(const std::string& section, const std::string& key) const {
        auto secIt = sections.find(toLower(section));
        if (secIt != sections.end()) {
            return secIt->second.hasKey(key);
        }
        return false;
    }

    size_t IniFile::sectionCount() const {
        return sections.size();
    }

    size_t IniFile::keyCount(const std::string& section) const {
        auto secIt = sections.find(toLower(section));
        if (secIt != sections.end()) {
            return secIt->second.keyCount();
        }
        return 0;
    }

    IniSection& IniFile::operator[](const std::string& section) {
        return sections[IniFile::toLower(section)];
    }

    const IniSection& IniFile::operator[](const std::string& section) const {
        auto it = sections.find(IniFile::toLower(section));
        if (it == sections.end()) {
            throw IniFileException("Section \"" + section + "\" does not exist.");
        }
        return it->second;
    }

    bool IniFile::addSection(const std::string& section) {
        return sections.emplace(toLower(section), IniSection()).second;
    }

    // Helper functions
    std::string IniFile::trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\n\r");
        size_t last = str.find_last_not_of(" \t\n\r");
        return (first == std::string::npos || last == std::string::npos) ? "" : str.substr(first, last - first + 1);
    }

    std::string IniFile::toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) { return std::tolower(c); });
        return result;
    }

    std::string IniFile::removeComment(const std::string& str) {
        size_t commentPos = std::min(str.find(';'), str.find('#'));
        return (commentPos != std::string::npos) ? str.substr(0, commentPos) : str;
    }

    std::vector<std::string> IniFile::split(const std::string& str, char delimiter) {
        std::vector<std::string> result;
        std::stringstream ss(str);
        std::string item;
        while (std::getline(ss, item, delimiter)) {
            result.push_back(trim(item));
        }
        return result;
    }

} // namespace IniLib
