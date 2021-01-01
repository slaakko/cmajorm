// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYMBOLS_DEBUG_FLAGS_INCLUDED
#define CMAJOR_SYMBOLS_DEBUG_FLAGS_INCLUDED

// ===========
// Debug flags
// ===========

// MODULE_CHECKING 
// ---------------
// Uncomment the following line to enable checking validity of module'a symbol table after it has been read from the module file.
// #define MODULE_CHECKING 1

// IMMUTABLE_MODULE_CHECK
// ----------------------
// Uncomment the following line to enable checking that a module that has been read from the module file and has been set immutable is not modified.
// #define IMMUTABLE_MODULE_CHECK 1

// VALID_CONVERSION_TABLE_ENTRY_CHECK
// ----------------------------------
// Uncomment the following line to enable checking that conversion table entries are valid.
// #define VALID_CONVERSION_TABLE_ENTRY_CHECK 1

// MODULE_READING_DEBUG
// --------------------
// Uncomment the following line to show debug messages related to reading and importing modules.
// #define MODULE_READING_DEBUG 1

#endif // CMAJOR_SYMBOLS_DEBUG_FLAGS_INCLUDED
