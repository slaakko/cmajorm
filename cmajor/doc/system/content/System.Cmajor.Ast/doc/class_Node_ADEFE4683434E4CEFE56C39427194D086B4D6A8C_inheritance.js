// JavaScript source code for drawing class inheritance diagrams and concept refinement diagrams

function pick(level, diagramNodes) {
    var levelNodes = [];
    var n = diagramNodes.length;
    for (var i = 0; i < n; ++i) {
        var c = diagramNodes[i];
        if (c.level == level) {
            levelNodes.push(c);
        }
    }
    return levelNodes;
}

function createDiagramNodeElements(levelNodes, maxTextWidth, maxTextHeight) {
    var textDimensions = { width: maxTextWidth, height: maxTextHeight };
    var n = levelNodes.length;
    for (var i = 0; i < n; ++i) {
        var levelNode = levelNodes[i];
        var svg = document.getElementById("inheritance_svg_diagram");
        var rectElement = document.createElementNS('http://www.w3.org/2000/svg', 'rect');
        rectElement.setAttribute("id", levelNode.id + "_rect");
        var linkElement = document.createElementNS('http://www.w3.org/2000/svg', 'a');
        linkElement.setAttribute("href", levelNode.link);
        var textElement = document.createElementNS('http://www.w3.org/2000/svg', 'text');
        linkElement.appendChild(textElement);
        textElement.setAttribute("id", levelNode.id + "_text");
        textElement.innerHTML = levelNode.name;
        svg.appendChild(rectElement);
        svg.appendChild(linkElement);
        var bb = textElement.getBBox();
        var textWidth = bb.width;
        var textHeight = bb.height;
        levelNode.textWidth = textWidth;
        levelNode.textHeight = textHeight;
        if (textWidth > textDimensions.width) {
            textDimensions.width = textWidth;
        }
        if (textHeight > textDimensions.height) {
            textDimensions.height = textHeight;
        }
    }
    return textDimensions;
}

