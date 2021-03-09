// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmview/CmajorSourceCodeView.hpp>
#include <sngcm/cmlexer/CmajorTokens.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>

#undef min
#undef max

namespace cmajor { namespace view {

using namespace soulng::unicode;
using namespace soulng::util;

CmajorSourceCodeViewCreateParams::CmajorSourceCodeViewCreateParams() : sourceCodeViewCreateParams()
{
    sourceCodeViewCreateParams.WindowClassName("cmajor.view.CmajorSourceCodeView");
    debugLocationBackgroundColor = Color(255, 241, 129);
    debugLocationTextColor = Color::Black;
    debugLocationPadding = Padding(0, 2, 4, 0);
}

CmajorSourceCodeViewCreateParams& CmajorSourceCodeViewCreateParams::Defaults()
{
    return *this;
}

CmajorSourceCodeViewCreateParams& CmajorSourceCodeViewCreateParams::WindowClassName(const std::string& windowClassName_)
{
    sourceCodeViewCreateParams.WindowClassName(windowClassName_);
    return *this;
}

CmajorSourceCodeViewCreateParams& CmajorSourceCodeViewCreateParams::WindowClassStyle(uint32_t windowClassStyle_)
{
    sourceCodeViewCreateParams.WindowClassStyle(windowClassStyle_);
    return *this;
}

CmajorSourceCodeViewCreateParams& CmajorSourceCodeViewCreateParams::WindowStyle(int windowStyle_)
{
    sourceCodeViewCreateParams.WindowStyle(windowStyle_);
    return *this;
}

CmajorSourceCodeViewCreateParams& CmajorSourceCodeViewCreateParams::WindowClassBackgroundColor(int windowClassBackgroundColor_)
{
    sourceCodeViewCreateParams.WindowClassBackgroundColor(windowClassBackgroundColor_);
    return *this;
}

CmajorSourceCodeViewCreateParams& CmajorSourceCodeViewCreateParams::BackgroundColor(const Color& backgroundColor_)
{
    sourceCodeViewCreateParams.BackgroundColor(backgroundColor_);
    return *this;
}

CmajorSourceCodeViewCreateParams& CmajorSourceCodeViewCreateParams::Text(const std::string& text_)
{
    sourceCodeViewCreateParams.Text(text_);
    return *this;
}

CmajorSourceCodeViewCreateParams& CmajorSourceCodeViewCreateParams::Location(const Point& location_)
{
    sourceCodeViewCreateParams.Location(location_);
    return *this;
}

CmajorSourceCodeViewCreateParams& CmajorSourceCodeViewCreateParams::SetSize(const Size& size_)
{
    sourceCodeViewCreateParams.SetSize(size_);
    return *this;
}

CmajorSourceCodeViewCreateParams& CmajorSourceCodeViewCreateParams::SetAnchors(Anchors anchors_)
{
    sourceCodeViewCreateParams.SetAnchors(anchors_);
    return *this;
}

CmajorSourceCodeViewCreateParams& CmajorSourceCodeViewCreateParams::SetDock(Dock dock_)
{
    sourceCodeViewCreateParams.SetDock(dock_);
    return *this;
}

CmajorSourceCodeViewCreateParams& CmajorSourceCodeViewCreateParams::TextColor(const Color& textColor_)
{
    sourceCodeViewCreateParams.TextColor(textColor_);
    return *this;
}

CmajorSourceCodeViewCreateParams& CmajorSourceCodeViewCreateParams::SelectionBackgroundColor(const Color& selectionBackgroundColor_)
{
    sourceCodeViewCreateParams.SelectionBackgroundColor(selectionBackgroundColor_);
    return *this;
}

CmajorSourceCodeViewCreateParams& CmajorSourceCodeViewCreateParams::FontFamilyName(const std::string& fontFamilyName_)
{
    sourceCodeViewCreateParams.FontFamilyName(fontFamilyName_);
    return *this;
}

CmajorSourceCodeViewCreateParams& CmajorSourceCodeViewCreateParams::FontSize(float fontSize_)
{
    sourceCodeViewCreateParams.FontSize(fontSize_);
    return *this;
}

CmajorSourceCodeViewCreateParams& CmajorSourceCodeViewCreateParams::SetSourceCodeTokenStyle(SourceCodeTokenKind tokenKind, TokenStyle tokenStyle)
{
    sourceCodeViewCreateParams.SetSourceCodeTokenStyle(tokenKind, tokenStyle);
    return *this;
}

CmajorSourceCodeViewCreateParams& CmajorSourceCodeViewCreateParams::DebugLocationBackgroundColor(const Color& debugLocationBackgroundColor_)
{
    debugLocationBackgroundColor = debugLocationBackgroundColor_;
    return *this;
}

CmajorSourceCodeViewCreateParams& CmajorSourceCodeViewCreateParams::DebugLocationTextColor(const Color& debugLocationTextColor_)
{
    debugLocationTextColor = debugLocationTextColor_;
    return *this;
}

CmajorSourceCodeViewCreateParams& CmajorSourceCodeViewCreateParams::DebugLocationPadding(const Padding& debugLocationPadding_)
{
    debugLocationPadding = debugLocationPadding_;
    return *this;
}

CmajorSourceCodeView::CmajorSourceCodeView(CmajorSourceCodeViewCreateParams& createParams) : 
    SourceCodeView(createParams.sourceCodeViewCreateParams), lexer(std::u32string(), "", 0), 
    debugLocationBackgroundColor(createParams.debugLocationBackgroundColor),
    debugLocationTextColor(createParams.debugLocationTextColor),
    debugLocationPadding(createParams.debugLocationPadding),
    debugStrip(nullptr),
    editor(nullptr)
{
    lexer.SetCommentTokenId(CmajorTokens::COMMENT);
    std::set<int> blockCommentStates;
    blockCommentStates.insert(124);
    blockCommentStates.insert(125);
    blockCommentStates.insert(126);
    blockCommentStates.insert(127);
    lexer.SetBlockCommentStates(blockCommentStates);
    SetFixed();
    SetIndentSize(4);
}

void CmajorSourceCodeView::ToggleBreakpoint()
{
/*  TODO
    if (debugStrip)
    {
        int line = CaretLine();
        Breakpoint* bp = debugStrip->GetBreakpoint(line);
        if (bp != null)
        {
            debugStrip->RemoveBreakpoint(bp);
            debugStrip->Invalidate();
        }
        else
        {
            Breakpoint* bp = new Breakpoint(line);
            debugStrip->AddBreakpoint(bp);
            debugStrip->Invalidate();
        }
    }
*/
}

TokenLine CmajorSourceCodeView::TokenizeLine(const std::u32string& line, int lineNumber, int startState)
{
    TokenLine tokenLine = lexer.TokenizeLine(line, lineNumber, startState);
    int tokenIndex = 0;
    for (Token& token : tokenLine.tokens)
    {
        std::u32string s = token.match.ToString();
        switch (startState)
        {
            case 0:
            {
                if (token.id == CONTINUE_TOKEN)
                {
                    if (s.length() >= 2)
                    {
                        if (s[0] == '/' && s[1] == '/')
                        {
                            token.id = CmajorTokens::COMMENT;
                            tokenLine.endState = 0;
                        }
                        else if (s[0] == '/' && s[1] == '*')
                        {
                            token.id = CmajorTokens::COMMENT;
                        }
                    }
                    else if (!s.empty())
                    {
                        if (IsWhiteSpace(s[0]))
                        {
                            token.id = CmajorTokens::WS;
                            if (tokenIndex == tokenLine.tokens.size() - 1)
                            {
                                tokenLine.endState = 0;
                            }
                        }
                    }
                }
                break;
            }
            default:
            {
                if (lexer.BlockCommentStates().find(startState) != lexer.BlockCommentStates().cend())
                {
                    token.id = CmajorTokens::COMMENT;
                }
                break;
            }
        }
        startState = 0;
        ++tokenIndex;
    }
    return tokenLine;
}

SourceCodeTokenKind CmajorSourceCodeView::GetTokenKind(const Token& token) const
{
    switch (token.id)
    {
        case CmajorTokens::ASSERT: case CmajorTokens::ELIF: case CmajorTokens::ENDIF: case CmajorTokens::BOOL: case CmajorTokens::TRUE: case CmajorTokens::FALSE:
        case CmajorTokens::SBYTE: case CmajorTokens::BYTE: case CmajorTokens::SHORT: case CmajorTokens::USHORT: case CmajorTokens::INT: case CmajorTokens::UINT:
        case CmajorTokens::LONG: case CmajorTokens::ULONG: case CmajorTokens::FLOAT: case CmajorTokens::DOUBLE: case CmajorTokens::CHAR: case CmajorTokens::WCHAR:
        case CmajorTokens::UCHAR: case CmajorTokens::VOID: case CmajorTokens::ENUM: case CmajorTokens::CAST: case CmajorTokens::INTERFACE: case CmajorTokens::NAMESPACE:
        case CmajorTokens::USING: case CmajorTokens::STATIC: case CmajorTokens::EXTERN: case CmajorTokens::AS: case CmajorTokens::IS: case CmajorTokens::EXPLICIT:
        case CmajorTokens::DELEGATE: case CmajorTokens::INLINE: case CmajorTokens::CDECL: case CmajorTokens::NOTHROW: case CmajorTokens::PUBLIC:
        case CmajorTokens::PROTECTED: case CmajorTokens::PRIVATE: case CmajorTokens::INTERNAL: case CmajorTokens::VIRTUAL: case CmajorTokens::ABSTRACT:
        case CmajorTokens::OVERRIDE: case CmajorTokens::SUPPRESS: case CmajorTokens::WINAPI: case CmajorTokens::OPERATOR: case CmajorTokens::CLASS:
        case CmajorTokens::RETURN: case CmajorTokens::IF: case CmajorTokens::ELSE: case CmajorTokens::SWITCH: case CmajorTokens::CASE: case CmajorTokens::DEFAULT:
        case CmajorTokens::WHILE: case CmajorTokens::DO: case CmajorTokens::FOR: case CmajorTokens::BREAK: case CmajorTokens::CONTINUE: case CmajorTokens::GOTO:
        case CmajorTokens::TYPEDEF: case CmajorTokens::TYPENAME: case CmajorTokens::TYPEID: case CmajorTokens::CONST: case CmajorTokens::CONSTEXPR:
        case CmajorTokens::NULLLIT: case CmajorTokens::THIS: case CmajorTokens::BASE: case CmajorTokens::CONSTRUCT: case CmajorTokens::DESTROY:
        case CmajorTokens::NEW: case CmajorTokens::DELETE: case CmajorTokens::SIZEOF: case CmajorTokens::TRY: case CmajorTokens::CATCH: case CmajorTokens::THROW:
        case CmajorTokens::CONCEPT: case CmajorTokens::WHERE: case CmajorTokens::AXIOM: case CmajorTokens::AND: case CmajorTokens::OR:
        {
            return SourceCodeTokenKind::keyword;
        }
        case CmajorTokens::ID:
        {
            return SourceCodeTokenKind::identifier;
        }
        case CmajorTokens::EXCLAMATION: case CmajorTokens::UNIT_TEST: case CmajorTokens::EQUIVALENCE: case CmajorTokens::IMPLICATION:
        case CmajorTokens::DISJUNCTION: case CmajorTokens::AMPAMP: case CmajorTokens::BITOR: case CmajorTokens::BITXOR: case CmajorTokens::AMP:
        case CmajorTokens::EQ: case CmajorTokens::NEQ: case CmajorTokens::LEQ: case CmajorTokens::GEQ: case CmajorTokens::SHIFTLEFT: case CmajorTokens::SHIFTRIGHT:
        case CmajorTokens::PLUS: case CmajorTokens::MINUS: case CmajorTokens::STAR: case CmajorTokens::DIV: case CmajorTokens::REM: case CmajorTokens::PLUSPLUS:
        case CmajorTokens::MINUSMINUS: case CmajorTokens::CPL: case CmajorTokens::DOT: case CmajorTokens::ARROW: case CmajorTokens::LBRACKET:
        case CmajorTokens::RBRACKET: case CmajorTokens::LPAREN: case CmajorTokens::RPAREN: case CmajorTokens::LANGLE: case CmajorTokens::RANGLE:
        case CmajorTokens::COMMA: case CmajorTokens::ASSIGN: case CmajorTokens::COLON:
        case CmajorTokens::SEMICOLON: case CmajorTokens::HASH:
        {
            return SourceCodeTokenKind::plain;
        }
        case CmajorTokens::LBRACE:
        {
            return SourceCodeTokenKind::beginBlock;
        }
        case CmajorTokens::RBRACE:
        {
            return SourceCodeTokenKind::endBlock;
        }
        case CmajorTokens::WS:
        {
            return SourceCodeTokenKind::space;
        }
        case CmajorTokens::FLOATINGLIT: case CmajorTokens::INTLIT:
        {
            return SourceCodeTokenKind::number;
        }
        case CmajorTokens::CHARLIT:
        {
            return SourceCodeTokenKind::character;
        }
        case CmajorTokens::STRINGLIT:
        {
            return SourceCodeTokenKind::string;
        }
        case CmajorTokens::COMMENT:
        {
            return SourceCodeTokenKind::comment;
        }
    }
    return SourceCodeTokenKind::plain;
}

void CmajorSourceCodeView::DrawHilites(Graphics& graphics, int lineIndex, const PointF& origin)
{
    if (!debugStrip) return;
/*  TODO 
    SourceSpan debugLocation = debugStrip->DebugLocation();
    if (!debugLocation.IsEmpty() && debugLocation.line == lineIndex + 1)
    {
        DrawDebugLocationHilite(graphics, debugLocation, origin);
    }
*/
}

void CmajorSourceCodeView::DrawDebugLocationHilite(Graphics& graphics, const SourceSpan& debugLocation, const PointF& origin)
{
    std::string hiliteText = ToUtf8(GetText(debugLocation));
    int n = std::max(0, static_cast<int>(debugLocation.ecol - debugLocation.scol));
    RectF r(PointF(origin.X + CharWidth() * (debugLocation.scol - 1), origin.Y), SizeF(CharWidth() * n, CharHeight()));
    Brush* backgroundBrush = GetOrInsertBrush(debugLocationBackgroundColor);
    RectF f(PointF(r.X - debugLocationPadding.left, r.Y - debugLocationPadding.top), SizeF(r.Width + debugLocationPadding.Horizontal(), r.Height + debugLocationPadding.Vertical()));
    graphics.FillRectangle(backgroundBrush, f);
    if (!hiliteText.empty())
    {
        Brush* textBrush = GetOrInsertBrush(debugLocationTextColor);
        DrawString(graphics, hiliteText, GetFont(), PointF(r.X, r.Y), *textBrush);
    }
}

} } // namespace cmajor::view
