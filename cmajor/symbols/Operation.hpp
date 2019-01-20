// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYMBOLS_OPERATION_INCLUDED
#define CMAJOR_SYMBOLS_OPERATION_INCLUDED
#include <cmajor/symbols/FunctionSymbol.hpp>

namespace cmajor { namespace symbols {

class Operation
{
public:
    virtual ~Operation();
    FunctionSymbol* Get(std::vector<Operation*>& operations)
    {
        if (!fun)
        {
            fun.reset(Create());
            operations.push_back(this);
        }
        return fun.get();
    }
    void Release() 
    { 
        fun.reset(); 
    }
protected:
    virtual FunctionSymbol* Create() const = 0;
private:
    std::unique_ptr<FunctionSymbol> fun;
};

} } // namespace cmajor::symbols

#endif // CMAJOR_SYMBOLS_OPERATION_INCLUDED
