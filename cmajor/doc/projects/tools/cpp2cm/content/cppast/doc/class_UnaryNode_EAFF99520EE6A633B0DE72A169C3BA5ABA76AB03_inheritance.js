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
        { name: "cppast.Node", id: "diagram_node_0", level: 0, subject: false, hasDerivedNodes: true, link: "../../cppast/doc/class_Node_DEB477A450EF6E99A28A9855849F398D50BF1609.html" },
        { name: "cppast.UnaryNode", id: "diagram_node_1", level: 1, subject: true, hasDerivedNodes: true, link: "../../cppast/doc/class_UnaryNode_EAFF99520EE6A633B0DE72A169C3BA5ABA76AB03.html" },
        { name: "cppast.AlignOfExpressionNode", id: "diagram_node_2", level: 2, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_AlignOfExpressionNode_6828194D7D4A5334C55A230DE71439093258B58B.html" },
        { name: "cppast.AssignmentInitializerNode", id: "diagram_node_5", level: 2, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_AssignmentInitializerNode_96F006F746B5D157DD07B0E3F5417E04FC47BD06.html" },
        { name: "cppast.ConstNode", id: "diagram_node_7", level: 2, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_ConstNode_4E4FB1A393B1C1F500FD5F0D01D38104F2A529C4.html" },
        { name: "cppast.ConversionFunctionIdNode", id: "diagram_node_3", level: 2, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_ConversionFunctionIdNode_4843B144C3CB6947CD84BCAAEFC59C92A2870A32.html" },
        { name: "cppast.DeleteExpressionNode", id: "diagram_node_10", level: 2, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_DeleteExpressionNode_AEE83657AA20B5D2CDF0F8E0447F94F68DCB153C.html" },
        { name: "cppast.DestructorIdNode", id: "diagram_node_11", level: 2, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_DestructorIdNode_6EDB43024F0444F3F0A7AC419D80DAE944B87252.html" },
        { name: "cppast.FunctionPtrIdNode", id: "diagram_node_13", level: 2, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_FunctionPtrIdNode_0AE0D8248C0B27EBFBF9997B77E0BF54117F1FCD.html" },
        { name: "cppast.IdentifierCapture", id: "diagram_node_12", level: 2, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_IdentifierCapture_A0E525308111586BD7EC86465FBEFAF99D929982.html" },
        { name: "cppast.InvokeExpressionNode", id: "diagram_node_9", level: 2, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_InvokeExpressionNode_8E2C5B3557C04DF37039AA90251FFA62730222B9.html" },
        { name: "cppast.LvalueRefNode", id: "diagram_node_14", level: 2, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_LvalueRefNode_0D0437D182778483EA54C53D3A98F5B9BF7FFF2A.html" },
        { name: "cppast.ParenthesizedExpressionNode", id: "diagram_node_15", level: 2, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_ParenthesizedExpressionNode_8CBB28376773B66387A4BCE112FC39E64B162AB2.html" },
        { name: "cppast.PointerNode", id: "diagram_node_6", level: 2, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_PointerNode_2653D2B09A8B298F8ED8DBBC43A65BB84A683EF7.html" },
        { name: "cppast.PostfixDecrementNode", id: "diagram_node_16", level: 2, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_PostfixDecrementNode_03356E4B45819451B7CDC424D624063714D0706D.html" },
        { name: "cppast.PostfixIncrementNode", id: "diagram_node_17", level: 2, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_PostfixIncrementNode_06C84C37D44872BBBBF3308C782F703D719F7C2B.html" },
        { name: "cppast.PrefixDecrementNode", id: "diagram_node_18", level: 2, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_PrefixDecrementNode_016DD9D7F75648E2683AD949993DF7DA4F09C9A9.html" },
        { name: "cppast.PrefixIncrementNode", id: "diagram_node_19", level: 2, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_PrefixIncrementNode_B6F294A87BD4705728A6A088412E80F4238D109C.html" },
        { name: "cppast.RvalueRefNode", id: "diagram_node_20", level: 2, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_RvalueRefNode_AD1D00E93B452D1E6D85932E77CBA07BDA466B3D.html" },
        { name: "cppast.SizeOfExpressionNode", id: "diagram_node_21", level: 2, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_SizeOfExpressionNode_B4CF2B6552CBBF91D88AAA311E05690760B067FA.html" },
        { name: "cppast.ThrowExpressionNode", id: "diagram_node_22", level: 2, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_ThrowExpressionNode_15059D2ABCF6E3DD83CDD46D46062403F06E7458.html" },
        { name: "cppast.TypeIdNode", id: "diagram_node_8", level: 2, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_TypeIdNode_80688BF18DBCFACC81D07AE3747161005139D734.html" },
        { name: "cppast.UnaryExpressionNode", id: "diagram_node_23", level: 2, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_UnaryExpressionNode_F8FC23EE14CB6BB47D36E947DC8855822E0F65D4.html" },
        { name: "cppast.UsingDeclarationNode", id: "diagram_node_24", level: 2, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_UsingDeclarationNode_7EC73D6D6961D14B8972DFC662C9976BE6BF7F0D.html" },
        { name: "cppast.UsingDirectiveNode", id: "diagram_node_4", level: 2, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_UsingDirectiveNode_DD0F8D621426FC0F95E24209369EDD7363BE13FB.html" },
        { name: "cppast.VolatileNode", id: "diagram_node_25", level: 2, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_VolatileNode_7C2E24A35B18612734B2A20907C9E3C28D87A96A.html" }];
    drawDiagram(diagramNodes);
}

