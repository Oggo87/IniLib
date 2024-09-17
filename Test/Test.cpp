#include "../IniLib.h"
#include <iostream>

using namespace std;

int main() {
    IniLib::IniFile ini;

    // Load an INI file
    if (ini.load("Test/config.ini")) {
        // Access sections and keys using []
        string value = ini["section1"]["key1"].getString();
        cout << "Key1: " << value << endl;

        // Set a new value
        ini["section1"]["key2"] = { "new_value1", "new_value2" };

        // Check existence
        if (ini.hasKey("section1", "key2")) {
            cout << "Key2 exists" << endl;
        }

        // Remove a key
        ini.removeKey("section1", "key1");

        // Clear a section, without removing it
        ini.clearSection("section2");

        // Remove a section
        ini.removeSection("section2");

        ini["section4"]["key3"] = "Added Section and Key";

        // Save changes to a file
        ini.save("config_modified.ini");

        // Pause and wait for input to continue
        cout << "Press any key to continue..." << std::endl;
        cin.get();
    }

    return 0;
}
