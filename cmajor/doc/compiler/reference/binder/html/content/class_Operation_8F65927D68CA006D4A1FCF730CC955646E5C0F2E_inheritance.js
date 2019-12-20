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
        { name: "cmajor::binder::Operation", id: "class_0", level: 0, subject: true, hasDerivedClasses: true, link: "./class_Operation_8F65927D68CA006D4A1FCF730CC955646E5C0F2E.html" },
        { name: "cmajor::binder::ArrayCopyAssignmentOperation", id: "class_1", level: 1, subject: false, hasDerivedClasses: false, link: "./class_ArrayCopyAssignmentOperation_D43AC3E8A433DB1155DFB4CE6FB20EA20C338450.html" },
        { name: "cmajor::binder::ArrayCopyConstructorOperation", id: "class_2", level: 1, subject: false, hasDerivedClasses: false, link: "./class_ArrayCopyConstructorOperation_FCBD03267C7EE08785A3B46CEB5E743F1CCC5B60.html" },
        { name: "cmajor::binder::ArrayDefaultConstructorOperation", id: "class_3", level: 1, subject: false, hasDerivedClasses: false, link: "./class_ArrayDefaultConstructorOperation_A4E03D75EC9728C3A3D4A172731F6668E5463B1A.html" },
        { name: "cmajor::binder::ArrayElementAccessOperation", id: "class_4", level: 1, subject: false, hasDerivedClasses: false, link: "./class_ArrayElementAccessOperation_4BA4410DA72C131115C91A830EBCBD78E7124B4E.html" },
        { name: "cmajor::binder::ArrayMoveAssignmentOperation", id: "class_5", level: 1, subject: false, hasDerivedClasses: false, link: "./class_ArrayMoveAssignmentOperation_E4ABBFFC3C09C404C8B2267D8D30DB91474D0C73.html" },
        { name: "cmajor::binder::ArrayMoveConstructorOperation", id: "class_6", level: 1, subject: false, hasDerivedClasses: false, link: "./class_ArrayMoveConstructorOperation_E90CFDC3537B79C26FE803C9A6B5E7A09C10C2E5.html" },
        { name: "cmajor::binder::ClassCopyAssignmentOperation", id: "class_7", level: 1, subject: false, hasDerivedClasses: false, link: "./class_ClassCopyAssignmentOperation_A49E9A026C96A4D623324169AE3824B17534F5A4.html" },
        { name: "cmajor::binder::ClassCopyConstructorOperation", id: "class_8", level: 1, subject: false, hasDerivedClasses: false, link: "./class_ClassCopyConstructorOperation_420966589808DE3929D7FAB4935D9C576328DE84.html" },
        { name: "cmajor::binder::ClassDefaultConstructorOperation", id: "class_9", level: 1, subject: false, hasDerivedClasses: false, link: "./class_ClassDefaultConstructorOperation_D7B64EA8C2EFFFF091781CCC2B33788C4741DECE.html" },
        { name: "cmajor::binder::ClassMoveAssignmentOperation", id: "class_10", level: 1, subject: false, hasDerivedClasses: false, link: "./class_ClassMoveAssignmentOperation_9D8DDCDC2702FB10DAA5491010C25BAAA2F246BE.html" },
        { name: "cmajor::binder::ClassMoveConstructorOperation", id: "class_11", level: 1, subject: false, hasDerivedClasses: false, link: "./class_ClassMoveConstructorOperation_B57CFECB9423637BA01AB7F00A58EF6AF67CC97C.html" },
        { name: "cmajor::binder::InterfaceCopyAssignmentOperation", id: "class_12", level: 1, subject: false, hasDerivedClasses: false, link: "./class_InterfaceCopyAssignmentOperation_10BCEA4E9537CCDC1032D8945833ED0ABF51BE6C.html" },
        { name: "cmajor::binder::InterfaceCopyConstructorOperation", id: "class_13", level: 1, subject: false, hasDerivedClasses: false, link: "./class_InterfaceCopyConstructorOperation_6815EB642037625D00C83CD01944464FD9E7B089.html" },
        { name: "cmajor::binder::InterfaceDefaultConstructorOperation", id: "class_14", level: 1, subject: false, hasDerivedClasses: false, link: "./class_InterfaceDefaultConstructorOperation_9B32D202AB639289FBFB07C154A603A51F829C4D.html" },
        { name: "cmajor::binder::InterfaceMoveAssignmentOperation", id: "class_15", level: 1, subject: false, hasDerivedClasses: false, link: "./class_InterfaceMoveAssignmentOperation_EDCA18A7A8192454DC5377C144A32A00FD87C1F3.html" },
        { name: "cmajor::binder::InterfaceMoveConstructorOperation", id: "class_16", level: 1, subject: false, hasDerivedClasses: false, link: "./class_InterfaceMoveConstructorOperation_193A1C5024C8C9401732D16575CD0BBC14CE904E.html" },
        { name: "cmajor::binder::LvalueReferenceCopyAssignmentOperation", id: "class_17", level: 1, subject: false, hasDerivedClasses: false, link: "./class_LvalueReferenceCopyAssignmentOperation_47146C63BC28EC18A54562E012E035F275270656.html" },
        { name: "cmajor::binder::LvalueReferenceCopyConstructorOperation", id: "class_18", level: 1, subject: false, hasDerivedClasses: false, link: "./class_LvalueReferenceCopyConstructorOperation_67EE3704A3480FFCC9FC764BC3BF880B9A52D050.html" },
        { name: "cmajor::binder::LvalueReferenceMoveAssignmentOperation", id: "class_19", level: 1, subject: false, hasDerivedClasses: false, link: "./class_LvalueReferenceMoveAssignmentOperation_33B267AC218893F356C0FCF202AB1842E97F3926.html" },
        { name: "cmajor::binder::LvalueReferenceReturnOperation", id: "class_20", level: 1, subject: false, hasDerivedClasses: false, link: "./class_LvalueReferenceReturnOperation_8D475683F96692CDE02810605351A3ABB0A24709.html" },
        { name: "cmajor::binder::OffsetPlusPointerOperation", id: "class_21", level: 1, subject: false, hasDerivedClasses: false, link: "./class_OffsetPlusPointerOperation_925E0182906E6FF8CFE175A0BD38F7851C682DD2.html" },
        { name: "cmajor::binder::PointerArrowOperation", id: "class_22", level: 1, subject: false, hasDerivedClasses: false, link: "./class_PointerArrowOperation_9F3AFC09B8B354119A22B7DFF94998E7C904AAD4.html" },
        { name: "cmajor::binder::PointerCopyAssignmentOperation", id: "class_23", level: 1, subject: false, hasDerivedClasses: false, link: "./class_PointerCopyAssignmentOperation_ACE649356AAB4C98894F4202E0711037DCF7E647.html" },
        { name: "cmajor::binder::PointerCopyConstructorOperation", id: "class_24", level: 1, subject: false, hasDerivedClasses: false, link: "./class_PointerCopyConstructorOperation_D2AA5D552D1485EC42619B77550B9E0658FEAACF.html" },
        { name: "cmajor::binder::PointerDefaultConstructorOperation", id: "class_25", level: 1, subject: false, hasDerivedClasses: false, link: "./class_PointerDefaultConstructorOperation_5380052011E0AA919DA1E6E83F6095BC2DBA0D56.html" },
        { name: "cmajor::binder::PointerEqualOperation", id: "class_26", level: 1, subject: false, hasDerivedClasses: false, link: "./class_PointerEqualOperation_59FB10164E51F1F94791760134FD8D797986BE6A.html" },
        { name: "cmajor::binder::PointerLessOperation", id: "class_27", level: 1, subject: false, hasDerivedClasses: false, link: "./class_PointerLessOperation_96FAE9DB9BA071BC51078C59690F9E0755E64E2C.html" },
        { name: "cmajor::binder::PointerMinusOffsetOperation", id: "class_28", level: 1, subject: false, hasDerivedClasses: false, link: "./class_PointerMinusOffsetOperation_FD9F9359DBCDA4E1D42DB64AA322F036B2F2BE0D.html" },
        { name: "cmajor::binder::PointerMinusPointerOperation", id: "class_29", level: 1, subject: false, hasDerivedClasses: false, link: "./class_PointerMinusPointerOperation_1F0513D57CE96C86CB475111A2502FDDE1EA3517.html" },
        { name: "cmajor::binder::PointerMoveAssignmentOperation", id: "class_30", level: 1, subject: false, hasDerivedClasses: false, link: "./class_PointerMoveAssignmentOperation_123C7C2FDFBEC72FF57061BFF82EEE395089C733.html" },
        { name: "cmajor::binder::PointerMoveConstructorOperation", id: "class_31", level: 1, subject: false, hasDerivedClasses: false, link: "./class_PointerMoveConstructorOperation_E0E67AE9C8D8169DF9D2CD41462CB4EAE7C508C1.html" },
        { name: "cmajor::binder::PointerPlusOffsetOperation", id: "class_32", level: 1, subject: false, hasDerivedClasses: false, link: "./class_PointerPlusOffsetOperation_656268883FCAF33F8615F7E54924EFE3E07B1DEB.html" },
        { name: "cmajor::binder::PointerReturnOperation", id: "class_33", level: 1, subject: false, hasDerivedClasses: false, link: "./class_PointerReturnOperation_FEE048B2BC3D229404AE1074E1B0EE634EA8DDF4.html" },
        { name: "cmajor::binder::RvalueReferenceCopyAssignmentOperation", id: "class_34", level: 1, subject: false, hasDerivedClasses: false, link: "./class_RvalueReferenceCopyAssignmentOperation_46809DCB8AEE1F3EE52ADF09F958E31361B20AB7.html" },
        { name: "cmajor::binder::RvalueReferenceCopyConstructorOperation", id: "class_35", level: 1, subject: false, hasDerivedClasses: false, link: "./class_RvalueReferenceCopyConstructorOperation_81935DF723206DC1E29B97F0FE0E3BD3040FA95D.html" },
        { name: "cmajor::binder::RvalueReferenceReturnOperation", id: "class_36", level: 1, subject: false, hasDerivedClasses: false, link: "./class_RvalueReferenceReturnOperation_ACA3C3AAFD230DB6127171EFA7140119E090DCB8.html" }];
    drawDiagram(classes);
}

