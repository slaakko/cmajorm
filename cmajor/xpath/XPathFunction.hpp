// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_XPATH_XPATH_FUNCTION
#define CMAJOR_XPATH_XPATH_FUNCTION
#include <cmajor/xpath/XPathObject.hpp>

namespace cmajor { namespace xpath {

class XPathContext;

std::u32string StringValue(cmajor::dom::Node* node);

class XPathFunction
{
public:
    XPathFunction(const std::u32string& name_, int minArity_, int maxArity_);
    virtual ~XPathFunction();
    virtual std::unique_ptr<XPathObject> Evaluate(XPathContext& context, std::vector<XPathObject*>& arguments) = 0;
    const std::u32string& Name() const { return name; }
    int MinArity() const { return minArity; }
    int MaxArity() const { return maxArity; }
private:
    std::u32string name;
    int minArity;
    int maxArity;
};

XPathFunction* GetXPathLibraryFunction(const std::u32string& functionName);

void InitFunction();
void DoneFunction();

} } // namespace cmajor::xpath

#endif // CMAJOR_XPATH_XPATH_FUNCTION
