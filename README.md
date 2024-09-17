# IniLib

An STL C++ INI read/write library, written mostly with Grand Prix 4 configuration files in mind

## Description

Yet another library to read, write and manipulate INI files. Written all in C++ and using only STL, it should be fairly portable, albeit it was developed with how INI files have been historically used in the Grand Prix 4 modding community.

It can be included in your project by coping `IniLib.h` and `IniLib.cpp` directly into it, or by using the pre-compiled static or dynamic libraries.

The following apply:

* All entries are single-line, multiline is not supported
* Section and Key names are case insensitive
* Empty Section and Key names are allowed
* Comments, starting with a semicolon (;) are allowed, both in line and trailing, but will not be retained
* Values are always strings or arrays of strings, separated by a comma (,). All typecasting is left to the user

After `load`, all data is store in memory, until `save`. There is no provision to store filenames, that have therefore to be explicitly indicated at all times. The order of sections and keys will be maintained on `load` and `save` operations.

Since the library relies on `std::string` to store all data, encoding is system-dependant.

A simple `Test.cpp` file is included in the repo, with some simple tests and use-cases.

## Documentation

The library is Doxygen-ready. Docs might be added to this section in the future

## Authors

Diego "Öggo" Noriega

## Version History