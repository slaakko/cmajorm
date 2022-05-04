// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SXBS_SERVER_INCLUDED
#define SXBS_SERVER_INCLUDED
#include <stdint.h>
#include <string>

namespace sxbs {

const int defaultPortNumber = 55001;

void RunServer();
void NotifyStopServer();
void InitServer();
void DoneServer();

} // sxbs

#endif // SXBS_SERVER_INCLUDED
