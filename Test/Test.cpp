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

        // Set a Section, Key and Value
        ini.set("SetSection", "SetKey", "SetValue");

        // Set a Key and Value in a given section
        ini["section2"].set("SetKey", "Set Value");

        // Remove a key
        ini.removeKey("section1", "key1");

        // Clear a section, without removing it
        ini.clearSection("section2");

        // Remove a section
        ini.removeSection("section2");

        ini["section4"]["key3"] = "Added Section and Key";

        // Section with different types
        ini["typeSection"]["intKey"] = 3;
        ini["typeSection"]["floatKey"] = 3.14159;
        ini["typeSection"]["shortKey"] = vector<short>({0xA, 33});
        ini["typeSection"]["boolKey"] = { "true", "0", "false", "1" };
        char charArray[] = { 'a', 'b', 'c' };
        ini["typeSection"]["charKey"] = charArray;

        int intValue = ini["typeSection"]["intKey"].getAs<int>();

        float floatValue = ini["typeSection"]["floatKey"].getAs<float>();

        vector<short> shortVector = ini["typeSection"]["shortKey"].getVectorAs<short>();

        bool* boolArray = ini["typeSection"]["boolKey"].getArrayAs<bool>();

        cout << "Int Value: " << intValue << endl;

        cout << "Float Value: " << floatValue << endl;

        cout << "Short Values: ";
        
        for (size_t i = 0; i < shortVector.size(); i++)
        {
            cout << shortVector[i] << ", ";
        }
        cout << endl;

        cout << "Bool Values: ";

        for (size_t i = 0; i < sizeof(boolArray); i++)
        {
            cout << boolArray[i] << ", ";
        }
        cout << endl;

        // Save changes to a file
        ini.save("config_modified.ini");

        // Pause and wait for input to continue
        cout << "Press any key to continue..." << endl;
        cin.get();
    }

    return 0;
}
