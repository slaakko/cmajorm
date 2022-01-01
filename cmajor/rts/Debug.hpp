// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_RTS_DEBUG_INCLUDED
#define CMAJOR_RTS_DEBUG_INCLUDED
#include <stdint.h>

namespace cmajor { namespace rt {

void StartCmdbSession();
bool IsCmdbSessionOpen();
void WriteBytesToCmdbSession(int fileHandle, const uint8_t* buffer, int64_t count);
int64_t ReadBytesFromCmdbSession(uint8_t* buffer, int64_t bufferSize);
void InitCmdbSession();
void DoneCmdbSession();

} } // namespace cmajor::rt

#endif // CMAJOR_RTS_DEBUG_INCLUDED