function drawDiagram(diagramNodes) {
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
    var allLevelNodes = [];
    while (cont) {
        var levelNodes = pick(level, diagramNodes);
        var n = levelNodes.length;
        cont = n > 0;
        if (cont) {
            var textDimensions = createDiagramNodeElements(levelNodes, maxTextWidth, maxTextHeight);
            if (textDimensions.width > maxTextWidth) {
                maxTextWidth = textDimensions.width;
            }
            if (textDimensions.height > maxTextHeight) {
                maxTextHeight = textDimensions.height;
            }
            allLevelNodes.push(levelNodes);
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
        var levelNodes = allLevelNodes[level];
        var n = levelNodes.length;
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
    var svg = document.getElementById("inheritance_svg_diagram");
    svg.setAttribute("width", totalWidth.toString());
    svg.setAttribute("height", totalHeight.toString());
    var prevRectY = 0;
    var prevRectX = 0;
    var prevHandleX2 = -1;
    var prevHandleY2 = -1;
    var prevY = 0;
    for (level = 0; level < maxLevel; ++level) {
        var direction = levelDirection[level];
        var levelNodes = allLevelNodes[level];
        var n = levelNodes.length;
        var rectY = prevY;
        prevY += levelHeight[level];
        var rectX = (totalWidth / n - rectWidth) / 2;
        var minHandleX = Number.MAX_SAFE_INTEGER;
        var maxHandleX = 0;
        var handleY = 0;
        for (var i = 0; i < n; ++i) {
            var levelNode = levelNodes[i];
            var textWidth = levelNode.textWidth;
            var textHeight = levelNode.textHeight;
            if (direction == horizontalDirection) {
                rectX = (totalWidth / n - rectWidth) / 2 + i * (rectWidth + rectXSpace);
            }
            else if (direction == verticalDirection) {
                rectX = prevRectX + (rectWidth + rectXSpace);
                rectY = prevRectY + horizontalRectYSpace + i * (rectHeight + verticalRectYSpace);
            }
            var textX = rectX + (rectWidth - textWidth) / 2;
            var textY = (rectY + rectHeight - yspace / 2) - (rectHeight - textHeight) / 2;
            var rectElement = document.getElementById(levelNode.id + "_rect");
            rectElement.setAttribute("x", rectX.toString());
            rectElement.setAttribute("y", rectY.toString());
            rectElement.setAttribute("width", rectWidth.toString());
            rectElement.setAttribute("height", rectHeight.toString());
            var fillColor = "white";
            if (levelNode.subject) {
                fillColor = "floralWhite";
            }
            rectElement.setAttribute("fill", fillColor);
            rectElement.setAttribute("stroke", "black");
            var textElement = document.getElementById(levelNode.id + "_text");
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
            } else if (level == maxLevel - 1 && levelNode.hasDerivedNodes) {
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

function drawInheritanceDiagram() {
    var diagramNodes = [
        { name: "System.Cmajor.Ast.Node", id: "diagram_node_0", level: 0, subject: true, hasDerivedNodes: true, link: "../../System.Cmajor.Ast/doc/class_Node_ADEFE4683434E4CEFE56C39427194D086B4D6A8C.html" },
        { name: "System.Cmajor.Ast.AliasNode", id: "diagram_node_7", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_AliasNode_A0C8437565CD54C709845AA488DDB516031E2B19.html" },
        { name: "System.Cmajor.Ast.ArrayNode", id: "diagram_node_9", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_ArrayNode_632CA0F61B32920389B61E99142A9FC391740603.html" },
        { name: "System.Cmajor.Ast.AsNode", id: "diagram_node_11", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_AsNode_0CE9516C4EA7E0E9192E464AF041373549E78A5B.html" },
        { name: "System.Cmajor.Ast.AxiomNode", id: "diagram_node_8", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_AxiomNode_3FDE6B70A18A5622F7E9612AA96E8865A2B31F92.html" },
        { name: "System.Cmajor.Ast.AxiomStatementNode", id: "diagram_node_3", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_AxiomStatementNode_3E778510CD671A6E7A958467C27A53B847FE8403.html" },
        { name: "System.Cmajor.Ast.BaseNode", id: "diagram_node_15", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_BaseNode_1D07E0EDC08B323B247934CCF95CCE933B8AA353.html" },
        { name: "System.Cmajor.Ast.BinaryNode", id: "diagram_node_13", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_BinaryNode_D003835B7706B8333E09F7899ACE3E0515114C8E.html" },
        { name: "System.Cmajor.Ast.BoolNode", id: "diagram_node_17", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_BoolNode_4CC25A719FF6FAF1D051A42A4112872786343E5C.html" },
        { name: "System.Cmajor.Ast.ByteNode", id: "diagram_node_14", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_ByteNode_460351D7277B4A414FD27F8DF256374D203CE508.html" },
        { name: "System.Cmajor.Ast.CastNode", id: "diagram_node_18", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_CastNode_BF411D28164C48B32718A53B9809D1373691E690.html" },
        { name: "System.Cmajor.Ast.CatchNode", id: "diagram_node_5", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_CatchNode_0B0329EDD3A1D45296A519F9DA96BC39C7B00745.html" },
        { name: "System.Cmajor.Ast.CharNode", id: "diagram_node_19", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_CharNode_719D5B6BF31B29C218E032528E984E82148EC8A4.html" },
        { name: "System.Cmajor.Ast.ClassDelegateNode", id: "diagram_node_21", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_ClassDelegateNode_8BD62C646842337A24017C444C055E738DAF2E9F.html" },
        { name: "System.Cmajor.Ast.ClassNode", id: "diagram_node_1", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_ClassNode_EA71C3906C8C8DC34EC24EA9CC36C84C91EECBA4.html" },
        { name: "System.Cmajor.Ast.CommentNode", id: "diagram_node_23", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_CommentNode_5699AAC5019EDFC3C10E6336ED592E6EE0B83F4B.html" },
        { name: "System.Cmajor.Ast.CompileUnitNode", id: "diagram_node_25", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_CompileUnitNode_16455AE52647861631419CCD604F849393F3C00B.html" },
        { name: "System.Cmajor.Ast.ConceptIdNode", id: "diagram_node_26", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_ConceptIdNode_58D69F7D33AFB84DE2288C35159485EE0F92CE0C.html" },
        { name: "System.Cmajor.Ast.ConceptNode", id: "diagram_node_27", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_ConceptNode_EC0C5691EE9EA0762DC299B5C0FF6058E9FA197C.html" },
        { name: "System.Cmajor.Ast.ConditionalCompilationExpressionNode", id: "diagram_node_22", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_ConditionalCompilationExpressionNode_CD2E40496D9468896CA1AE1CBE23B5752433859E.html" },
        { name: "System.Cmajor.Ast.ConditionalCompilationPartNode", id: "diagram_node_32", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_ConditionalCompilationPartNode_44A3AD370789F29E1F4CA334E131003346D25D70.html" },
        { name: "System.Cmajor.Ast.ConstNode", id: "diagram_node_20", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_ConstNode_7933644CFBB4317E34BF00064C0F65B0ECA7F68C.html" },
        { name: "System.Cmajor.Ast.ConstantNode", id: "diagram_node_34", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_ConstantNode_BAB197E33A60796AD4AE1A689FD15494B315ADC1.html" },
        { name: "System.Cmajor.Ast.ConstraintNode", id: "diagram_node_36", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_ConstraintNode_4E3455ADFC90EB58AF20DFC2C37205DD6EB4A358.html" },
        { name: "System.Cmajor.Ast.ConstructNode", id: "diagram_node_38", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_ConstructNode_63AE5DC920FF926199472B5F737C4569D93E8C19.html" },
        { name: "System.Cmajor.Ast.DelegateNode", id: "diagram_node_16", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_DelegateNode_6E9EB782C891719BF801F5AD20F7D4D6980109C5.html" },
        { name: "System.Cmajor.Ast.DoubleNode", id: "diagram_node_40", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_DoubleNode_456DD349F398B05C71FDB76E75E36120BECA0A65.html" },
        { name: "System.Cmajor.Ast.EnumConstantNode", id: "diagram_node_42", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_EnumConstantNode_B03A5A8EC69096A366C0B9D5A17E865E5DF4A72E.html" },
        { name: "System.Cmajor.Ast.EnumTypeNode", id: "diagram_node_47", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_EnumTypeNode_D9E7AAF808634609BB8EDFCDEB0B7619001201A3.html" },
        { name: "System.Cmajor.Ast.FloatNode", id: "diagram_node_12", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_FloatNode_9CFC47DC79D2E61AF70A35628224D45F2FE1E9E0.html" },
        { name: "System.Cmajor.Ast.FunctionNode", id: "diagram_node_31", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_FunctionNode_E71AC74C8D015BC5C1AE3CE1C6C15A8D6093E73C.html" },
        { name: "System.Cmajor.Ast.GlobalVariableNode", id: "diagram_node_48", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_GlobalVariableNode_533D75DADAE5DCBA38A71F75E4B96CC4B2B4521C.html" },
        { name: "System.Cmajor.Ast.IdentifierNode", id: "diagram_node_6", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_IdentifierNode_BC52239F4B1E70E7586C74CED9816534809D7213.html" },
        { name: "System.Cmajor.Ast.IndexingNode", id: "diagram_node_39", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_IndexingNode_E75CE115D11F3A4E4FA9A95A4B97B2CA9EFF3536.html" },
        { name: "System.Cmajor.Ast.InitializerNode", id: "diagram_node_49", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_InitializerNode_77F130651536C850CE0EDB164C47569A19F794F4.html" },
        { name: "System.Cmajor.Ast.IntNode", id: "diagram_node_46", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_IntNode_37DCBB1BED3FDABA8FA21332F2F7A8A826687935.html" },
        { name: "System.Cmajor.Ast.InterfaceNode", id: "diagram_node_50", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_InterfaceNode_04F17948C40935D62F6FA70013130BB187A1E4B0.html" },
        { name: "System.Cmajor.Ast.InvokeNode", id: "diagram_node_51", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_InvokeNode_BAC50B368165DE376981934E5FB77D9D8DFB7D35.html" },
        { name: "System.Cmajor.Ast.IsNode", id: "diagram_node_52", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_IsNode_E274EAB0D4CC882A393FD4AF1E1DD0B8DE012F21.html" },
        { name: "System.Cmajor.Ast.LValueRefNode", id: "diagram_node_30", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_LValueRefNode_477512C32F6D564F9126162E8B003003779BB0FF.html" },
        { name: "System.Cmajor.Ast.LabelNode", id: "diagram_node_4", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_LabelNode_7C5AE6929BB3B17DB1CABFDE2AC9818E89354E66.html" },
        { name: "System.Cmajor.Ast.LiteralNode", id: "diagram_node_29", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_LiteralNode_554A37F9FC552316EAE879B4EBEA292F9CCD3C93.html" },
        { name: "System.Cmajor.Ast.LongNode", id: "diagram_node_53", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_LongNode_6E837BA6C4E602FF3A2AAA48BFECA1C71B0F799A.html" },
        { name: "System.Cmajor.Ast.MemberVariableNode", id: "diagram_node_54", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_MemberVariableNode_AC8F645AEA77B089BA0BB1A6A4317CFE92C914D1.html" },
        { name: "System.Cmajor.Ast.NamespaceImportNode", id: "diagram_node_24", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_NamespaceImportNode_44B7560E0324FF6B4091C2CB30DCBAD974FF24DF.html" },
        { name: "System.Cmajor.Ast.NamespaceNode", id: "diagram_node_45", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_NamespaceNode_EC69A308CCAC1BE4A0658D05763290B5DF88DFD9.html" },
        { name: "System.Cmajor.Ast.NewNode", id: "diagram_node_44", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_NewNode_730BD959654C528BAAD1A7FDABA81BCA0151BDEF.html" },
        { name: "System.Cmajor.Ast.ParameterNode", id: "diagram_node_55", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_ParameterNode_0658DBEEB426A5314CA539B2CB221FF379DA7FE7.html" },
        { name: "System.Cmajor.Ast.PointerNode", id: "diagram_node_2", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_PointerNode_2E518D7EE922A87059347F9EC8577BAE246A6A70.html" },
        { name: "System.Cmajor.Ast.RValueRefNode", id: "diagram_node_35", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_RValueRefNode_E3E4387390D57059797AAFD15AF453EEA60A0AF4.html" },
        { name: "System.Cmajor.Ast.SByteNode", id: "diagram_node_56", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_SByteNode_856066FC8748ADFBE7913B0BD03206DC194D6C65.html" },
        { name: "System.Cmajor.Ast.ShortNode", id: "diagram_node_33", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_ShortNode_D7633EFBA992C96AFA72D2EE729DA52DFD5D2A65.html" },
        { name: "System.Cmajor.Ast.SizeOfNode", id: "diagram_node_37", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_SizeOfNode_A3C7961AA31F47D9135D18B73A7B3562AFDAED33.html" },
        { name: "System.Cmajor.Ast.StatementNode", id: "diagram_node_57", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_StatementNode_CA36A99767186B308040332BA6EB36117815F37E.html" },
        { name: "System.Cmajor.Ast.TemplateIdNode", id: "diagram_node_58", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_TemplateIdNode_AAA1077003CD878677FF2E9B4D0447444D361FDD.html" },
        { name: "System.Cmajor.Ast.TemplateParameterNode", id: "diagram_node_59", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_TemplateParameterNode_B03DE3D136F66F572F2D5A876EB93D57C0ED293A.html" },
        { name: "System.Cmajor.Ast.ThisNode", id: "diagram_node_60", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_ThisNode_18C195A592B559CE9A602184720E4D96CB4170E1.html" },
        { name: "System.Cmajor.Ast.TypeIdNode", id: "diagram_node_61", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_TypeIdNode_8CC8C4E1222965C1F12A8756963CC6CD1C5EB164.html" },
        { name: "System.Cmajor.Ast.TypeNameNode", id: "diagram_node_10", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_TypeNameNode_6EF0AA645491320E23E932CD85A7DE1CC8B112CD.html" },
        { name: "System.Cmajor.Ast.TypedefNode", id: "diagram_node_43", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_TypedefNode_CD1D573DF88D8B64DD86C459B5ED6E9EDF52B22A.html" },
        { name: "System.Cmajor.Ast.UCharNode", id: "diagram_node_41", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_UCharNode_7B37DD5CAF192EB62D92168DC490993D51A28675.html" },
        { name: "System.Cmajor.Ast.UIntNode", id: "diagram_node_62", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_UIntNode_AF349501BD5D7897F0BA850D8422A247BBBDC1E4.html" },
        { name: "System.Cmajor.Ast.ULongNode", id: "diagram_node_63", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_ULongNode_42CE3464A146278D6A0ABD9048A5EC85D41387D2.html" },
        { name: "System.Cmajor.Ast.UShortNode", id: "diagram_node_28", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_UShortNode_A31A40A608EF4D155F8DDA0D1165511705E5EA8C.html" },
        { name: "System.Cmajor.Ast.UnaryNode", id: "diagram_node_64", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_UnaryNode_3948D817A3D79CE9F2DC408A09984A0CD307F53B.html" },
        { name: "System.Cmajor.Ast.VoidNode", id: "diagram_node_65", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_VoidNode_9B1B016F708D142C0C0256DB8B7135542A7C5AB6.html" },
        { name: "System.Cmajor.Ast.WCharNode", id: "diagram_node_66", level: 1, subject: false, hasDerivedNodes: false, link: "../../System.Cmajor.Ast/doc/class_WCharNode_CC2BC532F69C1AA5C3404C67123D170BF3F951BB.html" }];
    drawDiagram(diagramNodes);
}

