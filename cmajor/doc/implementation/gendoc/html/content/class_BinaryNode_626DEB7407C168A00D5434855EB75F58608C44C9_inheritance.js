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
        { name: "cmajor::ast::Node", id: "class_0", level: 0, subject: false, hasDerivedClasses: true, link: "class_Node_66FC1D6AD8F487E453CE6E17163479C7C2DAC063.html" },
        { name: "cmajor::ast::BinaryNode", id: "class_1", level: 1, subject: true, hasDerivedClasses: true, link: "class_BinaryNode_626DEB7407C168A00D5434855EB75F58608C44C9.html" },
        { name: "cmajor::ast::AddNode", id: "class_2", level: 2, subject: false, hasDerivedClasses: false, link: "class_AddNode_B35243B9C3706BC1922876F148CD4774EB8560BB.html" },
        { name: "cmajor::ast::BitAndNode", id: "class_3", level: 2, subject: false, hasDerivedClasses: false, link: "class_BitAndNode_8866E0D20F8CBA0D50CA2C34B01BFB3A0C6B8C31.html" },
        { name: "cmajor::ast::BitOrNode", id: "class_4", level: 2, subject: false, hasDerivedClasses: false, link: "class_BitOrNode_4A1D1A38D833037597F9DC366DF8A9A70699D1A1.html" },
        { name: "cmajor::ast::BitXorNode", id: "class_5", level: 2, subject: false, hasDerivedClasses: false, link: "class_BitXorNode_0B30020E3526A78441B4855EA9CCC3D14692CCC7.html" },
        { name: "cmajor::ast::ConjunctionNode", id: "class_6", level: 2, subject: false, hasDerivedClasses: false, link: "class_ConjunctionNode_FCAEBB2CB8C567B27F8922CFB5F496977C3B8AAF.html" },
        { name: "cmajor::ast::DisjunctionNode", id: "class_7", level: 2, subject: false, hasDerivedClasses: false, link: "class_DisjunctionNode_29948A0AC945F87498F3BC1B149A8B9A448C28FF.html" },
        { name: "cmajor::ast::DivNode", id: "class_8", level: 2, subject: false, hasDerivedClasses: false, link: "class_DivNode_0126E647ED9F78D277A261F1991D0981314DF5AF.html" },
        { name: "cmajor::ast::EqualNode", id: "class_9", level: 2, subject: false, hasDerivedClasses: false, link: "class_EqualNode_2EAAC57CAF20C5A5E80F276F6021DE5404CC7022.html" },
        { name: "cmajor::ast::EquivalenceNode", id: "class_10", level: 2, subject: false, hasDerivedClasses: false, link: "class_EquivalenceNode_2A966D87C0CFD5EEC6E8376AF2572607FED4A84A.html" },
        { name: "cmajor::ast::GreaterNode", id: "class_11", level: 2, subject: false, hasDerivedClasses: false, link: "class_GreaterNode_1E6556E7B39C1E14F9FA5A5BEA422D1A74D5AFCD.html" },
        { name: "cmajor::ast::GreaterOrEqualNode", id: "class_12", level: 2, subject: false, hasDerivedClasses: false, link: "class_GreaterOrEqualNode_818C74EE7DC66DF5FCB4A79DEB0F38018B391237.html" },
        { name: "cmajor::ast::ImplicationNode", id: "class_13", level: 2, subject: false, hasDerivedClasses: false, link: "class_ImplicationNode_BA3FCBA692C7C7A4BA235C32F0AB07F91ACB6266.html" },
        { name: "cmajor::ast::LessNode", id: "class_14", level: 2, subject: false, hasDerivedClasses: false, link: "class_LessNode_6DA8784EA2F38855C3E3A6E06FE035E8E1C0E1AE.html" },
        { name: "cmajor::ast::LessOrEqualNode", id: "class_15", level: 2, subject: false, hasDerivedClasses: false, link: "class_LessOrEqualNode_1AC7E7607C5B5A7CD8F592503E97402E3C086C3F.html" },
        { name: "cmajor::ast::MulNode", id: "class_16", level: 2, subject: false, hasDerivedClasses: false, link: "class_MulNode_11325F5427517086DF14F1B757124EC13EADB182.html" },
        { name: "cmajor::ast::NotEqualNode", id: "class_17", level: 2, subject: false, hasDerivedClasses: false, link: "class_NotEqualNode_332F3FA54C5FC4DE5F1DF8CFAF05B476F6AE5F23.html" },
        { name: "cmajor::ast::RemNode", id: "class_18", level: 2, subject: false, hasDerivedClasses: false, link: "class_RemNode_ABE00C6686F0E7E1C5AEB13F920DAD58F75E76A3.html" },
        { name: "cmajor::ast::ShiftLeftNode", id: "class_19", level: 2, subject: false, hasDerivedClasses: false, link: "class_ShiftLeftNode_328513E2E67E505A903C773017F4DCBF0286A641.html" },
        { name: "cmajor::ast::ShiftRightNode", id: "class_20", level: 2, subject: false, hasDerivedClasses: false, link: "class_ShiftRightNode_C068E65F1319C4D719575D2D87D892DEF681703C.html" },
        { name: "cmajor::ast::SubNode", id: "class_21", level: 2, subject: false, hasDerivedClasses: false, link: "class_SubNode_14D10CD04462FFD246B1C97D5EF3CE7D49CA58EC.html" }];
    drawDiagram(classes);
}

