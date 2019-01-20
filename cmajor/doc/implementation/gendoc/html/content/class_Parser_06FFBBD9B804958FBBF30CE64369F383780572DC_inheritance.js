// JavaScript source code for drawing class inheritance diagrams

function pick(level, classes) {
    var levelClasses = [];
    var n = classes.length;
    for (var i = 0; i < n; ++i) {
        var c = classes[i];
        if (c.level == level) {
            levelClasses.push(c);
        }
    }
    return levelClasses;
}

function createClassElements(levelClasses, maxTextWidth, maxTextHeight) {
    var textDimensions = { width: maxTextWidth, height: maxTextHeight };
    var n = levelClasses.length;
    for (var i = 0; i < n; ++i) {
        var levelClass = levelClasses[i];
        var svg = document.getElementById("classInheritanceDiagram");
        var rectElement = document.createElementNS('http://www.w3.org/2000/svg', 'rect');
        rectElement.setAttribute("id", levelClass.id + "_rect");
        var linkElement = document.createElementNS('http://www.w3.org/2000/svg', 'a');
        linkElement.setAttribute("href", levelClass.link);
        var textElement = document.createElementNS('http://www.w3.org/2000/svg', 'text');
        linkElement.appendChild(textElement);
        textElement.setAttribute("id", levelClass.id + "_text");
        textElement.innerHTML = levelClass.name;
        svg.appendChild(rectElement);
        svg.appendChild(linkElement);
        var bb = textElement.getBBox();
        var textWidth = bb.width;
        var textHeight = bb.height;
        levelClass.textWidth = textWidth;
        levelClass.textHeight = textHeight;
        if (textWidth > textDimensions.width) {
            textDimensions.width = textWidth;
        }
        if (textHeight > textDimensions.height) {
            textDimensions.height = textHeight;
        }
    }
    return textDimensions;
}

