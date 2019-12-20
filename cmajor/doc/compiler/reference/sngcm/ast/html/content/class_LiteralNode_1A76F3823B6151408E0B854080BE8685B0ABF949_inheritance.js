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
        { name: "sngcm::ast::Node", id: "class_0", level: 0, subject: false, hasDerivedClasses: true, link: "./class_Node_7FDC87EBEBE372FAA4360E17F122F6028A1505D6.html" },
        { name: "sngcm::ast::LiteralNode", id: "class_1", level: 1, subject: true, hasDerivedClasses: true, link: "./class_LiteralNode_1A76F3823B6151408E0B854080BE8685B0ABF949.html" },
        { name: "sngcm::ast::ArrayLiteralNode", id: "class_2", level: 2, subject: false, hasDerivedClasses: false, link: "./class_ArrayLiteralNode_988557EE6037BCFDDA4392AE4C66A2D92D66D766.html" },
        { name: "sngcm::ast::BooleanLiteralNode", id: "class_3", level: 2, subject: false, hasDerivedClasses: false, link: "./class_BooleanLiteralNode_BA061A8FA86B8477955DDB406C513FDEBAF71B8D.html" },
        { name: "sngcm::ast::ByteLiteralNode", id: "class_4", level: 2, subject: false, hasDerivedClasses: false, link: "./class_ByteLiteralNode_B68F4970B8830DF04BE378616376125C60867498.html" },
        { name: "sngcm::ast::CharLiteralNode", id: "class_5", level: 2, subject: false, hasDerivedClasses: false, link: "./class_CharLiteralNode_79B44AD52D16F8D05FD9B71E97FD23B00AE977DD.html" },
        { name: "sngcm::ast::DoubleLiteralNode", id: "class_6", level: 2, subject: false, hasDerivedClasses: false, link: "./class_DoubleLiteralNode_D5CFE87D7CDB5EFED50331560B07FAC89521AC76.html" },
        { name: "sngcm::ast::FloatLiteralNode", id: "class_7", level: 2, subject: false, hasDerivedClasses: false, link: "./class_FloatLiteralNode_A99C6780902CF5D89BDC533130920183DAC76E50.html" },
        { name: "sngcm::ast::IntLiteralNode", id: "class_8", level: 2, subject: false, hasDerivedClasses: false, link: "./class_IntLiteralNode_3635610F208BAFA0526F4FF2BE23BE73D1D83FA2.html" },
        { name: "sngcm::ast::LongLiteralNode", id: "class_9", level: 2, subject: false, hasDerivedClasses: false, link: "./class_LongLiteralNode_3631CA81B20EC1F466F904BA69C0152A5E8D72CA.html" },
        { name: "sngcm::ast::NullLiteralNode", id: "class_10", level: 2, subject: false, hasDerivedClasses: false, link: "./class_NullLiteralNode_D3A1DD6B6CE29BA06293220DCDD0B127D6191A14.html" },
        { name: "sngcm::ast::SByteLiteralNode", id: "class_11", level: 2, subject: false, hasDerivedClasses: false, link: "./class_SByteLiteralNode_B9A5B0BDC2CECE80F25781D817A1F9ABFE6B1EF8.html" },
        { name: "sngcm::ast::ShortLiteralNode", id: "class_12", level: 2, subject: false, hasDerivedClasses: false, link: "./class_ShortLiteralNode_FDD4EC0716C295E29B28EAF1817D43C70B584E45.html" },
        { name: "sngcm::ast::StringLiteralNode", id: "class_13", level: 2, subject: false, hasDerivedClasses: false, link: "./class_StringLiteralNode_5F198EC06D6C70B076C7C0ADA756C11B8829FE31.html" },
        { name: "sngcm::ast::StructuredLiteralNode", id: "class_14", level: 2, subject: false, hasDerivedClasses: false, link: "./class_StructuredLiteralNode_21B802C8332CD70644D47102943C2D80F541E08E.html" },
        { name: "sngcm::ast::UCharLiteralNode", id: "class_15", level: 2, subject: false, hasDerivedClasses: false, link: "./class_UCharLiteralNode_E151461595DEB25D751F273215D1AC16537CA9B7.html" },
        { name: "sngcm::ast::UIntLiteralNode", id: "class_16", level: 2, subject: false, hasDerivedClasses: false, link: "./class_UIntLiteralNode_88EDCED33D52CC13D1BB4A572BDF21A9F50EB84A.html" },
        { name: "sngcm::ast::ULongLiteralNode", id: "class_17", level: 2, subject: false, hasDerivedClasses: false, link: "./class_ULongLiteralNode_C91A45D13051DE8A3A4F305C3101B2EB146F3083.html" },
        { name: "sngcm::ast::UShortLiteralNode", id: "class_18", level: 2, subject: false, hasDerivedClasses: false, link: "./class_UShortLiteralNode_A487D4B53D677B73E960D9C1B0663C6F57F80AC5.html" },
        { name: "sngcm::ast::UStringLiteralNode", id: "class_19", level: 2, subject: false, hasDerivedClasses: false, link: "./class_UStringLiteralNode_F80FD7C129A86274FD094A4207E6EAD98AC199A2.html" },
        { name: "sngcm::ast::UuidLiteralNode", id: "class_20", level: 2, subject: false, hasDerivedClasses: false, link: "./class_UuidLiteralNode_8A54D3E9EAC6499E12B4414060F3905DA1361C31.html" },
        { name: "sngcm::ast::WCharLiteralNode", id: "class_21", level: 2, subject: false, hasDerivedClasses: false, link: "./class_WCharLiteralNode_22E4B9AB53BA2799E64C26A437967AE0A8B19FC1.html" },
        { name: "sngcm::ast::WStringLiteralNode", id: "class_22", level: 2, subject: false, hasDerivedClasses: false, link: "./class_WStringLiteralNode_FE9E70285EDCCBD2AD2B3655AC611319558F65FB.html" }];
    drawDiagram(classes);
}

