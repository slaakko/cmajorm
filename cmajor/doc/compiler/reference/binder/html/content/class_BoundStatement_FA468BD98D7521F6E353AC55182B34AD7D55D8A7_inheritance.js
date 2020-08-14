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
        { name: "cmajor::ir::GenObject", id: "class_0", level: 0, subject: false, hasDerivedClasses: true, link: "./../../../ir/html/content/class_GenObject_58B5CE43FA7DE3F0DCCA4882DA8ADB648134410A.html" },
        { name: "cmajor::binder::BoundNode", id: "class_1", level: 1, subject: false, hasDerivedClasses: true, link: "./class_BoundNode_0DE6B6FC2B245301A2A10752E494079DE5D7401C.html" },
        { name: "cmajor::binder::BoundStatement", id: "class_2", level: 2, subject: true, hasDerivedClasses: true, link: "./class_BoundStatement_FA468BD98D7521F6E353AC55182B34AD7D55D8A7.html" },
        { name: "cmajor::binder::BoundAssignmentStatement", id: "class_3", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundAssignmentStatement_CF313984317B4F63ECBE67C3D703FEA4FDABA19A.html" },
        { name: "cmajor::binder::BoundBreakStatement", id: "class_4", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundBreakStatement_670BE810426808D6F8C26AA7B57F21BCC700B015.html" },
        { name: "cmajor::binder::BoundCaseStatement", id: "class_5", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundCaseStatement_4FEE6EA6482A8618DE2491D7D5E742F76FE4FA80.html" },
        { name: "cmajor::binder::BoundCatchStatement", id: "class_6", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundCatchStatement_CAC29640E4061B89F7B5C25BB28F2ABE3A16F627.html" },
        { name: "cmajor::binder::BoundCompoundStatement", id: "class_7", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundCompoundStatement_83075A2B16EF7318603436483D5B262D74A60859.html" },
        { name: "cmajor::binder::BoundConstructionStatement", id: "class_8", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundConstructionStatement_5A16B0CF5576EDDD07B3D16B8F54CBB6425CEC62.html" },
        { name: "cmajor::binder::BoundContinueStatement", id: "class_9", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundContinueStatement_979CE58FF83A9ED652425067EBFA731BBBE5384B.html" },
        { name: "cmajor::binder::BoundDefaultStatement", id: "class_10", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundDefaultStatement_7964CD8CACCBFB99CF250DDCD68A65146674800C.html" },
        { name: "cmajor::binder::BoundDoStatement", id: "class_11", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundDoStatement_905D756A814EB32AC0FFC221E021FA134EAFCEB4.html" },
        { name: "cmajor::binder::BoundEmptyStatement", id: "class_12", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundEmptyStatement_D60FE05E65E498F3A5E201D80539D95E6AA0BF5B.html" },
        { name: "cmajor::binder::BoundExpressionStatement", id: "class_13", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundExpressionStatement_FC39663CCD7EF04AA66CB2F57D05425C4BC6FA09.html" },
        { name: "cmajor::binder::BoundForStatement", id: "class_14", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundForStatement_B24BA4CF56989FCBB6AEB95A250EDF411B4215B2.html" },
        { name: "cmajor::binder::BoundGotoCaseStatement", id: "class_15", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundGotoCaseStatement_866A1AC6960DACD576576E5D25C9BE7AF42DC804.html" },
        { name: "cmajor::binder::BoundGotoDefaultStatement", id: "class_16", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundGotoDefaultStatement_8AF1EB875ADD6C393850B230CFC5FD003F737241.html" },
        { name: "cmajor::binder::BoundGotoStatement", id: "class_17", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundGotoStatement_FCFCCD2D9BADF935C7D7DE2684A43343B1AAD43C.html" },
        { name: "cmajor::binder::BoundIfStatement", id: "class_18", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundIfStatement_2EB35965234D473DF739E33B390A3950C2405080.html" },
        { name: "cmajor::binder::BoundInitializationStatement", id: "class_19", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundInitializationStatement_EDA41FE9A17314981F0AA0CC25E5F38E152AB506.html" },
        { name: "cmajor::binder::BoundRethrowStatement", id: "class_20", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundRethrowStatement_A720CEC1C74090443C19A23C0876F5A5D62007AA.html" },
        { name: "cmajor::binder::BoundReturnStatement", id: "class_21", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundReturnStatement_D998176947E97E412567764578B6117F430440D4.html" },
        { name: "cmajor::binder::BoundSequenceStatement", id: "class_22", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundSequenceStatement_7224125F0C7751D0B97CE7F7037F4D2655824F59.html" },
        { name: "cmajor::binder::BoundSetVmtPtrStatement", id: "class_23", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundSetVmtPtrStatement_D2863C630BC058CCC2EF18A5F41A45BCC328B025.html" },
        { name: "cmajor::binder::BoundSwitchStatement", id: "class_24", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundSwitchStatement_995D5CB824F96656D94DE278A00F91286CD6598F.html" },
        { name: "cmajor::binder::BoundThrowStatement", id: "class_25", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundThrowStatement_EC217A057808FE9076D0AB267E6FBFB3354ECCF2.html" },
        { name: "cmajor::binder::BoundTryStatement", id: "class_26", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundTryStatement_A14ED2DFB95B82CB86B48EC0AEA2D7C3D0757D10.html" },
        { name: "cmajor::binder::BoundWhileStatement", id: "class_27", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundWhileStatement_34710CA10F2EDB4A9B691B578C1B3F261E4C79FD.html" }];
    drawDiagram(classes);
}

