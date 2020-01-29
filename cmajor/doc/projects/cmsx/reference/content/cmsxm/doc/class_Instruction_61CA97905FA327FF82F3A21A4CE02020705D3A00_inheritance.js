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
        { name: "cmsx.machine.Instruction", id: "diagram_node_0", level: 0, subject: true, hasDerivedNodes: true, link: "../../cmsxm/doc/class_Instruction_61CA97905FA327FF82F3A21A4CE02020705D3A00.html" },
        { name: "cmsx.machine.Add", id: "diagram_node_3", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Add_C8290F1F0CF2A60C2D987CE8BE5AB71EF0C1C530.html" },
        { name: "cmsx.machine.Addi", id: "diagram_node_7", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Addi_89B13CBC570FF7B2AB0E5B9672610330F554010B.html" },
        { name: "cmsx.machine.Addu", id: "diagram_node_9", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Addu_BAC20B81E4181DA5A76E997651A6B7E089E775DF.html" },
        { name: "cmsx.machine.Addui", id: "diagram_node_11", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Addui_C010BC336B034635FEC2776063094D0CE99492AD.html" },
        { name: "cmsx.machine.And", id: "diagram_node_14", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_And_99A79F523B3F64345CA36DCE43BD467ACD01889B.html" },
        { name: "cmsx.machine.Andi", id: "diagram_node_5", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Andi_9A5D2C3F6CFA415973D0BE714B25F85A3935008A.html" },
        { name: "cmsx.machine.Andn", id: "diagram_node_2", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Andn_72A57B679811A8170291FF5A4B9C041EB454D247.html" },
        { name: "cmsx.machine.Andnh", id: "diagram_node_12", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Andnh_143BFE66F2A35918EEAFE07C511558F63B02A271.html" },
        { name: "cmsx.machine.Andni", id: "diagram_node_16", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Andni_65E2246484AEEB9C6983D80BF781C7457F9AF619.html" },
        { name: "cmsx.machine.Andnl", id: "diagram_node_20", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Andnl_A05FF66DE4BFC85A648BF5BBE8536F0F97BA7E00.html" },
        { name: "cmsx.machine.Andnmh", id: "diagram_node_26", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Andnmh_B443CA9E0F569F5724BF4141E34ADC8BF1D05E14.html" },
        { name: "cmsx.machine.Andnml", id: "diagram_node_27", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Andnml_14E5123066A32B42F876C3ECDD55E244152CFB3F.html" },
        { name: "cmsx.machine.Bdif", id: "diagram_node_28", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Bdif_F96592F1AC0CEE2A146A258C7C0F4BF2D1A80B5E.html" },
        { name: "cmsx.machine.Bdifi", id: "diagram_node_30", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Bdifi_09BFE1EFEACE36109DF2D63ED4E91203D3BD8593.html" },
        { name: "cmsx.machine.Bev", id: "diagram_node_32", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Bev_93E2D2F9A0D4905A2A2CE7E99B2A1ECE157F700C.html" },
        { name: "cmsx.machine.Bevb", id: "diagram_node_34", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Bevb_D21CCA979EE14C19888C49AB2E753AA6AAF24EB8.html" },
        { name: "cmsx.machine.Bn", id: "diagram_node_37", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Bn_6588E6CC661D3B23FE03E2BAA163F102A62823D6.html" },
        { name: "cmsx.machine.Bnb", id: "diagram_node_38", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Bnb_70D159024382F2B1E4F25F6B434B72A64060954D.html" },
        { name: "cmsx.machine.Bnn", id: "diagram_node_24", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Bnn_9BB77FF8FF0AED95F558D86EC61211B6D7510A52.html" },
        { name: "cmsx.machine.Bnnb", id: "diagram_node_39", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Bnnb_D558E60761996144009CA4E3A4C894B3AA326539.html" },
        { name: "cmsx.machine.Bnp", id: "diagram_node_42", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Bnp_81AC9CF8022BDD06C57B42D99D89001286F07BFF.html" },
        { name: "cmsx.machine.Bnpb", id: "diagram_node_43", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Bnpb_A1571F2A1B8234D0AFD080B4540109CEFCA760D7.html" },
        { name: "cmsx.machine.Bnz", id: "diagram_node_44", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Bnz_95E7B727D6F54788DAC56B420E95F395C166C6A4.html" },
        { name: "cmsx.machine.Bnzb", id: "diagram_node_40", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Bnzb_B768157B56293CC8381E16E659219839D11E5814.html" },
        { name: "cmsx.machine.Bod", id: "diagram_node_8", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Bod_9261D75224F9139048607B77DF409F3969D3C46F.html" },
        { name: "cmsx.machine.Bodb", id: "diagram_node_22", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Bodb_7EE7D6983B804D9AE2D8393EE5C41287E8251F14.html" },
        { name: "cmsx.machine.Bp", id: "diagram_node_31", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Bp_A9296D7DE323B650C6A9A63F0F9E2ABD1E7AB8FF.html" },
        { name: "cmsx.machine.Bpb", id: "diagram_node_46", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Bpb_9BAAC51ABDDC7888A61A13C3FCC27AC63B3AEA99.html" },
        { name: "cmsx.machine.Bz", id: "diagram_node_48", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Bz_86027E4E03A2FE8E95DF247CBDA10FC7CBAAA88B.html" },
        { name: "cmsx.machine.Bzb", id: "diagram_node_52", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Bzb_AD5F7C89C4173D41E3206D6BD5604D38C5C384FE.html" },
        { name: "cmsx.machine.Call", id: "diagram_node_53", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Call_A038F7C69C1AA07077BF859ABFDE6E1DCB257F0D.html" },
        { name: "cmsx.machine.Calli", id: "diagram_node_56", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Calli_504F138A79EBE806FD148BFF623FD215C38368DF.html" },
        { name: "cmsx.machine.Cmp", id: "diagram_node_57", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Cmp_90C0810557E6403552A323C1E798AEFA626B6F1E.html" },
        { name: "cmsx.machine.Cmpi", id: "diagram_node_21", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Cmpi_619A9F2C407C63B016E253476CDAE35E802A2D1C.html" },
        { name: "cmsx.machine.Cmpu", id: "diagram_node_59", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Cmpu_68058307EABEF8B61A0048BDE0759414CAFCDBFF.html" },
        { name: "cmsx.machine.Cmpui", id: "diagram_node_61", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Cmpui_B98AF3C9816130D111B2A339BE6111C61C1229D8.html" },
        { name: "cmsx.machine.Csev", id: "diagram_node_62", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Csev_0B4314BE96C8B84D1A498C2BE931657414F41DFF.html" },
        { name: "cmsx.machine.Csevi", id: "diagram_node_63", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Csevi_D572F788820D57DB1B3260BF7042413BBFFAFC75.html" },
        { name: "cmsx.machine.Csn", id: "diagram_node_65", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Csn_2D490744A197C07716F1D47BF3B4F114D82C04A9.html" },
        { name: "cmsx.machine.Csni", id: "diagram_node_66", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Csni_80D2BB94C427955544F6585802398AD4834493F4.html" },
        { name: "cmsx.machine.Csnn", id: "diagram_node_70", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Csnn_00F4F0E349753170AF0994808C55FB97AEC70BB0.html" },
        { name: "cmsx.machine.Csnni", id: "diagram_node_73", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Csnni_FEC941A19D90EB6A46CC2CE9074C6CAFF0D93AB2.html" },
        { name: "cmsx.machine.Csnp", id: "diagram_node_19", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Csnp_1674D79387D5DB0DB06B84134EA29E209756C9A7.html" },
        { name: "cmsx.machine.Csnpi", id: "diagram_node_74", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Csnpi_C83147AD2FA731C37FAEE1AC9C2D4FDB1A0A4D94.html" },
        { name: "cmsx.machine.Csnz", id: "diagram_node_71", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Csnz_7A0AE7AB83FE408795E73446B52850102D7D1AB1.html" },
        { name: "cmsx.machine.Csnzi", id: "diagram_node_76", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Csnzi_FEA98E68E0D308C70FA0734C7A8896B35CEACD6A.html" },
        { name: "cmsx.machine.Csod", id: "diagram_node_45", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Csod_9AD9B1C6DF41431572E430D437B69B6EA85A8EA4.html" },
        { name: "cmsx.machine.Csodi", id: "diagram_node_58", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Csodi_69014A54B0787175CFD37BA85F5C387A036C7D29.html" },
        { name: "cmsx.machine.Csp", id: "diagram_node_60", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Csp_EADEEA604B35F3ACD28B05805DF82C073BA7DDBA.html" },
        { name: "cmsx.machine.Cspi", id: "diagram_node_15", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Cspi_9955AF24EB4ABB4F3A1F1A6D55F5D183F4119F2B.html" },
        { name: "cmsx.machine.Csz", id: "diagram_node_75", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Csz_E70976C4100318E12F0BE3E010F301D96C7EBD01.html" },
        { name: "cmsx.machine.Cszi", id: "diagram_node_68", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Cszi_F241137856575FE95D698C919D7DE1AEDF1D12FB.html" },
        { name: "cmsx.machine.Div", id: "diagram_node_78", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Div_E867D1D0E87CE661228027F63AF140A5DAFC32BB.html" },
        { name: "cmsx.machine.Divi", id: "diagram_node_54", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Divi_20ED53C5A9FED433FC2F749139ACB5DF8E699B9A.html" },
        { name: "cmsx.machine.Divu", id: "diagram_node_18", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Divu_09D25C7ACB32DA4F261EA5426A2359C19C00A937.html" },
        { name: "cmsx.machine.Divui", id: "diagram_node_64", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Divui_9DE439E48A88C3F8D9D5F916B74F7BF930BE673B.html" },
        { name: "cmsx.machine.Fadd", id: "diagram_node_50", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Fadd_D52EF64512E1D604F48F21FFA5972186268FEF58.html" },
        { name: "cmsx.machine.Fcmp", id: "diagram_node_79", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Fcmp_8F5F8A3A549434A21C5746489918443295E81097.html" },
        { name: "cmsx.machine.Fdiv", id: "diagram_node_6", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Fdiv_FD495A4DA0971B88F35A42007874B833302604B9.html" },
        { name: "cmsx.machine.Fint", id: "diagram_node_67", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Fint_D173A01178E7374F977569FF1470923BCFBB50A6.html" },
        { name: "cmsx.machine.Fix", id: "diagram_node_80", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Fix_B81312D43D42161F98C7937B560A413AF1CE335C.html" },
        { name: "cmsx.machine.Fixu", id: "diagram_node_29", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Fixu_4689DFC40405F945EEA060FDD139B7B8C0D1CF71.html" },
        { name: "cmsx.machine.Flot", id: "diagram_node_77", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Flot_7CB2780C6195FCA0C28C7A4D534E7CC03FDBE493.html" },
        { name: "cmsx.machine.Floti", id: "diagram_node_36", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Floti_88A65442767653E19E9986032B59DC8DA47F9A47.html" },
        { name: "cmsx.machine.Flotu", id: "diagram_node_81", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Flotu_27B5D8F3E6CE544FC2FFAE7AB3BE38C4773CB0D5.html" },
        { name: "cmsx.machine.Flotui", id: "diagram_node_82", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Flotui_0E579AE75DC16B9332FDA8B34C2EBB1A3E37346C.html" },
        { name: "cmsx.machine.Fmul", id: "diagram_node_84", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Fmul_141D288776E6E6F48C8CFF12631533AA5AEABEB7.html" },
        { name: "cmsx.machine.Fsub", id: "diagram_node_86", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Fsub_F81328D5608EAAE9A451CC6CD81A1EBA687941BC.html" },
        { name: "cmsx.machine.Get", id: "diagram_node_87", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Get_578625131FFF4CD292E74C3BB3D648021113645D.html" },
        { name: "cmsx.machine.GetA", id: "diagram_node_88", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_GetA_60338104C339B274B8AE57B2BC65A01ADF99800E.html" },
        { name: "cmsx.machine.GetAb", id: "diagram_node_89", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_GetAb_B8D3AD45FDCDE5FAC31CE6995B86F41C4C8AD519.html" },
        { name: "cmsx.machine.Go", id: "diagram_node_90", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Go_09378E236B1DB685C89AC5512DF55B4392179ADA.html" },
        { name: "cmsx.machine.Goi", id: "diagram_node_91", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Goi_0EABDC02741982494E27E411CF5BB2A89A6751A4.html" },
        { name: "cmsx.machine.I16Addu", id: "diagram_node_92", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_I16Addu_A20352681089B95F1B55A145863AAC407CEF31D4.html" },
        { name: "cmsx.machine.I16Addui", id: "diagram_node_93", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_I16Addui_0F07018092A84CE2A657906B0656ED3C37CEB16B.html" },
        { name: "cmsx.machine.I2Addu", id: "diagram_node_1", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_I2Addu_DC30A794D033E8B0720A967FEE42D6376BE6CAB8.html" },
        { name: "cmsx.machine.I2Addui", id: "diagram_node_95", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_I2Addui_3BE435890EA2252EBA511CC418A85D0B8981D375.html" },
        { name: "cmsx.machine.I4Addu", id: "diagram_node_97", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_I4Addu_10CC9EBB637B1D086B91617B259D4C9FA6A44B18.html" },
        { name: "cmsx.machine.I4Addui", id: "diagram_node_25", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_I4Addui_8D8B18E3D847FABFF752A60793FCC68B59D63C55.html" },
        { name: "cmsx.machine.I8Addu", id: "diagram_node_13", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_I8Addu_72612A3B8F6C3AFE4BFDAA707F0150BDC1332A1C.html" },
        { name: "cmsx.machine.I8Addui", id: "diagram_node_98", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_I8Addui_B223BE20439452556C639088EF8C7F8FAC570DDC.html" },
        { name: "cmsx.machine.Inch", id: "diagram_node_94", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Inch_EFE0836DB1D9D1097AA376A533CDF6F7FCE1C719.html" },
        { name: "cmsx.machine.Incl", id: "diagram_node_99", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Incl_D543AA7FC3B38295F6FA32C128D88195DC54FE02.html" },
        { name: "cmsx.machine.Incmh", id: "diagram_node_100", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Incmh_738FA74D7065E61EE1A60A7DC6FC848A5AA5F3AC.html" },
        { name: "cmsx.machine.Incml", id: "diagram_node_101", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Incml_7B1232993018516D5D9565E3B265766E562BB1BE.html" },
        { name: "cmsx.machine.Jmp", id: "diagram_node_102", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Jmp_5C555B2F15C19F0C00241D731DC71D78D049BD44.html" },
        { name: "cmsx.machine.Jmpb", id: "diagram_node_103", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Jmpb_3CB0DB6047D2010B4C74F62E1DD3B471A311009F.html" },
        { name: "cmsx.machine.Ldb", id: "diagram_node_69", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Ldb_27037E4E427DD06CB9E45A24E1021F5FF9ED9C70.html" },
        { name: "cmsx.machine.Ldbi", id: "diagram_node_104", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Ldbi_7FF84006BD17C90289BDA59CE5C5F539AE18A24D.html" },
        { name: "cmsx.machine.Ldbu", id: "diagram_node_105", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Ldbu_81658BA6F4FAD059E157E5808018820AE3352225.html" },
        { name: "cmsx.machine.Ldbui", id: "diagram_node_106", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Ldbui_CF4A3ABDDE2CD3C580C665A3340EB4BB7CED29E5.html" },
        { name: "cmsx.machine.Ldht", id: "diagram_node_107", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Ldht_170AD26652EE4B4F7D13013489E71BD6A12B0289.html" },
        { name: "cmsx.machine.Ldhti", id: "diagram_node_109", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Ldhti_2FC3330D57331F78F5E2931F5D48473882882AD1.html" },
        { name: "cmsx.machine.Ldo", id: "diagram_node_108", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Ldo_1B876ACDAC8C7D64414C3B26804A385FEA8708FC.html" },
        { name: "cmsx.machine.Ldoi", id: "diagram_node_112", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Ldoi_DD7EA43C14BFED1599E06AA8E70C7B838D29658A.html" },
        { name: "cmsx.machine.Ldou", id: "diagram_node_113", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Ldou_24D33A158782D36BD554C5AAF3BC8698F2B9B48C.html" },
        { name: "cmsx.machine.Ldoui", id: "diagram_node_114", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Ldoui_E15CD504394B7D8A8EFE91805BADB4AF13C166CC.html" },
        { name: "cmsx.machine.Ldsf", id: "diagram_node_55", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Ldsf_CEC65DDDBB6741BFD2E462FF8F3E5411FBA59132.html" },
        { name: "cmsx.machine.Ldsfi", id: "diagram_node_115", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Ldsfi_566150CF0BB6AB3C91ED7E1A67CF9B414A6AAF62.html" },
        { name: "cmsx.machine.Ldt", id: "diagram_node_111", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Ldt_765367E35BDEB8C0A9773AB57059BC7248542AC8.html" },
        { name: "cmsx.machine.Ldti", id: "diagram_node_116", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Ldti_7EF882F109E7637C93B8F9E29672002364069647.html" },
        { name: "cmsx.machine.Ldtu", id: "diagram_node_117", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Ldtu_D5750BDE4387CCA72E577D520589DE58A46C00AB.html" },
        { name: "cmsx.machine.Ldtui", id: "diagram_node_83", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Ldtui_3C9E51DD10E79F244FDE10CAFC5D3C18096F4C1D.html" },
        { name: "cmsx.machine.Ldw", id: "diagram_node_118", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Ldw_5C97B7D97CE6B796C164B50D5CD63AF84B8B64FB.html" },
        { name: "cmsx.machine.Ldwi", id: "diagram_node_51", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Ldwi_DB347359C41A65F0BC0E172650EFED20AC51789C.html" },
        { name: "cmsx.machine.Ldwu", id: "diagram_node_119", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Ldwu_B3206EA378A13BBBCC8649FB2A1F17A25F1DC104.html" },
        { name: "cmsx.machine.Ldwui", id: "diagram_node_120", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Ldwui_023D050BAAA0AA557F25A7EB93327C26E757952F.html" },
        { name: "cmsx.machine.Mul", id: "diagram_node_121", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Mul_C16CB77616B96166569A661E6162F639826AD9AF.html" },
        { name: "cmsx.machine.Muli", id: "diagram_node_122", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Muli_1834AF8A0A1B84241206F5606A41DC871347FEE6.html" },
        { name: "cmsx.machine.Mulu", id: "diagram_node_123", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Mulu_4B3864952D91BDADCF4725E9951B4B8DAE175C39.html" },
        { name: "cmsx.machine.Mului", id: "diagram_node_124", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Mului_A0B1BC5876CD443B0D4B65F51F2DE7DD4A8E9457.html" },
        { name: "cmsx.machine.Mux", id: "diagram_node_125", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Mux_1A6546D4AA3D044196C0E6672D696B1EF0A7E75F.html" },
        { name: "cmsx.machine.Muxi", id: "diagram_node_126", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Muxi_4E4E7257ECE78166D8EF6E8C53AF5FF2274E178B.html" },
        { name: "cmsx.machine.Nand", id: "diagram_node_130", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Nand_4E37EED923E4D9017493A1F8600EA1DE22DBD6C6.html" },
        { name: "cmsx.machine.Nandi", id: "diagram_node_132", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Nandi_745DBA9E36E5CD73547DB7715BB8D33A6AC5C07E.html" },
        { name: "cmsx.machine.Neg", id: "diagram_node_133", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Neg_ACE705CFB058CFEB5F003FA16E0B14A628060FB4.html" },
        { name: "cmsx.machine.Negi", id: "diagram_node_134", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Negi_13F308C603BC09060D7F2B40A315459A64958331.html" },
        { name: "cmsx.machine.Negu", id: "diagram_node_135", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Negu_F0F85BB529A809EBF37BD29E77B7AC5C8FADEB84.html" },
        { name: "cmsx.machine.Negui", id: "diagram_node_136", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Negui_59C6DD2E261743A9B012706CF43E3E5C3117C0B9.html" },
        { name: "cmsx.machine.Nor", id: "diagram_node_137", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Nor_9B3DE970503E386B7BBE34DDC18C1C850638F83E.html" },
        { name: "cmsx.machine.Nori", id: "diagram_node_139", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Nori_733BA91D310D7EDC2496B385F2A6BE90164AC5AC.html" },
        { name: "cmsx.machine.Nxor", id: "diagram_node_140", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Nxor_AA86D6D84982E424E3263707DD5A68C10C0E50CB.html" },
        { name: "cmsx.machine.Nxori", id: "diagram_node_141", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Nxori_EE256BB1199F12F475BE0D8FE0065E1907F55672.html" },
        { name: "cmsx.machine.Odif", id: "diagram_node_142", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Odif_8FBDCC20F9BEEB2D8FF8BA3C09E431A906997D6D.html" },
        { name: "cmsx.machine.Odifi", id: "diagram_node_143", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Odifi_63873598CA65DDD4229B67E2EA1DF350D61FCF7F.html" },
        { name: "cmsx.machine.Or", id: "diagram_node_144", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Or_FDC582D10774C99F017F6C2438210334E12B498E.html" },
        { name: "cmsx.machine.Orh", id: "diagram_node_145", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Orh_FF4FC73614CF820202FFCEBEB69F7385C436C2A1.html" },
        { name: "cmsx.machine.Ori", id: "diagram_node_146", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Ori_0C19D06FB56EB6F1EDDA5141C976F3AB098040E2.html" },
        { name: "cmsx.machine.Orl", id: "diagram_node_148", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Orl_3659D592AD1715F366838F9604AA9A5E233AC452.html" },
        { name: "cmsx.machine.Ormh", id: "diagram_node_129", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Ormh_F6DA66FDFBF04F12A27DEF581C1711933DC3793C.html" },
        { name: "cmsx.machine.Orml", id: "diagram_node_131", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Orml_06FC21D0AE369543451D0A9D270AE5B848035CF5.html" },
        { name: "cmsx.machine.Orn", id: "diagram_node_149", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Orn_CB9CF7E05A111032E4D758B8E1756F2DBDB2FF78.html" },
        { name: "cmsx.machine.Orni", id: "diagram_node_110", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Orni_9C068B273F16333C13D8150B1E2984A02AC7BD51.html" },
        { name: "cmsx.machine.Pbev", id: "diagram_node_150", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Pbev_10BC83D5A3776874B1F10382B18F62B16505336C.html" },
        { name: "cmsx.machine.Pbevb", id: "diagram_node_128", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Pbevb_5A857AFA7552644219268001077D93BF5EAE745B.html" },
        { name: "cmsx.machine.Pbn", id: "diagram_node_152", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Pbn_2C4006506B55C948EAF1CC59C62E5A8810A40EBF.html" },
        { name: "cmsx.machine.Pbnb", id: "diagram_node_153", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Pbnb_52219C356925BB9CD930C9FD4C1048AD6A797857.html" },
        { name: "cmsx.machine.Pbnn", id: "diagram_node_155", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Pbnn_452228D369A2CC2AB5B44072EB57A9C9F31CA395.html" },
        { name: "cmsx.machine.Pbnnb", id: "diagram_node_156", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Pbnnb_2F6ABFCFF336DBE41989A9F1C560FD86B24564F7.html" },
        { name: "cmsx.machine.Pbnp", id: "diagram_node_157", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Pbnp_4CE05BF0DCEE4E66FA843090D20A6513F4A71168.html" },
        { name: "cmsx.machine.Pbnpb", id: "diagram_node_159", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Pbnpb_AB5E408A0651214D6AF4A85A0CF0F968152B986B.html" },
        { name: "cmsx.machine.Pbnz", id: "diagram_node_160", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Pbnz_9B4E75392D591B781A29FB81CAC143E675389D1E.html" },
        { name: "cmsx.machine.Pbnzb", id: "diagram_node_162", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Pbnzb_6A90A5BFEAA346AC0C56FCEC296163812EE606F3.html" },
        { name: "cmsx.machine.Pbod", id: "diagram_node_163", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Pbod_7A753E0990B43835BF3904605B28644085A5D428.html" },
        { name: "cmsx.machine.Pbodb", id: "diagram_node_164", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Pbodb_A818B2368DC67066013E9FE44A45AE47ABB50A0B.html" },
        { name: "cmsx.machine.Pbp", id: "diagram_node_166", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Pbp_766C0666389078A4A900B05872AF454118EBFD0A.html" },
        { name: "cmsx.machine.Pbpb", id: "diagram_node_168", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Pbpb_B1AB3F47E35C3055392E6310801B1B820C94103F.html" },
        { name: "cmsx.machine.Pbz", id: "diagram_node_170", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Pbz_4720AA40C125A6DEB9EBFB5198A3985483FA9D8F.html" },
        { name: "cmsx.machine.Pbzb", id: "diagram_node_167", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Pbzb_468B670D76EB1951C3730CB32047A72098E7D6B9.html" },
        { name: "cmsx.machine.Put", id: "diagram_node_171", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Put_5BC6E7195C745A2B46DF73F7C7B8109F4CC4BF9A.html" },
        { name: "cmsx.machine.Ret", id: "diagram_node_127", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Ret_01542CBBC0F4B0A656B4B9B032DBE18EAA134D1B.html" },
        { name: "cmsx.machine.SFlot", id: "diagram_node_4", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_SFlot_25DBE5E8EB92BCC417CE2BD323FE72641BCF9BAA.html" },
        { name: "cmsx.machine.SFloti", id: "diagram_node_41", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_SFloti_F9E02D7C833419AC3C2DC129150EBB10A16AF305.html" },
        { name: "cmsx.machine.SFlotu", id: "diagram_node_172", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_SFlotu_58A401C31C2CBB8339C553C05C703CD86EA88913.html" },
        { name: "cmsx.machine.SFlotui", id: "diagram_node_173", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_SFlotui_DDC840944EB45C15434AED2B97229DEA1C62B3FD.html" },
        { name: "cmsx.machine.Sadd", id: "diagram_node_174", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Sadd_CBA62EFF3863211450C7B4BF6EDFFA6D57E1B2B2.html" },
        { name: "cmsx.machine.Saddi", id: "diagram_node_175", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Saddi_CE1040B38B0B16CC0CCC26A874387F1431D5E599.html" },
        { name: "cmsx.machine.Seth", id: "diagram_node_176", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Seth_3F3CE537D4B754B965A4F5A978DD5B0387FBE4C8.html" },
        { name: "cmsx.machine.Setl", id: "diagram_node_177", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Setl_5BE89E417B219C544B42BD8DCF655B04669FCC59.html" },
        { name: "cmsx.machine.Setmh", id: "diagram_node_178", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Setmh_0D6282A32A77F7918DF689B2DC1CBA8ECED4791B.html" },
        { name: "cmsx.machine.Setml", id: "diagram_node_179", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Setml_0F3DD3B6F6E00BB727FFF0990ABF24E18CDC8655.html" },
        { name: "cmsx.machine.Sl", id: "diagram_node_151", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Sl_F91D74FB72237C7ED75D17EC94B047DF9981F250.html" },
        { name: "cmsx.machine.Sli", id: "diagram_node_180", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Sli_D258C967095C955AF19EA794720200764DB9D212.html" },
        { name: "cmsx.machine.Slu", id: "diagram_node_181", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Slu_A7B3834650E3755B1F49E563675CF01DD929345A.html" },
        { name: "cmsx.machine.Slui", id: "diagram_node_182", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Slui_AC58165CF83C28E811FC46831B9DEF02EB3A35B6.html" },
        { name: "cmsx.machine.Sr", id: "diagram_node_183", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Sr_495B25C6592D8A7D1461C6995B3294E64E8582ED.html" },
        { name: "cmsx.machine.Sri", id: "diagram_node_147", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Sri_AC6BF783E71B1BD312F9E866E7C09108DB884512.html" },
        { name: "cmsx.machine.Sru", id: "diagram_node_184", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Sru_9DADEB1CCDA319E6B98AD3509539BFC0C3FDB9F6.html" },
        { name: "cmsx.machine.Srui", id: "diagram_node_47", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Srui_9FFC7A30E1B7A21EAD2DDA875BFDA697ECFFF0F4.html" },
        { name: "cmsx.machine.Stb", id: "diagram_node_158", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Stb_A76A186EA1313C6B373E977560291DC9A0126F9C.html" },
        { name: "cmsx.machine.Stbi", id: "diagram_node_154", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Stbi_064B2BA4F1BDDD54C92B087FF7766D77FCCC6D55.html" },
        { name: "cmsx.machine.Stbu", id: "diagram_node_185", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Stbu_23C0E22FFFC4B400034AF7377CEAB2DEF1CD0CE1.html" },
        { name: "cmsx.machine.Stbui", id: "diagram_node_186", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Stbui_DE6CC87EBB3DDA574A92F518AF08A824AC0BB7EF.html" },
        { name: "cmsx.machine.Stco", id: "diagram_node_187", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Stco_3E1D86D89B143C2F22631078216CFCD486282952.html" },
        { name: "cmsx.machine.Stcoi", id: "diagram_node_188", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Stcoi_F0DD6CF36718B2CB8D6449B9F9F929E452428E10.html" },
        { name: "cmsx.machine.Stht", id: "diagram_node_189", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Stht_4984341253D6E0FE84662E2245A9037839FE38CD.html" },
        { name: "cmsx.machine.Sthti", id: "diagram_node_72", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Sthti_C1055B2441F6228C029F5CA48998569102FC9499.html" },
        { name: "cmsx.machine.Sto", id: "diagram_node_169", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Sto_A65AD1601C974139A8409E24B10D824D20C02A8E.html" },
        { name: "cmsx.machine.Stoi", id: "diagram_node_190", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Stoi_F6FFC3B69ED8D6D311943C5E628B435157292086.html" },
        { name: "cmsx.machine.Stou", id: "diagram_node_191", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Stou_ACB7082D533E5A4EAA93793CBB1B8C196A7C4243.html" },
        { name: "cmsx.machine.Stoui", id: "diagram_node_192", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Stoui_82A33966EF77BCC3318C385462B32B68DB422DA5.html" },
        { name: "cmsx.machine.Stsf", id: "diagram_node_193", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Stsf_58EB5D1770C9CB10616A3D5141B0FDC080989D73.html" },
        { name: "cmsx.machine.Stsfi", id: "diagram_node_195", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Stsfi_CEDC41135497A0177342BD801328C7C7E0A270A7.html" },
        { name: "cmsx.machine.Stt", id: "diagram_node_196", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Stt_98C76BE51BE796B521497BBCCB9057683D681CBF.html" },
        { name: "cmsx.machine.Stti", id: "diagram_node_197", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Stti_AC131B82C138E0E511D1ECCDB43FCDFBFA6FEAF0.html" },
        { name: "cmsx.machine.Sttu", id: "diagram_node_198", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Sttu_52D05A2AF21E5B6C026B3AB8D6F23D43247C0D73.html" },
        { name: "cmsx.machine.Sttui", id: "diagram_node_199", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Sttui_C6ADD9D794044286B3A5FD215F5664BB68B595A8.html" },
        { name: "cmsx.machine.Stw", id: "diagram_node_165", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Stw_F6CC6821F6C70AA999A68A06E22985DCD29E6EB1.html" },
        { name: "cmsx.machine.Stwi", id: "diagram_node_200", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Stwi_A1452779134347DEBBCE645BA23031761EBEB5A9.html" },
        { name: "cmsx.machine.Stwu", id: "diagram_node_201", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Stwu_5F836E3F27F70DBCE0A26B287590A9A20DB570B2.html" },
        { name: "cmsx.machine.Stwui", id: "diagram_node_203", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Stwui_20F4247B5E40E2ADF5A02B22EFC051715BE42CB9.html" },
        { name: "cmsx.machine.Sub", id: "diagram_node_204", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Sub_A8B57E6D2C54491BF1FCE8BD40D170F96BFFBA31.html" },
        { name: "cmsx.machine.Subi", id: "diagram_node_205", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Subi_6BFE381F2BC70E3ECDBA46943C2D7BB87F8670FA.html" },
        { name: "cmsx.machine.Subu", id: "diagram_node_33", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Subu_B47DAC0D941CC6CA95204DFA10DA46332AEB99A6.html" },
        { name: "cmsx.machine.Subui", id: "diagram_node_206", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Subui_842E226CE34AE7B5270909A401E864210FB9E3EB.html" },
        { name: "cmsx.machine.Swym", id: "diagram_node_207", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Swym_6A527406BE9D5247D7206004D6D49D027E0E6918.html" },
        { name: "cmsx.machine.Tdif", id: "diagram_node_208", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Tdif_DECDA1C6581CBAD5E90524BF493F849DC8AF49F5.html" },
        { name: "cmsx.machine.Tdifi", id: "diagram_node_209", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Tdifi_F95EB17803B0AB3660884B175F62865911A95BDA.html" },
        { name: "cmsx.machine.Trap", id: "diagram_node_49", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Trap_7128A7F66F6CF5E44FA32DC4763D049F7ACF0ED9.html" },
        { name: "cmsx.machine.Wdif", id: "diagram_node_210", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Wdif_67EF2B303F5E820C6299A63104E6A7F2FF08BD18.html" },
        { name: "cmsx.machine.Wdifi", id: "diagram_node_211", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Wdifi_E431F46CFE043DA4176B6F0199DB57BCE0FC683B.html" },
        { name: "cmsx.machine.Xor", id: "diagram_node_161", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Xor_2759FE5CAFB53AA2D67483F1FF9F995E52ACE21A.html" },
        { name: "cmsx.machine.Xori", id: "diagram_node_96", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Xori_527CFFA542DBF5DB93B1BAA5BD3D3743319F4D4C.html" },
        { name: "cmsx.machine.Zsev", id: "diagram_node_35", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Zsev_AF9D93B4B44CF8FA70F8559A552A231C25C86B8F.html" },
        { name: "cmsx.machine.Zsevi", id: "diagram_node_212", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Zsevi_010D18A8F36EC6953BBE64B226ADD80D036770B8.html" },
        { name: "cmsx.machine.Zsn", id: "diagram_node_213", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Zsn_FB75CF9875D2608B1F5214C468888E103049D5EC.html" },
        { name: "cmsx.machine.Zsni", id: "diagram_node_214", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Zsni_51356FE33DD83991FF94E3D093820F9C6F488BF6.html" },
        { name: "cmsx.machine.Zsnn", id: "diagram_node_23", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Zsnn_45DD0EE93E3AD630EAB792BD1D77BA51F02610D0.html" },
        { name: "cmsx.machine.Zsnni", id: "diagram_node_215", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Zsnni_47CAE2018D56A0670F822969174E61553BB25997.html" },
        { name: "cmsx.machine.Zsnp", id: "diagram_node_194", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Zsnp_3B94824ED155CB882513E4868D7DC212ED122F56.html" },
        { name: "cmsx.machine.Zsnpi", id: "diagram_node_85", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Zsnpi_1790428C4AB7A2FAC47CADF71D39005FE9E4BEAA.html" },
        { name: "cmsx.machine.Zsnz", id: "diagram_node_216", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Zsnz_DA13D56DC7129D8ACF95D1626A95BF556E3709BD.html" },
        { name: "cmsx.machine.Zsnzi", id: "diagram_node_217", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Zsnzi_BE604BCF9848371E313599DECCB6549FAB258E61.html" },
        { name: "cmsx.machine.Zsod", id: "diagram_node_10", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Zsod_46E6015DA510FE80FB3677F0B55196C086DABA6B.html" },
        { name: "cmsx.machine.Zsodi", id: "diagram_node_218", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Zsodi_61BB837A68F3D6F68B76F17F8F86134AD09D1C99.html" },
        { name: "cmsx.machine.Zsp", id: "diagram_node_138", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Zsp_14DD8B3CC81C8A887BFB9386EC68BBCC5DDB700D.html" },
        { name: "cmsx.machine.Zspi", id: "diagram_node_17", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Zspi_C0FD2D4115C701725E661C6881D0E91483E8553A.html" },
        { name: "cmsx.machine.Zsz", id: "diagram_node_202", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Zsz_C003A28B02953A62ABB90812C00A2F46DBF00550.html" },
        { name: "cmsx.machine.Zszi", id: "diagram_node_219", level: 1, subject: false, hasDerivedNodes: false, link: "../../cmsxm/doc/class_Zszi_8DD31C0FF73701B308FDC87BC0301D79CFAC6396.html" }];
    drawDiagram(diagramNodes);
}

