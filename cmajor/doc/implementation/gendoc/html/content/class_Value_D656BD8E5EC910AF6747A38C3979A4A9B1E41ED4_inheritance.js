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
        { name: "cmajor::symbols::Value", id: "class_0", level: 0, subject: true, hasDerivedClasses: true, link: "class_Value_D656BD8E5EC910AF6747A38C3979A4A9B1E41ED4.html" },
        { name: "cmajor::binder::ArrayReferenceValue", id: "class_1", level: 1, subject: false, hasDerivedClasses: false, link: "class_ArrayReferenceValue_0A150F6A7CB7CE4FD850D9B8333B26C2CCD935DD.html" },
        { name: "cmajor::symbols::ArrayValue", id: "class_2", level: 1, subject: false, hasDerivedClasses: false, link: "class_ArrayValue_173A9F89EFAEFF4D286BAFE15D981E0AC178597A.html" },
        { name: "cmajor::symbols::BoolValue", id: "class_3", level: 1, subject: false, hasDerivedClasses: false, link: "class_BoolValue_34FB4C955731A946B618B4AC8909FE8833ECAA6E.html" },
        { name: "cmajor::symbols::ByteValue", id: "class_4", level: 1, subject: false, hasDerivedClasses: false, link: "class_ByteValue_7D97881E4356D73208191D03E492FCD5C8B15F72.html" },
        { name: "cmajor::symbols::CharValue", id: "class_5", level: 1, subject: false, hasDerivedClasses: false, link: "class_CharValue_A2EBC5C85E91182D5D56754965416472645563C5.html" },
        { name: "cmajor::symbols::DoubleValue", id: "class_6", level: 1, subject: false, hasDerivedClasses: false, link: "class_DoubleValue_B7EB5CADA34D2F74456A3F4DDB3ECDCEACAF9A4F.html" },
        { name: "cmajor::symbols::FloatValue", id: "class_7", level: 1, subject: false, hasDerivedClasses: false, link: "class_FloatValue_6C33619472BCB9F3608014CC4B34FECC7CC795F6.html" },
        { name: "cmajor::binder::FunctionGroupValue", id: "class_8", level: 1, subject: false, hasDerivedClasses: false, link: "class_FunctionGroupValue_6118C489B10733D37B8B242F7E1A2FA0261E897B.html" },
        { name: "cmajor::symbols::IntValue", id: "class_9", level: 1, subject: false, hasDerivedClasses: false, link: "class_IntValue_504806ADDB04FAB4F5CA07058ADD14C5B7BB5226.html" },
        { name: "cmajor::symbols::LongValue", id: "class_10", level: 1, subject: false, hasDerivedClasses: false, link: "class_LongValue_1BD265CB5802007DF44E57306A8CE9482A5A8B33.html" },
        { name: "cmajor::symbols::NullValue", id: "class_11", level: 1, subject: false, hasDerivedClasses: false, link: "class_NullValue_980FA2272F530AA5296339D7B50E11353EE06D59.html" },
        { name: "cmajor::symbols::PointerValue", id: "class_12", level: 1, subject: false, hasDerivedClasses: false, link: "class_PointerValue_46AB68C9010A90C77167A957C63D66E17B8B9877.html" },
        { name: "cmajor::symbols::SByteValue", id: "class_13", level: 1, subject: false, hasDerivedClasses: false, link: "class_SByteValue_3C3EAF8E7CE424628F5D4890B22AC65A8BA88E29.html" },
        { name: "cmajor::binder::ScopedValue", id: "class_14", level: 1, subject: false, hasDerivedClasses: false, link: "class_ScopedValue_2C3ED81E6D73F2BE376CBEB734E95048BE78A48C.html" },
        { name: "cmajor::symbols::ShortValue", id: "class_15", level: 1, subject: false, hasDerivedClasses: false, link: "class_ShortValue_112ED573381608CB01C4659D171C7E96C35A044C.html" },
        { name: "cmajor::symbols::StringValue", id: "class_16", level: 1, subject: false, hasDerivedClasses: false, link: "class_StringValue_77F3582D5FB82BFD89712FF9BB37963ECE91973C.html" },
        { name: "cmajor::binder::StructuredReferenceValue", id: "class_17", level: 1, subject: false, hasDerivedClasses: false, link: "class_StructuredReferenceValue_1B156C5E52F7BF2E393459D37558362AEB307353.html" },
        { name: "cmajor::symbols::StructuredValue", id: "class_18", level: 1, subject: false, hasDerivedClasses: false, link: "class_StructuredValue_B2E6027F8490C6C3257E3986BF5B5EA7B7C7AF58.html" },
        { name: "cmajor::symbols::UCharValue", id: "class_19", level: 1, subject: false, hasDerivedClasses: false, link: "class_UCharValue_E02DF40BE40FB64E9A2D586E5B4AD88330C14DB4.html" },
        { name: "cmajor::symbols::UIntValue", id: "class_20", level: 1, subject: false, hasDerivedClasses: false, link: "class_UIntValue_48F155B503D9F354BD257AD831B2B69D2FBA1438.html" },
        { name: "cmajor::symbols::ULongValue", id: "class_21", level: 1, subject: false, hasDerivedClasses: false, link: "class_ULongValue_985C867DFFEE6DF3901A8074F6E0C0271BFD0427.html" },
        { name: "cmajor::symbols::UShortValue", id: "class_22", level: 1, subject: false, hasDerivedClasses: false, link: "class_UShortValue_9D6DCA00DA10F2F909978AC7DC1982CED1A7A6DF.html" },
        { name: "cmajor::symbols::UStringValue", id: "class_23", level: 1, subject: false, hasDerivedClasses: false, link: "class_UStringValue_5A5CEE9F935685402F095DC379B93904DC096386.html" },
        { name: "cmajor::symbols::UuidValue", id: "class_24", level: 1, subject: false, hasDerivedClasses: false, link: "class_UuidValue_5583CF4406966E4F279FDA623FD9CD8984C3D97A.html" },
        { name: "cmajor::symbols::WCharValue", id: "class_25", level: 1, subject: false, hasDerivedClasses: false, link: "class_WCharValue_04AB9642246F31FF2C443849F5BF383BCBC7936E.html" },
        { name: "cmajor::symbols::WStringValue", id: "class_26", level: 1, subject: false, hasDerivedClasses: false, link: "class_WStringValue_B8FBEDF5FCF8A938F11E2EC9AE07D9E9C5EB0A44.html" }];
    drawDiagram(classes);
}

