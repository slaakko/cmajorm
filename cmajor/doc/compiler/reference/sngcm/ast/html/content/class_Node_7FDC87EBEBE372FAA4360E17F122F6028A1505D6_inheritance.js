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
        { name: "sngcm::ast::Node", id: "class_0", level: 0, subject: true, hasDerivedClasses: true, link: "./class_Node_7FDC87EBEBE372FAA4360E17F122F6028A1505D6.html" },
        { name: "sngcm::ast::AliasNode", id: "class_1", level: 1, subject: false, hasDerivedClasses: false, link: "./class_AliasNode_B40A668A6AEB9BCBCA7C048E388679AFBA6FA6D4.html" },
        { name: "sngcm::ast::ArrayNode", id: "class_2", level: 1, subject: false, hasDerivedClasses: false, link: "./class_ArrayNode_40B5532DAE5DE789D8895E5D007F60BC2FB2917C.html" },
        { name: "sngcm::ast::AsNode", id: "class_3", level: 1, subject: false, hasDerivedClasses: false, link: "./class_AsNode_BF27922E8AEE3176141256A92E7AB18ED48D08FE.html" },
        { name: "sngcm::ast::AxiomNode", id: "class_4", level: 1, subject: false, hasDerivedClasses: false, link: "./class_AxiomNode_5436268F925DCE3FC86CB4E79017A595F8858C3F.html" },
        { name: "sngcm::ast::AxiomStatementNode", id: "class_5", level: 1, subject: false, hasDerivedClasses: false, link: "./class_AxiomStatementNode_DA9EE870715DB09AAA03EAC5C13883586486C0DE.html" },
        { name: "sngcm::ast::BaseNode", id: "class_6", level: 1, subject: false, hasDerivedClasses: false, link: "./class_BaseNode_7113B8E1AC43B2B6A895C1053E9CDC7EE7383665.html" },
        { name: "sngcm::ast::BinaryNode", id: "class_7", level: 1, subject: false, hasDerivedClasses: true, link: "./class_BinaryNode_3126E45E075D8E51330FC7DF648DA56ED7E10D9D.html" },
        { name: "sngcm::ast::BoolNode", id: "class_8", level: 1, subject: false, hasDerivedClasses: false, link: "./class_BoolNode_0E6C7C9D8889718865B3C7C62D3DFEFEF8036C2B.html" },
        { name: "sngcm::ast::ByteNode", id: "class_9", level: 1, subject: false, hasDerivedClasses: false, link: "./class_ByteNode_C492431344FCF899D2F17B048418CBC7A3C90B06.html" },
        { name: "sngcm::ast::CastNode", id: "class_10", level: 1, subject: false, hasDerivedClasses: false, link: "./class_CastNode_A559304C25EA22C2C8C24601FD0C6EBF85B9044C.html" },
        { name: "sngcm::ast::CatchNode", id: "class_11", level: 1, subject: false, hasDerivedClasses: false, link: "./class_CatchNode_C5332657F67C89B25013743776CA3876B3A5969A.html" },
        { name: "sngcm::ast::CharNode", id: "class_12", level: 1, subject: false, hasDerivedClasses: false, link: "./class_CharNode_D97C752E6DE2F8ED51F0342FF190332274DC276C.html" },
        { name: "sngcm::ast::ClassDelegateNode", id: "class_13", level: 1, subject: false, hasDerivedClasses: false, link: "./class_ClassDelegateNode_B7C5DF23170B00CAC64680593852E9BC52E4564F.html" },
        { name: "sngcm::ast::ClassNode", id: "class_14", level: 1, subject: false, hasDerivedClasses: false, link: "./class_ClassNode_FBB903AD81E52A0333DC75FA12ACE469ABB991DE.html" },
        { name: "sngcm::ast::CommentNode", id: "class_15", level: 1, subject: false, hasDerivedClasses: false, link: "./class_CommentNode_BB4676D1D3F6E3B12171DFEC41DF291E468870F9.html" },
        { name: "sngcm::ast::CompileUnitNode", id: "class_16", level: 1, subject: false, hasDerivedClasses: false, link: "./class_CompileUnitNode_CDA7B3852BEEDA056D8D4BEE4A1675A1E15F5527.html" },
        { name: "sngcm::ast::ConceptIdNode", id: "class_17", level: 1, subject: false, hasDerivedClasses: false, link: "./class_ConceptIdNode_CA66861A07095B3F9F74AA79D1B9C5B80FB4D50A.html" },
        { name: "sngcm::ast::ConceptNode", id: "class_18", level: 1, subject: false, hasDerivedClasses: true, link: "./class_ConceptNode_6E09C7B31968ECB17A6F5F9E92F29B5BABA40ECE.html" },
        { name: "sngcm::ast::ConditionalCompilationExpressionNode", id: "class_19", level: 1, subject: false, hasDerivedClasses: true, link: "./class_ConditionalCompilationExpressionNode_0E2A9F385165FF3648A34BD21A6EF60DC2D98B5D.html" },
        { name: "sngcm::ast::ConditionalCompilationPartNode", id: "class_20", level: 1, subject: false, hasDerivedClasses: false, link: "./class_ConditionalCompilationPartNode_01E32244B3CCEB630093E6268978BAD22E17D2B7.html" },
        { name: "sngcm::ast::ConstNode", id: "class_21", level: 1, subject: false, hasDerivedClasses: false, link: "./class_ConstNode_66928DA24CD664BE70B72083636D58FCB4AA10E4.html" },
        { name: "sngcm::ast::ConstantNode", id: "class_22", level: 1, subject: false, hasDerivedClasses: false, link: "./class_ConstantNode_ECFC68B3CBE578FCC251D354EADA06ACC6A94A9E.html" },
        { name: "sngcm::ast::ConstraintNode", id: "class_23", level: 1, subject: false, hasDerivedClasses: true, link: "./class_ConstraintNode_8F17769E0F8B7D0FAB7516BA5F8CD95CAA1E96A7.html" },
        { name: "sngcm::ast::ConstructNode", id: "class_24", level: 1, subject: false, hasDerivedClasses: false, link: "./class_ConstructNode_37FBC91BB28E4A6E6F08E35CBA2A7C566C551E1D.html" },
        { name: "sngcm::ast::DelegateNode", id: "class_25", level: 1, subject: false, hasDerivedClasses: false, link: "./class_DelegateNode_C771F36CEBE70670ED8842B6DAE78190A5FCDCA3.html" },
        { name: "sngcm::ast::DoubleNode", id: "class_26", level: 1, subject: false, hasDerivedClasses: false, link: "./class_DoubleNode_E7B613E244C35F5B6C1FD5106F577A1627A09507.html" },
        { name: "sngcm::ast::EnumConstantNode", id: "class_27", level: 1, subject: false, hasDerivedClasses: false, link: "./class_EnumConstantNode_381A2BE76E395318E6BC73069CF5EBA86913675C.html" },
        { name: "sngcm::ast::EnumTypeNode", id: "class_28", level: 1, subject: false, hasDerivedClasses: false, link: "./class_EnumTypeNode_7BE35177F05F418B863EEA546061B3404DBC7F99.html" },
        { name: "sngcm::ast::FloatNode", id: "class_29", level: 1, subject: false, hasDerivedClasses: false, link: "./class_FloatNode_1AD177C2BF6126F603E0E42937502FB3859F7074.html" },
        { name: "sngcm::ast::FunctionNode", id: "class_30", level: 1, subject: false, hasDerivedClasses: true, link: "./class_FunctionNode_95C464D9620C4D6FD74DA45BE5340174A5E1BC0D.html" },
        { name: "sngcm::ast::GlobalVariableNode", id: "class_31", level: 1, subject: false, hasDerivedClasses: false, link: "./class_GlobalVariableNode_1AEB1361F97CE62C22C406721A9C3CD3690112D5.html" },
        { name: "sngcm::ast::IdentifierNode", id: "class_32", level: 1, subject: false, hasDerivedClasses: true, link: "./class_IdentifierNode_4CE99E9F7A7A909CE85CE1E7ED4A63A5060B83E3.html" },
        { name: "sngcm::ast::IndexingNode", id: "class_33", level: 1, subject: false, hasDerivedClasses: false, link: "./class_IndexingNode_E13E2574CB0969DB344B18180601055DAD6CF744.html" },
        { name: "sngcm::ast::InitializerNode", id: "class_34", level: 1, subject: false, hasDerivedClasses: true, link: "./class_InitializerNode_589DF2F5C552F18E06D7F8235544D6A2162AA1D1.html" },
        { name: "sngcm::ast::IntNode", id: "class_35", level: 1, subject: false, hasDerivedClasses: false, link: "./class_IntNode_D24E49996B0668DE797B7DD60AABA409ABE42E4C.html" },
        { name: "sngcm::ast::InterfaceNode", id: "class_36", level: 1, subject: false, hasDerivedClasses: false, link: "./class_InterfaceNode_AC678E138F71FD1B7A1940199F1B74796F594179.html" },
        { name: "sngcm::ast::InvokeNode", id: "class_37", level: 1, subject: false, hasDerivedClasses: false, link: "./class_InvokeNode_EADEB88922FE8FAECE52C3D7306A5A8C8C1A900D.html" },
        { name: "sngcm::ast::IsNode", id: "class_38", level: 1, subject: false, hasDerivedClasses: false, link: "./class_IsNode_59B1652AD658C57D204B8E8C394CCF1F10BF5EDA.html" },
        { name: "sngcm::ast::LValueRefNode", id: "class_39", level: 1, subject: false, hasDerivedClasses: false, link: "./class_LValueRefNode_5F81C4F2A34E2029B768017805FF11E9A326CEA6.html" },
        { name: "sngcm::ast::LabelNode", id: "class_40", level: 1, subject: false, hasDerivedClasses: false, link: "./class_LabelNode_CA29FD57DD242F38283331C8FC8948ED8313B1FA.html" },
        { name: "sngcm::ast::LiteralNode", id: "class_41", level: 1, subject: false, hasDerivedClasses: true, link: "./class_LiteralNode_1A76F3823B6151408E0B854080BE8685B0ABF949.html" },
        { name: "sngcm::ast::LongNode", id: "class_42", level: 1, subject: false, hasDerivedClasses: false, link: "./class_LongNode_5579FC2689C50AEC0D7FE1B7A88408DCB86E5621.html" },
        { name: "sngcm::ast::MemberVariableNode", id: "class_43", level: 1, subject: false, hasDerivedClasses: false, link: "./class_MemberVariableNode_C955E22C018764E831E18F3BD2F014BD4E19BA4A.html" },
        { name: "sngcm::ast::NamespaceImportNode", id: "class_44", level: 1, subject: false, hasDerivedClasses: false, link: "./class_NamespaceImportNode_F4039A7361A135C6E948064058DEBCD3680DC86E.html" },
        { name: "sngcm::ast::NamespaceNode", id: "class_45", level: 1, subject: false, hasDerivedClasses: false, link: "./class_NamespaceNode_FCE3319C12CB53B9DE678E1E74EA6C7698B7CD4C.html" },
        { name: "sngcm::ast::NewNode", id: "class_46", level: 1, subject: false, hasDerivedClasses: false, link: "./class_NewNode_E3F5A751DD8FE942E013C9C13D86EE8ED58FEE6F.html" },
        { name: "sngcm::ast::ParameterNode", id: "class_47", level: 1, subject: false, hasDerivedClasses: false, link: "./class_ParameterNode_8174A86CB900D2F3FB2B7B55769E36D3F7EE04C1.html" },
        { name: "sngcm::ast::PointerNode", id: "class_48", level: 1, subject: false, hasDerivedClasses: false, link: "./class_PointerNode_C30FB1E28226404DA13D72F1CD82CB9BCECC8C34.html" },
        { name: "sngcm::ast::RValueRefNode", id: "class_49", level: 1, subject: false, hasDerivedClasses: false, link: "./class_RValueRefNode_C57F91D51447ED914B6A7D40CE46F87E30B97A9C.html" },
        { name: "sngcm::ast::SByteNode", id: "class_50", level: 1, subject: false, hasDerivedClasses: false, link: "./class_SByteNode_27369E01C88219836AB79F205DAA65703E0CB95E.html" },
        { name: "sngcm::ast::ShortNode", id: "class_51", level: 1, subject: false, hasDerivedClasses: false, link: "./class_ShortNode_DA0D6E3A91BC87ABB38E2A7D6220986402332F49.html" },
        { name: "sngcm::ast::SizeOfNode", id: "class_52", level: 1, subject: false, hasDerivedClasses: false, link: "./class_SizeOfNode_5C9F3380A12710E486F190B7E9A8721EC79228E5.html" },
        { name: "sngcm::ast::StatementNode", id: "class_53", level: 1, subject: false, hasDerivedClasses: true, link: "./class_StatementNode_C645560C48648EE9245172CF4CA54D85381420E3.html" },
        { name: "sngcm::ast::TemplateIdNode", id: "class_54", level: 1, subject: false, hasDerivedClasses: false, link: "./class_TemplateIdNode_511FF4648199D13B43B12A897052E66766561252.html" },
        { name: "sngcm::ast::TemplateParameterNode", id: "class_55", level: 1, subject: false, hasDerivedClasses: false, link: "./class_TemplateParameterNode_45810ABA7408E220FF86B3F36E69743A18321F60.html" },
        { name: "sngcm::ast::ThisNode", id: "class_56", level: 1, subject: false, hasDerivedClasses: false, link: "./class_ThisNode_8A637CECCFFC8DF61D9BD2516BA64201EBCE14E4.html" },
        { name: "sngcm::ast::TypeIdNode", id: "class_57", level: 1, subject: false, hasDerivedClasses: false, link: "./class_TypeIdNode_42DF28EFF8A89A059CED935D0E3482D2FC3C8192.html" },
        { name: "sngcm::ast::TypeNameNode", id: "class_58", level: 1, subject: false, hasDerivedClasses: false, link: "./class_TypeNameNode_63A0AFC1582769A4CFEF31DAF0DA6E3C08945640.html" },
        { name: "sngcm::ast::TypedefNode", id: "class_59", level: 1, subject: false, hasDerivedClasses: false, link: "./class_TypedefNode_D487B7D9495F57E9A6232BB7F877E959E657EF23.html" },
        { name: "sngcm::ast::UCharNode", id: "class_60", level: 1, subject: false, hasDerivedClasses: false, link: "./class_UCharNode_087FCBBFB880603D156FD3093E1E0E28721FA033.html" },
        { name: "sngcm::ast::UIntNode", id: "class_61", level: 1, subject: false, hasDerivedClasses: false, link: "./class_UIntNode_EB152EAD34C39474D543D4BA369175050C97C1F2.html" },
        { name: "sngcm::ast::ULongNode", id: "class_62", level: 1, subject: false, hasDerivedClasses: false, link: "./class_ULongNode_04CDFFCD3D56E0E680D1318B45A207E5514F0C26.html" },
        { name: "sngcm::ast::UShortNode", id: "class_63", level: 1, subject: false, hasDerivedClasses: false, link: "./class_UShortNode_CA23AAEABBB53A97BA0D2FA83EE144A49AD558AA.html" },
        { name: "sngcm::ast::UnaryNode", id: "class_64", level: 1, subject: false, hasDerivedClasses: true, link: "./class_UnaryNode_165135A5C7753F52B4F177BA0F0E4222CB08456A.html" },
        { name: "sngcm::ast::VoidNode", id: "class_65", level: 1, subject: false, hasDerivedClasses: false, link: "./class_VoidNode_0B8E194235D6FDC11EACED7C69334D85F4A90257.html" },
        { name: "sngcm::ast::WCharNode", id: "class_66", level: 1, subject: false, hasDerivedClasses: false, link: "./class_WCharNode_3A6052D36C4D98759C119600DD3C0F295A540F02.html" }];
    drawDiagram(classes);
}

