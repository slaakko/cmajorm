// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_UTIL_SOCKET_FWD_INCLUDED
#define SOULNG_UTIL_SOCKET_FWD_INCLUDED
#include <soulng/util/UtilApi.hpp>
#include <string>

namespace soulng { namespace util {

class TcpSocket;

UTIL_API void Write(TcpSocket& socket, const std::string& s);
UTIL_API std::string ReadStr(TcpSocket& socket);

} } // namespace soulng::util

#endif // SOULNG_UTIL_SOCKET_FWD_INCLUDED

