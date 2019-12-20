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
        { name: "cmajor::symbols::IntrinsicFunction", id: "class_0", level: 0, subject: false, hasDerivedClasses: true, link: "./class_IntrinsicFunction_F521E1B5E39D27BD0AB8435AD65FF36EA03CF46C.html" },
        { name: "cmajor::symbols::TypePredicate", id: "class_1", level: 1, subject: true, hasDerivedClasses: true, link: "./class_TypePredicate_748B17419691003CC94893EDE3A88321DE325D08.html" },
        { name: "cmajor::symbols::IsArrayTypePredicate", id: "class_2", level: 2, subject: false, hasDerivedClasses: false, link: "./class_IsArrayTypePredicate_D83D57CC5A46F953FC87C1E5775E2EF0AEB79C86.html" },
        { name: "cmajor::symbols::IsBasicTypePredicate", id: "class_3", level: 2, subject: false, hasDerivedClasses: false, link: "./class_IsBasicTypePredicate_405C86501EB896773CDE81CCC707600CFC8B129E.html" },
        { name: "cmajor::symbols::IsBoolTypePredicate", id: "class_4", level: 2, subject: false, hasDerivedClasses: false, link: "./class_IsBoolTypePredicate_0C00E71D4FD47B812FBC9153743A6BC7D6F702B4.html" },
        { name: "cmajor::symbols::IsByteTypePredicate", id: "class_5", level: 2, subject: false, hasDerivedClasses: false, link: "./class_IsByteTypePredicate_041ADCDA4C047E94DFC3ED902BC629B489522AEB.html" },
        { name: "cmajor::symbols::IsCharTypePredicate", id: "class_6", level: 2, subject: false, hasDerivedClasses: false, link: "./class_IsCharTypePredicate_15A4CD0A80985E7CC9010055CA26AE8634DE7CA9.html" },
        { name: "cmajor::symbols::IsClassDelegateTypePredicate", id: "class_7", level: 2, subject: false, hasDerivedClasses: false, link: "./class_IsClassDelegateTypePredicate_1C57B3B2A157B00BB57A19270DD3D28F5B0B498D.html" },
        { name: "cmajor::symbols::IsClassTypePredicate", id: "class_8", level: 2, subject: false, hasDerivedClasses: false, link: "./class_IsClassTypePredicate_C1F12AEC7443D2BC16195CD320ACE092CB86363F.html" },
        { name: "cmajor::symbols::IsConstTypePredicate", id: "class_9", level: 2, subject: false, hasDerivedClasses: false, link: "./class_IsConstTypePredicate_C6215B9C32349BB63B11D27327E61C12ED32FE94.html" },
        { name: "cmajor::symbols::IsDelegateTypePredicate", id: "class_10", level: 2, subject: false, hasDerivedClasses: false, link: "./class_IsDelegateTypePredicate_DFB54E30F0BE81650AA15386F7031D3394AB973A.html" },
        { name: "cmajor::symbols::IsDoubleTypePredicate", id: "class_11", level: 2, subject: false, hasDerivedClasses: false, link: "./class_IsDoubleTypePredicate_F91A6B36CA6907EFE0576916A2CA0DE28072F388.html" },
        { name: "cmajor::symbols::IsEnumeratedTypePredicate", id: "class_12", level: 2, subject: false, hasDerivedClasses: false, link: "./class_IsEnumeratedTypePredicate_2F38DD381A994F7094994556F808E783C8F37869.html" },
        { name: "cmajor::symbols::IsFloatTypePredicate", id: "class_13", level: 2, subject: false, hasDerivedClasses: false, link: "./class_IsFloatTypePredicate_71E0A6371F43D6AAB26AC1E4C2C10A5682A49F54.html" },
        { name: "cmajor::symbols::IsFloatingPointTypePredicate", id: "class_14", level: 2, subject: false, hasDerivedClasses: false, link: "./class_IsFloatingPointTypePredicate_BFE4E63689144E7ED8FBC6369E056D22600BD633.html" },
        { name: "cmajor::symbols::IsGenericPtrTypePredicate", id: "class_15", level: 2, subject: false, hasDerivedClasses: false, link: "./class_IsGenericPtrTypePredicate_E27AD709FE5B7441619A459142E2C0F96576A64E.html" },
        { name: "cmajor::symbols::IsIntTypePredicate", id: "class_16", level: 2, subject: false, hasDerivedClasses: false, link: "./class_IsIntTypePredicate_31A4CC4ABE5DD8931E8CBC09D64D2338B2D00FB7.html" },
        { name: "cmajor::symbols::IsIntegralTypePredicate", id: "class_17", level: 2, subject: false, hasDerivedClasses: false, link: "./class_IsIntegralTypePredicate_84606C2CEFB2A11687C861516F9B861EE44EE98F.html" },
        { name: "cmajor::symbols::IsInterfaceTypePredicate", id: "class_18", level: 2, subject: false, hasDerivedClasses: false, link: "./class_IsInterfaceTypePredicate_701503FC7F3912B05C096E7A8DEE74634648FA7D.html" },
        { name: "cmajor::symbols::IsLongTypePredicate", id: "class_19", level: 2, subject: false, hasDerivedClasses: false, link: "./class_IsLongTypePredicate_EEECA8F2C09EB0BB4DB55DB8758A3E772A226E56.html" },
        { name: "cmajor::symbols::IsLvalueReferenceTypePredicate", id: "class_20", level: 2, subject: false, hasDerivedClasses: false, link: "./class_IsLvalueReferenceTypePredicate_FC7E48FAEE283982EE30569FBA1BF9C4B406D642.html" },
        { name: "cmajor::symbols::IsPointerTypePredicate", id: "class_21", level: 2, subject: false, hasDerivedClasses: false, link: "./class_IsPointerTypePredicate_91DCA9CAE0367E58D2F43C0A6DC1947474673375.html" },
        { name: "cmajor::symbols::IsPolymorphicTypePredicate", id: "class_22", level: 2, subject: false, hasDerivedClasses: false, link: "./class_IsPolymorphicTypePredicate_A7C96E149CB305403DC496A8CBAF825544F950E2.html" },
        { name: "cmajor::symbols::IsReferenceTypePredicate", id: "class_23", level: 2, subject: false, hasDerivedClasses: false, link: "./class_IsReferenceTypePredicate_1164ED0E81F3450BF84C14381674800DF560931E.html" },
        { name: "cmajor::symbols::IsRvalueReferenceTypePredicate", id: "class_24", level: 2, subject: false, hasDerivedClasses: false, link: "./class_IsRvalueReferenceTypePredicate_5054795D08329FC480D1DDD23324CD3A50C8EE2D.html" },
        { name: "cmajor::symbols::IsSByteTypePredicate", id: "class_25", level: 2, subject: false, hasDerivedClasses: false, link: "./class_IsSByteTypePredicate_050B3B109F9A16936E035E50874D2BA1E857C9AE.html" },
        { name: "cmajor::symbols::IsShortTypePredicate", id: "class_26", level: 2, subject: false, hasDerivedClasses: false, link: "./class_IsShortTypePredicate_C20E90FDD6B0F1665ABCE368985D7A2633812F0A.html" },
        { name: "cmajor::symbols::IsSignedTypePredicate", id: "class_27", level: 2, subject: false, hasDerivedClasses: false, link: "./class_IsSignedTypePredicate_84CE2A555CA6FC018164A0A030CBEC618F1D8B2C.html" },
        { name: "cmajor::symbols::IsUCharTypePredicate", id: "class_28", level: 2, subject: false, hasDerivedClasses: false, link: "./class_IsUCharTypePredicate_774DD1621A66BC80451B37D981FEEC65498532A1.html" },
        { name: "cmajor::symbols::IsUIntTypePredicate", id: "class_29", level: 2, subject: false, hasDerivedClasses: false, link: "./class_IsUIntTypePredicate_6B4A641ED36D84081619FA6718D02DA9572101D6.html" },
        { name: "cmajor::symbols::IsULongTypePredicate", id: "class_30", level: 2, subject: false, hasDerivedClasses: false, link: "./class_IsULongTypePredicate_2061B6826768062BB975192CA16F930B9A0E04E0.html" },
        { name: "cmajor::symbols::IsUShortTypePredicate", id: "class_31", level: 2, subject: false, hasDerivedClasses: false, link: "./class_IsUShortTypePredicate_F91A1DA578CCFF86D3DCCA3B8642FED5FE7830DD.html" },
        { name: "cmajor::symbols::IsUnsignedTypePredicate", id: "class_32", level: 2, subject: false, hasDerivedClasses: false, link: "./class_IsUnsignedTypePredicate_F3142B8D7AB3D07CFFAC6CE4325BFC26147B99E2.html" },
        { name: "cmajor::symbols::IsVoidTypePredicate", id: "class_33", level: 2, subject: false, hasDerivedClasses: false, link: "./class_IsVoidTypePredicate_0AA23D8F46A70242E6AC6FC172953807789D9FB1.html" },
        { name: "cmajor::symbols::IsWCharTypePredicate", id: "class_34", level: 2, subject: false, hasDerivedClasses: false, link: "./class_IsWCharTypePredicate_0AEECB3286E3A102957843CD74D1BEE0945EFAB8.html" }];
    drawDiagram(classes);
}

