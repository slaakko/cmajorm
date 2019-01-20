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
        { name: "cmajor::ast::Node", id: "class_0", level: 0, subject: true, hasDerivedClasses: true, link: "class_Node_66FC1D6AD8F487E453CE6E17163479C7C2DAC063.html" },
        { name: "cmajor::ast::AliasNode", id: "class_1", level: 1, subject: false, hasDerivedClasses: false, link: "class_AliasNode_10693B62BC11E40999CE807E3B396F82083EAA67.html" },
        { name: "cmajor::ast::ArrayNode", id: "class_2", level: 1, subject: false, hasDerivedClasses: false, link: "class_ArrayNode_BB1B95D41DE161B895694B6CEBC72818440421BE.html" },
        { name: "cmajor::ast::AsNode", id: "class_3", level: 1, subject: false, hasDerivedClasses: false, link: "class_AsNode_DFE9A993309BB164E850E691F3176484AE68600E.html" },
        { name: "cmajor::ast::AxiomNode", id: "class_4", level: 1, subject: false, hasDerivedClasses: false, link: "class_AxiomNode_32355B173A9C1B11A7E9B59C08B0FBBA45CAC0BD.html" },
        { name: "cmajor::ast::AxiomStatementNode", id: "class_5", level: 1, subject: false, hasDerivedClasses: false, link: "class_AxiomStatementNode_9D169B5E38D2D903D070F4DE9B4761FC761B6BA4.html" },
        { name: "cmajor::ast::BaseNode", id: "class_6", level: 1, subject: false, hasDerivedClasses: false, link: "class_BaseNode_F5CB293E8C32B43BAF3B6028789B049D2BA72B52.html" },
        { name: "cmajor::ast::BinaryNode", id: "class_7", level: 1, subject: false, hasDerivedClasses: true, link: "class_BinaryNode_626DEB7407C168A00D5434855EB75F58608C44C9.html" },
        { name: "cmajor::ast::BoolNode", id: "class_8", level: 1, subject: false, hasDerivedClasses: false, link: "class_BoolNode_7F653F3E54BE7E76E7D3F6849A00BC9E61F4C441.html" },
        { name: "cmajor::ast::ByteNode", id: "class_9", level: 1, subject: false, hasDerivedClasses: false, link: "class_ByteNode_CC008AEF81F6E2506B1BFDBF6264C85D847AD97C.html" },
        { name: "cmajor::ast::CastNode", id: "class_10", level: 1, subject: false, hasDerivedClasses: false, link: "class_CastNode_3902465DF887E654C3D67C3D56B6BF1C9DB002B9.html" },
        { name: "cmajor::ast::CatchNode", id: "class_11", level: 1, subject: false, hasDerivedClasses: false, link: "class_CatchNode_10ECFB46997F44645EFECFE02EF09B520D324AF2.html" },
        { name: "cmajor::ast::CharNode", id: "class_12", level: 1, subject: false, hasDerivedClasses: false, link: "class_CharNode_54EE74E03AB82052879E0112E91C53E5436D0818.html" },
        { name: "cmajor::ast::ClassDelegateNode", id: "class_13", level: 1, subject: false, hasDerivedClasses: false, link: "class_ClassDelegateNode_A5CA5E1911FBA0AD8BDA29F9078E727FEE2C1345.html" },
        { name: "cmajor::ast::ClassNode", id: "class_14", level: 1, subject: false, hasDerivedClasses: false, link: "class_ClassNode_848AB786939FE8D089CE28B274B27269C84A59FE.html" },
        { name: "cmajor::ast::CompileUnitNode", id: "class_15", level: 1, subject: false, hasDerivedClasses: false, link: "class_CompileUnitNode_F6CDC04A745418A00AB314CF086C092721F42A14.html" },
        { name: "cmajor::ast::ConceptIdNode", id: "class_16", level: 1, subject: false, hasDerivedClasses: false, link: "class_ConceptIdNode_726D79E62740AAB16C8AA517A7DEB4DEB6184137.html" },
        { name: "cmajor::ast::ConceptNode", id: "class_17", level: 1, subject: false, hasDerivedClasses: true, link: "class_ConceptNode_8DEFFA82BA4F144471B950B22D8DF80D825881AB.html" },
        { name: "cmajor::ast::ConditionalCompilationExpressionNode", id: "class_18", level: 1, subject: false, hasDerivedClasses: true, link: "class_ConditionalCompilationExpressionNode_DBAA5DC9A5996E8ABBA1C693FCF249F5494751E2.html" },
        { name: "cmajor::ast::ConditionalCompilationPartNode", id: "class_19", level: 1, subject: false, hasDerivedClasses: false, link: "class_ConditionalCompilationPartNode_83F63E2147A6DF491F0B9D0E960DD1D8A1868545.html" },
        { name: "cmajor::ast::ConstNode", id: "class_20", level: 1, subject: false, hasDerivedClasses: false, link: "class_ConstNode_69FED93E0126C0B6DD77A4D634DED7DDB0558803.html" },
        { name: "cmajor::ast::ConstantNode", id: "class_21", level: 1, subject: false, hasDerivedClasses: false, link: "class_ConstantNode_3422FA9B65BAEA91920F47ABF06541C85D61E7E7.html" },
        { name: "cmajor::ast::ConstraintNode", id: "class_22", level: 1, subject: false, hasDerivedClasses: true, link: "class_ConstraintNode_C8CAFBDD270BC60F16280322541DF170148F294F.html" },
        { name: "cmajor::ast::ConstructNode", id: "class_23", level: 1, subject: false, hasDerivedClasses: false, link: "class_ConstructNode_D78F28E90A558D1A5F4DE9EC651A1E516BA2B366.html" },
        { name: "cmajor::ast::DelegateNode", id: "class_24", level: 1, subject: false, hasDerivedClasses: false, link: "class_DelegateNode_F4F4A951C249D2E6BFC9367C1A60DA7EF765C838.html" },
        { name: "cmajor::ast::DoubleNode", id: "class_25", level: 1, subject: false, hasDerivedClasses: false, link: "class_DoubleNode_F75FD29766F88AFC54AB08F31D57BD0CA20C305B.html" },
        { name: "cmajor::ast::EnumConstantNode", id: "class_26", level: 1, subject: false, hasDerivedClasses: false, link: "class_EnumConstantNode_A8CEDBE294E0416F4002BF72AE276C4803A9FC8F.html" },
        { name: "cmajor::ast::EnumTypeNode", id: "class_27", level: 1, subject: false, hasDerivedClasses: false, link: "class_EnumTypeNode_EB3DCFB03130D09501A26B7E961147E44295542C.html" },
        { name: "cmajor::ast::FloatNode", id: "class_28", level: 1, subject: false, hasDerivedClasses: false, link: "class_FloatNode_11EE6B7D6E995D56FFC5EA6DADBAB51A4C7D6C72.html" },
        { name: "cmajor::ast::FunctionNode", id: "class_29", level: 1, subject: false, hasDerivedClasses: true, link: "class_FunctionNode_3BCFFC89E645A619641A86E9841D09CB6102AB04.html" },
        { name: "cmajor::ast::IdentifierNode", id: "class_30", level: 1, subject: false, hasDerivedClasses: true, link: "class_IdentifierNode_4A1A7E3AFB614AEF7ED2B90309B9B619E86EE9E4.html" },
        { name: "cmajor::ast::IndexingNode", id: "class_31", level: 1, subject: false, hasDerivedClasses: false, link: "class_IndexingNode_4DD6319D044DBC81CA480C0BBEAF730ABE8FD89E.html" },
        { name: "cmajor::ast::InitializerNode", id: "class_32", level: 1, subject: false, hasDerivedClasses: true, link: "class_InitializerNode_5CA1158B044B99FDE4276BD8DB9D9AE3B3F82968.html" },
        { name: "cmajor::ast::IntNode", id: "class_33", level: 1, subject: false, hasDerivedClasses: false, link: "class_IntNode_BF54BA8BBF2DCD19C6D55113194C0367415A5319.html" },
        { name: "cmajor::ast::InterfaceNode", id: "class_34", level: 1, subject: false, hasDerivedClasses: false, link: "class_InterfaceNode_8CEFA1F425668F852574D2197AA1FD374A2B0D3C.html" },
        { name: "cmajor::ast::InvokeNode", id: "class_35", level: 1, subject: false, hasDerivedClasses: false, link: "class_InvokeNode_3FBD74203803CF881935D6D89C7CD6E8F62D0E62.html" },
        { name: "cmajor::ast::IsNode", id: "class_36", level: 1, subject: false, hasDerivedClasses: false, link: "class_IsNode_DC5D8073755C011653C2810F7CAE7668BAD574B3.html" },
        { name: "cmajor::ast::LValueRefNode", id: "class_37", level: 1, subject: false, hasDerivedClasses: false, link: "class_LValueRefNode_4382AD34AE48B2D9C40D9CF8862318CF222E085F.html" },
        { name: "cmajor::ast::LabelNode", id: "class_38", level: 1, subject: false, hasDerivedClasses: false, link: "class_LabelNode_4C2A7BA263D87AFB92308A26A4B5AEE4A4236D7E.html" },
        { name: "cmajor::ast::LiteralNode", id: "class_39", level: 1, subject: false, hasDerivedClasses: true, link: "class_LiteralNode_33955D862408948682E1F5E18AC7C76B2C911BCC.html" },
        { name: "cmajor::ast::LongNode", id: "class_40", level: 1, subject: false, hasDerivedClasses: false, link: "class_LongNode_86954C05064DC9EA36349217F83C1206CD5C24C9.html" },
        { name: "cmajor::ast::MemberVariableNode", id: "class_41", level: 1, subject: false, hasDerivedClasses: false, link: "class_MemberVariableNode_8ECD03B83E3B0B970C2EFDF682A3FE0CDB8B0D40.html" },
        { name: "cmajor::ast::NamespaceImportNode", id: "class_42", level: 1, subject: false, hasDerivedClasses: false, link: "class_NamespaceImportNode_C6981FA8D11A989621CF3345C3559AAA91E72579.html" },
        { name: "cmajor::ast::NamespaceNode", id: "class_43", level: 1, subject: false, hasDerivedClasses: false, link: "class_NamespaceNode_14753D11DE3FD97E8D3A4D2206225CB72D3383F4.html" },
        { name: "cmajor::ast::NewNode", id: "class_44", level: 1, subject: false, hasDerivedClasses: false, link: "class_NewNode_EE3C3F86520AB28F6F7105C86570A3D2746391CC.html" },
        { name: "cmajor::ast::ParameterNode", id: "class_45", level: 1, subject: false, hasDerivedClasses: false, link: "class_ParameterNode_6A092C19F0E267DB1C50EC127B93CFC3BD91B2A2.html" },
        { name: "cmajor::ast::PointerNode", id: "class_46", level: 1, subject: false, hasDerivedClasses: false, link: "class_PointerNode_52B1D0DAA43D00188CA1702C94F437F2EA2EDF65.html" },
        { name: "cmajor::ast::RValueRefNode", id: "class_47", level: 1, subject: false, hasDerivedClasses: false, link: "class_RValueRefNode_6667F440467E1256EE90F8791A06AC133458FFC1.html" },
        { name: "cmajor::ast::SByteNode", id: "class_48", level: 1, subject: false, hasDerivedClasses: false, link: "class_SByteNode_510878231A1D99BDE97C6AB3215045C8A8A02DB8.html" },
        { name: "cmajor::ast::ShortNode", id: "class_49", level: 1, subject: false, hasDerivedClasses: false, link: "class_ShortNode_5E0BE2B31873FABEDF13800D08568332D6669883.html" },
        { name: "cmajor::ast::SizeOfNode", id: "class_50", level: 1, subject: false, hasDerivedClasses: false, link: "class_SizeOfNode_B89492F8576FDDD28502820882B89F1B0D2D749D.html" },
        { name: "cmajor::ast::StatementNode", id: "class_51", level: 1, subject: false, hasDerivedClasses: true, link: "class_StatementNode_5B30CD05CFBD6FC4DC65F9EB63116552354DEF08.html" },
        { name: "cmajor::ast::TemplateIdNode", id: "class_52", level: 1, subject: false, hasDerivedClasses: false, link: "class_TemplateIdNode_F438015A719C74294AC166E43FDE34F0D577057C.html" },
        { name: "cmajor::ast::TemplateParameterNode", id: "class_53", level: 1, subject: false, hasDerivedClasses: false, link: "class_TemplateParameterNode_374F6D33E33DDBBE25A59CEDD5165A04A2D7CDAB.html" },
        { name: "cmajor::ast::ThisNode", id: "class_54", level: 1, subject: false, hasDerivedClasses: false, link: "class_ThisNode_786ADF931EE61D33AA9053443ABA5DEA1EE90B09.html" },
        { name: "cmajor::ast::TypeNameNode", id: "class_55", level: 1, subject: false, hasDerivedClasses: false, link: "class_TypeNameNode_175111C93C5F120BC7D6A6378704E51F707A174F.html" },
        { name: "cmajor::ast::TypedefNode", id: "class_56", level: 1, subject: false, hasDerivedClasses: false, link: "class_TypedefNode_B96BB9228B4B26C121F8637936127893A2F45FF2.html" },
        { name: "cmajor::ast::UCharNode", id: "class_57", level: 1, subject: false, hasDerivedClasses: false, link: "class_UCharNode_82EA556E7DDE846873F01014337AF75E89C6EBBC.html" },
        { name: "cmajor::ast::UIntNode", id: "class_58", level: 1, subject: false, hasDerivedClasses: false, link: "class_UIntNode_DDCE482023EFED9380036B27A0BCDE811F8DDD65.html" },
        { name: "cmajor::ast::ULongNode", id: "class_59", level: 1, subject: false, hasDerivedClasses: false, link: "class_ULongNode_DBB305474AFE431E871A01FFD347A6B976F94B2D.html" },
        { name: "cmajor::ast::UShortNode", id: "class_60", level: 1, subject: false, hasDerivedClasses: false, link: "class_UShortNode_FC23A6B0F63A8B6743984B39906CAF83F2A612AF.html" },
        { name: "cmajor::ast::UnaryNode", id: "class_61", level: 1, subject: false, hasDerivedClasses: true, link: "class_UnaryNode_C5C127545093E9BD85CF05D482B5C675FCD04A6E.html" },
        { name: "cmajor::ast::VoidNode", id: "class_62", level: 1, subject: false, hasDerivedClasses: false, link: "class_VoidNode_24E8AE51C29CB69249491B636C08BC507950598F.html" },
        { name: "cmajor::ast::WCharNode", id: "class_63", level: 1, subject: false, hasDerivedClasses: false, link: "class_WCharNode_C6CA94A0FB00F3CE5C8C1C12BAEF39FB8AB8C899.html" }];
    drawDiagram(classes);
}