function drawDiagram(classes) {
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
    var allLevelClasses = [];
    while (cont) {
        var levelClasses = pick(level, classes);
        var n = levelClasses.length;
        cont = n > 0;
        if (cont) {
            var textDimensions = createClassElements(levelClasses, maxTextWidth, maxTextHeight);
            if (textDimensions.width > maxTextWidth) {
                maxTextWidth = textDimensions.width;
            }
            if (textDimensions.height > maxTextHeight) {
                maxTextHeight = textDimensions.height;
            }
            allLevelClasses.push(levelClasses);
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
        var levelClasses = allLevelClasses[level];
        var n = levelClasses.length;
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
    var svg = document.getElementById("classInheritanceDiagram");
    svg.setAttribute("width", totalWidth.toString());
    svg.setAttribute("height", totalHeight.toString());
    var prevRectY = 0;
    var prevRectX = 0;
    var prevHandleX2 = -1;
    var prevHandleY2 = -1;
    var prevY = 0;
    for (level = 0; level < maxLevel; ++level) {
        var direction = levelDirection[level];
        var levelClasses = allLevelClasses[level];
        var n = levelClasses.length;
        var rectY = prevY;
        prevY += levelHeight[level];
        var rectX = (totalWidth / n - rectWidth) / 2;
        var minHandleX = Number.MAX_SAFE_INTEGER;
        var maxHandleX = 0;
        var handleY = 0;
        for (var i = 0; i < n; ++i) {
            var levelClass = levelClasses[i];
            var textWidth = levelClass.textWidth;
            var textHeight = levelClass.textHeight;
            if (direction == horizontalDirection) {
                rectX = (totalWidth / n - rectWidth) / 2 + i * (rectWidth + rectXSpace);
            }
            else if (direction == verticalDirection) {
                rectX = prevRectX + (rectWidth + rectXSpace);
                rectY = prevRectY + horizontalRectYSpace + i * (rectHeight + verticalRectYSpace);
            }
            var textX = rectX + (rectWidth - textWidth) / 2;
            var textY = (rectY + rectHeight - yspace / 2) - (rectHeight - textHeight) / 2;
            var rectElement = document.getElementById(levelClass.id + "_rect");
            rectElement.setAttribute("x", rectX.toString());
            rectElement.setAttribute("y", rectY.toString());
            rectElement.setAttribute("width", rectWidth.toString());
            rectElement.setAttribute("height", rectHeight.toString());
            var fillColor = "white";
            if (levelClass.subject) {
                fillColor = "floralWhite";
            }
            rectElement.setAttribute("fill", fillColor);
            rectElement.setAttribute("stroke", "black");
            var textElement = document.getElementById(levelClass.id + "_text");
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
            } else if (level == maxLevel - 1 && levelClass.hasDerivedClasses) {
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

function drawClassInheritanceDiagram() {
    var classes = [
        { name: "cmajor::parsing::ParsingObject", id: "class_0", level: 0, subject: false, hasDerivedClasses: true, link: "class_ParsingObject_D6A163B08653CF53190C94952F145DFC2EB09994.html" },
        { name: "cmajor::parsing::Parser", id: "class_1", level: 1, subject: true, hasDerivedClasses: true, link: "class_Parser_06FFBBD9B804958FBBF30CE64369F383780572DC.html" },
        { name: "cmajor::parsing::AlphabeticParser", id: "class_2", level: 2, subject: false, hasDerivedClasses: false, link: "class_AlphabeticParser_DBD67FC75A9229827DF986E40A19F9FF82453303.html" },
        { name: "cmajor::parsing::AnyCharParser", id: "class_3", level: 2, subject: false, hasDerivedClasses: false, link: "class_AnyCharParser_966AC367D15DA51629FDC642B1E329BA3FD53C8D.html" },
        { name: "cmajor::parsing::BaseCharParser", id: "class_4", level: 2, subject: false, hasDerivedClasses: false, link: "class_BaseCharParser_43995E8BEB3EEB0EBF60EF639DB2E655C29AED48.html" },
        { name: "cmajor::parsing::BinaryParser", id: "class_5", level: 2, subject: false, hasDerivedClasses: true, link: "class_BinaryParser_01B3F4AD87A11BF51E8461473B91C32C547664B6.html" },
        { name: "cmajor::parsing::CasedLetterParser", id: "class_6", level: 2, subject: false, hasDerivedClasses: false, link: "class_CasedLetterParser_1F97DDF3E2F9A98E33BC9D31A55EB1C622258BD4.html" },
        { name: "cmajor::parsing::CharParser", id: "class_7", level: 2, subject: false, hasDerivedClasses: false, link: "class_CharParser_4BE81992B401971DBE477A1CF33C6D78756AD750.html" },
        { name: "cmajor::parsing::CharSetParser", id: "class_8", level: 2, subject: false, hasDerivedClasses: false, link: "class_CharSetParser_9F0D26BD8BF9D8E76C98F61C5F36070E23455341.html" },
        { name: "cmajor::parsing::ClosePunctuationParser", id: "class_9", level: 2, subject: false, hasDerivedClasses: false, link: "class_ClosePunctuationParser_89D89F8972BB7BF6FE769105870C29E72B067547.html" },
        { name: "cmajor::parsing::ConnectorPunctuationParser", id: "class_10", level: 2, subject: false, hasDerivedClasses: false, link: "class_ConnectorPunctuationParser_68C5E85307992489A22E477071248D91A6EA3928.html" },
        { name: "cmajor::parsing::ControlParser", id: "class_11", level: 2, subject: false, hasDerivedClasses: false, link: "class_ControlParser_62E6C9DA41378B57BFBE7826371B59C26677CDA8.html" },
        { name: "cmajor::parsing::CurrencySymbolParser", id: "class_12", level: 2, subject: false, hasDerivedClasses: false, link: "class_CurrencySymbolParser_2008E4F4392C2A0DEF0DEA115616C8CF26307E96.html" },
        { name: "cmajor::parsing::DashPunctuationParser", id: "class_13", level: 2, subject: false, hasDerivedClasses: false, link: "class_DashPunctuationParser_79EA4E37B878681F7F553BB300997E32BD9842F1.html" },
        { name: "cmajor::parsing::DecimalNumberParser", id: "class_14", level: 2, subject: false, hasDerivedClasses: false, link: "class_DecimalNumberParser_0CA9397D4B9333665A46B5E869655DE6CDFAFEE5.html" },
        { name: "cmajor::parsing::DigitParser", id: "class_15", level: 2, subject: false, hasDerivedClasses: false, link: "class_DigitParser_622518B4627B415AC6C64215870B1AE6A58D3F84.html" },
        { name: "cmajor::parsing::EmptyParser", id: "class_16", level: 2, subject: false, hasDerivedClasses: false, link: "class_EmptyParser_53D2FDFB6022D514A1ED2FE488B01C152604B4AF.html" },
        { name: "cmajor::parsing::EnclosingMarkParser", id: "class_17", level: 2, subject: false, hasDerivedClasses: false, link: "class_EnclosingMarkParser_65E76E2ED6D53284CF44FC497269483E191F4971.html" },
        { name: "cmajor::parsing::FinalPunctuationParser", id: "class_18", level: 2, subject: false, hasDerivedClasses: false, link: "class_FinalPunctuationParser_8C5A00039206229A7C766BA6DCFE9AC95FAC68A8.html" },
        { name: "cmajor::parsing::FormatParser", id: "class_19", level: 2, subject: false, hasDerivedClasses: false, link: "class_FormatParser_4AFAEDD502DD9D8DC21158B551E989F54BE2C7E8.html" },
        { name: "cmajor::parsing::GraphicParser", id: "class_20", level: 2, subject: false, hasDerivedClasses: false, link: "class_GraphicParser_4E65ABD96CFBE2ECAFBDD0CCE3DD30A9EB8A0B6E.html" },
        { name: "cmajor::parsing::HexDigitParser", id: "class_21", level: 2, subject: false, hasDerivedClasses: false, link: "class_HexDigitParser_565141D9BDA2CC46E7B1F56F7C90F97D3FD2FDDB.html" },
        { name: "cmajor::parsing::IdContParser", id: "class_22", level: 2, subject: false, hasDerivedClasses: false, link: "class_IdContParser_6470AC60E9210A8858739E7895BF7C5167434F05.html" },
        { name: "cmajor::parsing::IdStartParser", id: "class_23", level: 2, subject: false, hasDerivedClasses: false, link: "class_IdStartParser_C1757FCB4DCFB0C20ECD233ABFFA39DBBA7D2CFE.html" },
        { name: "cmajor::parsing::InitialPunctuationParser", id: "class_24", level: 2, subject: false, hasDerivedClasses: false, link: "class_InitialPunctuationParser_40284E202DB252469254FA2C6D5C405E74C33CD7.html" },
        { name: "cmajor::parsing::KeywordListParser", id: "class_25", level: 2, subject: false, hasDerivedClasses: false, link: "class_KeywordListParser_C3233C82E9F0E98A58E28C3F3A5B2C690A66B19B.html" },
        { name: "cmajor::parsing::KeywordParser", id: "class_26", level: 2, subject: false, hasDerivedClasses: false, link: "class_KeywordParser_D0D445DF85E20066B09A63FC7A001AC6CABD2503.html" },
        { name: "cmajor::parsing::LetterNumberParser", id: "class_27", level: 2, subject: false, hasDerivedClasses: false, link: "class_LetterNumberParser_AF635C50BFF1C75AD06476B161687930A9E0AFF3.html" },
        { name: "cmajor::parsing::LetterParser", id: "class_28", level: 2, subject: false, hasDerivedClasses: false, link: "class_LetterParser_DAEBA9D0F9AC1374ACC0917544CCDDB988990337.html" },
        { name: "cmajor::parsing::LineSeparatorParser", id: "class_29", level: 2, subject: false, hasDerivedClasses: false, link: "class_LineSeparatorParser_2F3B3A35DDE3435E31CC1BA0FA6C9C6D17190F17.html" },
        { name: "cmajor::parsing::LowerLetterParser", id: "class_30", level: 2, subject: false, hasDerivedClasses: false, link: "class_LowerLetterParser_E1E0AD007C9D113F630D5790DB1135BD580FA9BC.html" },
        { name: "cmajor::parsing::MarkParser", id: "class_31", level: 2, subject: false, hasDerivedClasses: false, link: "class_MarkParser_2966397F120DD85857EAB715BBE2FF1E5E429A54.html" },
        { name: "cmajor::parsing::MathSymbolParser", id: "class_32", level: 2, subject: false, hasDerivedClasses: false, link: "class_MathSymbolParser_36F7039FF60581B9720E41D86764ABD2A5445FBF.html" },
        { name: "cmajor::parsing::ModifierLetterParser", id: "class_33", level: 2, subject: false, hasDerivedClasses: false, link: "class_ModifierLetterParser_AE31FA2B10CB9C6ECD7E774CE1138E03E4B49633.html" },
        { name: "cmajor::parsing::ModifierSymbolParser", id: "class_34", level: 2, subject: false, hasDerivedClasses: false, link: "class_ModifierSymbolParser_7771193D5A70752EF3F70569F5972A26CF45B30E.html" },
        { name: "cmajor::parsing::NonspacingMarkParser", id: "class_35", level: 2, subject: false, hasDerivedClasses: false, link: "class_NonspacingMarkParser_6724F88A511D3C2E03428DE17281731B51821ECF.html" },
        { name: "cmajor::parsing::NonterminalParser", id: "class_36", level: 2, subject: false, hasDerivedClasses: false, link: "class_NonterminalParser_56DD7EE360F8E2637FBB60CB429476F12426A97D.html" },
        { name: "cmajor::parsing::NumberParser", id: "class_37", level: 2, subject: false, hasDerivedClasses: false, link: "class_NumberParser_8FB3147501E18E6F061250A0AD28DB965A56AA6C.html" },
        { name: "cmajor::parsing::OpenPunctuationParser", id: "class_38", level: 2, subject: false, hasDerivedClasses: false, link: "class_OpenPunctuationParser_E3EB43090C2618FB0E7A6A07D47A92934D76FE71.html" },
        { name: "cmajor::parsing::OtherLetterParser", id: "class_39", level: 2, subject: false, hasDerivedClasses: false, link: "class_OtherLetterParser_7950919A66D5E00C876F858B4F3DFA0F3394E5D4.html" },
        { name: "cmajor::parsing::OtherNumberParser", id: "class_40", level: 2, subject: false, hasDerivedClasses: false, link: "class_OtherNumberParser_7B8F1B90C801B86B7418BDBC84901F557716C053.html" },
        { name: "cmajor::parsing::OtherParser", id: "class_41", level: 2, subject: false, hasDerivedClasses: false, link: "class_OtherParser_2D40ABCA39753B14DFA5F71C7EA1F4AEB49761E4.html" },
        { name: "cmajor::parsing::OtherPunctuationParser", id: "class_42", level: 2, subject: false, hasDerivedClasses: false, link: "class_OtherPunctuationParser_B0F5E8D70B3232A07B50CF7015D9741E9A24CD23.html" },
        { name: "cmajor::parsing::OtherSymbolParser", id: "class_43", level: 2, subject: false, hasDerivedClasses: false, link: "class_OtherSymbolParser_CBBECC04A09D5FFC768FFD3DDCE00F352A2079C5.html" },
        { name: "cmajor::parsing::ParagraphSeparatorParser", id: "class_44", level: 2, subject: false, hasDerivedClasses: false, link: "class_ParagraphSeparatorParser_E8EE8D34FE541B8CD1238B67C7381C7B9E911911.html" },
        { name: "cmajor::parsing::PrivateUseParser", id: "class_45", level: 2, subject: false, hasDerivedClasses: false, link: "class_PrivateUseParser_82D4EABFC92C3C93E645EF8A1FAA806958018B48.html" },
        { name: "cmajor::parsing::PunctuationParser", id: "class_46", level: 2, subject: false, hasDerivedClasses: false, link: "class_PunctuationParser_F355812060F743CF3520EE91A32399C39059D124.html" },
        { name: "cmajor::parsing::RangeParser", id: "class_47", level: 2, subject: false, hasDerivedClasses: false, link: "class_RangeParser_3E797A64F0DA1EDD340267FE2FB788216DE86929.html" },
        { name: "cmajor::parsing::Rule", id: "class_48", level: 2, subject: false, hasDerivedClasses: false, link: "class_Rule_16F872E2C86FFAE73C79F2ECDE26C7EB2A095DF3.html" },
        { name: "cmajor::parsing::SeparatorParser", id: "class_49", level: 2, subject: false, hasDerivedClasses: false, link: "class_SeparatorParser_BBD5C188DDA429B54C1533BBE0055B3EB6B64F24.html" },
        { name: "cmajor::parsing::SpaceParser", id: "class_50", level: 2, subject: false, hasDerivedClasses: false, link: "class_SpaceParser_2D56FF50EDBA175EE4D1B62066D931E3AF3607E5.html" },
        { name: "cmajor::parsing::SpaceSeparatorParser", id: "class_51", level: 2, subject: false, hasDerivedClasses: false, link: "class_SpaceSeparatorParser_3616F1C67A32BA805BCA3899974540AB456C0B69.html" },
        { name: "cmajor::parsing::SpacingMarkParser", id: "class_52", level: 2, subject: false, hasDerivedClasses: false, link: "class_SpacingMarkParser_B70EBAC8270B45938DCBD320634E7D82036278CA.html" },
        { name: "cmajor::parsing::StringParser", id: "class_53", level: 2, subject: false, hasDerivedClasses: false, link: "class_StringParser_251C72604F4B6C0F336B2A336FFEBA97832B39C8.html" },
        { name: "cmajor::parsing::SurrogateParser", id: "class_54", level: 2, subject: false, hasDerivedClasses: false, link: "class_SurrogateParser_EEEE6FD3AAF15811E8689FDFD562906742A25B3A.html" },
        { name: "cmajor::parsing::SymbolParser", id: "class_55", level: 2, subject: false, hasDerivedClasses: false, link: "class_SymbolParser_D435345C40ABD46174EEC5CB661013DBD190FF34.html" },
        { name: "cmajor::parsing::TitleLetterParser", id: "class_56", level: 2, subject: false, hasDerivedClasses: false, link: "class_TitleLetterParser_7915A7AAA37ABC81FF96B2E591A9564D0017FC9F.html" },
        { name: "cmajor::parsing::UnaryParser", id: "class_57", level: 2, subject: false, hasDerivedClasses: true, link: "class_UnaryParser_FF89216268C9CC5DF03A4E25BB32CA6CC0452C0B.html" },
        { name: "cmajor::parsing::UnassignedParser", id: "class_58", level: 2, subject: false, hasDerivedClasses: false, link: "class_UnassignedParser_12DE92AE1F080449970FA7D79588DEEE7CAFC4C4.html" },
        { name: "cmajor::parsing::UpperLetterParser", id: "class_59", level: 2, subject: false, hasDerivedClasses: false, link: "class_UpperLetterParser_F40702C63245957A4A94A26B151A66E2C9438969.html" }];
    drawDiagram(classes);
}

