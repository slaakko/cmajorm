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
        { name: "cppsymbols.Symbol", id: "diagram_node_0", level: 0, subject: false, hasDerivedNodes: true, link: "../../cppsymbols/doc/class_Symbol_8A528ACAFCB10A97E11D5657F8828BEBB03E90DE.html" },
        { name: "cppsymbols.ContainerSymbol", id: "diagram_node_1", level: 1, subject: false, hasDerivedNodes: true, link: "../../cppsymbols/doc/class_ContainerSymbol_D2DD52C0F23F1518DA3A29F9A2B3F2356C7E7961.html" },
        { name: "cppsymbols.TypeSymbol", id: "diagram_node_2", level: 2, subject: false, hasDerivedNodes: true, link: "../../cppsymbols/doc/class_TypeSymbol_2611849784E8B1387D3D0120DAB9A90F555DCCC2.html" },
        { name: "cppsymbols.SimpleTypeSymbol", id: "diagram_node_3", level: 3, subject: true, hasDerivedNodes: true, link: "../../cppsymbols/doc/class_SimpleTypeSymbol_FF53B46E2B151DC621CD56C5DFB5FF489800D724.html" },
        { name: "cppsymbols.AutoTypeSymbol", id: "diagram_node_5", level: 4, subject: false, hasDerivedNodes: false, link: "../../cppsymbols/doc/class_AutoTypeSymbol_11F1B14D0D7ED91F853FB9D2875ECAC2E8FCD2E8.html" },
        { name: "cppsymbols.BoolTypeSymbol", id: "diagram_node_8", level: 4, subject: false, hasDerivedNodes: false, link: "../../cppsymbols/doc/class_BoolTypeSymbol_44285D4F36A0A39672544F5E178951E46F74D07A.html" },
        { name: "cppsymbols.Char16TypeSymbol", id: "diagram_node_7", level: 4, subject: false, hasDerivedNodes: false, link: "../../cppsymbols/doc/class_Char16TypeSymbol_59F43BDE97363E73174ADEDE451FE1A4F92A432F.html" },
        { name: "cppsymbols.Char32TypeSymbol", id: "diagram_node_9", level: 4, subject: false, hasDerivedNodes: false, link: "../../cppsymbols/doc/class_Char32TypeSymbol_40A8E092619664E4ACFC637938F5D3C61F93E45B.html" },
        { name: "cppsymbols.CharTypeSymbol", id: "diagram_node_10", level: 4, subject: false, hasDerivedNodes: false, link: "../../cppsymbols/doc/class_CharTypeSymbol_16C49FF4616A546769A6948A679AE6B0E8D827D3.html" },
        { name: "cppsymbols.DoubleTypeSymbol", id: "diagram_node_4", level: 4, subject: false, hasDerivedNodes: false, link: "../../cppsymbols/doc/class_DoubleTypeSymbol_0B2DE94BAA9C2523AFC9F3948696149716CF7769.html" },
        { name: "cppsymbols.FloatTypeSymbol", id: "diagram_node_11", level: 4, subject: false, hasDerivedNodes: false, link: "../../cppsymbols/doc/class_FloatTypeSymbol_8B4572AF1E40F62F8D6EAD09071C442AA5EC8DF1.html" },
        { name: "cppsymbols.IntTypeSymbol", id: "diagram_node_6", level: 4, subject: false, hasDerivedNodes: false, link: "../../cppsymbols/doc/class_IntTypeSymbol_09754B31FA7858FB54F8E6FBD1B3CB38C6640FCB.html" },
        { name: "cppsymbols.LongIntTypeSymbol", id: "diagram_node_12", level: 4, subject: false, hasDerivedNodes: false, link: "../../cppsymbols/doc/class_LongIntTypeSymbol_9518346586AEC40CCE5FCF57B322F7E188C8CAAF.html" },
        { name: "cppsymbols.LongLongIntTypeSymbol", id: "diagram_node_13", level: 4, subject: false, hasDerivedNodes: false, link: "../../cppsymbols/doc/class_LongLongIntTypeSymbol_B6F3D60830FBA44BAF492FC9429E5FE04662EBC8.html" },
        { name: "cppsymbols.ShortIntTypeSymbol", id: "diagram_node_14", level: 4, subject: false, hasDerivedNodes: false, link: "../../cppsymbols/doc/class_ShortIntTypeSymbol_01A7291C102011CE5B30537D2E108ADF9E46F1F9.html" },
        { name: "cppsymbols.SignedCharTypeSymbol", id: "diagram_node_15", level: 4, subject: false, hasDerivedNodes: false, link: "../../cppsymbols/doc/class_SignedCharTypeSymbol_E350A8BA5EFF74B4E80795EA23AC919B0114459D.html" },
        { name: "cppsymbols.UnsignedCharTypeSymbol", id: "diagram_node_16", level: 4, subject: false, hasDerivedNodes: false, link: "../../cppsymbols/doc/class_UnsignedCharTypeSymbol_59F587BB0A1A35DF1142C2945110D6005BBDE979.html" },
        { name: "cppsymbols.UnsignedIntTypeSymbol", id: "diagram_node_17", level: 4, subject: false, hasDerivedNodes: false, link: "../../cppsymbols/doc/class_UnsignedIntTypeSymbol_B498B315A3CB8D0387B0C477EDC031A8987C8134.html" },
        { name: "cppsymbols.UnsignedLongIntTypeSymbol", id: "diagram_node_18", level: 4, subject: false, hasDerivedNodes: false, link: "../../cppsymbols/doc/class_UnsignedLongIntTypeSymbol_400B234A74E3E9AE029D57C6010E1F43607EF0BF.html" },
        { name: "cppsymbols.UnsignedLongLongIntTypeSymbol", id: "diagram_node_19", level: 4, subject: false, hasDerivedNodes: false, link: "../../cppsymbols/doc/class_UnsignedLongLongIntTypeSymbol_386760603A706EEC74B71345EE69846E438990B5.html" },
        { name: "cppsymbols.UnsignedShortIntTypeSymbol", id: "diagram_node_20", level: 4, subject: false, hasDerivedNodes: false, link: "../../cppsymbols/doc/class_UnsignedShortIntTypeSymbol_8CE22D84CB2F69F9C55FAFF7F9E124B3EE21880F.html" },
        { name: "cppsymbols.VoidTypeSymbol", id: "diagram_node_21", level: 4, subject: false, hasDerivedNodes: false, link: "../../cppsymbols/doc/class_VoidTypeSymbol_44F67DBD264B700B0936438638282A4B092C6642.html" },
        { name: "cppsymbols.WCharTypeSymbol", id: "diagram_node_22", level: 4, subject: false, hasDerivedNodes: false, link: "../../cppsymbols/doc/class_WCharTypeSymbol_70C3A4C829976B55007F64A2B09F4DA8F5118A82.html" }];
    drawDiagram(diagramNodes);
}

