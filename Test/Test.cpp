#include "../IniLib.h"
#include <iostream>

using namespace std;

int main() {
    IniLib::IniFile ini;

    // Load an INI file
    if (ini.load("Test/config.ini")) {
        // Access sections and keys using []
        try
        {
            string value = ini["section1"]["key1"].getString();
            cout << "Key1: " << value << endl;

            cin.get();
        }
        catch (IniLib::IniFileException ex)
        {
            cout << ex.what() << endl;
        }

        // Set a new value
        ini["section1"]["key2"] = { "new_value1", "new_value2" };

        // Check existence
        if (ini.hasKey("section1", "key2")) {
            cout << "Key2 exists" << endl;
        }

        // Remove a key
        ini.removeKey("section1", "key1");

        // Save changes to a file
        ini.save("config_modified.ini");

        cin.get();
    }

    return 0;
}
