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
        { name: "cmajor::ast::StatementNode", id: "class_1", level: 1, subject: true, hasDerivedClasses: true, link: "class_StatementNode_5B30CD05CFBD6FC4DC65F9EB63116552354DEF08.html" },
        { name: "cmajor::ast::AssertStatementNode", id: "class_2", level: 2, subject: false, hasDerivedClasses: false, link: "class_AssertStatementNode_2B50BD335E2B4B0D3372FFD214D3E8437F53544E.html" },
        { name: "cmajor::ast::AssignmentStatementNode", id: "class_3", level: 2, subject: false, hasDerivedClasses: false, link: "class_AssignmentStatementNode_EEC3DC79A1FC1DA85D237B860CE62B6392E5B0CB.html" },
        { name: "cmajor::ast::BreakStatementNode", id: "class_4", level: 2, subject: false, hasDerivedClasses: false, link: "class_BreakStatementNode_D25E92C72F645E3C6E7504D88E9BBF157984DDE0.html" },
        { name: "cmajor::ast::CaseStatementNode", id: "class_5", level: 2, subject: false, hasDerivedClasses: false, link: "class_CaseStatementNode_A2762FBF140AE64FBE72272559EA28A8791CD772.html" },
        { name: "cmajor::ast::CompoundStatementNode", id: "class_6", level: 2, subject: false, hasDerivedClasses: false, link: "class_CompoundStatementNode_0696D97F97A719021BDDF41E41B940C1B5B03070.html" },
        { name: "cmajor::ast::ConditionalCompilationStatementNode", id: "class_7", level: 2, subject: false, hasDerivedClasses: false, link: "class_ConditionalCompilationStatementNode_9D6E094A649C86A6CB3D822043E0308349E0982F.html" },
        { name: "cmajor::ast::ConstructionStatementNode", id: "class_8", level: 2, subject: false, hasDerivedClasses: false, link: "class_ConstructionStatementNode_F85CF7259E915C8CA4094ADAFEDB2BE0E6FF81AD.html" },
        { name: "cmajor::ast::ContinueStatementNode", id: "class_9", level: 2, subject: false, hasDerivedClasses: false, link: "class_ContinueStatementNode_1D07468CCF3BA26F71F1C6FF4AD987D8A5B5EB3E.html" },
        { name: "cmajor::ast::DefaultStatementNode", id: "class_10", level: 2, subject: false, hasDerivedClasses: false, link: "class_DefaultStatementNode_1D5415B64C2A9A56EC7013C40FFA0504C42C66DF.html" },
        { name: "cmajor::ast::DeleteStatementNode", id: "class_11", level: 2, subject: false, hasDerivedClasses: false, link: "class_DeleteStatementNode_853D6F573E987786D255C7BB9790A77A5D3CB76F.html" },
        { name: "cmajor::ast::DestroyStatementNode", id: "class_12", level: 2, subject: false, hasDerivedClasses: false, link: "class_DestroyStatementNode_E6D8EA0DD75C92CCBCC2A3524395528BC756E059.html" },
        { name: "cmajor::ast::DoStatementNode", id: "class_13", level: 2, subject: false, hasDerivedClasses: false, link: "class_DoStatementNode_66BD6F34717045DCDD27CB6479E397E7860B40C7.html" },
        { name: "cmajor::ast::EmptyStatementNode", id: "class_14", level: 2, subject: false, hasDerivedClasses: false, link: "class_EmptyStatementNode_FEC7B4F234FD613B891D150B3E2FCE69CAAA56AB.html" },
        { name: "cmajor::ast::ExpressionStatementNode", id: "class_15", level: 2, subject: false, hasDerivedClasses: false, link: "class_ExpressionStatementNode_22F62CCFF31C0C80014243627F4B1EA0D7DFA78B.html" },
        { name: "cmajor::ast::ForStatementNode", id: "class_16", level: 2, subject: false, hasDerivedClasses: false, link: "class_ForStatementNode_D09288619FB91C32246D0AC86C0333311B7DBD31.html" },
        { name: "cmajor::ast::GotoCaseStatementNode", id: "class_17", level: 2, subject: false, hasDerivedClasses: false, link: "class_GotoCaseStatementNode_9C735CFB577BDECCD2BDF19EDE5899B4C9D56BDB.html" },
        { name: "cmajor::ast::GotoDefaultStatementNode", id: "class_18", level: 2, subject: false, hasDerivedClasses: false, link: "class_GotoDefaultStatementNode_FDDBFA68154DF076FDA16C1BBF16250487B4ECC0.html" },
        { name: "cmajor::ast::GotoStatementNode", id: "class_19", level: 2, subject: false, hasDerivedClasses: false, link: "class_GotoStatementNode_418986D20C07E7C37186E2FE5413698022923041.html" },
        { name: "cmajor::ast::IfStatementNode", id: "class_20", level: 2, subject: false, hasDerivedClasses: false, link: "class_IfStatementNode_598C07D356B27008EF7BC51C0329C91D11B53C10.html" },
        { name: "cmajor::ast::RangeForStatementNode", id: "class_21", level: 2, subject: false, hasDerivedClasses: false, link: "class_RangeForStatementNode_EFD048A5EBA76CEC9675351105634FBDC66A8EE4.html" },
        { name: "cmajor::ast::ReturnStatementNode", id: "class_22", level: 2, subject: false, hasDerivedClasses: false, link: "class_ReturnStatementNode_67414E5E1EAA6260ED935769CD7451BE06F9E900.html" },
        { name: "cmajor::ast::SwitchStatementNode", id: "class_23", level: 2, subject: false, hasDerivedClasses: false, link: "class_SwitchStatementNode_3813900B6711608EFACDF68C5C89C3926A654C68.html" },
        { name: "cmajor::ast::ThrowStatementNode", id: "class_24", level: 2, subject: false, hasDerivedClasses: false, link: "class_ThrowStatementNode_C5D2723F1E0788C81B9118AA6E7C0A7C99986446.html" },
        { name: "cmajor::ast::TryStatementNode", id: "class_25", level: 2, subject: false, hasDerivedClasses: false, link: "class_TryStatementNode_9AB1331524BD05CEE7E8AAAE7C681E8FF8053D53.html" },
        { name: "cmajor::ast::WhileStatementNode", id: "class_26", level: 2, subject: false, hasDerivedClasses: false, link: "class_WhileStatementNode_55EE576177721DDB89FC3888605492A4C2BA301F.html" }];
    drawDiagram(classes);
}

