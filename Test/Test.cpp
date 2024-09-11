#include "IniFile.h"
#include <iostream>

int main() {
    IniLib::IniFile ini;

    // Load an INI file
    if (ini.load("config.ini")) {
        // Access sections and keys using []
        std::string value = ini["section1"]["key1"][0];
        std::cout << "Key1: " << value << std::endl;

        // Set a new value
        ini["section1"]["key2"] = { "new_value1", "new_value2" };

        // Check existence
        if (ini.hasKey("section1", "key2")) {
            std::cout << "Key2 exists" << std::endl;
        }

        // Remove a key
        ini.removeKey("section1", "key1");

        // Save changes to a file
        ini.save("config_modified.ini");
    }

    return 0;
}
