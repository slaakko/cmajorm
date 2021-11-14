// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmcode/Themes.hpp>
#include <cmajor/cmcode/Config.hpp>
#include <cmajor/cmcode/SolutionExplorer.hpp>
#include <cmajor/cmcode/ToolBar.hpp>
#include <wing/Theme.hpp>
#include <wing/Menu.hpp>
#include <wing/CheckBox.hpp>
#include <wing/StatusBar.hpp>
#include <wing/ToolBar.hpp>
#include <wing/SplitContainer.hpp>

namespace cmcode {

using namespace cmajor::wing;

void MakeLightTheme()
{
    std::unique_ptr<Theme> lightTheme(new Theme("light", "light-theme.xml"));

    lightTheme->SetColor("window.background", Color(238, 238, 242));
    lightTheme->SetColor("window.title.bar", Color(238, 238, 242));
    lightTheme->SetColor("window.title.text", Color(0, 0, 0));
    lightTheme->SetColor("window.title.disabled.text", Color(67, 67, 67));
    lightTheme->SetColor("window.sys.button", Color(30, 30, 30));
    lightTheme->SetColor("window.sys.button.selected", Color(0, 0, 0));
    lightTheme->SetColor("window.sys.button.hover.background", Color(252, 252, 253));

    lightTheme->SetColor("scroll.bar.background", Color(241, 241, 241));

    lightTheme->SetColor("cmajor.editor.background", Color(255, 255, 255));
    lightTheme->SetColor("resource.editor.background", Color(255, 255, 255));
    lightTheme->SetColor("resource.editor.text", Color::Black);
    lightTheme->SetColor("text.editor.background", Color(255, 255, 255));
    lightTheme->SetColor("text.editor.text", Color::Black);
    lightTheme->SetColor("debug.strip.background", DefaultDebugStripBackgroundColor());
    lightTheme->SetColor("debug.strip.break.point", DefaultBreakpointColor()); 

    lightTheme->SetColor("solution.explorer.background", DefaultSolutionExplorerBackgroundColor());
    lightTheme->SetColor("solution.explorer.text", Color::Black);
    lightTheme->SetColor("solution.explorer.state.indicator", Color::Red);
    lightTheme->SetColor("solution.explorer.node.selected", Color(201, 222, 245));

    lightTheme->SetColor("error.view.background", Color::White);
    lightTheme->SetColor("error.view.text", Color::Black);
    lightTheme->SetColor("error.view.node.selected", Color(201, 222, 245));
    lightTheme->SetColor("error.view.state.indicator", Color::Red);

    lightTheme->SetColor("search.results.background", Color::White);
    lightTheme->SetColor("search.results.text", Color::Black);
    lightTheme->SetColor("search.results.node.selected", Color(201, 222, 245));
    lightTheme->SetColor("search.results.state.indicator", Color::Red);

    lightTheme->SetColor("locals.background", Color::White);
    lightTheme->SetColor("locals.text", Color::Black);
    lightTheme->SetColor("locals.node.selected", Color(201, 222, 245));
    lightTheme->SetColor("locals.state.indicator", Color::Red);

    lightTheme->SetColor("call.stack.background", Color::White);
    lightTheme->SetColor("call.stack.text", Color::Black);
    lightTheme->SetColor("call.stack.node.selected", Color(201, 222, 245));
    lightTheme->SetColor("call.stack.state.indicator", Color::Red);

    lightTheme->SetColor("port.map.background", Color::White);
    lightTheme->SetColor("port.map.text", Color::Black);
    lightTheme->SetColor("port.map.node.selected", Color(201, 222, 245));
    lightTheme->SetColor("port.map.state.indicator", Color::Red);

    lightTheme->SetColor("tree.view.active.node.text", Color::Black);

    lightTheme->SetColor("tool.bar.background", DefaultControlBackgroundColor());
    lightTheme->SetColor("tool.bar.frame", DefaultToolBarBorderColor());
    lightTheme->SetColor("tool.bar.tool.tip", DefaultToolBarToolTipColor());
    lightTheme->SetColor("tool.bar.tool.button.dark", DefaultToolButtonDarkColor());
    lightTheme->SetColor("tool.bar.tool.button.light", DefaultToolButtonLightColor());
    lightTheme->SetColor("tool.bar.tool.button.medium.light", DefaultToolButtonMediumLightColor());
    lightTheme->SetColor("tool.bar.tool.button.medium.dark", DefaultToolButtonMediumDarkColor());
    lightTheme->SetColor("tool.bar.tool.button.text", DefaultToolButtonTextColor());
    lightTheme->SetColor("tool.bar.tool.button.disabled", DefaultToolButtonDisabledColor());
    lightTheme->SetColor("tool.bar.tool.button.separator1", DefaultToolButtonSeparatorColor1());
    lightTheme->SetColor("tool.bar.tool.button.separator2", DefaultToolButtonSeparatorColor2());

    lightTheme->SetColor("tab.page", DefaultTabNormalBackgroundColor());

    lightTheme->SetColor("code.tab.control.background", DefaultTabControlBackgroundColor());
    lightTheme->SetColor("code.tab.control.frame", DefaultTabControlFrameColor());
    lightTheme->SetColor("code.tab.control.text", DefaultTabControlTextColor());
    lightTheme->SetColor("code.tab.control.tab.normal.background", DefaultTabNormalBackgroundColor());
    lightTheme->SetColor("code.tab.control.tab.selected.background", DefaultTabSelectedBackgroundColor());
    lightTheme->SetColor("code.tab.control.close.box.selected.background", DefaultTabControlCloseBoxSelectedColor());

    lightTheme->SetColor("output.tab.control.background", DefaultTabControlBackgroundColor());
    lightTheme->SetColor("output.tab.control.frame", DefaultTabControlFrameColor());
    lightTheme->SetColor("output.tab.control.text", DefaultTabControlTextColor());
    lightTheme->SetColor("output.tab.control.tab.normal.background", DefaultTabNormalBackgroundColor());
    lightTheme->SetColor("output.tab.control.tab.selected.background", DefaultTabSelectedBackgroundColor());
    lightTheme->SetColor("output.tab.control.close.box.selected.background", DefaultTabControlCloseBoxSelectedColor());

    lightTheme->SetColor("output.log.background", Color::White);
    lightTheme->SetColor("output.log.text", Color::Black);

    lightTheme->SetColor("log.view.background", Color::White);
    lightTheme->SetColor("log.view.text", Color::Black);

    lightTheme->SetColor("splitContainer.background", DefaultControlBackgroundColor());
    lightTheme->SetColor("splitter.background", DefaultControlBackgroundColor());
    lightTheme->SetColor("splitter.edge", DefaultSplitterEdgeColor()); 

    lightTheme->SetColor("selection.background", DefaultSelectionBackgroundColor());

    lightTheme->SetColor("plain.text", Color::Black);
    lightTheme->SetColor("space.text", Color::Black);
    lightTheme->SetColor("keyword.text", Color::Blue);
    lightTheme->SetColor("identifier.text", Color::Black);
    lightTheme->SetColor("string.text", Color(163, 21, 21));
    lightTheme->SetColor("character.text", Color(163, 21, 21));
    lightTheme->SetColor("number.text", Color::Black);
    lightTheme->SetColor("comment.text", Color(0, 128, 0));
    lightTheme->SetColor("lineNumber.text", Color(43, 145, 175));
    lightTheme->SetColor("beginBlock.text", Color::Black);
    lightTheme->SetColor("endBlock.text", Color::Black);

    lightTheme->SetColor("debug.location.background", Color(255, 241, 129));
    lightTheme->SetColor("debug.location.text", Color::Black);
    lightTheme->SetColor("text", Color::Black);

    lightTheme->SetColor("menu.background", DefaultMenuBackgroundColor());
    lightTheme->SetColor("menu.text", DefaultMenuTextColor());
    lightTheme->SetColor("menu.disabled.text", DefaultDisabledMenuTextColor());
    lightTheme->SetColor("menu.mouse.over.background", DefaultMenuMouseOverColor());
    lightTheme->SetColor("menu.open.background", DefaultMenuOpenColor());
    lightTheme->SetColor("menu.shadow", DefaultMenuShadowColor());
    lightTheme->SetColor("menu.child.indicator", Color::Black);
    lightTheme->SetColor("menu.frame", Color::Black);
    lightTheme->SetColor("menu.item.separator", Color::DarkGray);

    lightTheme->SetColor("status.bar.background", DefaultControlBackgroundColor());
    lightTheme->SetColor("status.bar.text", DefaultTextColor()); 
    lightTheme->SetColor("status.bar.top.line", DefaultTopLineColor());
    lightTheme->SetColor("status.bar.sunken.border.outer.top.left", DefaultSunkenBorderOuterTopLeftColor());
    lightTheme->SetColor("status.bar.sunken.border.inner.top.left", DefaultSunkenBorderInnerTopLeftColor());
    lightTheme->SetColor("status.bar.sunken.border.outer.right.bottom", DefaultSunkenBorderOuterRightBottomColor());
    lightTheme->SetColor("status.bar.sunken.border.inner.right.bottom", DefaultSunkenBorderInnerRightBottomColor());
    lightTheme->SetColor("status.bar.raised.border.outer.top.left", DefaultRaisedBorderOuterTopLeftColor());
    lightTheme->SetColor("status.bar.raised.border.outer.right.bottom", DefaultRaisedBorderOuterRightBottomColor());
    lightTheme->SetColor("status.bar.raised.border.inner.top.left", DefaultRaisedBorderInnerTopLeftColor());
    lightTheme->SetColor("status.bar.raised.border.inner.right.bottom", DefaultRaisedBorderInnerRightBottomColor());

    lightTheme->SetBitmapName("prev", "cmcode.prev.light.bitmap");
    lightTheme->SetBitmapName("next", "cmcode.next.light.bitmap");
    lightTheme->SetBitmapName("save", "cmcode.save.light.bitmap");
    lightTheme->SetBitmapName("save.all", "cmcode.save.all.light.bitmap");
    lightTheme->SetBitmapName("build.solution", "cmcode.build.solution.light.bitmap");
    lightTheme->SetBitmapName("build.project", "cmcode.build.project.light.bitmap");
    lightTheme->SetBitmapName("cancel.build", "cmcode.cancel.build.light.bitmap");
    lightTheme->SetBitmapName("debug.start", "cmcode.debug.start.light.bitmap");
    lightTheme->SetBitmapName("debug.stop", "cmcode.debug.stop.light.bitmap");
    lightTheme->SetBitmapName("show.next.statement", "cmcode.debug.show.next.statement.light.bitmap");
    lightTheme->SetBitmapName("step.over", "cmcode.debug.step.over.light.bitmap");
    lightTheme->SetBitmapName("step.into", "cmcode.debug.step.into.light.bitmap");
    lightTheme->SetBitmapName("step.out", "cmcode.debug.step.out.light.bitmap");

    lightTheme->SetBitmapName("file", "file.light.bitmap");
    lightTheme->SetBitmapName("xmlfile", "xml_file.light.bitmap");
    lightTheme->SetBitmapName("project", "cmproject.light.bitmap");
    lightTheme->SetBitmapName("solution", "cmsolution.light.bitmap");

    lightTheme->SetCursorName("horizontal.splitter.cursor", "horizontal.splitter.light.wing.cursor");
    lightTheme->SetCursorName("vertical.splitter.cursor", "vertical.splitter.light.wing.cursor");

    AddTheme(lightTheme.release());
}

void MakeDarkTheme()
{
    std::unique_ptr<Theme> darkTheme(new Theme("dark", "dark-theme.xml"));

    darkTheme->SetColor("window.background", Color(37, 37, 38));
    darkTheme->SetColor("window.title.bar", Color(37, 37, 38));
    darkTheme->SetColor("window.title.text", Color(220, 220, 220));
    darkTheme->SetColor("window.title.disabled.text", Color(67, 67, 67));
    darkTheme->SetColor("window.sys.button", Color(214, 214, 214));
    darkTheme->SetColor("window.sys.button.selected", Color(255, 255, 255));
    darkTheme->SetColor("window.sys.button.hover.background", Color(51, 51,51));

    darkTheme->SetColor("scroll.bar.background", Color(46, 46, 46));

    darkTheme->SetColor("cmajor.editor.background", Color(30, 30, 30));
    darkTheme->SetColor("resource.editor.background", Color(30, 30, 30));
    darkTheme->SetColor("resource.editor.text", Color(220, 220, 220));
    darkTheme->SetColor("text.editor.background", Color(30, 30, 30));
    darkTheme->SetColor("text.editor.text", Color(220, 220, 220));
    darkTheme->SetColor("debug.strip.background", Color(67, 67, 67));
    darkTheme->SetColor("debug.strip.break.point", DefaultBreakpointColor()); 

    darkTheme->SetColor("solution.explorer.background", Color(37, 37, 38));
    darkTheme->SetColor("solution.explorer.text", Color(220, 220, 220));
    darkTheme->SetColor("solution.explorer.state.indicator", Color(122, 193, 255));
    darkTheme->SetColor("solution.explorer.node.selected", Color(38, 79, 120));

    darkTheme->SetColor("error.view.background", Color(37, 37, 38));
    darkTheme->SetColor("error.view.text", Color(220, 220, 220));
    darkTheme->SetColor("error.view.node.selected", Color(38, 79, 120));
    darkTheme->SetColor("error.view.state.indicator", Color(122, 193, 255));

    darkTheme->SetColor("search.results.background", Color(37, 37, 38));
    darkTheme->SetColor("search.results.text", Color(220, 220, 220));
    darkTheme->SetColor("search.results.node.selected", Color(38, 79, 120));
    darkTheme->SetColor("search.results.state.indicator", Color(122, 193, 255));

    darkTheme->SetColor("locals.background", Color(37, 37, 38));
    darkTheme->SetColor("locals.text", Color(220, 220, 220));
    darkTheme->SetColor("locals.node.selected", Color(38, 79, 120));
    darkTheme->SetColor("locals.state.indicator", Color(122, 193, 255));

    darkTheme->SetColor("call.stack.background", Color(37, 37, 38));
    darkTheme->SetColor("call.stack.text", Color(220, 220, 220));
    darkTheme->SetColor("call.stack.node.selected", Color(38, 79, 120));
    darkTheme->SetColor("call.stack.state.indicator", Color(122, 193, 255));

    darkTheme->SetColor("port.map.background", Color(37, 37, 38));
    darkTheme->SetColor("port.map.text", Color(220, 220, 220));
    darkTheme->SetColor("port.map.node.selected", Color(38, 79, 120));
    darkTheme->SetColor("port.map.state.indicator", Color(122, 193, 255));

    darkTheme->SetColor("tree.view.active.node.text", Color::White);

    darkTheme->SetColor("tool.bar.background", Color(45, 45, 48));
    darkTheme->SetColor("tool.bar.frame", Color(63, 63, 70));
    darkTheme->SetColor("tool.bar.tool.tip", DefaultToolBarToolTipColor());
    darkTheme->SetColor("tool.bar.tool.button.dark", Color(25, 25, 25));
    darkTheme->SetColor("tool.bar.tool.button.light", Color(80, 80, 80));
    darkTheme->SetColor("tool.bar.tool.button.medium.light", Color(53, 53, 55));
    darkTheme->SetColor("tool.bar.tool.button.medium.dark", Color(30, 30, 30)); 
    darkTheme->SetColor("tool.bar.tool.button.text", Color(220, 220, 220));
    darkTheme->SetColor("tool.bar.tool.button.disabled", Color(67, 67, 67));
    darkTheme->SetColor("tool.bar.tool.button.separator1", Color(34, 34, 34));
    darkTheme->SetColor("tool.bar.tool.button.separator2", Color(70, 70, 74));

    darkTheme->SetColor("tab.page", Color(45, 45, 45));

    darkTheme->SetColor("code.tab.control.background", Color(37, 37, 38));
    darkTheme->SetColor("code.tab.control.frame", Color(60, 60, 60));
    darkTheme->SetColor("code.tab.control.text", Color(150, 150, 150));
    darkTheme->SetColor("code.tab.control.tab.normal.background", Color(45, 45, 45));
    darkTheme->SetColor("code.tab.control.tab.selected.background", Color(30, 30, 30));
    darkTheme->SetColor("code.tab.control.close.box.selected.background", Color(49, 50, 50));

    darkTheme->SetColor("output.tab.control.background", Color(37, 37, 38));
    darkTheme->SetColor("output.tab.control.frame", Color(60, 60, 60));
    darkTheme->SetColor("output.tab.control.text", Color(150, 150, 150));
    darkTheme->SetColor("output.tab.control.tab.normal.background", Color(45, 45, 45));
    darkTheme->SetColor("output.tab.control.tab.selected.background", Color(30, 30, 30));
    darkTheme->SetColor("output.tab.control.close.box.selected.background", Color(49, 50, 50));

    darkTheme->SetColor("output.log.background", Color(37, 37, 38));
    darkTheme->SetColor("output.log.text", Color(241, 241, 241));

    darkTheme->SetColor("log.view.background", Color(37, 37, 38));
    darkTheme->SetColor("log.view.text", Color(241, 241, 241));

    darkTheme->SetColor("splitContainer.background", Color(37, 37, 38));
    darkTheme->SetColor("splitter.background", Color(37, 37, 38));
    darkTheme->SetColor("splitter.edge", Color(60, 60, 60));

    darkTheme->SetColor("selection.background", Color(38, 79, 120));

    darkTheme->SetColor("plain.text", Color(220, 220, 220));
    darkTheme->SetColor("space.text", Color(220, 220, 220));
    darkTheme->SetColor("keyword.text", Color(86, 156, 214));
    darkTheme->SetColor("identifier.text", Color(220, 220, 220));
    darkTheme->SetColor("string.text", Color(214, 157, 133));
    darkTheme->SetColor("character.text", Color(214, 157, 133));
    darkTheme->SetColor("number.text", Color(181, 206, 168));
    darkTheme->SetColor("comment.text", Color(87, 166, 74));
    darkTheme->SetColor("lineNumber.text", Color(43, 145, 175));
    darkTheme->SetColor("beginBlock.text", Color(220, 220, 220));
    darkTheme->SetColor("endBlock.text", Color(220, 220, 220));

    darkTheme->SetColor("debug.location.background", Color(197, 200, 112)); 
    darkTheme->SetColor("debug.location.text", Color::Black); 
    darkTheme->SetColor("text", Color(241, 241, 241));

    darkTheme->SetColor("menu.background", Color(37, 37, 38));
    darkTheme->SetColor("menu.text", Color(181, 181, 181));
    darkTheme->SetColor("menu.disabled.text", Color(67, 67, 67));
    darkTheme->SetColor("menu.mouse.over.background", Color(80, 80, 80));
    darkTheme->SetColor("menu.open.background", Color(37, 37, 38));
    darkTheme->SetColor("menu.shadow", Color(36, 36, 37));
    darkTheme->SetColor("menu.child.indicator", Color(166, 166, 166));
    darkTheme->SetColor("menu.frame", Color(60, 60, 60));
    darkTheme->SetColor("menu.item.separator", Color(91, 91, 92));

    darkTheme->SetColor("status.bar.background", Color(60, 60, 60));
    darkTheme->SetColor("status.bar.text", Color(181, 181, 181));
    darkTheme->SetColor("status.bar.top.line", Color(63, 63, 70));
    darkTheme->SetColor("status.bar.sunken.border.outer.top.left", Color(25, 25, 25));
    darkTheme->SetColor("status.bar.sunken.border.inner.top.left", Color(30, 30, 30));
    darkTheme->SetColor("status.bar.sunken.border.outer.right.bottom", Color(53, 53, 55));
    darkTheme->SetColor("status.bar.sunken.border.inner.right.bottom", Color(45, 45, 48));
    darkTheme->SetColor("status.bar.raised.border.outer.top.left", Color(45, 45, 48));
    darkTheme->SetColor("status.bar.raised.border.outer.right.bottom", Color(25, 25, 25));
    darkTheme->SetColor("status.bar.raised.border.inner.top.left", Color(53, 53, 55));
    darkTheme->SetColor("status.bar.raised.border.inner.right.bottom", Color(30, 30, 30));

    darkTheme->SetBitmapName("prev", "cmcode.prev.dark.bitmap");
    darkTheme->SetBitmapName("next", "cmcode.next.dark.bitmap");
    darkTheme->SetBitmapName("save", "cmcode.save.dark.bitmap");
    darkTheme->SetBitmapName("save.all", "cmcode.save.all.dark.bitmap");
    darkTheme->SetBitmapName("build.solution", "cmcode.build.solution.dark.bitmap");
    darkTheme->SetBitmapName("build.project", "cmcode.build.project.dark.bitmap");
    darkTheme->SetBitmapName("cancel.build", "cmcode.cancel.build.dark.bitmap");
    darkTheme->SetBitmapName("debug.start", "cmcode.debug.start.dark.bitmap");
    darkTheme->SetBitmapName("debug.stop", "cmcode.debug.stop.dark.bitmap");
    darkTheme->SetBitmapName("show.next.statement", "cmcode.debug.show.next.statement.dark.bitmap");
    darkTheme->SetBitmapName("step.over", "cmcode.debug.step.over.dark.bitmap");
    darkTheme->SetBitmapName("step.into", "cmcode.debug.step.into.dark.bitmap");
    darkTheme->SetBitmapName("step.out", "cmcode.debug.step.out.dark.bitmap");

    darkTheme->SetBitmapName("file", "file.dark.bitmap");
    darkTheme->SetBitmapName("xmlfile", "xml_file.dark.bitmap");
    darkTheme->SetBitmapName("project", "cmproject.dark.bitmap");
    darkTheme->SetBitmapName("solution", "cmsolution.dark.bitmap");

    darkTheme->SetCursorName("horizontal.splitter.cursor", "horizontal.splitter.dark.wing.cursor");
    darkTheme->SetCursorName("vertical.splitter.cursor", "vertical.splitter.dark.wing.cursor");

    AddTheme(darkTheme.release());
}

void MakeThemes()
{
    MakeLightTheme();
    MakeDarkTheme();
}

void InitThemes()
{
    if (!ThemesFileExists())
    {
        MakeThemes();
        SaveThemes();
    }
    LoadThemes();
    const Options& options = GetOptions();
    SetTheme(options.theme);
}

} // namespace cmcode
