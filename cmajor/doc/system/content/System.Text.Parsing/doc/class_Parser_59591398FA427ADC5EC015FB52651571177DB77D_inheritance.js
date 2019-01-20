// JavaScript source code for drawing class inheritance diagrams and concept refinement diagrams

function pick(level, diagramNodes) {
    var levelNodes = [];
    var n = diagramNodes.length;
    for (var i = 0; i < n; ++i) {
        var c = diagramNodes[i];
        if (c.level == level) {
            levelNodes.push(c);
        }
    }
    return levelNodes;
}

function createDiagramNodeElements(levelNodes, maxTextWidth, maxTextHeight) {
    var textDimensions = { width: maxTextWidth, height: maxTextHeight };
    var n = levelNodes.length;
    for (var i = 0; i < n; ++i) {
        var levelNode = levelNodes[i];
        var svg = document.getElementById("inheritance_svg_diagram");
        var rectElement = document.createElementNS('http://www.w3.org/2000/svg', 'rect');
        rectElement.setAttribute("id", levelNode.id + "_rect");
        var linkElement = document.createElementNS('http://www.w3.org/2000/svg', 'a');
        linkElement.setAttribute("href", levelNode.link);
        var textElement = document.createElementNS('http://www.w3.org/2000/svg', 'text');
        linkElement.appendChild(textElement);
        textElement.setAttribute("id", levelNode.id + "_text");
        textElement.innerHTML = levelNode.name;
        svg.appendChild(rectElement);
        svg.appendChild(linkElement);
        var bb = textElement.getBBox();
        var textWidth = bb.width;
        var textHeight = bb.height;
        levelNode.textWidth = textWidth;
        levelNode.textHeight = textHeight;
        if (textWidth > textDimensions.width) {
            textDimensions.width = textWidth;
        }
        if (textHeight > textDimensions.height) {
            textDimensions.height = textHeight;
        }
    }
    return textDimensions;
}

