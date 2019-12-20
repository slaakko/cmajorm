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
        { name: "cmajor::binder::BoundExpression", id: "class_2", level: 2, subject: true, hasDerivedClasses: true, link: "./class_BoundExpression_6C13A7CD518141FC69CADA397796946086EEA452.html" },
        { name: "cmajor::binder::BoundAddressOfExpression", id: "class_3", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundAddressOfExpression_A5E64A329637AA93D0A30E7D1814C2EEE18E77AC.html" },
        { name: "cmajor::binder::BoundAsExpression", id: "class_4", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundAsExpression_801F8209CE20D1833758832CB25511162B89B9E1.html" },
        { name: "cmajor::binder::BoundBitCast", id: "class_5", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundBitCast_3555C7754DA41091352EB8855BE2C25DDD0324B9.html" },
        { name: "cmajor::binder::BoundClassDelegateCall", id: "class_6", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundClassDelegateCall_26162C838B3AF67704003CCD2EB0BEAEB31960BE.html" },
        { name: "cmajor::binder::BoundClassOrClassDelegateConversionResult", id: "class_7", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundClassOrClassDelegateConversionResult_D567FAC30733A2FFC5F2046B64EE2668ECCB14D4.html" },
        { name: "cmajor::binder::BoundConjunction", id: "class_8", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundConjunction_2AC2B0CD92C9B5896BEAD429EB0EE89309478CA8.html" },
        { name: "cmajor::binder::BoundConstant", id: "class_9", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundConstant_4F99952EEB608967A62DFA4EB371782CD2C2C305.html" },
        { name: "cmajor::binder::BoundConstructAndReturnTemporaryExpression", id: "class_10", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundConstructAndReturnTemporaryExpression_B8171A28E33D7C8E1DB1ED4B12DAF35314C1C7F1.html" },
        { name: "cmajor::binder::BoundConstructExpression", id: "class_11", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundConstructExpression_9519ADD2DD5B4CD90CB0E2D0B6F5839B8EC86FEE.html" },
        { name: "cmajor::binder::BoundConversion", id: "class_12", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundConversion_EAE9ECDF9392F64A339764A1F3415D0B33E49E78.html" },
        { name: "cmajor::binder::BoundDelegateCall", id: "class_13", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundDelegateCall_E0D935F2EC14B697C235FC3CD519F0432213A6DC.html" },
        { name: "cmajor::binder::BoundDereferenceExpression", id: "class_14", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundDereferenceExpression_148C0D624D0D019C3691247C87FF0A797FA273CF.html" },
        { name: "cmajor::binder::BoundDisjunction", id: "class_15", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundDisjunction_D6F8EC06B62E996A115B5E04FFF3930937DF493A.html" },
        { name: "cmajor::binder::BoundEnumConstant", id: "class_16", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundEnumConstant_64D2349EFF17704F48853830738512BA35F3A51C.html" },
        { name: "cmajor::binder::BoundFunctionCall", id: "class_17", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundFunctionCall_3B6FF42595EC5C47E14802245CBCB5F47F5F3EAC.html" },
        { name: "cmajor::binder::BoundFunctionGroupExpression", id: "class_18", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundFunctionGroupExpression_DBE9589AB7AC869E137796EFE30ACDE4EE11A27F.html" },
        { name: "cmajor::binder::BoundFunctionPtr", id: "class_19", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundFunctionPtr_484736F4C9F94241A850DC79D71AB4813A1F314A.html" },
        { name: "cmajor::binder::BoundGlobalVariable", id: "class_20", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundGlobalVariable_7D7F08972E9D8B490FB2CD6E6E6FE9D7F044C9E9.html" },
        { name: "cmajor::binder::BoundIsExpression", id: "class_21", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundIsExpression_F285D49EF42A61F29AF03EE57616326ED7354E31.html" },
        { name: "cmajor::binder::BoundLiteral", id: "class_22", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundLiteral_DF1E22194FB48EAFA2C4993DC33E08DF24AF2F68.html" },
        { name: "cmajor::binder::BoundLocalVariable", id: "class_23", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundLocalVariable_A57A3CCE331BFEC9A961AA545EA665CA0BE43FD1.html" },
        { name: "cmajor::binder::BoundMemberExpression", id: "class_24", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundMemberExpression_A140A910BB1C0AD08ADA71E4479201E4281A387E.html" },
        { name: "cmajor::binder::BoundMemberVariable", id: "class_25", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundMemberVariable_821DBEEE08AD07613BC7E1C4D846438683D4C96E.html" },
        { name: "cmajor::binder::BoundNamespaceExpression", id: "class_26", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundNamespaceExpression_FD16E8F993428DCEC3740B7D751536BD24985319.html" },
        { name: "cmajor::binder::BoundParameter", id: "class_27", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundParameter_4BB9E4325E4AE1836E6E8B01866834DDC5B6CF15.html" },
        { name: "cmajor::binder::BoundReferenceToPointerExpression", id: "class_28", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundReferenceToPointerExpression_6B616912996B9A65540A4ACAC567521B4ABC73D4.html" },
        { name: "cmajor::binder::BoundSizeOfExpression", id: "class_29", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundSizeOfExpression_0A449849E9BDD7D11179D9ECC7A57EBFA10D2E16.html" },
        { name: "cmajor::binder::BoundTemporary", id: "class_30", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundTemporary_315499F7B13852EFEFF919C8053CF901987BEE7E.html" },
        { name: "cmajor::binder::BoundTypeExpression", id: "class_31", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundTypeExpression_C2681C3ACE7F6FBEBF9AD6464D7478D7E60F3A3A.html" },
        { name: "cmajor::binder::BoundTypeIdExpression", id: "class_32", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundTypeIdExpression_E68FCD3DC0071D2E030AD1747FBBB40E602F9511.html" },
        { name: "cmajor::binder::BoundTypeNameExpression", id: "class_33", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BoundTypeNameExpression_4B31DB86C15C38624020EEBE7398FBA93B17540A.html" }];
    drawDiagram(classes);
}

