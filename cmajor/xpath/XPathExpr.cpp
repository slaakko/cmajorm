// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/xpath/XPathExpr.hpp>
#include <cmajor/xpath/XPathFunction.hpp>
#include <cmajor/dom/CharacterData.hpp>
#include <cmajor/dom/Document.hpp>
#include <cmajor/dom/Element.hpp>
#include <cmajor/util/Unicode.hpp>
#include <map>
#include <sstream>

namespace cmajor { namespace xpath {

using namespace cmajor::unicode;

XPathExpr::~XPathExpr()
{
}

XPathUnaryExpr::XPathUnaryExpr(XPathExpr* operand_) : operand(operand_)
{
}

XPathBinaryExpr::XPathBinaryExpr(XPathExpr* left_, XPathExpr* right_) : left(left_), right(right_)
{
}

XPathOrExpr::XPathOrExpr(XPathExpr* left_, XPathExpr* right_) : XPathBinaryExpr(left_, right_)
{
}

std::unique_ptr<XPathObject> XPathOrExpr::Evaluate(XPathContext& context)
{
    XPathFunction* boolean = GetXPathLibraryFunction(U"boolean");
    std::unique_ptr<XPathObject> left = Left()->Evaluate(context);
    std::vector<XPathObject*> leftArgs;
    leftArgs.push_back(left.get());
    std::unique_ptr<XPathObject> leftAsBoolean = boolean->Evaluate(context, leftArgs);
    if (leftAsBoolean->Type() != XPathObjectType::boolean)
    {
        throw std::runtime_error("boolean result expected");
    }
    if (static_cast<XPathBoolean*>(leftAsBoolean.get())->Value())
    {
        return leftAsBoolean;
    }
    std::unique_ptr<XPathObject> right = Right()->Evaluate(context);
    std::vector<XPathObject*> rightArgs;
    rightArgs.push_back(right.get());
    std::unique_ptr<XPathObject> rightAsBoolean = boolean->Evaluate(context, rightArgs);
    if (rightAsBoolean->Type() != XPathObjectType::boolean)
    {
        throw std::runtime_error("boolean result expected");
    }
    return rightAsBoolean;
}

std::unique_ptr<dom::Node> XPathOrExpr::ToDom() const
{
    std::unique_ptr<dom::Element> element(new dom::Element(U"or"));
    element->AppendChild(Left()->ToDom());
    element->AppendChild(Right()->ToDom());
    return std::unique_ptr<dom::Node>(element.release());
}

XPathAndExpr::XPathAndExpr(XPathExpr* left_, XPathExpr* right_) : XPathBinaryExpr(left_, right_)
{
}

std::unique_ptr<XPathObject> XPathAndExpr::Evaluate(XPathContext& context)
{
    XPathFunction* boolean = GetXPathLibraryFunction(U"boolean");
    std::unique_ptr<XPathObject> left = Left()->Evaluate(context);
    std::vector<XPathObject*> leftArgs;
    leftArgs.push_back(left.get());
    std::unique_ptr<XPathObject> leftAsBoolean = boolean->Evaluate(context, leftArgs);
    if (leftAsBoolean->Type() != XPathObjectType::boolean)
    {
        throw std::runtime_error("boolean result expected");
    }
    if (!static_cast<XPathBoolean*>(leftAsBoolean.get())->Value())
    {
        return leftAsBoolean;
    }
    std::unique_ptr<XPathObject> right = Right()->Evaluate(context);
    std::vector<XPathObject*> rightArgs;
    rightArgs.push_back(right.get());
    std::unique_ptr<XPathObject> rightAsBoolean = boolean->Evaluate(context, rightArgs);
    if (rightAsBoolean->Type() != XPathObjectType::boolean)
    {
        throw std::runtime_error("boolean result expected");
    }
    return rightAsBoolean;
}

std::unique_ptr<dom::Node> XPathAndExpr::ToDom() const
{
    std::unique_ptr<dom::Element> element(new dom::Element(U"and"));
    element->AppendChild(Left()->ToDom());
    element->AppendChild(Right()->ToDom());
    return std::unique_ptr<dom::Node>(element.release());
}

std::unique_ptr<XPathObject> CompareNodeSets(XPathContext& context, XPathObject* left, XPathObject* right, Operator comparisonOp)
{
    if (left->Type() == XPathObjectType::nodeSet && right->Type() == XPathObjectType::nodeSet)
    {
        XPathNodeSet* leftNodeSet = static_cast<XPathNodeSet*>(left);
        XPathNodeSet* rightNodeSet = static_cast<XPathNodeSet*>(right);
        int n = leftNodeSet->Length();
        for (int i = 0; i < n; ++i)
        {
            cmajor::dom::Node* leftNode = (*leftNodeSet)[i];
            std::u32string leftStr = StringValue(leftNode);
            int m = rightNodeSet->Length();
            for (int j = 0; j < m; ++j)
            {
                cmajor::dom::Node* rightNode = (*rightNodeSet)[j];
                std::u32string rightStr = StringValue(rightNode);
                switch (comparisonOp)
                {
                    case Operator::equal:
                    {
                        if (leftStr == rightStr)
                        {
                            std::unique_ptr<XPathObject> result(new XPathBoolean(true));
                            return result;
                        }
                        break;
                    }
                    case Operator::notEqual:
                    {
                        if (leftStr != rightStr)
                        {
                            std::unique_ptr<XPathObject> result(new XPathBoolean(true));
                            return result;
                        }
                        break;
                    }
                    case Operator::less:
                    {
                        if (leftStr < rightStr)
                        {
                            std::unique_ptr<XPathObject> result(new XPathBoolean(true));
                            return result;
                        }
                        break;
                    }
                    case Operator::greater:
                    {
                        if (leftStr > rightStr)
                        {
                            std::unique_ptr<XPathObject> result(new XPathBoolean(true));
                            return result;
                        }
                        break;
                    }
                    case Operator::lessOrEqual:
                    {
                        if (leftStr <= rightStr)
                        {
                            std::unique_ptr<XPathObject> result(new XPathBoolean(true));
                            return result;
                        }
                        break;
                    }
                    case Operator::greaterOrEqual:
                    {
                        if (leftStr >= rightStr)
                        {
                            std::unique_ptr<XPathObject> result(new XPathBoolean(true));
                            return result;
                        }
                        break;
                    }
                }
            }
        }
    }
    else if (left->Type() == XPathObjectType::nodeSet)
    {
        XPathFunction* numberFunction = GetXPathLibraryFunction(U"number");
        XPathFunction* booleanFunction = GetXPathLibraryFunction(U"boolean");
        if (right->Type() == XPathObjectType::number)
        {
            double rightNumber = static_cast<XPathNumber*>(right)->Value();
            XPathNodeSet* leftNodeSet = static_cast<XPathNodeSet*>(left);
            int n = leftNodeSet->Length();
            for (int i = 0; i < n; ++i)
            {
                cmajor::dom::Node* leftNode = (*leftNodeSet)[i];
                XPathString leftAsString(StringValue(leftNode));
                std::vector<XPathObject*> leftArgs;
                leftArgs.push_back(&leftAsString);
                std::unique_ptr<XPathObject> leftAsNumber = numberFunction->Evaluate(context, leftArgs);
                if (leftAsNumber->Type() != XPathObjectType::number)
                {
                    throw std::runtime_error("number result expected");
                }
                double leftNumber = static_cast<XPathNumber*>(leftAsNumber.get())->Value();
                switch (comparisonOp)
                {
                    case Operator::equal:
                    {
                        if (leftNumber == rightNumber)
                        {
                            std::unique_ptr<XPathObject> result(new XPathBoolean(true));
                            return result;
                        }
                        break;
                    }
                    case Operator::notEqual:
                    {
                        if (leftNumber != rightNumber)
                        {
                            std::unique_ptr<XPathObject> result(new XPathBoolean(true));
                            return result;
                        }
                        break;
                    }
                    case Operator::less:
                    {
                        if (leftNumber < rightNumber)
                        {
                            std::unique_ptr<XPathObject> result(new XPathBoolean(true));
                            return result;
                        }
                        break;
                    }
                    case Operator::greater:
                    {
                        if (leftNumber > rightNumber)
                        {
                            std::unique_ptr<XPathObject> result(new XPathBoolean(true));
                            return result;
                        }
                        break;
                    }
                    case Operator::lessOrEqual:
                    {
                        if (leftNumber <= rightNumber)
                        {
                            std::unique_ptr<XPathObject> result(new XPathBoolean(true));
                            return result;
                        }
                        break;
                    }
                    case Operator::greaterOrEqual:
                    {
                        if (leftNumber >= rightNumber)
                        {
                            std::unique_ptr<XPathObject> result(new XPathBoolean(true));
                            return result;
                        }
                        break;
                    }
                }
            }
        }
        else if (right->Type() == XPathObjectType::string)
        {
            std::u32string rightString = static_cast<XPathString*>(right)->Value();
            XPathNodeSet* leftNodeSet = static_cast<XPathNodeSet*>(left);
            int n = leftNodeSet->Length();
            for (int i = 0; i < n; ++i)
            {
                cmajor::dom::Node* leftNode = (*leftNodeSet)[i];
                std::u32string leftString = StringValue(leftNode);
                switch (comparisonOp)
                {
                    case Operator::equal:
                    {
                        if (leftString == rightString)
                        {
                            std::unique_ptr<XPathObject> result(new XPathBoolean(true));
                            return result;
                        }
                        break;
                    }
                    case Operator::notEqual:
                    {
                        if (leftString != rightString)
                        {
                            std::unique_ptr<XPathObject> result(new XPathBoolean(true));
                            return result;
                        }
                        break;
                    }
                    case Operator::less:
                    {
                        if (leftString < rightString)
                        {
                            std::unique_ptr<XPathObject> result(new XPathBoolean(true));
                            return result;
                        }
                        break;
                    }
                    case Operator::greater:
                    {
                        if (leftString > rightString)
                        {
                            std::unique_ptr<XPathObject> result(new XPathBoolean(true));
                            return result;
                        }
                        break;
                    }
                    case Operator::lessOrEqual:
                    {
                        if (leftString <= rightString)
                        {
                            std::unique_ptr<XPathObject> result(new XPathBoolean(true));
                            return result;
                        }
                        break;
                    }
                    case Operator::greaterOrEqual:
                    {
                        if (leftString >= rightString)
                        {
                            std::unique_ptr<XPathObject> result(new XPathBoolean(true));
                            return result;
                        }
                        break;
                    }
                }
            }
        }
        else if (right->Type() == XPathObjectType::boolean)
        {
            bool rightBool = static_cast<XPathBoolean*>(right)->Value();
            std::vector<XPathObject*> leftArgs;
            leftArgs.push_back(left);
            std::unique_ptr<XPathObject> leftAsBool = booleanFunction->Evaluate(context, leftArgs);
            if (leftAsBool->Type() != XPathObjectType::boolean)
            {
                throw std::runtime_error("boolean result expected");
            }
            bool leftBool = static_cast<XPathBoolean*>(leftAsBool.get())->Value();
            switch (comparisonOp)
            {
                case Operator::equal:
                {
                    if (leftBool == rightBool)
                    {
                        std::unique_ptr<XPathObject> result(new XPathBoolean(true));
                        return result;
                    }
                    break;
                }
                case Operator::notEqual:
                {
                    if (leftBool != rightBool)
                    {
                        std::unique_ptr<XPathObject> result(new XPathBoolean(true));
                        return result;
                    }
                    break;
                }
                case Operator::less:
                {
                    if (leftBool < rightBool)
                    {
                        std::unique_ptr<XPathObject> result(new XPathBoolean(true));
                        return result;
                    }
                    break;
                }
                case Operator::greater:
                {
                    if (leftBool > rightBool)
                    {
                        std::unique_ptr<XPathObject> result(new XPathBoolean(true));
                        return result;
                    }
                    break;
                }
                case Operator::lessOrEqual:
                {
                    if (leftBool <= rightBool)
                    {
                        std::unique_ptr<XPathObject> result(new XPathBoolean(true));
                        return result;
                    }
                    break;
                }
                case Operator::greaterOrEqual:
                {
                    if (leftBool >= rightBool)
                    {
                        std::unique_ptr<XPathObject> result(new XPathBoolean(true));
                        return result;
                    }
                    break;
                }
            }
        }
    }
    else if (right->Type() == XPathObjectType::nodeSet)
    {
        XPathFunction* numberFunction = GetXPathLibraryFunction(U"number");
        XPathFunction* booleanFunction = GetXPathLibraryFunction(U"boolean");
        if (left->Type() == XPathObjectType::number)
        {
            double leftNumber = static_cast<XPathNumber*>(left)->Value();
            XPathNodeSet* rightNodeSet = static_cast<XPathNodeSet*>(right);
            int n = rightNodeSet->Length();
            for (int i = 0; i < n; ++i)
            {
                cmajor::dom::Node* rightNode = (*rightNodeSet)[i];
                XPathString rightAsString(StringValue(rightNode));
                std::vector<XPathObject*> rightArgs;
                rightArgs.push_back(&rightAsString);
                std::unique_ptr<XPathObject> rightAsNumber = numberFunction->Evaluate(context, rightArgs);
                if (rightAsNumber->Type() != XPathObjectType::number)
                {
                    throw std::runtime_error("number result expected");
                }
                double rightNumber = static_cast<XPathNumber*>(rightAsNumber.get())->Value();
                switch (comparisonOp)
                {
                    case Operator::equal:
                    {
                        if (leftNumber == rightNumber)
                        {
                            std::unique_ptr<XPathObject> result(new XPathBoolean(true));
                            return result;
                        }
                        break;
                    }
                    case Operator::notEqual:
                    {
                        if (leftNumber != rightNumber)
                        {
                            std::unique_ptr<XPathObject> result(new XPathBoolean(true));
                            return result;
                        }
                        break;
                    }
                    case Operator::less:
                    {
                        if (leftNumber < rightNumber)
                        {
                            std::unique_ptr<XPathObject> result(new XPathBoolean(true));
                            return result;
                        }
                        break;
                    }
                    case Operator::greater:
                    {
                        if (leftNumber > rightNumber)
                        {
                            std::unique_ptr<XPathObject> result(new XPathBoolean(true));
                            return result;
                        }
                        break;
                    }
                    case Operator::lessOrEqual:
                    {
                        if (leftNumber <= rightNumber)
                        {
                            std::unique_ptr<XPathObject> result(new XPathBoolean(true));
                            return result;
                        }
                        break;
                    }
                    case Operator::greaterOrEqual:
                    {
                        if (leftNumber >= rightNumber)
                        {
                            std::unique_ptr<XPathObject> result(new XPathBoolean(true));
                            return result;
                        }
                        break;
                    }
                }
            }
        }
        else if (left->Type() == XPathObjectType::string)
        {
            std::u32string leftString = static_cast<XPathString*>(left)->Value();
            XPathNodeSet* rightNodeSet = static_cast<XPathNodeSet*>(right);
            int n = rightNodeSet->Length();
            for (int i = 0; i < n; ++i)
            {
                cmajor::dom::Node* rightNode = (*rightNodeSet)[i];
                std::u32string rightString = StringValue(rightNode);
                switch (comparisonOp)
                {
                    case Operator::equal:
                    {
                        if (leftString == rightString)
                        {
                            std::unique_ptr<XPathObject> result(new XPathBoolean(true));
                            return result;
                        }
                        break;
                    }
                    case Operator::notEqual:
                    {
                        if (leftString != rightString)
                        {
                            std::unique_ptr<XPathObject> result(new XPathBoolean(true));
                            return result;
                        }
                        break;
                    }
                    case Operator::less:
                    {
                        if (leftString < rightString)
                        {
                            std::unique_ptr<XPathObject> result(new XPathBoolean(true));
                            return result;
                        }
                        break;
                    }
                    case Operator::greater:
                    {
                        if (leftString > rightString)
                        {
                            std::unique_ptr<XPathObject> result(new XPathBoolean(true));
                            return result;
                        }
                        break;
                    }
                    case Operator::lessOrEqual:
                    {
                        if (leftString <= rightString)
                        {
                            std::unique_ptr<XPathObject> result(new XPathBoolean(true));
                            return result;
                        }
                        break;
                    }
                    case Operator::greaterOrEqual:
                    {
                        if (leftString >= rightString)
                        {
                            std::unique_ptr<XPathObject> result(new XPathBoolean(true));
                            return result;
                        }
                        break;
                    }
                }
            }
        }
        else if (left->Type() == XPathObjectType::boolean)
        {
            bool leftBool = static_cast<XPathBoolean*>(left)->Value();
            std::vector<XPathObject*> rightArgs;
            rightArgs.push_back(right);
            std::unique_ptr<XPathObject> rightAsBool = booleanFunction->Evaluate(context, rightArgs);
            if (rightAsBool->Type() != XPathObjectType::boolean)
            {
                throw std::runtime_error("boolean result expected");
            }
            bool rightBool = static_cast<XPathBoolean*>(rightAsBool.get())->Value();
            switch (comparisonOp)
            {
                case Operator::equal:
                {
                    if (leftBool == rightBool)
                    {
                        std::unique_ptr<XPathObject> result(new XPathBoolean(true));
                        return result;
                    }
                    break;
                }
                case Operator::notEqual:
                {
                    if (leftBool != rightBool)
                    {
                        std::unique_ptr<XPathObject> result(new XPathBoolean(true));
                        return result;
                    }
                    break;
                }
                case Operator::less:
                {
                    if (leftBool < rightBool)
                    {
                        std::unique_ptr<XPathObject> result(new XPathBoolean(true));
                        return result;
                    }
                    break;
                }
                case Operator::greater:
                {
                    if (leftBool > rightBool)
                    {
                        std::unique_ptr<XPathObject> result(new XPathBoolean(true));
                        return result;
                    }
                    break;
                }
                case Operator::lessOrEqual:
                {
                    if (leftBool <= rightBool)
                    {
                        std::unique_ptr<XPathObject> result(new XPathBoolean(true));
                        return result;
                    }
                    break;
                }
                case Operator::greaterOrEqual:
                {
                    if (leftBool >= rightBool)
                    {
                        std::unique_ptr<XPathObject> result(new XPathBoolean(true));
                        return result;
                    }
                    break;
                }
            }
        }
    }
    std::unique_ptr<XPathObject> result(new XPathBoolean(false));
    return result;
}

std::unique_ptr<XPathObject> CompareEquality(XPathContext& context, XPathObject* left, XPathObject* right)
{
    if (left->Type() == XPathObjectType::nodeSet || right->Type() == XPathObjectType::nodeSet)
    {
        return CompareNodeSets(context, left, right, Operator::equal);
    }
    else 
    {
        XPathFunction* numberFunction = GetXPathLibraryFunction(U"number");
        XPathFunction* booleanFunction = GetXPathLibraryFunction(U"boolean");
        XPathFunction* stringFunction = GetXPathLibraryFunction(U"string");
        if (left->Type() == XPathObjectType::number || right->Type() == XPathObjectType::number)
        {
            std::vector<XPathObject*> leftArgs;
            leftArgs.push_back(left);
            std::unique_ptr<XPathObject> leftAsNumber = numberFunction->Evaluate(context, leftArgs);
            if (leftAsNumber->Type() != XPathObjectType::number)
            {
                throw std::runtime_error("number result expected");
            }
            double leftNumber = static_cast<XPathNumber*>(leftAsNumber.get())->Value();
            std::vector<XPathObject*> rightArgs;
            rightArgs.push_back(right);
            std::unique_ptr<XPathObject> rightAsNumber = numberFunction->Evaluate(context, rightArgs);
            if (rightAsNumber->Type() != XPathObjectType::number)
            {
                throw std::runtime_error("number result expected");
            }
            double rightNumber = static_cast<XPathNumber*>(rightAsNumber.get())->Value();
            if (leftNumber == rightNumber)
            {
                std::unique_ptr<XPathObject> result(new XPathBoolean(true));
                return result;
            }
            else
            {
                std::unique_ptr<XPathObject> result(new XPathBoolean(false));
                return result;
            }
        }
        else
        {
            std::vector<XPathObject*> leftArgs;
            leftArgs.push_back(left);
            std::unique_ptr<XPathObject> leftAsString = stringFunction->Evaluate(context, leftArgs);
            if (leftAsString->Type() != XPathObjectType::string)
            {
                throw std::runtime_error("string result expected");
            }
            std::u32string leftString = static_cast<XPathString*>(leftAsString.get())->Value();
            std::vector<XPathObject*> rightArgs;
            rightArgs.push_back(right);
            std::unique_ptr<XPathObject> rightAsString = stringFunction->Evaluate(context, rightArgs);
            if (rightAsString->Type() != XPathObjectType::string)
            {
                throw std::runtime_error("string result expected");
            }
            std::u32string rightString = static_cast<XPathString*>(rightAsString.get())->Value();
            if (leftString == rightString)
            {
                std::unique_ptr<XPathObject> result(new XPathBoolean(true));
                return result;
            }
            else
            {
                std::unique_ptr<XPathObject> result(new XPathBoolean(false));
                return result;
            }
        }
    }
    std::unique_ptr<XPathObject> result(new XPathBoolean(false));
    return result;
}

XPathEqualExpr::XPathEqualExpr(XPathExpr* left_, XPathExpr* right_) : XPathBinaryExpr(left_, right_)
{
}

std::unique_ptr<XPathObject> XPathEqualExpr::Evaluate(XPathContext& context)
{
    std::unique_ptr<XPathObject> left = Left()->Evaluate(context);
    std::unique_ptr<XPathObject> right = Right()->Evaluate(context);
    return CompareEquality(context, left.get(), right.get());
}

std::unique_ptr<dom::Node> XPathEqualExpr::ToDom() const
{
    std::unique_ptr<dom::Element> element(new dom::Element(U"equal"));
    element->AppendChild(Left()->ToDom());
    element->AppendChild(Right()->ToDom());
    return std::unique_ptr<dom::Node>(element.release());
}

XPathNotEqualExpr::XPathNotEqualExpr(XPathExpr* left_, XPathExpr* right_) : XPathBinaryExpr(left_, right_)
{
}

std::unique_ptr<XPathObject> XPathNotEqualExpr::Evaluate(XPathContext& context)
{
    std::unique_ptr<XPathObject> left = Left()->Evaluate(context);
    std::unique_ptr<XPathObject> right = Right()->Evaluate(context);
    std::unique_ptr<XPathObject> equal = CompareEquality(context, left.get(), right.get());
    if (equal->Type() != XPathObjectType::boolean)
    {
        throw std::runtime_error("boolean result expected");
    }
    return std::unique_ptr<XPathObject>(new XPathBoolean(!static_cast<XPathBoolean*>(equal.get())->Value()));
}

std::unique_ptr<dom::Node> XPathNotEqualExpr::ToDom() const
{
    std::unique_ptr<dom::Element> element(new dom::Element(U"notEqual"));
    element->AppendChild(Left()->ToDom());
    element->AppendChild(Right()->ToDom());
    return std::unique_ptr<dom::Node>(element.release());
}

std::unique_ptr<XPathObject> Compare(XPathContext& context, XPathObject* left, XPathObject* right, Operator comparisonOp)
{
    if (left->Type() == XPathObjectType::nodeSet || right->Type() == XPathObjectType::nodeSet)
    {
        return CompareNodeSets(context, left, right, comparisonOp);
    }
    else
    {
        XPathFunction* numberFunction = GetXPathLibraryFunction(U"number");
        std::vector<XPathObject*> leftArgs;
        leftArgs.push_back(left);
        std::unique_ptr<XPathObject> leftAsNumber = numberFunction->Evaluate(context, leftArgs);
        if (leftAsNumber->Type() != XPathObjectType::number)
        {
            throw std::runtime_error("number result expected");
        }
        double leftNumber = static_cast<XPathNumber*>(leftAsNumber.get())->Value();
        std::vector<XPathObject*> rightArgs;
        rightArgs.push_back(right);
        std::unique_ptr<XPathObject> rightAsNumber = numberFunction->Evaluate(context, rightArgs);
        if (rightAsNumber->Type() != XPathObjectType::number)
        {
            throw std::runtime_error("number result expected");
        }
        double rightNumber = static_cast<XPathNumber*>(rightAsNumber.get())->Value();
        switch (comparisonOp)
        {
            case Operator::less:
            {
                if (leftNumber < rightNumber)
                {
                    std::unique_ptr<XPathObject> result(new XPathBoolean(true));
                    return result;
                }
                else
                {
                    std::unique_ptr<XPathObject> result(new XPathBoolean(false));
                    return result;
                }
                break;
            }
            case Operator::greater:
            {
                if (leftNumber > rightNumber)
                {
                    std::unique_ptr<XPathObject> result(new XPathBoolean(true));
                    return result;
                }
                else
                {
                    std::unique_ptr<XPathObject> result(new XPathBoolean(false));
                    return result;
                }
                break;
            }
            case Operator::lessOrEqual:
            {
                if (leftNumber <= rightNumber)
                {
                    std::unique_ptr<XPathObject> result(new XPathBoolean(true));
                    return result;
                }
                else
                {
                    std::unique_ptr<XPathObject> result(new XPathBoolean(false));
                    return result;
                }
                break;
            }
            case Operator::greaterOrEqual:
            {
                if (leftNumber >= rightNumber)
                {
                    std::unique_ptr<XPathObject> result(new XPathBoolean(true));
                    return result;
                }
                else
                {
                    std::unique_ptr<XPathObject> result(new XPathBoolean(false));
                    return result;
                }
                break;
            }
        }
    }
    std::unique_ptr<XPathObject> result(new XPathBoolean(false));
    return result;
}

XPathLessExpr::XPathLessExpr(XPathExpr* left_, XPathExpr* right_) : XPathBinaryExpr(left_, right_)
{
}

std::unique_ptr<XPathObject> XPathLessExpr::Evaluate(XPathContext& context)
{
    std::unique_ptr<XPathObject> left = Left()->Evaluate(context);
    std::unique_ptr<XPathObject> right = Right()->Evaluate(context);
    return Compare(context, left.get(), right.get(), Operator::less);
}

std::unique_ptr<dom::Node> XPathLessExpr::ToDom() const
{
    std::unique_ptr<dom::Element> element(new dom::Element(U"less"));
    element->AppendChild(Left()->ToDom());
    element->AppendChild(Right()->ToDom());
    return std::unique_ptr<dom::Node>(element.release());
}

XPathGreaterExpr::XPathGreaterExpr(XPathExpr* left_, XPathExpr* right_) : XPathBinaryExpr(left_, right_)
{
}

std::unique_ptr<XPathObject> XPathGreaterExpr::Evaluate(XPathContext& context)
{
    std::unique_ptr<XPathObject> left = Left()->Evaluate(context);
    std::unique_ptr<XPathObject> right = Right()->Evaluate(context);
    return Compare(context, left.get(), right.get(), Operator::greater);
}

std::unique_ptr<dom::Node> XPathGreaterExpr::ToDom() const
{
    std::unique_ptr<dom::Element> element(new dom::Element(U"greater"));
    element->AppendChild(Left()->ToDom());
    element->AppendChild(Right()->ToDom());
    return std::unique_ptr<dom::Node>(element.release());
}

XPathLessOrEqualExpr::XPathLessOrEqualExpr(XPathExpr* left_, XPathExpr* right_) : XPathBinaryExpr(left_, right_)
{
}

std::unique_ptr<XPathObject> XPathLessOrEqualExpr::Evaluate(XPathContext& context)
{
    std::unique_ptr<XPathObject> left = Left()->Evaluate(context);
    std::unique_ptr<XPathObject> right = Right()->Evaluate(context);
    return Compare(context, left.get(), right.get(), Operator::lessOrEqual);
}

std::unique_ptr<dom::Node> XPathLessOrEqualExpr::ToDom() const
{
    std::unique_ptr<dom::Element> element(new dom::Element(U"lessOrEqual"));
    element->AppendChild(Left()->ToDom());
    element->AppendChild(Right()->ToDom());
    return std::unique_ptr<dom::Node>(element.release());
}

XPathGreaterOrEqualExpr::XPathGreaterOrEqualExpr(XPathExpr* left_, XPathExpr* right_) : XPathBinaryExpr(left_, right_)
{
}

std::unique_ptr<XPathObject> XPathGreaterOrEqualExpr::Evaluate(XPathContext& context)
{
    std::unique_ptr<XPathObject> left = Left()->Evaluate(context);
    std::unique_ptr<XPathObject> right = Right()->Evaluate(context);
    return Compare(context, left.get(), right.get(), Operator::greaterOrEqual);
}

std::unique_ptr<dom::Node> XPathGreaterOrEqualExpr::ToDom() const
{
    std::unique_ptr<dom::Element> element(new dom::Element(U"greaterOrEqual"));
    element->AppendChild(Left()->ToDom());
    element->AppendChild(Right()->ToDom());
    return std::unique_ptr<dom::Node>(element.release());
}

XPathAddExpr::XPathAddExpr(XPathExpr* left_, XPathExpr* right_) : XPathBinaryExpr(left_, right_)
{
}

std::unique_ptr<XPathObject> XPathAddExpr::Evaluate(XPathContext& context)
{
    std::unique_ptr<XPathObject> left = Left()->Evaluate(context);
    std::unique_ptr<XPathObject> right = Right()->Evaluate(context);
    XPathFunction* numberFunction = GetXPathLibraryFunction(U"number");
    std::vector<XPathObject*> leftArgs;
    leftArgs.push_back(left.get());
    std::unique_ptr<XPathObject> leftAsNumber = numberFunction->Evaluate(context, leftArgs);
    if (leftAsNumber->Type() != XPathObjectType::number)
    {
        throw std::runtime_error("number result expected");
    }
    double leftNumber = static_cast<XPathNumber*>(leftAsNumber.get())->Value();
    std::vector<XPathObject*> rightArgs;
    rightArgs.push_back(right.get());
    std::unique_ptr<XPathObject> rightAsNumber = numberFunction->Evaluate(context, rightArgs);
    if (rightAsNumber->Type() != XPathObjectType::number)
    {
        throw std::runtime_error("number result expected");
    }
    double rightNumber = static_cast<XPathNumber*>(rightAsNumber.get())->Value();
    return std::unique_ptr<XPathObject>(new XPathNumber(leftNumber + rightNumber));
}

std::unique_ptr<dom::Node> XPathAddExpr::ToDom() const
{
    std::unique_ptr<dom::Element> element(new dom::Element(U"add"));
    element->AppendChild(Left()->ToDom());
    element->AppendChild(Right()->ToDom());
    return std::unique_ptr<dom::Node>(element.release());
}

XPathSubExpr::XPathSubExpr(XPathExpr* left_, XPathExpr* right_) : XPathBinaryExpr(left_, right_)
{
}

std::unique_ptr<XPathObject> XPathSubExpr::Evaluate(XPathContext& context)
{
    std::unique_ptr<XPathObject> left = Left()->Evaluate(context);
    std::unique_ptr<XPathObject> right = Right()->Evaluate(context);
    XPathFunction* numberFunction = GetXPathLibraryFunction(U"number");
    std::vector<XPathObject*> leftArgs;
    leftArgs.push_back(left.get());
    std::unique_ptr<XPathObject> leftAsNumber = numberFunction->Evaluate(context, leftArgs);
    if (leftAsNumber->Type() != XPathObjectType::number)
    {
        throw std::runtime_error("number result expected");
    }
    double leftNumber = static_cast<XPathNumber*>(leftAsNumber.get())->Value();
    std::vector<XPathObject*> rightArgs;
    rightArgs.push_back(right.get());
    std::unique_ptr<XPathObject> rightAsNumber = numberFunction->Evaluate(context, rightArgs);
    if (rightAsNumber->Type() != XPathObjectType::number)
    {
        throw std::runtime_error("number result expected");
    }
    double rightNumber = static_cast<XPathNumber*>(rightAsNumber.get())->Value();
    return std::unique_ptr<XPathObject>(new XPathNumber(leftNumber - rightNumber));
}

std::unique_ptr<dom::Node> XPathSubExpr::ToDom() const
{
    std::unique_ptr<dom::Element> element(new dom::Element(U"sub"));
    element->AppendChild(Left()->ToDom());
    element->AppendChild(Right()->ToDom());
    return std::unique_ptr<dom::Node>(element.release());
}

XPathMulExpr::XPathMulExpr(XPathExpr* left_, XPathExpr* right_) : XPathBinaryExpr(left_, right_)
{
}

std::unique_ptr<XPathObject> XPathMulExpr::Evaluate(XPathContext& context)
{
    std::unique_ptr<XPathObject> left = Left()->Evaluate(context);
    std::unique_ptr<XPathObject> right = Right()->Evaluate(context);
    XPathFunction* numberFunction = GetXPathLibraryFunction(U"number");
    std::vector<XPathObject*> leftArgs;
    leftArgs.push_back(left.get());
    std::unique_ptr<XPathObject> leftAsNumber = numberFunction->Evaluate(context, leftArgs);
    if (leftAsNumber->Type() != XPathObjectType::number)
    {
        throw std::runtime_error("number result expected");
    }
    double leftNumber = static_cast<XPathNumber*>(leftAsNumber.get())->Value();
    std::vector<XPathObject*> rightArgs;
    rightArgs.push_back(right.get());
    std::unique_ptr<XPathObject> rightAsNumber = numberFunction->Evaluate(context, rightArgs);
    if (rightAsNumber->Type() != XPathObjectType::number)
    {
        throw std::runtime_error("number result expected");
    }
    double rightNumber = static_cast<XPathNumber*>(rightAsNumber.get())->Value();
    return std::unique_ptr<XPathObject>(new XPathNumber(leftNumber * rightNumber));
}

std::unique_ptr<dom::Node> XPathMulExpr::ToDom() const
{
    std::unique_ptr<dom::Element> element(new dom::Element(U"mul"));
    element->AppendChild(Left()->ToDom());
    element->AppendChild(Right()->ToDom());
    return std::unique_ptr<dom::Node>(element.release());
}

XPathDivExpr::XPathDivExpr(XPathExpr* left_, XPathExpr* right_) : XPathBinaryExpr(left_, right_)
{
}

std::unique_ptr<XPathObject> XPathDivExpr::Evaluate(XPathContext& context)
{
    std::unique_ptr<XPathObject> left = Left()->Evaluate(context);
    std::unique_ptr<XPathObject> right = Right()->Evaluate(context);
    XPathFunction* numberFunction = GetXPathLibraryFunction(U"number");
    std::vector<XPathObject*> leftArgs;
    leftArgs.push_back(left.get());
    std::unique_ptr<XPathObject> leftAsNumber = numberFunction->Evaluate(context, leftArgs);
    if (leftAsNumber->Type() != XPathObjectType::number)
    {
        throw std::runtime_error("number result expected");
    }
    double leftNumber = static_cast<XPathNumber*>(leftAsNumber.get())->Value();
    std::vector<XPathObject*> rightArgs;
    rightArgs.push_back(right.get());
    std::unique_ptr<XPathObject> rightAsNumber = numberFunction->Evaluate(context, rightArgs);
    if (rightAsNumber->Type() != XPathObjectType::number)
    {
        throw std::runtime_error("number result expected");
    }
    double rightNumber = static_cast<XPathNumber*>(rightAsNumber.get())->Value();
    return std::unique_ptr<XPathObject>(new XPathNumber(leftNumber / rightNumber));
}

std::unique_ptr<dom::Node> XPathDivExpr::ToDom() const
{
    std::unique_ptr<dom::Element> element(new dom::Element(U"div"));
    element->AppendChild(Left()->ToDom());
    element->AppendChild(Right()->ToDom());
    return std::unique_ptr<dom::Node>(element.release());
}

XPathModExpr::XPathModExpr(XPathExpr* left_, XPathExpr* right_) : XPathBinaryExpr(left_, right_)
{
}

std::unique_ptr<XPathObject> XPathModExpr::Evaluate(XPathContext& context)
{
    std::unique_ptr<XPathObject> left = Left()->Evaluate(context);
    std::unique_ptr<XPathObject> right = Right()->Evaluate(context);
    XPathFunction* numberFunction = GetXPathLibraryFunction(U"number");
    std::vector<XPathObject*> leftArgs;
    leftArgs.push_back(left.get());
    std::unique_ptr<XPathObject> leftAsNumber = numberFunction->Evaluate(context, leftArgs);
    if (leftAsNumber->Type() != XPathObjectType::number)
    {
        throw std::runtime_error("number result expected");
    }
    int64_t leftNumber = static_cast<int64_t>(static_cast<XPathNumber*>(leftAsNumber.get())->Value());
    std::vector<XPathObject*> rightArgs;
    rightArgs.push_back(right.get());
    std::unique_ptr<XPathObject> rightAsNumber = numberFunction->Evaluate(context, rightArgs);
    if (rightAsNumber->Type() != XPathObjectType::number)
    {
        throw std::runtime_error("number result expected");
    }
    int64_t rightNumber = static_cast<int64_t>(static_cast<XPathNumber*>(rightAsNumber.get())->Value());
    return std::unique_ptr<XPathObject>(new XPathNumber(double(leftNumber % rightNumber)));
}

std::unique_ptr<dom::Node> XPathModExpr::ToDom() const
{
    std::unique_ptr<dom::Element> element(new dom::Element(U"mod"));
    element->AppendChild(Left()->ToDom());
    element->AppendChild(Right()->ToDom());
    return std::unique_ptr<dom::Node>(element.release());
}

XPathUnaryMinusExpr::XPathUnaryMinusExpr(XPathExpr* operand_) : XPathUnaryExpr(operand_)
{
}

std::unique_ptr<XPathObject> XPathUnaryMinusExpr::Evaluate(XPathContext& context)
{
    std::unique_ptr<XPathObject> operand = Operand()->Evaluate(context);
    XPathFunction* numberFunction = GetXPathLibraryFunction(U"number");
    std::vector<XPathObject*> operandArgs;
    operandArgs.push_back(operand.get());
    std::unique_ptr<XPathObject> operandAsNumber = numberFunction->Evaluate(context, operandArgs);
    if (operandAsNumber->Type() != XPathObjectType::number)
    {
        throw std::runtime_error("number result expected");
    }
    double operandNumber = static_cast<XPathNumber*>(operandAsNumber.get())->Value();
    return std::unique_ptr<XPathObject>(new XPathNumber(-operandNumber));
}

std::unique_ptr<dom::Node> XPathUnaryMinusExpr::ToDom() const
{
    std::unique_ptr<dom::Element> element(new dom::Element(U"unaryMinus"));
    element->AppendChild(Operand()->ToDom());
    return std::unique_ptr<dom::Node>(element.release());
}

XPathUnionExpr::XPathUnionExpr(XPathExpr* left_, XPathExpr* right_) : XPathBinaryExpr(left_, right_)
{
}

std::unique_ptr<XPathObject> XPathUnionExpr::Evaluate(XPathContext& context)
{
    std::unique_ptr<XPathNodeSet> result(new XPathNodeSet());
    std::unique_ptr<XPathObject> leftResult = Left()->Evaluate(context);
    if (leftResult->Type() != XPathObjectType::nodeSet)
    {
        throw std::runtime_error("node set expected");
    }
    XPathNodeSet* leftNodeSet = static_cast<XPathNodeSet*>(leftResult.get());
    int n = leftNodeSet->Length();
    for (int i = 0; i < n; ++i)
    {
        result->Add((*leftNodeSet)[i]);
    }
    std::unique_ptr<XPathObject> rightResult = Right()->Evaluate(context);
    if (rightResult->Type() != XPathObjectType::nodeSet)
    {
        throw std::runtime_error("node set expected");
    }
    XPathNodeSet* rightNodeSet = static_cast<XPathNodeSet*>(rightResult.get());
    int m = rightNodeSet->Length();
    for (int i = 0; i < m; ++i)
    {
        result->Add((*rightNodeSet)[i]);
    }
    return std::unique_ptr<XPathObject>(result.release());
}

std::unique_ptr<dom::Node> XPathUnionExpr::ToDom() const
{
    std::unique_ptr<dom::Element> element(new dom::Element(U"union"));
    element->AppendChild(Left()->ToDom());
    element->AppendChild(Right()->ToDom());
    return std::unique_ptr<dom::Node>(element.release());
}

XPathCombineStepExpr::XPathCombineStepExpr(XPathExpr* left_, XPathExpr* right_) : XPathBinaryExpr(left_, right_)
{
}

std::unique_ptr<XPathObject> XPathCombineStepExpr::Evaluate(XPathContext& context)
{
    std::unique_ptr<XPathNodeSet> result(new XPathNodeSet());
    std::unique_ptr<XPathObject> leftResult = Left()->Evaluate(context);
    if (leftResult->Type() != XPathObjectType::nodeSet)
    {
        throw std::runtime_error("node set expected");
    }
    XPathNodeSet* leftNodeSet = static_cast<XPathNodeSet*>(leftResult.get());
    int n = leftNodeSet->Length();
    for (int i = 0; i < n; ++i)
    {
        cmajor::dom::Node* node = (*leftNodeSet)[i];
        XPathContext rightContext(node, i + 1, n);
        std::unique_ptr<XPathObject> rightResult = Right()->Evaluate(rightContext);
        if (rightResult->Type() != XPathObjectType::nodeSet)
        {
            throw std::runtime_error("node set expected");
        }
        XPathNodeSet* rightNodeSet = static_cast<XPathNodeSet*>(rightResult.get());
        int m = rightNodeSet->Length();
        for (int i = 0; i < m; ++i)
        { 
            cmajor::dom::Node* node = (*rightNodeSet)[i];
            result->Add(node);
        }
    }
    return std::unique_ptr<XPathObject>(result.release());
}

std::unique_ptr<dom::Node> XPathCombineStepExpr::ToDom() const
{
    std::unique_ptr<dom::Element> element(new dom::Element(U"combineStep"));
    element->AppendChild(Left()->ToDom());
    element->AppendChild(Right()->ToDom());
    return std::unique_ptr<dom::Node>(element.release());
}

XPathRootNodeExpr::XPathRootNodeExpr()
{
}

std::unique_ptr<XPathObject> XPathRootNodeExpr::Evaluate(XPathContext& context)
{
    std::unique_ptr<XPathNodeSet> nodeSet(new XPathNodeSet());
    if (context.Node()->GetNodeType() == cmajor::dom::NodeType::documentNode)
    {
        nodeSet->Add(context.Node());
    }
    else
    {
        nodeSet->Add(context.Node()->OwnerDocument());
    }
    return std::unique_ptr<XPathObject>(nodeSet.release());
}

std::unique_ptr<dom::Node> XPathRootNodeExpr::ToDom() const
{
    std::unique_ptr<dom::Element> element(new dom::Element(U"root"));
    return std::unique_ptr<dom::Node>(element.release());
}

XPathFilterExpr::XPathFilterExpr(XPathExpr* expr_, XPathExpr* predicate_) : XPathUnaryExpr(expr_), predicate(predicate_)
{
}

std::unique_ptr<XPathObject> XPathFilterExpr::Evaluate(XPathContext& context)
{
    std::unique_ptr<XPathObject> result = Operand()->Evaluate(context);
    if (result->Type() != XPathObjectType::nodeSet)
    {
        throw std::runtime_error("node-set expected");
    }
    std::unique_ptr<XPathNodeSet> nodeSet(static_cast<XPathNodeSet*>(result.release()));
    std::unique_ptr<XPathNodeSet> filteredNodeSet(new XPathNodeSet());
    int n = nodeSet->Length();
    for (int i = 0; i < n; ++i)
    {
        cmajor::dom::Node* node = (*nodeSet)[i];
        XPathContext context(node, i + 1, n);
        std::unique_ptr<XPathObject> result = predicate->Evaluate(context);
        bool booleanResult = false;
        if (result->Type() == XPathObjectType::number)
        {
            XPathNumber* number = static_cast<XPathNumber*>(result.get());
            if (number->Value() == context.Position())
            {
                booleanResult = true;
            }
        }
        else
        {
            XPathFunction* boolean = GetXPathLibraryFunction(U"boolean");
            std::vector<XPathObject*> args;
            args.push_back(result.get());
            std::unique_ptr<XPathObject> resultAsBoolean = boolean->Evaluate(context, args);
            booleanResult = static_cast<XPathBoolean*>(resultAsBoolean.get())->Value();
        }
        if (booleanResult)
        {
            filteredNodeSet->Add(node);
        }
    }
    std::swap(nodeSet, filteredNodeSet);
    return std::unique_ptr<XPathObject>(nodeSet.release());
}

std::unique_ptr<dom::Node> XPathFilterExpr::ToDom() const
{
    std::unique_ptr<dom::Element> element(new dom::Element(U"filter"));
    element->AppendChild(Operand()->ToDom());
    std::unique_ptr<dom::Element> predicateElement(new dom::Element(U"predicate"));
    predicateElement->AppendChild(predicate->ToDom());
    element->AppendChild(std::unique_ptr<dom::Node>(predicateElement.release()));
    return std::unique_ptr<dom::Node>(element.release());
}

class NodeSelectionOp : public cmajor::dom::NodeOp
{
public:
    NodeSelectionOp(XPathNodeTestExpr* nodeTest_, XPathNodeSet& nodeSet_, Axis axis_);
    void Apply(cmajor::dom::Node* node) override;
private:
    XPathNodeTestExpr* nodeTest;
    XPathNodeSet& nodeSet;
    Axis axis;
};

NodeSelectionOp::NodeSelectionOp(XPathNodeTestExpr* nodeTest_, XPathNodeSet& nodeSet_, Axis axis_) : nodeTest(nodeTest_), nodeSet(nodeSet_), axis(axis_)
{
}

void NodeSelectionOp::Apply(cmajor::dom::Node* node)
{
    if (nodeTest->Select(node, axis))
    {
        nodeSet.Add(node);
    }
}

XPathLocationStepExpr::XPathLocationStepExpr(Axis axis_, XPathNodeTestExpr* nodeTest_) : axis(axis_), nodeTest(nodeTest_)
{
}

void XPathLocationStepExpr::AddPredicate(XPathExpr* predicate)
{
    predicates.push_back(std::unique_ptr<XPathExpr>(predicate));
}

std::unique_ptr<XPathObject> XPathLocationStepExpr::Evaluate(XPathContext& context)
{
    std::unique_ptr<XPathNodeSet> nodeSet(new XPathNodeSet());
    NodeSelectionOp selectNodes(nodeTest.get(), *nodeSet, axis);
    context.Node()->Walk(selectNodes, axis);
    for (const std::unique_ptr<XPathExpr>& predicate : predicates)
    {
        std::unique_ptr<XPathNodeSet> filteredNodeSet(new XPathNodeSet());
        int n = nodeSet->Length();
        for (int i = 0; i < n; ++i)
        {
            cmajor::dom::Node* node = (*nodeSet)[i];
            XPathContext context(node, i + 1, n);
            std::unique_ptr<XPathObject> result = predicate->Evaluate(context);
            bool booleanResult = false;
            if (result->Type() == XPathObjectType::number)
            { 
                XPathNumber* number = static_cast<XPathNumber*>(result.get());
                if (number->Value() == context.Position())
                {
                    booleanResult = true;
                }
            }
            else
            {
                XPathFunction* boolean = GetXPathLibraryFunction(U"boolean");
                std::vector<XPathObject*> args;
                args.push_back(result.get());
                std::unique_ptr<XPathObject> resultAsBoolean = boolean->Evaluate(context, args);
                booleanResult = static_cast<XPathBoolean*>(resultAsBoolean.get())->Value();
            }
            if (booleanResult)
            {
                filteredNodeSet->Add(node);
            }
        }
        std::swap(nodeSet, filteredNodeSet);
    }
    return std::unique_ptr<XPathObject>(nodeSet.release());
}

std::unique_ptr<dom::Node> XPathLocationStepExpr::ToDom() const
{
    std::unique_ptr<dom::Element> element(new dom::Element(U"locationStep"));
    dom::Element* axisElement = new dom::Element(U"axis");
    std::u32string axisName = AxisName(axis);
    axisElement->SetAttribute(U"name", axisName);
    element->AppendChild(std::unique_ptr<dom::Node>(axisElement));
    dom::Element* nodeTestElement = new dom::Element(U"nodeTest");
    nodeTestElement->AppendChild(nodeTest->ToDom());
    element->AppendChild(std::unique_ptr<dom::Node>(nodeTestElement));
    std::unique_ptr<dom::Element> predicatesElement(new dom::Element(U"predicates"));
    for (const std::unique_ptr<XPathExpr>& predicate : predicates)
    {
        predicatesElement->AppendChild(predicate->ToDom());
    }
    element->AppendChild(std::unique_ptr<dom::Node>(predicatesElement.release()));
    return std::unique_ptr<dom::Node>(element.release());
}

class AxisMap
{
public:
    AxisMap();
    Axis GetAxis(const std::u32string& axis) const;
private:
    std::map<std::u32string, Axis> axisMap;
};

AxisMap::AxisMap()
{
    axisMap[U"ancestor"] = Axis::ancestor;
    axisMap[U"ancestor-or-self"] = Axis::ancestorOrSelf;
    axisMap[U"attribute"] = Axis::attribute;
    axisMap[U"child"] = Axis::child;
    axisMap[U"descendant"] = Axis::descendant;
    axisMap[U"descendant-or-self"] = Axis::descendantOrSelf;
    axisMap[U"following"] = Axis::following;
    axisMap[U"following-sibling"] = Axis::followingSibling;
    axisMap[U"namespace"] = Axis::ns;
    axisMap[U"parent"] = Axis::parent;
    axisMap[U"preceding"] = Axis::preceding;
    axisMap[U"preceding-sibling"] = Axis::precedingSibling;
    axisMap[U"self"] = Axis::self;
}

Axis AxisMap::GetAxis(const std::u32string& axis) const
{
    auto it = axisMap.find(axis);
    if (it != axisMap.cend())
    {
        return it->second;
    }
    else
    {
        throw std::runtime_error("axis '" + ToUtf8(axis) + "' not found");
    }
}

AxisMap axisMap;

Axis GetAxis(const std::u32string& axisName)
{
    return axisMap.GetAxis(axisName);
}

XPathPILiteralTest::XPathPILiteralTest(XPathExpr* literal_) : literal(literal_)
{
}

bool XPathPILiteralTest::Select(cmajor::dom::Node* node, Axis axis) const
{
    if (node->GetNodeType() == cmajor::dom::NodeType::processingInstructionNode)
    {
        cmajor::dom::ProcessingInstruction* pi = static_cast<cmajor::dom::ProcessingInstruction*>(node);
        if (pi->Target() == literal->TextValue())
        {
            return true;
        }
    }
    return false;
}

std::unique_ptr<dom::Node> XPathPILiteralTest::ToDom() const
{
    std::unique_ptr<dom::Element> element(new dom::Element(U"piLiteralTest"));
    element->AppendChild(literal->ToDom());
    return std::unique_ptr<dom::Node>(element.release());
}

bool XPathCommentNodeTest::Select(cmajor::dom::Node* node, Axis axis) const
{
    return node->GetNodeType() == cmajor::dom::NodeType::commentNode;
}

std::unique_ptr<dom::Node> XPathCommentNodeTest::ToDom() const
{
    std::unique_ptr<dom::Element> element(new dom::Element(U"commentNodeTest"));
    return std::unique_ptr<dom::Node>(element.release());
}

bool XPathTextNodeTest::Select(cmajor::dom::Node* node, Axis axis) const
{
    return node->GetNodeType() == cmajor::dom::NodeType::textNode;
}

std::unique_ptr<dom::Node> XPathTextNodeTest::ToDom() const
{
    std::unique_ptr<dom::Element> element(new dom::Element(U"textNodeTest"));
    return std::unique_ptr<dom::Node>(element.release());
}

bool XPathPINodeTest::Select(cmajor::dom::Node* node, Axis axis) const
{
    return node->GetNodeType() == cmajor::dom::NodeType::processingInstructionNode;
}

std::unique_ptr<dom::Node> XPathPINodeTest::ToDom() const
{
    std::unique_ptr<dom::Element> element(new dom::Element(U"piNodeTest"));
    return std::unique_ptr<dom::Node>(element.release());
}

bool XPathPrincipalNodeTest::Select(cmajor::dom::Node* node, Axis axis) const
{
    if (axis == Axis::attribute)
    {
        return node->GetNodeType() == cmajor::dom::NodeType::attributeNode;
    }
    else if (axis == Axis::ns)
    {
        return false; // todo
    }
    else
    {
        return node->GetNodeType() == cmajor::dom::NodeType::elementNode;
    }
}

std::unique_ptr<dom::Node> XPathPrincipalNodeTest::ToDom() const
{
    std::unique_ptr<dom::Element> element(new dom::Element(U"principalNodeTest"));
    return std::unique_ptr<dom::Node>(element.release());
}

bool XPathAnyNodeTest::Select(cmajor::dom::Node* node, Axis axis) const
{
    return true;
}

std::unique_ptr<dom::Node> XPathAnyNodeTest::ToDom() const
{
    std::unique_ptr<dom::Element> element(new dom::Element(U"anyNodeTest"));
    return std::unique_ptr<dom::Node>(element.release());
}

XPathPrefixTest::XPathPrefixTest(const std::u32string& name_) : name(name_)
{
}

bool XPathPrefixTest::Select(cmajor::dom::Node* node, Axis axis) const
{
    if (axis == Axis::attribute)
    {
        if (node->GetNodeType() == cmajor::dom::NodeType::attributeNode)
        {
            cmajor::dom::Attr* attr = static_cast<cmajor::dom::Attr*>(node);
            if (attr->Prefix() == name)
            {
                return true;
            }
        }
    }
    else if (axis == Axis::ns)
    {
        // todo
        return false;
    }
    else if (node->GetNodeType() == cmajor::dom::NodeType::elementNode)
    {
        cmajor::dom::Element* element = static_cast<cmajor::dom::Element*>(node); 
        if (element->Prefix() == name)
        {
            return true;
        }
    }
    return false;
}

std::unique_ptr<dom::Node> XPathPrefixTest::ToDom() const
{
    std::unique_ptr<dom::Element> element(new dom::Element(U"prefixTest"));
    element->SetAttribute(U"prefix", name);
    return std::unique_ptr<dom::Node>(element.release());
}

XPathNameTest::XPathNameTest(const std::u32string& name_) : name(name_)
{
}

bool XPathNameTest::Select(cmajor::dom::Node* node, Axis axis) const
{
    if (axis == Axis::attribute)
    {
        if (node->GetNodeType() == cmajor::dom::NodeType::attributeNode)
        {
            cmajor::dom::Attr* attr = static_cast<cmajor::dom::Attr*>(node);
            if (attr->Name() == name)
            {
                return true;
            }
        }
    }
    else if (axis != Axis::ns)
    {
        if (node->GetNodeType() == cmajor::dom::NodeType::elementNode)
        {
            cmajor::dom::Element* element = static_cast<cmajor::dom::Element*>(node);
            if (element->Name() == name)
            {
                return true;
            }
        }
    }
    return false;
}

std::unique_ptr<dom::Node> XPathNameTest::ToDom() const
{
    std::unique_ptr<dom::Element> element(new dom::Element(U"nameTest"));
    element->SetAttribute(U"name", name);
    return std::unique_ptr<dom::Node>(element.release());
}

XPathVariableReference::XPathVariableReference(const std::u32string& name_) : name(name_)
{
}

std::unique_ptr<dom::Node> XPathVariableReference::ToDom() const
{
    std::unique_ptr<dom::Element> element(new dom::Element(U"variableReference"));
    element->SetAttribute(U"name", name);
    return std::unique_ptr<dom::Node>(element.release());
}

XPathLiteral::XPathLiteral(const std::u32string& value_) : value(value_)
{
}

std::unique_ptr<XPathObject> XPathLiteral::Evaluate(XPathContext& context)
{
    return std::unique_ptr<XPathObject>(new XPathString(value));
}

std::unique_ptr<dom::Node> XPathLiteral::ToDom() const
{
    std::unique_ptr<dom::Element> element(new dom::Element(U"literal"));
    element->SetAttribute(U"value", value);
    return std::unique_ptr<dom::Node>(element.release());
}

XPathNumberExpr::XPathNumberExpr(const std::u32string& value_)
{
    std::string s = ToUtf8(value_);
    std::stringstream strm;
    strm.str(s);
    strm >> value;
}

XPathNumberExpr::XPathNumberExpr(double value_) : value(value_)
{
}

std::unique_ptr<XPathObject> XPathNumberExpr::Evaluate(XPathContext& context)
{
    return std::unique_ptr<XPathObject>(new XPathNumber(value));
}

std::unique_ptr<dom::Node> XPathNumberExpr::ToDom() const
{
    std::unique_ptr<dom::Element> element(new dom::Element(U"number"));
    element->SetAttribute(U"value", ToUtf32(std::to_string(value)));
    return std::unique_ptr<dom::Node>(element.release());
}

XPathFunctionCall::XPathFunctionCall(const std::u32string& functionName_) : functionName(functionName_)
{
}

void XPathFunctionCall::AddArgument(XPathExpr* argument)
{
    arguments.push_back(std::unique_ptr<XPathExpr>(argument));
}

std::unique_ptr<XPathObject> XPathFunctionCall::Evaluate(XPathContext& context)
{
    XPathFunction* function = GetXPathLibraryFunction(functionName);
    if (arguments.size() < function->MinArity() || arguments.size() > function->MaxArity())
    {
        throw std::runtime_error("function '" + ToUtf8(functionName) + "' takes " + std::to_string(function->MinArity()) + "..." + std::to_string(function->MaxArity()) + " arguments (" + 
            std::to_string(arguments.size()) + " arguments provided)");
    }
    std::vector<std::unique_ptr<XPathObject>> ownedArgs;
    std::vector<XPathObject*> args;
    for (const std::unique_ptr<XPathExpr>& argument : arguments)
    {
        std::unique_ptr<XPathObject> arg = argument->Evaluate(context);
        args.push_back(arg.get());
        ownedArgs.push_back(std::move(arg));
    }
    std::unique_ptr<XPathObject> result = function->Evaluate(context, args);
    return result;
}

std::unique_ptr<dom::Node> XPathFunctionCall::ToDom() const
{
    std::unique_ptr<dom::Element> element(new dom::Element(U"functionCall"));
    element->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(functionName)));
    std::unique_ptr<dom::Element> argumentsElement(new dom::Element(U"arguments"));
    for (const std::unique_ptr<XPathExpr>& argument : arguments)
    {
        argumentsElement->AppendChild(argument->ToDom());
    }
    element->AppendChild(std::unique_ptr<dom::Node>(argumentsElement.release()));
    return std::unique_ptr<dom::Node>(element.release());
}

} } // namespace cmajor::xpath
