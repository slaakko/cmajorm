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
        { name: "cppast.Node", id: "diagram_node_0", level: 0, subject: true, hasDerivedNodes: true, link: "../../cppast/doc/class_Node_DEB477A450EF6E99A28A9855849F398D50BF1609.html" },
        { name: "cppast.AssignCapture", id: "diagram_node_4", level: 1, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_AssignCapture_28133785D186673042BA45320D4C49E7427E4B50.html" },
        { name: "cppast.BaseClassNode", id: "diagram_node_5", level: 1, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_BaseClassNode_30E8ED0B84339E3B5C9BE11AD6B41F507DAAA7F6.html" },
        { name: "cppast.BinaryNode", id: "diagram_node_7", level: 1, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_BinaryNode_69D6C5D662616978E829A18F735D960D5A59FCB4.html" },
        { name: "cppast.CatchAllNode", id: "diagram_node_8", level: 1, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_CatchAllNode_77C2AF85D8E8E26367F4CA9524B01F313FE9926D.html" },
        { name: "cppast.ClassNode", id: "diagram_node_9", level: 1, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_ClassNode_CBFA571C349CB1518C014B5DB86EE58DEAF8C1DB.html" },
        { name: "cppast.CompositeNode&lt;NodeT&gt;", id: "diagram_node_2", level: 1, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_CompositeNode_NodeT_2446AABC55658CCCD2E902CC14643EAE2863C51E.html" },
        { name: "cppast.ConditionalExpressionNode", id: "diagram_node_1", level: 1, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_ConditionalExpressionNode_FA29E6FDAFAB77076A1A6DECE6250969A6CDAD4A.html" },
        { name: "cppast.CppCastNode", id: "diagram_node_10", level: 1, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_CppCastNode_0603C49342B96235ADA53D1F950A6DEC7FEB7C90.html" },
        { name: "cppast.CtorInitializerNode", id: "diagram_node_6", level: 1, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_CtorInitializerNode_AD86F98C6D8204C08F102D495650AF7C4D53BD2C.html" },
        { name: "cppast.DefinedNode", id: "diagram_node_12", level: 1, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_DefinedNode_68DB0C20A6529E03AAD686F195D170D02D17CF65.html" },
        { name: "cppast.EnumeratorNode", id: "diagram_node_13", level: 1, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_EnumeratorNode_9C964F97DD8517ADEAD441F724CEBCFCE4ECB54E.html" },
        { name: "cppast.ExceptionHandlerNode", id: "diagram_node_14", level: 1, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_ExceptionHandlerNode_94D4FD102D23C096AA0D801E70F850BB3A5D7BAE.html" },
        { name: "cppast.ExplicitInstantiationNode", id: "diagram_node_15", level: 1, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_ExplicitInstantiationNode_364261A35B9A5B5A78BE3DAFB8C9CD8FDC2DAD44.html" },
        { name: "cppast.ExplicitSpecializationNode", id: "diagram_node_16", level: 1, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_ExplicitSpecializationNode_23DD5DC9C40E9AEC468A2D8FB97DC0F3267F02A2.html" },
        { name: "cppast.ForwardClassDeclarationNode", id: "diagram_node_18", level: 1, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_ForwardClassDeclarationNode_AD7470BB0BEE8531C255A1CA0ED7F77E55635DD1.html" },
        { name: "cppast.FunctionDeclaratorNode", id: "diagram_node_19", level: 1, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_FunctionDeclaratorNode_C1051AB956A088107E1CB54897FCBEE735BE4A7D.html" },
        { name: "cppast.FunctionNode", id: "diagram_node_20", level: 1, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_FunctionNode_E630158AF925A2B095B03AE4D473F48A83CCBC40.html" },
        { name: "cppast.IdentifierNode", id: "diagram_node_21", level: 1, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_IdentifierNode_F085A94295201408D19F6BCCAA6D30C97CF3ED1D.html" },
        { name: "cppast.LambdaExpressionNode", id: "diagram_node_22", level: 1, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_LambdaExpressionNode_B41592994B297007121DB6B416E19C5D1CC8157C.html" },
        { name: "cppast.LiteralNode", id: "diagram_node_23", level: 1, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_LiteralNode_6A6F766C97AA23566D6BB996C1D5D970C73FF5B1.html" },
        { name: "cppast.MemberDeclarationNode", id: "diagram_node_24", level: 1, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_MemberDeclarationNode_80DC1687A070A62FC2F5F1668E7389AB09041902.html" },
        { name: "cppast.MemberInitializerNode", id: "diagram_node_17", level: 1, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_MemberInitializerNode_ADA8CCC59C410CF54AC25268EED5079F207043EB.html" },
        { name: "cppast.NewExpressionNode", id: "diagram_node_25", level: 1, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_NewExpressionNode_F210F3CEB93B7B9E7096007599EEBF523845A8D9.html" },
        { name: "cppast.OperatorFunctionIdNode", id: "diagram_node_3", level: 1, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_OperatorFunctionIdNode_79359FB1F6FF96BC7DF574CC5714F90F769F2CC7.html" },
        { name: "cppast.ParameterNode", id: "diagram_node_26", level: 1, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_ParameterNode_EA4C5640D5B0347E60EE3F21BAFCE0CCBB1C17F4.html" },
        { name: "cppast.RefCapture", id: "diagram_node_27", level: 1, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_RefCapture_494D41AFA537C79130EDFE2D8E584F5F6E2E4A0D.html" },
        { name: "cppast.SimpleDeclarationNode", id: "diagram_node_28", level: 1, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_SimpleDeclarationNode_75E59B7426F3C7156EAD9DA9D415F64FDD963719.html" },
        { name: "cppast.SimpleTypeNode", id: "diagram_node_11", level: 1, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_SimpleTypeNode_B6F9F7C04FEF26BD651DCF38D56809D91A1B5119.html" },
        { name: "cppast.SpecialMemberFunctionNode", id: "diagram_node_30", level: 1, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_SpecialMemberFunctionNode_3B61C058AF638CD5B36D17C036FF631FBB10E07C.html" },
        { name: "cppast.StatementNode", id: "diagram_node_31", level: 1, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_StatementNode_874DBF1445D87E0ED6EA88B30D1110E9B8B61359.html" },
        { name: "cppast.TemplateDeclarationNode", id: "diagram_node_32", level: 1, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_TemplateDeclarationNode_F7E6A44571890BFDA9401B394ADC65DB25B063A5.html" },
        { name: "cppast.TemplateParameterNode", id: "diagram_node_34", level: 1, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_TemplateParameterNode_876364C44AE8C25EFF83D974742C876BD45EACA4.html" },
        { name: "cppast.ThisCapture", id: "diagram_node_29", level: 1, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_ThisCapture_37DA6F93D91FECFB7B5AFCF2B1E8FB345CC5581B.html" },
        { name: "cppast.ThisNode", id: "diagram_node_35", level: 1, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_ThisNode_DF4B17144B6D03DE64EF8DB448577700F998696E.html" },
        { name: "cppast.UnaryNode", id: "diagram_node_36", level: 1, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_UnaryNode_EAFF99520EE6A633B0DE72A169C3BA5ABA76AB03.html" },
        { name: "cppast.UsingTypeDeclarationNode", id: "diagram_node_33", level: 1, subject: false, hasDerivedNodes: false, link: "../../cppast/doc/class_UsingTypeDeclarationNode_0628E97268F27CF9EB11E953271982F96214E528.html" }];
    drawDiagram(diagramNodes);
}

