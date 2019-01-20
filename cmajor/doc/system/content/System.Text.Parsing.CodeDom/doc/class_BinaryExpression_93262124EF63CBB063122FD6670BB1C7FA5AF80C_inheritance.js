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
        { name: "System.Text.Parsing.CodeDom.DomObject", id: "diagram_node_0", level: 0, subject: false, hasDerivedNodes: true, link: "../../System.Text.Parsing.CodeDom/doc/class_DomObject_A6EA469160F8FD3B17826E7729430E5F3D07C4DF.html" },
        { name: "System.Text.Parsing.CodeDom.Expression", id: "diagram_node_1", level: 1, subject: false, hasDerivedNodes: true, link: "../../System.Text.Parsing.CodeDom/doc/class_Expression_63F45770E98CB553F848612A60783A045B1137F7.html" },
        { name: "System.Text.Parsing.CodeDom.BinaryExpression", id: "diagram_node_2", level: 2, subject: true, hasDerivedNodes: true, link: "../../System.Text.Parsing.CodeDom/doc/class_BinaryExpression_93262124EF63CBB063122FD6670BB1C7FA5AF80C.html" },
        { name: "System.Text.Parsing.CodeDom.Add", id: "diagram_node_3", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing.CodeDom/doc/class_Add_D8266E0EBE7DB2DA07CF9189C1A15DF16E9823E3.html" },
        { name: "System.Text.Parsing.CodeDom.As", id: "diagram_node_4", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing.CodeDom/doc/class_As_17FE45AC637C4950CB0B2BBFEE789A84A32233B8.html" },
        { name: "System.Text.Parsing.CodeDom.BitAnd", id: "diagram_node_7", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing.CodeDom/doc/class_BitAnd_C18E427EDF0266B9DE4A7AF49DF7814F4508A300.html" },
        { name: "System.Text.Parsing.CodeDom.BitOr", id: "diagram_node_8", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing.CodeDom/doc/class_BitOr_7AFF6998C572E610457C1E96CEA6E60FE5F10EE5.html" },
        { name: "System.Text.Parsing.CodeDom.BitXor", id: "diagram_node_5", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing.CodeDom/doc/class_BitXor_6E4A68B918EAF05039242CA6E5A108EDE3A875CC.html" },
        { name: "System.Text.Parsing.CodeDom.Conjunction", id: "diagram_node_9", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing.CodeDom/doc/class_Conjunction_0132C4E38ACB57C960C99B8800089A5ED52CB3AD.html" },
        { name: "System.Text.Parsing.CodeDom.Disjunction", id: "diagram_node_6", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing.CodeDom/doc/class_Disjunction_F31E3B03B7D35E04D91C7E729F2351D3957CC86E.html" },
        { name: "System.Text.Parsing.CodeDom.Div", id: "diagram_node_10", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing.CodeDom/doc/class_Div_870BC7AD6CF194517E350C8AF37CEAA3E22F8231.html" },
        { name: "System.Text.Parsing.CodeDom.Equal", id: "diagram_node_11", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing.CodeDom/doc/class_Equal_3EF4BE86B509FD01F694F7C4806B5C900557528A.html" },
        { name: "System.Text.Parsing.CodeDom.Greater", id: "diagram_node_12", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing.CodeDom/doc/class_Greater_615FAE96F23C7D50557A473DB2E1090BB39F1311.html" },
        { name: "System.Text.Parsing.CodeDom.GreaterOrEqual", id: "diagram_node_13", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing.CodeDom/doc/class_GreaterOrEqual_4400FB33354DC18FE0061555E68C302AD3CE11E6.html" },
        { name: "System.Text.Parsing.CodeDom.Is", id: "diagram_node_15", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing.CodeDom/doc/class_Is_EC0C6DA58F33941C0957B3F764732D8472F6E088.html" },
        { name: "System.Text.Parsing.CodeDom.Less", id: "diagram_node_16", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing.CodeDom/doc/class_Less_1580A37986A6536C269A3DC15DC3E14E4C0804EA.html" },
        { name: "System.Text.Parsing.CodeDom.LessOrEqual", id: "diagram_node_17", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing.CodeDom/doc/class_LessOrEqual_9091963D31C6715767D56C999742B7722B15871F.html" },
        { name: "System.Text.Parsing.CodeDom.Mul", id: "diagram_node_18", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing.CodeDom/doc/class_Mul_34532DF2D76B7BA144AA0E44CCBF15EA39D37CCF.html" },
        { name: "System.Text.Parsing.CodeDom.NotEqual", id: "diagram_node_19", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing.CodeDom/doc/class_NotEqual_57BE2D644B155B649C949C1AB40D6F621FE9F457.html" },
        { name: "System.Text.Parsing.CodeDom.Rem", id: "diagram_node_14", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing.CodeDom/doc/class_Rem_B6CCE54FD2D032B79B8776E88368A5362BF0845F.html" },
        { name: "System.Text.Parsing.CodeDom.ShiftLeft", id: "diagram_node_20", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing.CodeDom/doc/class_ShiftLeft_0C58636617E67D99E2D4B947D93C487C9EC0EEAE.html" },
        { name: "System.Text.Parsing.CodeDom.ShiftRight", id: "diagram_node_21", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing.CodeDom/doc/class_ShiftRight_27E7AC65159A3C02A710CE1FA049B105D3C389B9.html" },
        { name: "System.Text.Parsing.CodeDom.Sub", id: "diagram_node_22", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing.CodeDom/doc/class_Sub_66511E41AA55D3A9AFDEFA0F4167C1D40E23242C.html" }];
    drawDiagram(diagramNodes);
}

