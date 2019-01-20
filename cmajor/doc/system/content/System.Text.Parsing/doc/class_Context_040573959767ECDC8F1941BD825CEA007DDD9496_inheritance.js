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
        { name: "System.Text.Parsing.Context", id: "diagram_node_0", level: 0, subject: true, hasDerivedNodes: true, link: "../../System.Text.Parsing/doc/class_Context_040573959767ECDC8F1941BD825CEA007DDD9496.html" },
        { name: "System.Json.JsonGrammar.ArrayRule.Context", id: "diagram_node_46", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Json/doc/class_Context_AFEF333695DD586BEFCFA47EA307519802EF9BD3.html" },
        { name: "System.Json.JsonGrammar.EscapeRule.Context", id: "diagram_node_25", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Json/doc/class_Context_BCC9FCB3E87A02D9FE9414B76F130D8B1771991A.html" },
        { name: "System.Json.JsonGrammar.FieldRule.Context", id: "diagram_node_49", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Json/doc/class_Context_C2EF2873965CE9318EBD0F1FE7D4908017FAAE75.html" },
        { name: "System.Json.JsonGrammar.NumberRule.Context", id: "diagram_node_1", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Json/doc/class_Context_DC3014771158BE06916FB7B289746C6E33F2CB6B.html" },
        { name: "System.Json.JsonGrammar.ObjectRule.Context", id: "diagram_node_52", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Json/doc/class_Context_CCB8DD1BC453001E743B1DAE18359E22470CB70C.html" },
        { name: "System.Json.JsonGrammar.StringRule.Context", id: "diagram_node_11", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Json/doc/class_Context_A9DFFCA47D4C031CE59C631968775B728DC27723.html" },
        { name: "System.Json.JsonGrammar.ValueRule.Context", id: "diagram_node_54", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Json/doc/class_Context_29B31567A5E1B939D1D54BB81169ED882D01557F.html" },
        { name: "System.Net.Http.HttpChunkHeaderGrammar.headerRule.Context", id: "diagram_node_78", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_1EC93FB267495306AAC071C3920CFD3BE491A11F.html" },
        { name: "System.Net.Http.HttpDateGrammar.dateRule.Context", id: "diagram_node_107", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_2A943865A53633D72FD99EF98630C88FD15AFFE0.html" },
        { name: "System.Net.Http.HttpFieldValueGrammar.csvFieldValueRule.Context", id: "diagram_node_109", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_CC30D02335DD7D46A25620EC5D754E01324FADF0.html" },
        { name: "System.Net.Http.HttpGrammar.CHARRule.Context", id: "diagram_node_110", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_311993A1813F0A60A30A50BEA848DC45B3432981.html" },
        { name: "System.Net.Http.HttpGrammar.Day1Rule.Context", id: "diagram_node_111", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_973067A812B0A64255BCCDE11A6B67B0569CC384.html" },
        { name: "System.Net.Http.HttpGrammar.DayRule.Context", id: "diagram_node_112", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_E271B70067BCD740A1D2FE14CAD6A1726914F493.html" },
        { name: "System.Net.Http.HttpGrammar.HourRule.Context", id: "diagram_node_113", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_2BCBD0B88D53AC622D202E3F53EF268228A5BD40.html" },
        { name: "System.Net.Http.HttpGrammar.MinRule.Context", id: "diagram_node_114", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_68DAAED4E624853C47DB4D5860208F8F32134B90.html" },
        { name: "System.Net.Http.HttpGrammar.SecRule.Context", id: "diagram_node_115", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_F8CF053CA51335DEF280BD3682559C4308F3E6A6.html" },
        { name: "System.Net.Http.HttpGrammar.Year2Rule.Context", id: "diagram_node_116", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_148A26C19205048A094344A2D5D9F5CEC4B9F38B.html" },
        { name: "System.Net.Http.HttpGrammar.YearRule.Context", id: "diagram_node_88", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_328372F777C8159C823896F29E075F462972744C.html" },
        { name: "System.Net.Http.HttpGrammar.asctimeDateRule.Context", id: "diagram_node_117", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_8DCDDC30676D64A4B40865D14AEA7FC5E2E66820.html" },
        { name: "System.Net.Http.HttpGrammar.attributeRule.Context", id: "diagram_node_70", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_321EDE2E9380973620583CFE30FFE6F1E2ECE377.html" },
        { name: "System.Net.Http.HttpGrammar.attrvalRule.Context", id: "diagram_node_118", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_E327558C44F5A34BC308B7E3F82322A0637C5946.html" },
        { name: "System.Net.Http.HttpGrammar.chunkExtNameRule.Context", id: "diagram_node_119", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_85496104B73E47E9D16DAC00F9D155EEB1416283.html" },
        { name: "System.Net.Http.HttpGrammar.chunkExtValRule.Context", id: "diagram_node_69", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_1D7D22EBC8DF8C2CE3C515F0823A28519314F595.html" },
        { name: "System.Net.Http.HttpGrammar.chunkExtensionsRule.Context", id: "diagram_node_121", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_B1EE6649B800EECF8C972AF75E9AF5DB7F3644F0.html" },
        { name: "System.Net.Http.HttpGrammar.chunkHeaderRule.Context", id: "diagram_node_122", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_C3E4165139DB2B73C939FB2D920290D6A2B987B9.html" },
        { name: "System.Net.Http.HttpGrammar.commaSeparatedFieldValueRule.Context", id: "diagram_node_123", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_2C2D6AA80FB0C2A381DAE35D9CF18E8BFD4BF066.html" },
        { name: "System.Net.Http.HttpGrammar.ctextRule.Context", id: "diagram_node_124", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_8258FA3E44A6CD57120C7A0E9AA61556BDF60D98.html" },
        { name: "System.Net.Http.HttpGrammar.date1Rule.Context", id: "diagram_node_125", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_ED9C3B99F10C0B9AD3346487A306D8348B9E4CB2.html" },
        { name: "System.Net.Http.HttpGrammar.date2Rule.Context", id: "diagram_node_127", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_9DA7A828C0968E877402C53A7B3389972A11A163.html" },
        { name: "System.Net.Http.HttpGrammar.date3Rule.Context", id: "diagram_node_128", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_E01D542ADA499D8F77A69478104F59E1A4374454.html" },
        { name: "System.Net.Http.HttpGrammar.dateValueRule.Context", id: "diagram_node_129", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_7D75567F54EAFBA9B495407150BB4D3A03AC3DDD.html" },
        { name: "System.Net.Http.HttpGrammar.deltaSecondsRule.Context", id: "diagram_node_130", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_E2D75DB9302F9E8267D350CE1174FA379AE99BDE.html" },
        { name: "System.Net.Http.HttpGrammar.fieldNameRule.Context", id: "diagram_node_131", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_A5FCF59EECA4CCA3BDEAD18946BF530D8172CDEE.html" },
        { name: "System.Net.Http.HttpGrammar.fieldValueRule.Context", id: "diagram_node_132", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_40D0EED8C4AF620CCFC5D8E5557567789915A488.html" },
        { name: "System.Net.Http.HttpGrammar.generalFieldValueRule.Context", id: "diagram_node_133", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_8F3404E9709BAA4EBFCB2632461764938B133503.html" },
        { name: "System.Net.Http.HttpGrammar.generalParameterRule.Context", id: "diagram_node_134", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_6E35BF9CA6BBBCCF698244145F7BF841290658D6.html" },
        { name: "System.Net.Http.HttpGrammar.httpDateRule.Context", id: "diagram_node_135", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_9FC0E19C3E28D7A7E20353AFD23602F801A5D2E1.html" },
        { name: "System.Net.Http.HttpGrammar.httpHeaderRule.Context", id: "diagram_node_108", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_A237F8EED91E6216D43EB668A6E1F407021862B2.html" },
        { name: "System.Net.Http.HttpGrammar.httpTokenRule.Context", id: "diagram_node_136", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_C574F9C7FC7810EF9555D6818C52D0C27C54AEAF.html" },
        { name: "System.Net.Http.HttpGrammar.httpVersionRule.Context", id: "diagram_node_51", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_AEC613CA5BEA169688FA13C09F6EF87C178105C9.html" },
        { name: "System.Net.Http.HttpGrammar.mediaTypeRule.Context", id: "diagram_node_103", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_28A46A841B510E819A1C1EFE1A626C4BAD35822B.html" },
        { name: "System.Net.Http.HttpGrammar.monthRule.Context", id: "diagram_node_138", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_C68B37A342397C69B703E48B13B6BEF5423423FE.html" },
        { name: "System.Net.Http.HttpGrammar.parameterRule.Context", id: "diagram_node_56", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_5CDD6A463F1EA7B091F4688EC214352562687B74.html" },
        { name: "System.Net.Http.HttpGrammar.qdtextRule.Context", id: "diagram_node_139", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_B6F4D33607327E07FADEB66A22A0777AE9ECB4A7.html" },
        { name: "System.Net.Http.HttpGrammar.quotedPairRule.Context", id: "diagram_node_140", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_9F0F458E5AC311CFE9A9D2DBC770A34C2C120F3B.html" },
        { name: "System.Net.Http.HttpGrammar.quotedStringRule.Context", id: "diagram_node_141", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_6BF76684687572A173B0952A509FE687AC71ADDB.html" },
        { name: "System.Net.Http.HttpGrammar.reasonPhraseRule.Context", id: "diagram_node_142", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_1C8C57E970C0E9D298F38D50F3F2A43275CD37FF.html" },
        { name: "System.Net.Http.HttpGrammar.rfc1123DateRule.Context", id: "diagram_node_36", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_80DA0E7D37A084BD2AFECDC4D8B88FFFF9E9F1AB.html" },
        { name: "System.Net.Http.HttpGrammar.rfc850DateRule.Context", id: "diagram_node_143", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_0242B59C2F7B8DD8B634FAC3E240BDC623D85BFA.html" },
        { name: "System.Net.Http.HttpGrammar.statusCodeRule.Context", id: "diagram_node_144", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_75BE4D3853FC92F4CEA78BC8F25E4421DFAB540B.html" },
        { name: "System.Net.Http.HttpGrammar.statusLineRule.Context", id: "diagram_node_2", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_723046382F82B9C977BF78AD14060B0DD33E209D.html" },
        { name: "System.Net.Http.HttpGrammar.subtypeRule.Context", id: "diagram_node_145", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_626FBF2FFE2AA76E885DFAC32683FBF54EF59324.html" },
        { name: "System.Net.Http.HttpGrammar.timeRule.Context", id: "diagram_node_146", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_55699DCA7391CD2C4FC32C2346F6DF5F2ECA6907.html" },
        { name: "System.Net.Http.HttpGrammar.typeRule.Context", id: "diagram_node_147", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_5168F5DBB18A84CD2FF82E61922AA583CFFBE24F.html" },
        { name: "System.Net.Http.HttpHeaderGrammar.headerRule.Context", id: "diagram_node_73", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_D9ADCA1D5AAA83B578A1089993DEFF91ED4E192F.html" },
        { name: "System.Net.Http.HttpMediaTypeGrammar.mediaTypeValueRule.Context", id: "diagram_node_148", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_D2613179E0A00E01DF558A99572C25CE6ED48244.html" },
        { name: "System.Net.Http.HttpStatusLineGrammar.statusRule.Context", id: "diagram_node_149", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_3DC5CBEE279183505F35022C41CF8CD05B73D12E.html" },
        { name: "System.Net.Http.UriGrammar.AbsoluteUriRule.Context", id: "diagram_node_150", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_5AAAF1A7FB84CD6C759353BD9F2D14876208DE93.html" },
        { name: "System.Net.Http.UriGrammar.AuthorityRule.Context", id: "diagram_node_151", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_0C99C434082D65EDFEDEE5D21F23F87A8759A167.html" },
        { name: "System.Net.Http.UriGrammar.DomainLabelRule.Context", id: "diagram_node_96", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_5B35F9C73D73742DF70636E05C0C51719C99C5D3.html" },
        { name: "System.Net.Http.UriGrammar.HierPartRule.Context", id: "diagram_node_153", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_C328BC4B485A36C66264F55006F3E8BB8F72C071.html" },
        { name: "System.Net.Http.UriGrammar.HostNameRule.Context", id: "diagram_node_155", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_64671063EB734B3E4FB0FA4923D55AF026038216.html" },
        { name: "System.Net.Http.UriGrammar.HostPortRule.Context", id: "diagram_node_156", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_BE8562A78C0FBEC1441186F2785B02A0EF92E8C4.html" },
        { name: "System.Net.Http.UriGrammar.NetPathRule.Context", id: "diagram_node_157", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_656A3CFF3844F2B32636CB069A2571AD06F798A7.html" },
        { name: "System.Net.Http.UriGrammar.PortRule.Context", id: "diagram_node_137", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_43CC69F2D35246CEECD394F75EF1EBC52D022CE1.html" },
        { name: "System.Net.Http.UriGrammar.RelativeUriRule.Context", id: "diagram_node_158", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_1CC44B067C5457D5C13E36169FB8C98B2ECD2A19.html" },
        { name: "System.Net.Http.UriGrammar.ServerRule.Context", id: "diagram_node_159", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_6905304182DACAE13CF36B3D318062B5F4C52B9B.html" },
        { name: "System.Net.Http.UriGrammar.TopLabelRule.Context", id: "diagram_node_160", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_CAF0CD31689EB68D128B90D6AFE66313FDF3EA1D.html" },
        { name: "System.Net.Http.UriGrammar.UriReferenceRule.Context", id: "diagram_node_161", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Context_3627C13261443D2FAF69F158244322B7158E8AFF.html" },
        { name: "System.Text.Parsing.stdlib.boolRule.Context", id: "diagram_node_3", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_Context_ADBA85E2E02F7E7231A06C4B17A8011D75FC0E2D.html" },
        { name: "System.Text.Parsing.stdlib.byteRule.Context", id: "diagram_node_8", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_Context_ECD3B130A0619EB35F7B454B93B42925757F7DE7.html" },
        { name: "System.Text.Parsing.stdlib.charRule.Context", id: "diagram_node_6", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_Context_9A0F2200F51BF8AA550323C4F2EDF991155908BD.html" },
        { name: "System.Text.Parsing.stdlib.doubleRule.Context", id: "diagram_node_10", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_Context_76E806ADA2843FDD75319B1BCBD6D329D8E07510.html" },
        { name: "System.Text.Parsing.stdlib.escapeRule.Context", id: "diagram_node_15", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_Context_2AA850BA72AE658B4D36906F8C2E2AF3F0D4C293.html" },
        { name: "System.Text.Parsing.stdlib.floatRule.Context", id: "diagram_node_13", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_Context_B042BE4BB589CF038B308F8A0003757C287052B1.html" },
        { name: "System.Text.Parsing.stdlib.hexRule.Context", id: "diagram_node_18", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_Context_F7CC9ECB8BFD015E976EDFDCA010DD2E7E66BB4F.html" },
        { name: "System.Text.Parsing.stdlib.hex_literalRule.Context", id: "diagram_node_9", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_Context_BB7B12E9650F4C0FAEF6553E3BFD0F48BEA6B816.html" },
        { name: "System.Text.Parsing.stdlib.hexbyteRule.Context", id: "diagram_node_17", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_Context_93EF22174A06FB61DDC9665AA75FDE0524FD6E3F.html" },
        { name: "System.Text.Parsing.stdlib.hexdwordRule.Context", id: "diagram_node_19", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_Context_38F312432FE03D76DCBF29B0F672EAB8CC22C002.html" },
        { name: "System.Text.Parsing.stdlib.hexqwordRule.Context", id: "diagram_node_20", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_Context_5AEFF056096C9DB4DD9A789139375764C6AC9C87.html" },
        { name: "System.Text.Parsing.stdlib.hexwordRule.Context", id: "diagram_node_22", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_Context_9A0D3F77D56B57DCEF83D26ED2A2727D23AFA119.html" },
        { name: "System.Text.Parsing.stdlib.identifierRule.Context", id: "diagram_node_26", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_Context_980A770826B0D84832D576F0A98A53C7F561AD34.html" },
        { name: "System.Text.Parsing.stdlib.intRule.Context", id: "diagram_node_28", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_Context_6A39AD94127F969F1D23CF740EFDD5A7FAD738D2.html" },
        { name: "System.Text.Parsing.stdlib.longRule.Context", id: "diagram_node_30", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_Context_575D937C7970C0F8BB113EC2D2DEA09DDCC074B3.html" },
        { name: "System.Text.Parsing.stdlib.numRule.Context", id: "diagram_node_16", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_Context_3664830904D570FF377F49DFE01B45C3A25DD87A.html" },
        { name: "System.Text.Parsing.stdlib.octal_digitRule.Context", id: "diagram_node_32", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_Context_08F70E9272E44DE52B20EA9152A0FBD4A81E74F7.html" },
        { name: "System.Text.Parsing.stdlib.qualified_idRule.Context", id: "diagram_node_34", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_Context_0BB11C4741B5F63EFCA1A19C7ACBB749E43CC2E2.html" },
        { name: "System.Text.Parsing.stdlib.sbyteRule.Context", id: "diagram_node_35", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_Context_855FECD8147E889D8EE4657F7D5146484A6B40E0.html" },
        { name: "System.Text.Parsing.stdlib.shortRule.Context", id: "diagram_node_38", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_Context_5D0D42188F71D20ECF040EAC6CA0C0F0DADFD87A.html" },
        { name: "System.Text.Parsing.stdlib.stringRule.Context", id: "diagram_node_39", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_Context_418248BD78CB44D4E18AB434C473F2F1E471649A.html" },
        { name: "System.Text.Parsing.stdlib.uintRule.Context", id: "diagram_node_41", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_Context_9A5CFEB43F650C44EB0839CC2527BA254CA5FA5C.html" },
        { name: "System.Text.Parsing.stdlib.ulongRule.Context", id: "diagram_node_42", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_Context_EC7F146CE01EB601A40BC87EAACE69AF8938A1FE.html" },
        { name: "System.Text.Parsing.stdlib.urealRule.Context", id: "diagram_node_43", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_Context_82786F341A2A2D903014844D45B9FA98CCD36994.html" },
        { name: "System.Text.Parsing.stdlib.ushortRule.Context", id: "diagram_node_45", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_Context_02ABE7999CC9A1172B3261949625EFD3F3C7F3E3.html" },
        { name: "System.Text.RegularExpressions.RegularExpressionGrammar.AlternativeExpressionRule.Context", id: "diagram_node_40", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Text.RegularExpressions/doc/class_Context_0C61E182102956552E65E5786F357793121668DE.html" },
        { name: "System.Text.RegularExpressions.RegularExpressionGrammar.CharClassCharRule.Context", id: "diagram_node_53", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Text.RegularExpressions/doc/class_Context_7A52FE4D43A082F3F2C3C8C0342125580652BBD2.html" },
        { name: "System.Text.RegularExpressions.RegularExpressionGrammar.CharClassRule.Context", id: "diagram_node_47", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Text.RegularExpressions/doc/class_Context_63EB906843D5D4373C35066FF5009683B089C34B.html" },
        { name: "System.Text.RegularExpressions.RegularExpressionGrammar.CharRangeRule.Context", id: "diagram_node_57", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Text.RegularExpressions/doc/class_Context_6F4E6746E48FD0579E593CDF9C2928E1F21F533F.html" },
        { name: "System.Text.RegularExpressions.RegularExpressionGrammar.CharRule.Context", id: "diagram_node_12", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Text.RegularExpressions/doc/class_Context_0C8D185307CAF62D15099706D5FE138E1B6E1288.html" },
        { name: "System.Text.RegularExpressions.RegularExpressionGrammar.ClassRule.Context", id: "diagram_node_59", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Text.RegularExpressions/doc/class_Context_2995333F7FB4E575DF46CB6813BEB1F6F3A7E737.html" },
        { name: "System.Text.RegularExpressions.RegularExpressionGrammar.PostfixExpressionRule.Context", id: "diagram_node_60", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Text.RegularExpressions/doc/class_Context_C54426BCE970844EC5951E8505ABAD41E90D3B3C.html" },
        { name: "System.Text.RegularExpressions.RegularExpressionGrammar.PrimaryExpressionRule.Context", id: "diagram_node_7", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Text.RegularExpressions/doc/class_Context_0D46FA6C66A259182789E6D19E7461B00281875B.html" },
        { name: "System.Text.RegularExpressions.RegularExpressionGrammar.RegularExpressionRule.Context", id: "diagram_node_63", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Text.RegularExpressions/doc/class_Context_8F48190AFFFA303A9B132F56189CDD4703FC16E1.html" },
        { name: "System.Text.RegularExpressions.RegularExpressionGrammar.SequenceExpressionRule.Context", id: "diagram_node_33", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Text.RegularExpressions/doc/class_Context_B08E85EEE770E2B21335C89CF53D58AB5087B3A6.html" },
        { name: "System.XPath.XPathExpressionGrammar.AbbreviatedAbsoluteLocationPathRule.Context", id: "diagram_node_163", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_Context_89EDFA550517CD67E7B25BB4F62DD042CFA7A3CF.html" },
        { name: "System.XPath.XPathExpressionGrammar.AbbreviatedAxisSpecifierRule.Context", id: "diagram_node_164", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_Context_8C8C5CC402BB573C90A4B318CAF2F3AF31FF8B23.html" },
        { name: "System.XPath.XPathExpressionGrammar.AbbreviatedStepRule.Context", id: "diagram_node_90", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_Context_411D44FAB8E8E99A2D4E8FDD13DAAEA4BF0874E9.html" },
        { name: "System.XPath.XPathExpressionGrammar.AbsoluteLocationPathRule.Context", id: "diagram_node_24", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_Context_DDF30A823AA8886AC67B2F371CA166A4D9204E75.html" },
        { name: "System.XPath.XPathExpressionGrammar.AdditiveExprRule.Context", id: "diagram_node_165", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_Context_5F81F6ABC400125701FB7AF54F3F0A5CFFEACAE2.html" },
        { name: "System.XPath.XPathExpressionGrammar.AndExprRule.Context", id: "diagram_node_166", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_Context_4EFD973534F9193F9E5B011141F89048513B64AC.html" },
        { name: "System.XPath.XPathExpressionGrammar.ArgumentRule.Context", id: "diagram_node_167", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_Context_75FC2F2D6E93F894E52B9DFBB0F18197C9FD0DDC.html" },
        { name: "System.XPath.XPathExpressionGrammar.AxisNameRule.Context", id: "diagram_node_168", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_Context_81B3610077D7320CAD07FB69B83E98F11D58A8D2.html" },
        { name: "System.XPath.XPathExpressionGrammar.AxisSpecifierRule.Context", id: "diagram_node_169", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_Context_F59A44DA40DE61A821DA6FC17B4FE3A380548E64.html" },
        { name: "System.XPath.XPathExpressionGrammar.EqualityExprRule.Context", id: "diagram_node_4", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_Context_F0DC261DD6D7E3A5ED2569FFA8D2F14833EDED9F.html" },
        { name: "System.XPath.XPathExpressionGrammar.ExprRule.Context", id: "diagram_node_120", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_Context_33C29259A66DB599520BA2BCFC60747287DE1E05.html" },
        { name: "System.XPath.XPathExpressionGrammar.FilterExprRule.Context", id: "diagram_node_170", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_Context_0D38EBFA3B8CE5DE4C688CDBE9265EE0B385E318.html" },
        { name: "System.XPath.XPathExpressionGrammar.FunctionCallRule.Context", id: "diagram_node_171", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_Context_841A56DE41DD0AB7B25A8BCB8D9F5A83BCB2DE37.html" },
        { name: "System.XPath.XPathExpressionGrammar.FunctionNameRule.Context", id: "diagram_node_173", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_Context_02F31FF96E053A3DF820D8E4B971CCFB6A9A4B57.html" },
        { name: "System.XPath.XPathExpressionGrammar.LiteralRule.Context", id: "diagram_node_174", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_Context_4115E868A61003AF125A59354B4CFF76A4EB19EE.html" },
        { name: "System.XPath.XPathExpressionGrammar.LocalPartRule.Context", id: "diagram_node_175", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_Context_3B0DE0CB752A3EB0834B64DC34533AA91CD4D026.html" },
        { name: "System.XPath.XPathExpressionGrammar.LocationPathRule.Context", id: "diagram_node_162", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_Context_D02C60F9EDCB39E71F63066878EC97015989E375.html" },
        { name: "System.XPath.XPathExpressionGrammar.MultiplicativeExprRule.Context", id: "diagram_node_176", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_Context_92E374EFC7697F3DA34B303B1289948934043732.html" },
        { name: "System.XPath.XPathExpressionGrammar.NCNameRule.Context", id: "diagram_node_177", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_Context_67B1C694634226C0E37D3B3E0BB627AA0D012CB7.html" },
        { name: "System.XPath.XPathExpressionGrammar.NameTestRule.Context", id: "diagram_node_178", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_Context_0696CCAD44EE94C35FAF17044D8674D005CFAF53.html" },
        { name: "System.XPath.XPathExpressionGrammar.NodeTestRule.Context", id: "diagram_node_126", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_Context_09B218341C6EF989B44891A3ABB700053546616A.html" },
        { name: "System.XPath.XPathExpressionGrammar.NodeTypeRule.Context", id: "diagram_node_179", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_Context_E0BFAB3CF8493F6C3EC9873DA9A2AFD18769D657.html" },
        { name: "System.XPath.XPathExpressionGrammar.NumberRule.Context", id: "diagram_node_152", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_Context_28760DA6A191C0F87BEF1686160509C94DD3EFA3.html" },
        { name: "System.XPath.XPathExpressionGrammar.OrExprRule.Context", id: "diagram_node_93", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_Context_BBB0C5B6E4063E3766DB9ECB062A8CCDF8B47C32.html" },
        { name: "System.XPath.XPathExpressionGrammar.PathExprRule.Context", id: "diagram_node_172", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_Context_C79FCF8BEA2118E3246991B1967A8CB33D61142E.html" },
        { name: "System.XPath.XPathExpressionGrammar.PredicateExprRule.Context", id: "diagram_node_180", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_Context_EA2D2185CD320E0489A0555539B3E301AB96E7BA.html" },
        { name: "System.XPath.XPathExpressionGrammar.PredicateRule.Context", id: "diagram_node_181", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_Context_7B16DCE9151AEAFA7E4FBC0574682427E9671DA8.html" },
        { name: "System.XPath.XPathExpressionGrammar.PrefixRule.Context", id: "diagram_node_182", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_Context_BC3BB8F444784615345F4C890273023EB2A14E3D.html" },
        { name: "System.XPath.XPathExpressionGrammar.PrefixedNameRule.Context", id: "diagram_node_183", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_Context_735AF91F79BA92A06AFD998BF1F0F1CD66012E54.html" },
        { name: "System.XPath.XPathExpressionGrammar.PrimaryExprRule.Context", id: "diagram_node_184", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_Context_1C3B19DDCB03C13B3EB10B98A5E52D4D9E8403FF.html" },
        { name: "System.XPath.XPathExpressionGrammar.QNameRule.Context", id: "diagram_node_185", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_Context_6C2B2778632044472FF996C78892E8FC9544CD21.html" },
        { name: "System.XPath.XPathExpressionGrammar.RelationalExprRule.Context", id: "diagram_node_154", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_Context_652D7781151768F5F5858457503C406ABB2819CB.html" },
        { name: "System.XPath.XPathExpressionGrammar.RelativeLocationPathRule.Context", id: "diagram_node_186", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_Context_B453286DCF7FD24C50BBBE558831227D735C0BF0.html" },
        { name: "System.XPath.XPathExpressionGrammar.StepRule.Context", id: "diagram_node_187", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_Context_24BAF8A11778D2640B939B3C113339E6EE1B8C93.html" },
        { name: "System.XPath.XPathExpressionGrammar.UnaryExprRule.Context", id: "diagram_node_21", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_Context_4B52889388B81759717F7A56542EFB699C4ED763.html" },
        { name: "System.XPath.XPathExpressionGrammar.UnionExprRule.Context", id: "diagram_node_188", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_Context_9BCDFCA05B297A30FD0CAD2F1D39B35A5AFCEF9C.html" },
        { name: "System.XPath.XPathExpressionGrammar.UnprefixedNameRule.Context", id: "diagram_node_189", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_Context_C25C44171B437A0A32F7C194770BDDAF7F1CA82B.html" },
        { name: "System.XPath.XPathExpressionGrammar.VariableReferenceRule.Context", id: "diagram_node_190", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_Context_21D5F2E27BCD89F40CB159A87186F9909784F654.html" },
        { name: "System.Xml.XmlGrammar.AttDefRule.Context", id: "diagram_node_31", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_Context_C2BE20376F9E9EFAA7CC4B62ED099EEECFB3DADB.html" },
        { name: "System.Xml.XmlGrammar.AttValueRule.Context", id: "diagram_node_61", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_Context_2195A12EDAADB8C89F8DE52986EAA1F91FAE088A.html" },
        { name: "System.Xml.XmlGrammar.AttlistDeclRule.Context", id: "diagram_node_65", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_Context_12735A043A5F853D886712F850F8D76536927E81.html" },
        { name: "System.Xml.XmlGrammar.AttributeRule.Context", id: "diagram_node_23", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_Context_62FD01506832B2455CA068BF7108FE638E455449.html" },
        { name: "System.Xml.XmlGrammar.CDSectRule.Context", id: "diagram_node_50", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_Context_7D0127ECCD95748E2144340F1C9A3607C179F680.html" },
        { name: "System.Xml.XmlGrammar.CDataRule.Context", id: "diagram_node_68", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_Context_63BD385421769FA8EAF751C36B9F020189B2D95F.html" },
        { name: "System.Xml.XmlGrammar.CharDataRule.Context", id: "diagram_node_71", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_Context_C5E4E7CA8E1F0584468BC081BF060D0F2716DD20.html" },
        { name: "System.Xml.XmlGrammar.CharRefRule.Context", id: "diagram_node_72", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_Context_97091BE700057B5FB2C9B040AC630E17D18B76E6.html" },
        { name: "System.Xml.XmlGrammar.CommentRule.Context", id: "diagram_node_27", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_Context_84C2EC2BCE26BE24E817A48091FB3D290EC58A80.html" },
        { name: "System.Xml.XmlGrammar.ConditionalSectRule.Context", id: "diagram_node_74", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_Context_CA2711A142C739A7FBDF6D8FBD207514911248A9.html" },
        { name: "System.Xml.XmlGrammar.ContentRule.Context", id: "diagram_node_44", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_Context_938E8AA60EBE77135FD89824461C0D5073BAB410.html" },
        { name: "System.Xml.XmlGrammar.DeclSepRule.Context", id: "diagram_node_58", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_Context_36E710942B4BCFF4629B33C82DCF197C387E635C.html" },
        { name: "System.Xml.XmlGrammar.DefaultDeclRule.Context", id: "diagram_node_67", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_Context_8B6BDA9FD8FEE65ACBDF9A189A832E0B607D8E8C.html" },
        { name: "System.Xml.XmlGrammar.DocTypeDeclRule.Context", id: "diagram_node_29", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_Context_4EAEC7EBE2D7AC0F3B33C033D6CB695761FF6769.html" },
        { name: "System.Xml.XmlGrammar.DocumentRule.Context", id: "diagram_node_55", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_Context_9FC0D36D9C6EE03B21C8A9BCBC40B16A709E7970.html" },
        { name: "System.Xml.XmlGrammar.ETagRule.Context", id: "diagram_node_75", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_Context_A5E358867746DE6EFDF46A8C9794A8BD9965B63D.html" },
        { name: "System.Xml.XmlGrammar.ElementDeclRule.Context", id: "diagram_node_76", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_Context_ECFF17CB785E1159980C372DC61DEEC9DB79DC50.html" },
        { name: "System.Xml.XmlGrammar.ElementRule.Context", id: "diagram_node_77", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_Context_9F41956F0ACC5D6002F8F42EB3970EC54155C743.html" },
        { name: "System.Xml.XmlGrammar.EncNameRule.Context", id: "diagram_node_64", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_Context_B3D9D3FAB3722046991D5C7BA7BF9142D003A039.html" },
        { name: "System.Xml.XmlGrammar.EncodingDeclRule.Context", id: "diagram_node_5", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_Context_541E1B589BDF06020A9A4793EBB7BC6F17EA3D5D.html" },
        { name: "System.Xml.XmlGrammar.EntityDeclRule.Context", id: "diagram_node_37", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_Context_985FE96EB466F9B6DDCAC11A4ADB3B9892D12FB0.html" },
        { name: "System.Xml.XmlGrammar.EntityDefRule.Context", id: "diagram_node_66", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_Context_C7CEF09EED2D05C655006481D3E3CF5B18601A74.html" },
        { name: "System.Xml.XmlGrammar.EntityRefRule.Context", id: "diagram_node_79", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_Context_6848C0399039250818165D3DA29F62669489F8DF.html" },
        { name: "System.Xml.XmlGrammar.EntityValueRule.Context", id: "diagram_node_80", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_Context_6652360ACEC2F470E296D221BC7B39B6546A0327.html" },
        { name: "System.Xml.XmlGrammar.ExtParsedEntRule.Context", id: "diagram_node_81", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_Context_4B0E0BA0AECDC3F05BC61143B2E8A42D3C9CB1D0.html" },
        { name: "System.Xml.XmlGrammar.ExtSubsetDeclRule.Context", id: "diagram_node_82", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_Context_C80070A6CFB5A6F476047436CAF1ED5D2CD8BB7A.html" },
        { name: "System.Xml.XmlGrammar.ExtSubsetRule.Context", id: "diagram_node_83", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_Context_51451DC5F938A4160A3AC4951E1A60936BE6C451.html" },
        { name: "System.Xml.XmlGrammar.GEDeclRule.Context", id: "diagram_node_84", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_Context_524D1C7ACE6CA3849265F8014F8B5017D97DFF21.html" },
        { name: "System.Xml.XmlGrammar.IncludeSectRule.Context", id: "diagram_node_85", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_Context_FC59C49868082A2615858709CAC9DCFDB212DFD3.html" },
        { name: "System.Xml.XmlGrammar.IntSubsetRule.Context", id: "diagram_node_86", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_Context_ED3177170817C9FA963C7ED09B9894BBC7EA5C87.html" },
        { name: "System.Xml.XmlGrammar.MarkupDeclRule.Context", id: "diagram_node_87", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_Context_D2FE942CB6BA0E0AF74CB33139EFBEDA3B8EC9BA.html" },
        { name: "System.Xml.XmlGrammar.MiscRule.Context", id: "diagram_node_89", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_Context_53404E1FB13E1B55FA68B205C98ABC28EF1A5FA6.html" },
        { name: "System.Xml.XmlGrammar.NDataDeclRule.Context", id: "diagram_node_91", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_Context_4DEC03A9E6195A30CA908110FF9A5697712A081C.html" },
        { name: "System.Xml.XmlGrammar.NameRule.Context", id: "diagram_node_92", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_Context_98F33489EC9E780E87D2355AB9C8496750B1C3E2.html" },
        { name: "System.Xml.XmlGrammar.NotationDeclRule.Context", id: "diagram_node_94", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_Context_A86E4F1BEB1331C2D808E8EFDDF5460106BE5A9A.html" },
        { name: "System.Xml.XmlGrammar.PEDeclRule.Context", id: "diagram_node_95", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_Context_33ACC4C37742B99E7E1BC28E05EDC55AA3BD5553.html" },
        { name: "System.Xml.XmlGrammar.PEDefRule.Context", id: "diagram_node_97", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_Context_353AB67CBBE61A8DB8D86CED70575AEA5B6072F9.html" },
        { name: "System.Xml.XmlGrammar.PEReferenceRule.Context", id: "diagram_node_98", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_Context_7F150263C077241A2B5CC118C6CB3A84E4C73539.html" },
        { name: "System.Xml.XmlGrammar.PIRule.Context", id: "diagram_node_99", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_Context_0102EDD8F77D0F8A431AF2B58654EE240EC12CEF.html" },
        { name: "System.Xml.XmlGrammar.PITargetRule.Context", id: "diagram_node_100", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_Context_507B1CC93697A45C9B7FA9C3FA4152961D62DA30.html" },
        { name: "System.Xml.XmlGrammar.PrologRule.Context", id: "diagram_node_62", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_Context_D0B426996CBBC54A2C05CADA5C065CE47A8777B9.html" },
        { name: "System.Xml.XmlGrammar.PubidLiteralRule.Context", id: "diagram_node_48", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_Context_69F04B1BAD1864EF4956EDC929717D8B25B1A132.html" },
        { name: "System.Xml.XmlGrammar.ReferenceRule.Context", id: "diagram_node_101", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_Context_61D985425EDF94D5236BEEE32968DA9328D7664F.html" },
        { name: "System.Xml.XmlGrammar.SDDeclRule.Context", id: "diagram_node_14", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_Context_E1796F4AB0AD9A6F2BA32BB2665ED7F7DD9CF2A0.html" },
        { name: "System.Xml.XmlGrammar.SystemLiteralRule.Context", id: "diagram_node_102", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_Context_BDACA992A8C4C903228BECED17AE63706B795D24.html" },
        { name: "System.Xml.XmlGrammar.TextDeclRule.Context", id: "diagram_node_104", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_Context_D9177563DA196BB22B9B81B8626FD7E99F3412A0.html" },
        { name: "System.Xml.XmlGrammar.VersionInfoRule.Context", id: "diagram_node_105", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_Context_CC4430F77930BE8290A883DEBEDB6C152EBFEC49.html" },
        { name: "System.Xml.XmlGrammar.XMLDeclRule.Context", id: "diagram_node_106", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_Context_50BBB0D86091E9CF5F5C6EC5C5E6B787DB99675E.html" }];
    drawDiagram(diagramNodes);
}