function drawDiagram(diagramNodes) {
    var cont = true;
    var level = 0;
    var yspace = 8;
    var xspace = 8;
    var minRectWidth = 100;
    var minRectHeight = 40;
    var maxTextWidth = 0;
    var maxTextHeight = 0;
    var triangleHeight = 20;
    var triangleWidth = 20;
    var targetHandleHeight = 20;
    var sourceHandleHeight = 40;
    var rectXSpace = 20;
    var horizontalRectYSpace = triangleHeight + targetHandleHeight + sourceHandleHeight;
    var verticalRectYSpace = 20;
    var derivedTriangleHeight = 8;
    var derivedTriangleWidth = 8;
    var widthThreshold = 1800;
    var allLevelNodes = [];
    while (cont) {
        var levelNodes = pick(level, diagramNodes);
        var n = levelNodes.length;
        cont = n > 0;
        if (cont) {
            var textDimensions = createDiagramNodeElements(levelNodes, maxTextWidth, maxTextHeight);
            if (textDimensions.width > maxTextWidth) {
                maxTextWidth = textDimensions.width;
            }
            if (textDimensions.height > maxTextHeight) {
                maxTextHeight = textDimensions.height;
            }
            allLevelNodes.push(levelNodes);
            ++level;
        }
    }
    var maxLevel = level;
    var rectWidth = Math.max(minRectWidth, maxTextWidth + 2 * xspace);
    var rectHeight = Math.max(minRectHeight, maxTextHeight + 2 * yspace);
    var totalWidth = 0;
    var totalHeight = 0;
    var horizontalDirection = 0;
    var verticalDirection = 1;
    var levelDirection = [];
    var levelHeight = [];
    var prevW = 0;
    for (level = 0; level < maxLevel; ++level) {
        var levelNodes = allLevelNodes[level];
        var n = levelNodes.length;
        var w = n * (rectWidth + rectXSpace);
        var h = rectHeight + horizontalRectYSpace;
        if (w < widthThreshold) {
            levelDirection.push(horizontalDirection);
            if (w > totalWidth) {
                totalWidth = w;
            }
        }
        else {
            w = prevW + rectWidth + rectXSpace;
            h = n * (rectHeight + verticalRectYSpace);
            levelDirection.push(verticalDirection);
            totalWidth += w;
        }
        totalHeight += h;
        levelHeight.push(h);
        prevW = w;
    }
    var svg = document.getElementById("inheritance_svg_diagram");
    svg.setAttribute("width", totalWidth.toString());
    svg.setAttribute("height", totalHeight.toString());
    var prevRectY = 0;
    var prevRectX = 0;
    var prevHandleX2 = -1;
    var prevHandleY2 = -1;
    var prevY = 0;
    for (level = 0; level < maxLevel; ++level) {
        var direction = levelDirection[level];
        var levelNodes = allLevelNodes[level];
        var n = levelNodes.length;
        var rectY = prevY;
        prevY += levelHeight[level];
        var rectX = (totalWidth / n - rectWidth) / 2;
        var minHandleX = Number.MAX_SAFE_INTEGER;
        var maxHandleX = 0;
        var handleY = 0;
        for (var i = 0; i < n; ++i) {
            var levelNode = levelNodes[i];
            var textWidth = levelNode.textWidth;
            var textHeight = levelNode.textHeight;
            if (direction == horizontalDirection) {
                rectX = (totalWidth / n - rectWidth) / 2 + i * (rectWidth + rectXSpace);
            }
            else if (direction == verticalDirection) {
                rectX = prevRectX + (rectWidth + rectXSpace);
                rectY = prevRectY + horizontalRectYSpace + i * (rectHeight + verticalRectYSpace);
            }
            var textX = rectX + (rectWidth - textWidth) / 2;
            var textY = (rectY + rectHeight - yspace / 2) - (rectHeight - textHeight) / 2;
            var rectElement = document.getElementById(levelNode.id + "_rect");
            rectElement.setAttribute("x", rectX.toString());
            rectElement.setAttribute("y", rectY.toString());
            rectElement.setAttribute("width", rectWidth.toString());
            rectElement.setAttribute("height", rectHeight.toString());
            var fillColor = "white";
            if (levelNode.subject) {
                fillColor = "floralWhite";
            }
            rectElement.setAttribute("fill", fillColor);
            rectElement.setAttribute("stroke", "black");
            var textElement = document.getElementById(levelNode.id + "_text");
            textElement.setAttribute("x", textX.toString());
            textElement.setAttribute("y", textY.toString());
            if (level < maxLevel - 1) {
                var triangleElement = document.createElementNS('http://www.w3.org/2000/svg', 'polygon');
                var tipX = rectX + rectWidth / 2;
                var tipY = rectY + rectHeight;
                var leftX = tipX - triangleWidth / 2;
                var leftY = rectY + rectHeight + triangleHeight;
                var rightX = tipX + triangleWidth / 2;
                var rightY = rectY + rectHeight + triangleHeight;
                triangleElement.setAttribute("points",
                    tipX.toString() + "," + tipY.toString() + " " +
                    leftX.toString() + "," + leftY.toString() + " " +
                    rightX.toString() + "," + rightY.toString());
                triangleElement.setAttribute("fill", "white");
                triangleElement.setAttribute("stroke", "black");
                svg.appendChild(triangleElement);
                var targetHandleElement = document.createElementNS('http://www.w3.org/2000/svg', 'line');
                var handleX1 = tipX;
                var handleY1 = tipY + triangleHeight;
                var handleX2 = tipX;
                var handleY2 = tipY + triangleHeight + targetHandleHeight;
                targetHandleElement.setAttribute("x1", handleX1.toString());
                targetHandleElement.setAttribute("y1", handleY1.toString());
                targetHandleElement.setAttribute("x2", handleX2.toString());
                targetHandleElement.setAttribute("y2", handleY2.toString());
                targetHandleElement.setAttribute("stroke", "black");
                svg.appendChild(targetHandleElement);
                prevHandleX2 = handleX1;
                prevHandleY2 = handleY1;
                if (handleX1 < minHandleX) {
                    minHandleX = handleX1;
                    handleY = handleY2;
                }
                if (handleX1 > maxHandleX) {
                    maxHandleX = handleX1;
                    handleY = handleY2;
                }
            } else if (level == maxLevel - 1 && levelNode.hasDerivedNodes) {
                var derivedTriangleElement = document.createElementNS('http://www.w3.org/2000/svg', 'polygon');
                var cornerX = rectX + rectWidth;
                var cornerY = rectY + rectHeight;
                var verticalX = rectX + rectWidth;
                var verticalY = rectY + rectHeight - derivedTriangleHeight;
                var horizontalX = rectX + rectWidth - derivedTriangleWidth;
                var horizontalY = rectY + rectHeight;
                derivedTriangleElement.setAttribute("points",
                    cornerX.toString() + "," + cornerY.toString() + " " +
                    verticalX.toString() + "," + verticalY.toString() + " " +
                    horizontalX.toString() + "," + horizontalY.toString());
                derivedTriangleElement.setAttribute("fill", "black");
                svg.appendChild(derivedTriangleElement);
            }
            if (level > 0 && direction == horizontalDirection) {
                var sourceHandleElement = document.createElementNS('http://www.w3.org/2000/svg', 'line');
                var handleX1 = rectX + rectWidth / 2;
                var handleY1 = rectY;
                var handleX2 = rectX + rectWidth / 2;
                var handleY2 = rectY - sourceHandleHeight;
                sourceHandleElement.setAttribute("x1", handleX1.toString());
                sourceHandleElement.setAttribute("y1", handleY1.toString());
                sourceHandleElement.setAttribute("x2", handleX2.toString());
                sourceHandleElement.setAttribute("y2", handleY2.toString());
                sourceHandleElement.setAttribute("stroke", "black");
                svg.appendChild(sourceHandleElement);
                if (handleX1 < minHandleX) {
                    minHandleX = handleX1;
                    handleY = handleY2;
                }
                if (handleX1 > maxHandleX) {
                    maxHandleX = handleX1;
                    handleY = handleY2;
                }
            }
            else if (level > 0 && direction == verticalDirection) {
                var sourceHandleElement = document.createElementNS('http://www.w3.org/2000/svg', 'line');
                var handleX1 = rectX;
                var handleY1 = rectY + rectHeight / 2;
                var handleX2 = rectX - rectWidth / 2 - rectXSpace;
                var handleY2 = rectY + rectHeight / 2;
                sourceHandleElement.setAttribute("x1", handleX1.toString());
                sourceHandleElement.setAttribute("y1", handleY1.toString());
                sourceHandleElement.setAttribute("x2", handleX2.toString());
                sourceHandleElement.setAttribute("y2", handleY2.toString());
                sourceHandleElement.setAttribute("stroke", "black");
                svg.appendChild(sourceHandleElement);
                if (prevHandleX2 != -1 && prevHandleY2 != -1) {
                    var connectorHandleElement = document.createElementNS('http://www.w3.org/2000/svg', 'line');
                    connectorHandleElement.setAttribute("x1", handleX2.toString());
                    connectorHandleElement.setAttribute("y1", handleY2.toString());
                    connectorHandleElement.setAttribute("x2", prevHandleX2.toString());
                    connectorHandleElement.setAttribute("y2", prevHandleY2.toString());
                    connectorHandleElement.setAttribute("stroke", "black");
                    svg.appendChild(connectorHandleElement);
                }
                prevHandleX2 = handleX2
                prevHandleY2 = handleY2;
            }
        }
        if (minHandleX < maxHandleX && direction == horizontalDirection) {
            var hlineElement = document.createElementNS('http://www.w3.org/2000/svg', 'line');
            hlineElement.setAttribute("x1", minHandleX.toString());
            hlineElement.setAttribute("y1", handleY.toString());
            hlineElement.setAttribute("x2", maxHandleX.toString());
            hlineElement.setAttribute("y2", handleY.toString());
            hlineElement.setAttribute("stroke", "black");
            svg.appendChild(hlineElement);
        }
        prevRectY = rectY;
        prevRectX = rectX;
    }
}

