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
        { name: "cmajor::symbols::Symbol", id: "class_0", level: 0, subject: false, hasDerivedClasses: true, link: "./class_Symbol_7AEBC014717F74E379D617070579BCDAE3CACAA3.html" },
        { name: "cmajor::symbols::ContainerSymbol", id: "class_1", level: 1, subject: false, hasDerivedClasses: true, link: "./class_ContainerSymbol_4B08BF3CBF422A1BDE2F14FAEAF47E262AA566CC.html" },
        { name: "cmajor::symbols::FunctionSymbol", id: "class_2", level: 2, subject: true, hasDerivedClasses: true, link: "./class_FunctionSymbol_5AC1C26A9572181B3E31247997BF40453B22EDEF.html" },
        { name: "cmajor::symbols::ArrayBeginFunction", id: "class_3", level: 3, subject: false, hasDerivedClasses: false, link: "./class_ArrayBeginFunction_E3B5BE55331C65B83658FEE1FD3AFF20BD90B444.html" },
        { name: "cmajor::symbols::ArrayCBeginFunction", id: "class_4", level: 3, subject: false, hasDerivedClasses: false, link: "./class_ArrayCBeginFunction_DB986604D76D5F669E06DF1930205AD914519F6A.html" },
        { name: "cmajor::symbols::ArrayCEndFunction", id: "class_5", level: 3, subject: false, hasDerivedClasses: false, link: "./class_ArrayCEndFunction_F44549EEC8764EC187641687DCFEA9A4BE987BCB.html" },
        { name: "cmajor::symbols::ArrayEndFunction", id: "class_6", level: 3, subject: false, hasDerivedClasses: false, link: "./class_ArrayEndFunction_1B398C4882F41336C2807F45A8E2E9A631866FF1.html" },
        { name: "cmajor::symbols::ArrayLengthFunction", id: "class_7", level: 3, subject: false, hasDerivedClasses: false, link: "./class_ArrayLengthFunction_9078AC53FF16E7839CADC1ED62796CAA5861D0FE.html" },
        { name: "cmajor::symbols::ArrayTypeCopyAssignment", id: "class_8", level: 3, subject: false, hasDerivedClasses: false, link: "./class_ArrayTypeCopyAssignment_952D1B10ED9AF57F108E72CA9B0338EAD2C67F62.html" },
        { name: "cmajor::symbols::ArrayTypeCopyConstructor", id: "class_9", level: 3, subject: false, hasDerivedClasses: false, link: "./class_ArrayTypeCopyConstructor_5D0F119C84F5958468B16224E4D7C719735610A6.html" },
        { name: "cmajor::symbols::ArrayTypeDefaultConstructor", id: "class_10", level: 3, subject: false, hasDerivedClasses: false, link: "./class_ArrayTypeDefaultConstructor_38294FD8E26FE80F23CBD4E629F5F5F37B73FB5C.html" },
        { name: "cmajor::symbols::ArrayTypeElementAccess", id: "class_11", level: 3, subject: false, hasDerivedClasses: false, link: "./class_ArrayTypeElementAccess_9D625DD8987CD5481EE9AC05F42490C43355D77F.html" },
        { name: "cmajor::symbols::ArrayTypeMoveAssignment", id: "class_12", level: 3, subject: false, hasDerivedClasses: false, link: "./class_ArrayTypeMoveAssignment_53909E0DDD59D87DB10CAE58CB60CDC72A621979.html" },
        { name: "cmajor::symbols::ArrayTypeMoveConstructor", id: "class_13", level: 3, subject: false, hasDerivedClasses: false, link: "./class_ArrayTypeMoveConstructor_84E665BE90BB384875B5FC22601843A1BCE7A926.html" },
        { name: "cmajor::symbols::BasicTypeBinaryOperation&lt;BinOp&gt;", id: "class_14", level: 3, subject: false, hasDerivedClasses: true, link: "./class_BasicTypeBinaryOperation_0264B4B65138C1098E6EAD5F75B0395C088C752F.html" },
        { name: "cmajor::symbols::BasicTypeComparisonOperation&lt;ComparisonOp&gt;", id: "class_15", level: 3, subject: false, hasDerivedClasses: true, link: "./class_BasicTypeComparisonOperation_BB29E6BC03B6BDA9E1714482A4558DB943088212.html" },
        { name: "cmajor::symbols::BasicTypeConversion&lt;ConversionOp&gt;", id: "class_16", level: 3, subject: false, hasDerivedClasses: true, link: "./class_BasicTypeConversion_7FA43BB8A119B2DB949DA4C92E083CC6D5EB99A7.html" },
        { name: "cmajor::symbols::BasicTypeCopyAssignment", id: "class_17", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BasicTypeCopyAssignment_922BF8F1216F356F38A0A550DBD33768F04FED1E.html" },
        { name: "cmajor::symbols::BasicTypeCopyCtor", id: "class_18", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BasicTypeCopyCtor_5404169B12845246D1A8F433FAF850A2BF71BBFD.html" },
        { name: "cmajor::symbols::BasicTypeDefaultCtor&lt;DefaultOp&gt;", id: "class_19", level: 3, subject: false, hasDerivedClasses: true, link: "./class_BasicTypeDefaultCtor_E669AC7FA6920EE54FB8BD65B006D26C5287D8AB.html" },
        { name: "cmajor::symbols::BasicTypeMoveAssignment", id: "class_20", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BasicTypeMoveAssignment_761CAAB2D14A3906B26D1E37C65D1CF780BF2243.html" },
        { name: "cmajor::symbols::BasicTypeMoveCtor", id: "class_21", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BasicTypeMoveCtor_238784583C9AA743DC72C1383D8312F01001226C.html" },
        { name: "cmajor::symbols::BasicTypeReturn", id: "class_22", level: 3, subject: false, hasDerivedClasses: false, link: "./class_BasicTypeReturn_7ED6F2ECCFF3C2E9356ABDDA865962FDE485E847.html" },
        { name: "cmajor::symbols::BasicTypeUnaryOperation&lt;UnOp&gt;", id: "class_23", level: 3, subject: false, hasDerivedClasses: true, link: "./class_BasicTypeUnaryOperation_01894EAD38F4C275E1A68F7F76ED8FB0CB7CDD35.html" },
        { name: "cmajor::symbols::ClassDelegateTypeCopyAssignment", id: "class_24", level: 3, subject: false, hasDerivedClasses: false, link: "./class_ClassDelegateTypeCopyAssignment_5EAB185C58F584FDD22422B76853A7652D7E1301.html" },
        { name: "cmajor::symbols::ClassDelegateTypeCopyConstructor", id: "class_25", level: 3, subject: false, hasDerivedClasses: false, link: "./class_ClassDelegateTypeCopyConstructor_7342591B087266A07DAF19DCA6DF4EFAAC9091EB.html" },
        { name: "cmajor::symbols::ClassDelegateTypeDefaultConstructor", id: "class_26", level: 3, subject: false, hasDerivedClasses: false, link: "./class_ClassDelegateTypeDefaultConstructor_F7754688B06C08FE1C39588318337CECA7C900EA.html" },
        { name: "cmajor::symbols::ClassDelegateTypeEquality", id: "class_27", level: 3, subject: false, hasDerivedClasses: false, link: "./class_ClassDelegateTypeEquality_D8D11D96DE90CF96621C142F8A3ED787FAF9A616.html" },
        { name: "cmajor::symbols::ClassDelegateTypeMoveAssignment", id: "class_28", level: 3, subject: false, hasDerivedClasses: false, link: "./class_ClassDelegateTypeMoveAssignment_4CD945BC2A25DE91CFA762186A5E89C41F30E6E6.html" },
        { name: "cmajor::symbols::ClassDelegateTypeMoveConstructor", id: "class_29", level: 3, subject: false, hasDerivedClasses: false, link: "./class_ClassDelegateTypeMoveConstructor_6629D67090B2622D53C7D767CEFCA75702884464.html" },
        { name: "cmajor::symbols::ClassToInterfaceConversion", id: "class_30", level: 3, subject: false, hasDerivedClasses: false, link: "./class_ClassToInterfaceConversion_85BD4B0B5D40814F624C33A15507EDCBD8602B1C.html" },
        { name: "cmajor::symbols::ConstructorSymbol", id: "class_31", level: 3, subject: false, hasDerivedClasses: false, link: "./class_ConstructorSymbol_7601F80D7A15880441E04E3E7E153B24B38E567D.html" },
        { name: "cmajor::symbols::ConversionFunctionSymbol", id: "class_32", level: 3, subject: false, hasDerivedClasses: false, link: "./class_ConversionFunctionSymbol_CECA9FC6A79500F8D9A17A984949A26A604E8689.html" },
        { name: "cmajor::symbols::DelegateToVoidPtrConversion", id: "class_33", level: 3, subject: false, hasDerivedClasses: false, link: "./class_DelegateToVoidPtrConversion_F294F90D68A92528AB98BF87B0A85BE8288259C9.html" },
        { name: "cmajor::symbols::DelegateTypeCopyAssignment", id: "class_34", level: 3, subject: false, hasDerivedClasses: false, link: "./class_DelegateTypeCopyAssignment_D36C61DC985FC4D6BCAE8070B44765CF6979FB8A.html" },
        { name: "cmajor::symbols::DelegateTypeCopyConstructor", id: "class_35", level: 3, subject: false, hasDerivedClasses: false, link: "./class_DelegateTypeCopyConstructor_C0779E762E7695B34DFB8C1ADA1BDAF284FC2652.html" },
        { name: "cmajor::symbols::DelegateTypeDefaultConstructor", id: "class_36", level: 3, subject: false, hasDerivedClasses: false, link: "./class_DelegateTypeDefaultConstructor_67AC30667126234A4FE4638DE9DF9725247B4B3E.html" },
        { name: "cmajor::symbols::DelegateTypeEquality", id: "class_37", level: 3, subject: false, hasDerivedClasses: false, link: "./class_DelegateTypeEquality_934A12D0DDC874173FA1A9D95A99196E960895C5.html" },
        { name: "cmajor::symbols::DelegateTypeMoveAssignment", id: "class_38", level: 3, subject: false, hasDerivedClasses: false, link: "./class_DelegateTypeMoveAssignment_1F85A51434AE6835041402AEB94D4BBF93F8108D.html" },
        { name: "cmajor::symbols::DelegateTypeMoveConstructor", id: "class_39", level: 3, subject: false, hasDerivedClasses: false, link: "./class_DelegateTypeMoveConstructor_88282D48A23F8A92EB58593A6D681E47BBF1F642.html" },
        { name: "cmajor::symbols::DelegateTypeReturn", id: "class_40", level: 3, subject: false, hasDerivedClasses: false, link: "./class_DelegateTypeReturn_C8C29694EE3687C3609B2E2CC13A334A0B7A3116.html" },
        { name: "cmajor::symbols::DestructorSymbol", id: "class_41", level: 3, subject: false, hasDerivedClasses: false, link: "./class_DestructorSymbol_6626180D5ED2C1B180810259C5E3AB2C9CC4E182.html" },
        { name: "cmajor::symbols::EnumTypeCopyAssignment", id: "class_42", level: 3, subject: false, hasDerivedClasses: false, link: "./class_EnumTypeCopyAssignment_B5FB3C462C4DE7DDFFFB97579854AA6D7B740D8A.html" },
        { name: "cmajor::symbols::EnumTypeCopyConstructor", id: "class_43", level: 3, subject: false, hasDerivedClasses: false, link: "./class_EnumTypeCopyConstructor_DE3AA336CA836BDB10238F08C80C4D5702DAB3FC.html" },
        { name: "cmajor::symbols::EnumTypeDefaultConstructor", id: "class_44", level: 3, subject: false, hasDerivedClasses: false, link: "./class_EnumTypeDefaultConstructor_00A51727348FFA9FE5852ECEC0AFEBA61B402553.html" },
        { name: "cmajor::symbols::EnumTypeEqualityOp", id: "class_45", level: 3, subject: false, hasDerivedClasses: false, link: "./class_EnumTypeEqualityOp_5084535E8D8FB127786A5174AFA46DB9A16A4CA3.html" },
        { name: "cmajor::symbols::EnumTypeMoveAssignment", id: "class_46", level: 3, subject: false, hasDerivedClasses: false, link: "./class_EnumTypeMoveAssignment_F667D38CC6268FB67EC6A8C8625A2E3AC5EDEBA9.html" },
        { name: "cmajor::symbols::EnumTypeMoveConstructor", id: "class_47", level: 3, subject: false, hasDerivedClasses: false, link: "./class_EnumTypeMoveConstructor_598D7599585D283FAC50B59EBE69911F25511CFE.html" },
        { name: "cmajor::symbols::EnumTypeReturn", id: "class_48", level: 3, subject: false, hasDerivedClasses: false, link: "./class_EnumTypeReturn_43B5684F3A8DEA111DDE2F7CFD93253747BA15C4.html" },
        { name: "cmajor::symbols::EnumTypeToUnderlyingTypeConversion", id: "class_49", level: 3, subject: false, hasDerivedClasses: false, link: "./class_EnumTypeToUnderlyingTypeConversion_7B44AE9A264821070F137CED2A0005D64A33772F.html" },
        { name: "cmajor::symbols::FunctionToDelegateConversion", id: "class_50", level: 3, subject: false, hasDerivedClasses: false, link: "./class_FunctionToDelegateConversion_528ACF569CD05CE5D8431A13AF117DAAFC3FB1FE.html" },
        { name: "cmajor::symbols::InterfaceTypeCopyAssignment", id: "class_51", level: 3, subject: false, hasDerivedClasses: false, link: "./class_InterfaceTypeCopyAssignment_2A0FA136C0242AD1764F1EE1A8FF4136D53A634F.html" },
        { name: "cmajor::symbols::InterfaceTypeCopyConstructor", id: "class_52", level: 3, subject: false, hasDerivedClasses: false, link: "./class_InterfaceTypeCopyConstructor_1D43C871BC43BB0E814AECB6C6F64C3C8E6A9A7C.html" },
        { name: "cmajor::symbols::InterfaceTypeDefaultConstructor", id: "class_53", level: 3, subject: false, hasDerivedClasses: false, link: "./class_InterfaceTypeDefaultConstructor_D4D7401497310FFF66BCA146BFD58C7C93AD5919.html" },
        { name: "cmajor::symbols::InterfaceTypeMoveAssignment", id: "class_54", level: 3, subject: false, hasDerivedClasses: false, link: "./class_InterfaceTypeMoveAssignment_B1B4158704A84E54E75033B6CF269354A40D3D3C.html" },
        { name: "cmajor::symbols::InterfaceTypeMoveConstructor", id: "class_55", level: 3, subject: false, hasDerivedClasses: false, link: "./class_InterfaceTypeMoveConstructor_34E562EEF532A7F0F9C93A72D52C78583CECC8B3.html" },
        { name: "cmajor::symbols::MemberFunctionSymbol", id: "class_56", level: 3, subject: false, hasDerivedClasses: false, link: "./class_MemberFunctionSymbol_3BF2D77D69165719C9D08538201EC0A12DBC5B03.html" },
        { name: "cmajor::symbols::MemberFunctionToClassDelegateConversion", id: "class_57", level: 3, subject: false, hasDerivedClasses: false, link: "./class_MemberFunctionToClassDelegateConversion_6CA93A3F92B3723079C3A15901A0CBFFB0D119BC.html" },
        { name: "cmajor::symbols::StaticConstructorSymbol", id: "class_58", level: 3, subject: false, hasDerivedClasses: false, link: "./class_StaticConstructorSymbol_E9F44480525BDC50F6884592FC0E0D3B2C72B31E.html" },
        { name: "cmajor::symbols::StringLengthFunction", id: "class_59", level: 3, subject: false, hasDerivedClasses: false, link: "./class_StringLengthFunction_E3506646CDDF4E6606AADB2097ECE07162729FB5.html" },
        { name: "cmajor::symbols::TrapFunction", id: "class_60", level: 3, subject: false, hasDerivedClasses: false, link: "./class_TrapFunction_E88AFE1E491BF2A9DE42A43ECAF19EBC84D1CF54.html" },
        { name: "cmajor::symbols::UnderlyingTypeToEnumTypeConversion", id: "class_61", level: 3, subject: false, hasDerivedClasses: false, link: "./class_UnderlyingTypeToEnumTypeConversion_0AE88B332A17FAADFB513EB7C74C219739BD34D0.html" },
        { name: "cmajor::symbols::VoidPtrToDelegateConversion", id: "class_62", level: 3, subject: false, hasDerivedClasses: false, link: "./class_VoidPtrToDelegateConversion_F1B289CFA68B441FC9CC603F4AF2889029684299.html" }];
    drawDiagram(classes);
}

