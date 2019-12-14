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
        { name: "System.Cmajor.Ast.Node", id: "diagram_node_0", level: 0, subject: false, hasDerivedNodes: true, link: "../../System.Cmajor.Ast/doc/class_Node_ADEFE4683434E4CEFE56C39427194D086B4D6A8C.html" },
        { name: "System.Cmajor.Ast.LiteralNode", id: "diagram_node_1", level: 1, subject: true, hasDerivedNodes: true, link: "../../System.Cmajor.Ast/doc/class_LiteralNode_554A37F9FC552316EAE879B4EBEA292F9CCD3C93.html" },
        { name: "System.Cmajor.Ast.ArrayLiteralNode", id: "diagram_node_3", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_ArrayLiteralNode_AE63A553F00FD5E55D0E7A6533CEB195B345144E.html" },
        { name: "System.Cmajor.Ast.BooleanLiteralNode", id: "diagram_node_7", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_BooleanLiteralNode_52C048DEE35A0D9BC217D92F102F7F3931EDC31A.html" },
        { name: "System.Cmajor.Ast.ByteLiteralNode", id: "diagram_node_6", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_ByteLiteralNode_2262AC3ED002AD958E2ADEC39AC4441772505A08.html" },
        { name: "System.Cmajor.Ast.CharLiteralNode", id: "diagram_node_9", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_CharLiteralNode_19D88C11BD46F0BA1BA756AA186840AB3005CB7E.html" },
        { name: "System.Cmajor.Ast.DoubleLiteralNode", id: "diagram_node_11", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_DoubleLiteralNode_11073D2D5A723A33458CD58A9AEED996EFE180B1.html" },
        { name: "System.Cmajor.Ast.FloatLiteralNode", id: "diagram_node_4", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_FloatLiteralNode_F02BDFC6F5F5D4C26C522B55A393D9F2331544B2.html" },
        { name: "System.Cmajor.Ast.IntLiteralNode", id: "diagram_node_2", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_IntLiteralNode_4E3FFC43B5D642232CB74022AF0C1ACB85488851.html" },
        { name: "System.Cmajor.Ast.LongLiteralNode", id: "diagram_node_8", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_LongLiteralNode_BCDF10D598F21B7C0EE9072C0CB0F74193447E0D.html" },
        { name: "System.Cmajor.Ast.NullLiteralNode", id: "diagram_node_12", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_NullLiteralNode_9261DBD537D642F4B641C107465F4F40BAEC47A3.html" },
        { name: "System.Cmajor.Ast.SByteLiteralNode", id: "diagram_node_13", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_SByteLiteralNode_6E497D20C7A51E01FA4241C655B698E5BA08F1BD.html" },
        { name: "System.Cmajor.Ast.ShortLiteralNode", id: "diagram_node_5", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_ShortLiteralNode_F6FAFAD20211BE3864F4C70046CCF46CB9C58F1F.html" },
        { name: "System.Cmajor.Ast.StringLiteralNode", id: "diagram_node_14", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_StringLiteralNode_213370ADDB748A815CFF114A736BCE7956D083AA.html" },
        { name: "System.Cmajor.Ast.StructuredLiteralNode", id: "diagram_node_15", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_StructuredLiteralNode_97164EBE2E37F966E3BDA01EA9292B5B8FDA4269.html" },
        { name: "System.Cmajor.Ast.UCharLiteralNode", id: "diagram_node_16", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_UCharLiteralNode_5B70C33FFE47DB0B1614F734216226C816033A7B.html" },
        { name: "System.Cmajor.Ast.UIntLiteralNode", id: "diagram_node_10", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_UIntLiteralNode_1BB1321C4FA07F6327B4C79576533FFC35854AE1.html" },
        { name: "System.Cmajor.Ast.ULongLiteralNode", id: "diagram_node_17", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_ULongLiteralNode_31747C0970AF1F61F11AA657D9F9BDA096D5FCB4.html" },
        { name: "System.Cmajor.Ast.UShortLiteralNode", id: "diagram_node_18", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_UShortLiteralNode_832019C7AA7A7527DC32E31504632448F4A7EB58.html" },
        { name: "System.Cmajor.Ast.UStringLiteralNode", id: "diagram_node_19", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_UStringLiteralNode_F6A2E96DECA3027A45EFAE2FE7FE37CE66183579.html" },
        { name: "System.Cmajor.Ast.UuidLiteralNode", id: "diagram_node_21", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_UuidLiteralNode_065091A38C5A152894410FF9E650D231559588A5.html" },
        { name: "System.Cmajor.Ast.WCharLiteralNode", id: "diagram_node_22", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_WCharLiteralNode_CCACFB1A5E6524248D33BA7F185B3FCFFE643FCC.html" },
        { name: "System.Cmajor.Ast.WStringLiteralNode", id: "diagram_node_20", level: 2, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_WStringLiteralNode_CC0334F505B3FBC4E18C9FAB9F9025A4B2C4B7A8.html" }];
    drawDiagram(diagramNodes);
}

