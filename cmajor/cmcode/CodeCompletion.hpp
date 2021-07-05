// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMCODE_CODE_COMPLETION_INCLUDED
#define CMCODE_CODE_COMPLETION_INCLUDED

namespace cmcode {

void StartCodeCompletionService(int pid);
void StopCodeCompletionService(bool log);

} // namespace cmcode

#endif // CMCODE_CODE_COMPLETION_INCLUDED
