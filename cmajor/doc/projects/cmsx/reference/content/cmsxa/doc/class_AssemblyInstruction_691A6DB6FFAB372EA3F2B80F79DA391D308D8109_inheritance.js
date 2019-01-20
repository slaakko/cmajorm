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
        { name: "cmsx.assembly.AssemblyInstruction", id: "diagram_node_0", level: 0, subject: true, hasDerivedNodes: true, link: "../../cmsxa/doc/class_AssemblyInstruction_691A6DB6FFAB372EA3F2B80F79DA391D308D8109.html" },
        { name: "cmsx.assembly.BranchInstruction", id: "diagram_node_1", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxa/doc/class_BranchInstruction_BFD0092AB63B41631CE91F5C54FFF663E1909246.html" },
        { name: "cmsx.assembly.ByteInstruction", id: "diagram_node_2", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxa/doc/class_ByteInstruction_F094BE0DA036806F1BD871C2E5511184F2D257AC.html" },
        { name: "cmsx.assembly.CallInstruction", id: "diagram_node_3", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxa/doc/class_CallInstruction_46269230A8F63D4AE97231E910516D6BB3801412.html" },
        { name: "cmsx.assembly.ExternInstruction", id: "diagram_node_5", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxa/doc/class_ExternInstruction_0BDDE82157630CB7F682A2998A8BB5CA8451D436.html" },
        { name: "cmsx.assembly.FloatingPointInstruction", id: "diagram_node_8", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxa/doc/class_FloatingPointInstruction_E3F6E7A5D63003482FC23BC36A0C0F15B223B116.html" },
        { name: "cmsx.assembly.FloatingRoundingInstruction", id: "diagram_node_7", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxa/doc/class_FloatingRoundingInstruction_E77F51B6D748AFF6DBA255BB94834EA58BB5BFA0.html" },
        { name: "cmsx.assembly.GetInstruction", id: "diagram_node_9", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxa/doc/class_GetInstruction_9545441F5C7BABF656E66555ED31BBB514FC30BA.html" },
        { name: "cmsx.assembly.GoInstruction", id: "diagram_node_11", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxa/doc/class_GoInstruction_0FB9411805899B8718B34B9BE4D4E5847ADBF56E.html" },
        { name: "cmsx.assembly.IsInstruction", id: "diagram_node_4", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxa/doc/class_IsInstruction_7EEB0462095DF3EFAD29DA109B8799935254CD55.html" },
        { name: "cmsx.assembly.JumpInstruction", id: "diagram_node_13", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxa/doc/class_JumpInstruction_1E773721463B47195B86E0549AE062CBF9788625.html" },
        { name: "cmsx.assembly.LinkOnceInstruction", id: "diagram_node_14", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxa/doc/class_LinkOnceInstruction_C98A05AB4BE41632FE878A4A3D7995E749EA2E95.html" },
        { name: "cmsx.assembly.LoadInstruction", id: "diagram_node_15", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxa/doc/class_LoadInstruction_40EE37DAF58B12F5022562ADB2B8075381029566.html" },
        { name: "cmsx.assembly.NegateInstruction", id: "diagram_node_16", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxa/doc/class_NegateInstruction_10C762B046ED87E32947C6693DDE7AC2A7829AFB.html" },
        { name: "cmsx.assembly.OctaInstruction", id: "diagram_node_17", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxa/doc/class_OctaInstruction_AD47BD468DD2A28B71A735F12FAF8D0D6937B429.html" },
        { name: "cmsx.assembly.PutInstruction", id: "diagram_node_18", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxa/doc/class_PutInstruction_9E08F1C06FE0ACCB961E71D00D3590E0EE3DF44E.html" },
        { name: "cmsx.assembly.RegisterInstruction", id: "diagram_node_12", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxa/doc/class_RegisterInstruction_4E97DCE4A875711160F17CC1EE114D0ABF1E8CA0.html" },
        { name: "cmsx.assembly.RetInstruction", id: "diagram_node_19", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxa/doc/class_RetInstruction_A18A8A064A45CDDA1B335AF9DFAA2F138455C8BC.html" },
        { name: "cmsx.assembly.SetInstruction", id: "diagram_node_20", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxa/doc/class_SetInstruction_586953088A61B3B8CF528AD2A3909E10A81DDC0F.html" },
        { name: "cmsx.assembly.StoreConstantInstruction", id: "diagram_node_6", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxa/doc/class_StoreConstantInstruction_8699B9BF63E500864295326DA78D4180CD167713.html" },
        { name: "cmsx.assembly.StoreInstruction", id: "diagram_node_21", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxa/doc/class_StoreInstruction_03516E91F549358CE38804D0A1B52072511A1A22.html" },
        { name: "cmsx.assembly.SwymInstruction", id: "diagram_node_22", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxa/doc/class_SwymInstruction_C664B2ECFDF61A4F1EF9CF0BBB98B14AB94A4E46.html" },
        { name: "cmsx.assembly.TetraInstruction", id: "diagram_node_10", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxa/doc/class_TetraInstruction_B481ACF98AB6347039996895EE79A3D775F6E52E.html" },
        { name: "cmsx.assembly.TrapInstruction", id: "diagram_node_23", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxa/doc/class_TrapInstruction_1A8DDB85F9DC0E165D4042FCF877F60C6CAF1043.html" },
        { name: "cmsx.assembly.WydeImmediateInstruction", id: "diagram_node_24", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxa/doc/class_WydeImmediateInstruction_A7EA4D45E92D295A58004FE7C43309AE09AE65C9.html" },
        { name: "cmsx.assembly.WydeInstruction", id: "diagram_node_25", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxa/doc/class_WydeInstruction_A4184735AE76D242F1D45EE6DF6AF3840D00CC0A.html" }];
    drawDiagram(diagramNodes);
}