function drawInheritanceDiagram() {
    var diagramNodes = [
        { name: "System.Text.Parsing.ParsingObject", id: "diagram_node_0", level: 0, subject: false, hasDerivedNodes: true, link: "../../System.Text.Parsing/doc/class_ParsingObject_7DEF54CB93890E63DE64B53AA974CF41FBA38B34.html" },
        { name: "System.Text.Parsing.Parser", id: "diagram_node_1", level: 1, subject: true, hasDerivedNodes: true, link: "../../System.Text.Parsing/doc/class_Parser_59591398FA427ADC5EC015FB52651571177DB77D.html" },
        { name: "System.Text.Parsing.AlphabeticParser", id: "diagram_node_2", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_AlphabeticParser_45DD8543651C76160EC9B3B2107BA2BF503D7E2F.html" },
        { name: "System.Text.Parsing.AnyCharParser", id: "diagram_node_7", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_AnyCharParser_D8CBAF47F831F823FE8E427A422286DAB5951B4B.html" },
        { name: "System.Text.Parsing.BaseCharParser", id: "diagram_node_9", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_BaseCharParser_F1DEF3A63C197D74465AC1D960F197C940D83E2E.html" },
        { name: "System.Text.Parsing.BinaryParser", id: "diagram_node_5", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_BinaryParser_19E488A784487BB055BADC530295F1AA7A5F0BCA.html" },
        { name: "System.Text.Parsing.CasedLetterParser", id: "diagram_node_11", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_CasedLetterParser_6C4CB1503DDB17FE7152408AFFEB79501A61B243.html" },
        { name: "System.Text.Parsing.CharParser", id: "diagram_node_13", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_CharParser_F30B4B7DDC9B8D3CD1476C03049B3C7189028FDA.html" },
        { name: "System.Text.Parsing.CharSetParser", id: "diagram_node_16", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_CharSetParser_1549A5D12A015735356B2B65DB64C4AF85157B9A.html" },
        { name: "System.Text.Parsing.ClosePunctuationParser", id: "diagram_node_10", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_ClosePunctuationParser_EEBF21313A80F897CA549B02A6216FE79BF4E902.html" },
        { name: "System.Text.Parsing.ConnectorPunctuationParser", id: "diagram_node_14", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_ConnectorPunctuationParser_2C109CCB1929B5A7DF6B45B53976529B0BBE333B.html" },
        { name: "System.Text.Parsing.ControlParser", id: "diagram_node_17", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_ControlParser_1334D26AE4230C2F284FC6316329790AE23C7A28.html" },
        { name: "System.Text.Parsing.CurrencySymbolParser", id: "diagram_node_18", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_CurrencySymbolParser_20757D3E45D7CCAABB4B408EB8F3EF95925411CC.html" },
        { name: "System.Text.Parsing.DashPunctuationParser", id: "diagram_node_20", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_DashPunctuationParser_5AD7DDB1B6093A2764FFBEFB693A74F5328ADCC0.html" },
        { name: "System.Text.Parsing.DecimalNumberParser", id: "diagram_node_23", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_DecimalNumberParser_730379C9B4D622C667F5095CC66454F3215D4EFF.html" },
        { name: "System.Text.Parsing.DigitParser", id: "diagram_node_24", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_DigitParser_A021D42468DFF59A104E5196A3F76F3F12DB9E70.html" },
        { name: "System.Text.Parsing.EmptyParser", id: "diagram_node_26", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_EmptyParser_E7A601A33228A4EE630BE03260AC60547351C266.html" },
        { name: "System.Text.Parsing.EnclosingMarkParser", id: "diagram_node_28", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_EnclosingMarkParser_F2F5F8431121DAD3720B7EC0D8B35ECDC47FA49F.html" },
        { name: "System.Text.Parsing.FinalPunctuationParser", id: "diagram_node_12", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_FinalPunctuationParser_7C92D80BD253997107DD6B218F341C05EA286E5B.html" },
        { name: "System.Text.Parsing.FormatParser", id: "diagram_node_4", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_FormatParser_3606CA5D49BB1FBCB55AC9A61CA2764AEDFD83DF.html" },
        { name: "System.Text.Parsing.GraphicParser", id: "diagram_node_30", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_GraphicParser_CCC378BAB252129868AEF9610BA84DB9CB865D56.html" },
        { name: "System.Text.Parsing.HexDigitParser", id: "diagram_node_32", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_HexDigitParser_C159CE4D9314E481C4C80F2734D59ED21BCDDFC3.html" },
        { name: "System.Text.Parsing.IdContParser", id: "diagram_node_34", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_IdContParser_2E96D9CCED8FB679DACCC042D65F1FF8319AF14A.html" },
        { name: "System.Text.Parsing.IdStartParser", id: "diagram_node_35", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_IdStartParser_7E19B4157FAD686B9AAA24B3303CBA84D5E3B55A.html" },
        { name: "System.Text.Parsing.InitialPunctuationParser", id: "diagram_node_8", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_InitialPunctuationParser_FF9A6C6B18A9668464DA0A7894EDEE822BA05F4C.html" },
        { name: "System.Text.Parsing.KeywordListParser", id: "diagram_node_37", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_KeywordListParser_02161DAF5C377B76A9433EE79663623F8710F2EB.html" },
        { name: "System.Text.Parsing.KeywordParser", id: "diagram_node_38", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_KeywordParser_A653B0BB080315E503BF719C91EC3C4DAD3F604F.html" },
        { name: "System.Text.Parsing.LetterNumberParser", id: "diagram_node_41", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_LetterNumberParser_A2E1DA9BF27BD67D2A8F0CA4BFD23F5013D49F07.html" },
        { name: "System.Text.Parsing.LetterParser", id: "diagram_node_36", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_LetterParser_AA49F4AA1BFE22DA5E008596CBB5F724C716D263.html" },
        { name: "System.Text.Parsing.LineSeparatorParser", id: "diagram_node_40", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_LineSeparatorParser_17BDA29613745BBAD949CECBD8755C565F8E1385.html" },
        { name: "System.Text.Parsing.LowerLetterParser", id: "diagram_node_22", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_LowerLetterParser_E9090D72F69FFF9253FCC31853EDA189052E5F70.html" },
        { name: "System.Text.Parsing.MarkParser", id: "diagram_node_43", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_MarkParser_B14EEC34C28AB3CECAE517086AD43EDFFAF8E52A.html" },
        { name: "System.Text.Parsing.MathSymbolParser", id: "diagram_node_33", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_MathSymbolParser_0960D3ACEF2D8F2A35CDE8C263D1C178E92D0AB6.html" },
        { name: "System.Text.Parsing.ModifierLetterParser", id: "diagram_node_27", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_ModifierLetterParser_F521D6EB139B02380EEF8F896E33A367C02CBABD.html" },
        { name: "System.Text.Parsing.ModifierSymbolParser", id: "diagram_node_3", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_ModifierSymbolParser_33606880C86B88E8F1CDAB2FACCC0A695B112F3A.html" },
        { name: "System.Text.Parsing.NonspacingMarkParser", id: "diagram_node_45", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_NonspacingMarkParser_77C0D960D80024D464FB412D04C55ACE03242DEE.html" },
        { name: "System.Text.Parsing.NonterminalParser", id: "diagram_node_46", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_NonterminalParser_9EA7AE3E5E1CB990FBCEF93ACFF8557CE334E348.html" },
        { name: "System.Text.Parsing.NumberParser", id: "diagram_node_15", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_NumberParser_C6AC535F4CF08D081B4EF82D22F3328637E3FC7A.html" },
        { name: "System.Text.Parsing.OpenPunctuationParser", id: "diagram_node_44", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_OpenPunctuationParser_D86559E7718C6E51274F05C4E573EDA35EF670B3.html" },
        { name: "System.Text.Parsing.OtherLetterParser", id: "diagram_node_47", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_OtherLetterParser_819A69E6334348DB437B88300F19EFE0E795AFA9.html" },
        { name: "System.Text.Parsing.OtherNumberParser", id: "diagram_node_48", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_OtherNumberParser_5E44F04780E5AF76882D4398BCF95F19900B6F58.html" },
        { name: "System.Text.Parsing.OtherParser", id: "diagram_node_49", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_OtherParser_24F37841D60C93B62B7FDD5DFEFF977254540B60.html" },
        { name: "System.Text.Parsing.OtherPunctuationParser", id: "diagram_node_19", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_OtherPunctuationParser_642E6FBEFE02269CFF5F054BE3C0A5F152B99813.html" },
        { name: "System.Text.Parsing.OtherSymbolParser", id: "diagram_node_50", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_OtherSymbolParser_A5097353A1C3851E3352A7223FF3BF4131CC203D.html" },
        { name: "System.Text.Parsing.ParagraphSeparatorParser", id: "diagram_node_51", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_ParagraphSeparatorParser_9F58A4F5735238FB34B93B7A180B0EC20131F987.html" },
        { name: "System.Text.Parsing.PrivateUseParser", id: "diagram_node_21", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_PrivateUseParser_C4511180B4C5389C10E1979630653AC168E71F95.html" },
        { name: "System.Text.Parsing.PunctuationParser", id: "diagram_node_52", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_PunctuationParser_CFAC6556C1168A91712DB882C5D1B9DB5F5237BE.html" },
        { name: "System.Text.Parsing.RangeParser", id: "diagram_node_54", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_RangeParser_A1D22AC11434C18DF491491487AA4116C3494013.html" },
        { name: "System.Text.Parsing.RuleParser", id: "diagram_node_39", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_RuleParser_88E602BC11B27BF33746B36B66E0A7961C3B3969.html" },
        { name: "System.Text.Parsing.SeparatorParser", id: "diagram_node_55", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_SeparatorParser_94FDB93F8087C6949DA2645502E77D1B5F3ADE81.html" },
        { name: "System.Text.Parsing.SpaceParser", id: "diagram_node_56", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_SpaceParser_6418127CA0B0CB09FC5DF96A2B28D5B6741FDF5A.html" },
        { name: "System.Text.Parsing.SpaceSeparatorParser", id: "diagram_node_29", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_SpaceSeparatorParser_2A483B13630174EAB1AEE4FA77E62872AF94C471.html" },
        { name: "System.Text.Parsing.SpacingMarkParser", id: "diagram_node_25", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_SpacingMarkParser_3AD4A1C6C577E4206A0EBC5373871C84DFE9E122.html" },
        { name: "System.Text.Parsing.StringParser", id: "diagram_node_53", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_StringParser_765B96F3702C707FA03EC3AFF074BEF47356D039.html" },
        { name: "System.Text.Parsing.SurrogateParser", id: "diagram_node_6", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_SurrogateParser_621E59A0FC3C49F9DADBE6C4C595442238B39AE7.html" },
        { name: "System.Text.Parsing.SymbolParser", id: "diagram_node_57", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_SymbolParser_A5146C5ECEFE464AAA32ED863013DC2B99B7E00B.html" },
        { name: "System.Text.Parsing.TitleLetterParser", id: "diagram_node_58", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_TitleLetterParser_CB5E9F95B1BFAA4498E885363D18ADB1DF1AF544.html" },
        { name: "System.Text.Parsing.UnaryParser", id: "diagram_node_59", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_UnaryParser_74354A3C3770DC721B12D0E37FAE9B6A88FFA71A.html" },
        { name: "System.Text.Parsing.UnassignedParser", id: "diagram_node_31", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_UnassignedParser_B837C0573CAFFB26E8BFF6AD38B8AED47DB1F3B5.html" },
        { name: "System.Text.Parsing.UpperLetterParser", id: "diagram_node_42", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_UpperLetterParser_60FA6653414FF82D2E34FEADF0F18A7C3A0A601F.html" }];
    drawDiagram(diagramNodes);
}

