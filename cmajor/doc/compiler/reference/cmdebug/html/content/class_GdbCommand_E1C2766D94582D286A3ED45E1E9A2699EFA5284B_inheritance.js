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
        { name: "cmajor::debug::GdbCommand", id: "class_0", level: 0, subject: true, hasDerivedClasses: true, link: "./class_GdbCommand_E1C2766D94582D286A3ED45E1E9A2699EFA5284B.html" },
        { name: "cmajor::debug::GdbBreakDeleteCommand", id: "class_1", level: 1, subject: false, hasDerivedClasses: false, link: "./class_GdbBreakDeleteCommand_6410447CA8165BE280CD9694D14CB0AD87244500.html" },
        { name: "cmajor::debug::GdbBreakInsertCommand", id: "class_2", level: 1, subject: false, hasDerivedClasses: false, link: "./class_GdbBreakInsertCommand_E8BEDE741184E7E8DFED8B7ECB872C34A173BBC9.html" },
        { name: "cmajor::debug::GdbDeleteAllBreakpointsCommand", id: "class_3", level: 1, subject: false, hasDerivedClasses: false, link: "./class_GdbDeleteAllBreakpointsCommand_0937FB91DA919F00277ED58C3879C5038156D8A8.html" },
        { name: "cmajor::debug::GdbExecContinueCommand", id: "class_4", level: 1, subject: false, hasDerivedClasses: false, link: "./class_GdbExecContinueCommand_DFFC95D76ACBEDA65557A171C199E5A67EF1C16D.html" },
        { name: "cmajor::debug::GdbExecFinishCommand", id: "class_5", level: 1, subject: false, hasDerivedClasses: false, link: "./class_GdbExecFinishCommand_643BB4EFA709BD8D2C58A336B59F8406EDB98DE6.html" },
        { name: "cmajor::debug::GdbExecNextCommand", id: "class_6", level: 1, subject: false, hasDerivedClasses: false, link: "./class_GdbExecNextCommand_A652C08E3B2B1D3DF835A57CCB790748A12671A5.html" },
        { name: "cmajor::debug::GdbExecRunCommand", id: "class_7", level: 1, subject: false, hasDerivedClasses: false, link: "./class_GdbExecRunCommand_AB47500E7A41A732E4A3493812DB3E50F23FF3FE.html" },
        { name: "cmajor::debug::GdbExecStepCommand", id: "class_8", level: 1, subject: false, hasDerivedClasses: false, link: "./class_GdbExecStepCommand_50A56813B8C838FBA0E68E657780E92BA2EB1C96.html" },
        { name: "cmajor::debug::GdbExecUntilCommand", id: "class_9", level: 1, subject: false, hasDerivedClasses: false, link: "./class_GdbExecUntilCommand_6BA1EC4E1300A77E0349B73C3C6D32E545BFCCE4.html" },
        { name: "cmajor::debug::GdbExitCommand", id: "class_10", level: 1, subject: false, hasDerivedClasses: false, link: "./class_GdbExitCommand_62A504494A9F606D1875CB73A7385D7B0C9A7130.html" },
        { name: "cmajor::debug::GdbPrintCommand", id: "class_11", level: 1, subject: false, hasDerivedClasses: false, link: "./class_GdbPrintCommand_DA4E7CC630A5DAEAA69D30EA710227D9E691BEA0.html" },
        { name: "cmajor::debug::GdbStackInfoDepthCommand", id: "class_12", level: 1, subject: false, hasDerivedClasses: false, link: "./class_GdbStackInfoDepthCommand_2FB01D9E44094DE990CBB477FA9E6436AE8EEE86.html" },
        { name: "cmajor::debug::GdbStackListFramesCommand", id: "class_13", level: 1, subject: false, hasDerivedClasses: false, link: "./class_GdbStackListFramesCommand_ECF037D9734B5237CD8A5D775E565C0D4DC97A63.html" },
        { name: "cmajor::debug::GdbVarAssignCommand", id: "class_14", level: 1, subject: false, hasDerivedClasses: false, link: "./class_GdbVarAssignCommand_241C8294A8FE17947340FE848272D1B138FC9002.html" },
        { name: "cmajor::debug::GdbVarCreateCommand", id: "class_15", level: 1, subject: false, hasDerivedClasses: false, link: "./class_GdbVarCreateCommand_E5E81C566823A74CBCCC8882B7761402C5CD94D1.html" },
        { name: "cmajor::debug::GdbVarDeleteCommand", id: "class_16", level: 1, subject: false, hasDerivedClasses: false, link: "./class_GdbVarDeleteCommand_1A46592C8C49C16C892FC166F62D2D467BB86F19.html" },
        { name: "cmajor::debug::GdbVarEvaluateExpressionCommand", id: "class_17", level: 1, subject: false, hasDerivedClasses: false, link: "./class_GdbVarEvaluateExpressionCommand_B714220170D69E335B625B67CA36D7F74065DE3F.html" },
        { name: "cmajor::debug::GdbVarInfoExpressionCommand", id: "class_18", level: 1, subject: false, hasDerivedClasses: false, link: "./class_GdbVarInfoExpressionCommand_174D7ADFE527980C996F1B34B0CF1AC93537A90C.html" },
        { name: "cmajor::debug::GdbVarInfoNumChildrenCommand", id: "class_19", level: 1, subject: false, hasDerivedClasses: false, link: "./class_GdbVarInfoNumChildrenCommand_ABF541E7BDB37ED9F1E9B5932F0C507E3A6F0AD7.html" },
        { name: "cmajor::debug::GdbVarInfoPathExpressionCommand", id: "class_20", level: 1, subject: false, hasDerivedClasses: false, link: "./class_GdbVarInfoPathExpressionCommand_B7C15F4E92756FCE280D6AFF04BCA9320404BC8D.html" },
        { name: "cmajor::debug::GdbVarInfoTypeCommand", id: "class_21", level: 1, subject: false, hasDerivedClasses: false, link: "./class_GdbVarInfoTypeCommand_46FF35507A6821516AC2EF4959B2FE647A2C0E01.html" },
        { name: "cmajor::debug::GdbVarListChildrenCommand", id: "class_22", level: 1, subject: false, hasDerivedClasses: false, link: "./class_GdbVarListChildrenCommand_6DAEBB2C725FEB44DA98F1A76CCA4A1B51088ED2.html" },
        { name: "cmajor::debug::GdbVarSetFormatCommand", id: "class_23", level: 1, subject: false, hasDerivedClasses: false, link: "./class_GdbVarSetFormatCommand_87AC7C0B0275F4B73F8642FAE1126CC21B9EBFE2.html" },
        { name: "cmajor::debug::GdbVarSetFrozenCommand", id: "class_24", level: 1, subject: false, hasDerivedClasses: false, link: "./class_GdbVarSetFrozenCommand_060AD6B765EEBA79A1FC0E119D380A73AD8C53D6.html" },
        { name: "cmajor::debug::GdbVarSetUpdateRangeCommand", id: "class_25", level: 1, subject: false, hasDerivedClasses: false, link: "./class_GdbVarSetUpdateRangeCommand_E8749D4D52AED076AF6E2AAD3DE4AB845BD5D6E7.html" },
        { name: "cmajor::debug::GdbVarSetVisualizerCommand", id: "class_26", level: 1, subject: false, hasDerivedClasses: false, link: "./class_GdbVarSetVisualizerCommand_937F07A406162485E75A743ECE4FAE9904A696FF.html" },
        { name: "cmajor::debug::GdbVarShowAttributesCommand", id: "class_27", level: 1, subject: false, hasDerivedClasses: false, link: "./class_GdbVarShowAttributesCommand_C5E51BFDE912D519947BE85FD556619FCCBA4519.html" },
        { name: "cmajor::debug::GdbVarShowFormatCommand", id: "class_28", level: 1, subject: false, hasDerivedClasses: false, link: "./class_GdbVarShowFormatCommand_BBC3F647B9F7EDB012577691D74FC781623F0E4B.html" },
        { name: "cmajor::debug::GdbVarUpdateCommand", id: "class_29", level: 1, subject: false, hasDerivedClasses: false, link: "./class_GdbVarUpdateCommand_F8E1BF748754774DCE8ED11B3BE0A0198AA5FF60.html" }];
    drawDiagram(classes);
}

