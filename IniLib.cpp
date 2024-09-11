#include "IniLib.h"
#include <sstream>
#include <fstream>
#include <algorithm>
#include <cctype>

namespace IniLib {

    // IniSection class methods
    std::vector<std::string> IniSection::get(const std::string& key, const std::vector<std::string>& defaultValue) const {
        auto keyIt = keyValues.find(IniFile::toLower(key));
        if (keyIt != keyValues.end()) {
            return keyIt->second;
        }
        return defaultValue;
    }

    std::string IniSection::get(const std::string& key, const std::string& defaultValue) const {
        auto keyIt = keyValues.find(IniFile::toLower(key));
        if (keyIt != keyValues.end() && !keyIt->second.empty()) {
            return keyIt->second.front();
        }
        return defaultValue;
    }

    void IniSection::set(const std::string& key, const std::vector<std::string>& values) {
        keyValues[IniFile::toLower(key)] = values;
    }

    void IniSection::set(const std::string& key, const std::string& value) {
        keyValues[IniFile::toLower(key)] = { value };
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

    std::vector<std::string>& IniSection::operator[](const std::string& key) {
        return keyValues[IniFile::toLower(key)];
    }

    const std::vector<std::string>& IniSection::operator[](const std::string& key) const {
        return keyValues.at(IniFile::toLower(key));
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
                file << kv.first << "=" << join(kv.second, ",") << "\n";
            }
            file << "\n";
        }
        return true;
    }

    std::vector<std::string> IniFile::get(const std::string& section, const std::string& key, const std::vector<std::string>& defaultValue) const {
        auto secIt = sections.find(toLower(section));
        if (secIt != sections.end()) {
            return secIt->second.get(key, defaultValue);
        }
        return defaultValue;
    }

    std::string IniFile::get(const std::string& section, const std::string& key, const std::string& defaultValue) const {
        auto secIt = sections.find(toLower(section));
        if (secIt != sections.end()) {
            return secIt->second.get(key, defaultValue);
        }
        return defaultValue;
    }

    void IniFile::set(const std::string& section, const std::string& key, const std::vector<std::string>& values) {
        sections[toLower(section)].set(key, values);
    }

    void IniFile::set(const std::string& section, const std::string& key, const std::string& value) {
        sections[toLower(section)].set(key, value);
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
        return sections[toLower(section)];
    }

    const IniSection& IniFile::operator[](const std::string& section) const {
        return sections.at(toLower(section));
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

    std::string IniFile::join(const std::vector<std::string>& vec, const std::string& delimiter) {
        std::ostringstream oss;
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i != 0) oss << delimiter;
            oss << vec[i];
        }
        return oss.str();
    }

} // namespace IniLib
