// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SXBS_TOKENIZER_INCLUDED
#define SXBS_TOKENIZER_INCLUDED
#include <stdint.h>
#include <string>
#include <memory>
#include <vector>
#include <map>

namespace sxbs {

const int32_t END = 0;
const int32_t BUILD = 1;
const int32_t LINE = 2;
const int32_t EXIT = 3;
const int32_t STOP = 4;
const int32_t SERVER = 5;
const int32_t REQUEST = 6;
const int32_t REPLY = 7;
const int32_t DEBUG = 8;
const int32_t RELEASE = 9;
const int32_t ERROR = 10;
const int32_t ANY = 11;

class Tokenizer
{
public:
    Tokenizer(const std::string& messageStr);
    int32_t Pos() const { return pos; }
    void SetPos(int32_t pos_) { pos = pos_; }
    int32_t operator*() const;
    void operator++();
    std::string GetWord(int32_t pos) const;
    std::string ErrorLines() const;
private:
    std::vector<std::string> words;
    int32_t pos;
    int32_t farthestPos;
    std::map<std::string, int32_t> tokenMap;
};

} // sxbs

#endif // SXBS_TOKENIZER_INCLUDED
