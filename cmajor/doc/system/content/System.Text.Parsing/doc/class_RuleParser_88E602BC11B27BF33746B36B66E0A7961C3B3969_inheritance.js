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
        { name: "System.Text.Parsing.ParsingObject", id: "diagram_node_0", level: 0, subject: false, hasDerivedNodes: true, link: "../../System.Text.Parsing/doc/class_ParsingObject_7DEF54CB93890E63DE64B53AA974CF41FBA38B34.html" },
        { name: "System.Text.Parsing.Parser", id: "diagram_node_1", level: 1, subject: false, hasDerivedNodes: true, link: "../../System.Text.Parsing/doc/class_Parser_59591398FA427ADC5EC015FB52651571177DB77D.html" },
        { name: "System.Text.Parsing.RuleParser", id: "diagram_node_2", level: 2, subject: true, hasDerivedNodes: true, link: "../../System.Text.Parsing/doc/class_RuleParser_88E602BC11B27BF33746B36B66E0A7961C3B3969.html" },
        { name: "System.Json.JsonGrammar.ArrayRule", id: "diagram_node_19", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Json/doc/class_ArrayRule_97D9572F1E83FBA5D44A80D2E6F6F0F63D0E73D0.html" },
        { name: "System.Json.JsonGrammar.EscapeRule", id: "diagram_node_49", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Json/doc/class_EscapeRule_F6F8C5008CC9D44A04F0BB4D7CFA25EAF565A0A4.html" },
        { name: "System.Json.JsonGrammar.FieldRule", id: "diagram_node_51", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Json/doc/class_FieldRule_A2491AFE296C3CD848691B73370AC927B83536EC.html" },
        { name: "System.Json.JsonGrammar.NumberRule", id: "diagram_node_52", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Json/doc/class_NumberRule_27C8EFF3C84114164EA6042FB8F167C6EB90366C.html" },
        { name: "System.Json.JsonGrammar.ObjectRule", id: "diagram_node_53", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Json/doc/class_ObjectRule_5E2C4DD6D6F88EBF302BC3AE586ACFE38942C13D.html" },
        { name: "System.Json.JsonGrammar.StringRule", id: "diagram_node_58", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Json/doc/class_StringRule_6A3C489F690184D3F17D7916567A6918B614FF1C.html" },
        { name: "System.Json.JsonGrammar.ValueRule", id: "diagram_node_7", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Json/doc/class_ValueRule_B33645C537AF028FF2E642F30FE36D99549C75E4.html" },
        { name: "System.Net.Http.HttpChunkHeaderGrammar.headerRule", id: "diagram_node_106", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_headerRule_84E4C57C71F558B63F0295A9C6E8AF9218093C40.html" },
        { name: "System.Net.Http.HttpDateGrammar.dateRule", id: "diagram_node_112", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_dateRule_266440D66F9502D2DE5B650B4573E982CCCD58B5.html" },
        { name: "System.Net.Http.HttpFieldValueGrammar.csvFieldValueRule", id: "diagram_node_113", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_csvFieldValueRule_CC1E5DCB9CFEA65016D5CCD075ADCD38CE9C32F6.html" },
        { name: "System.Net.Http.HttpGrammar.CHARRule", id: "diagram_node_116", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_CHARRule_0547A2CDCF2A39B32B5A3BEB7B294847325F0906.html" },
        { name: "System.Net.Http.HttpGrammar.Day1Rule", id: "diagram_node_118", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Day1Rule_ED74D1C40C2383832C5A92F9E4F21950A63990E5.html" },
        { name: "System.Net.Http.HttpGrammar.DayRule", id: "diagram_node_119", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_DayRule_5BDC26248C38F88FEEB3686360214096284CD84A.html" },
        { name: "System.Net.Http.HttpGrammar.HourRule", id: "diagram_node_120", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_HourRule_5ED1FE0BF9A22F165C1F055397BD5381ECDBF117.html" },
        { name: "System.Net.Http.HttpGrammar.MinRule", id: "diagram_node_74", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_MinRule_4C44AA8D307EB944D082CCCD39C2F47AA07C178B.html" },
        { name: "System.Net.Http.HttpGrammar.SecRule", id: "diagram_node_122", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_SecRule_24A4570A45A365D1F05B6613CA8685B927DE8557.html" },
        { name: "System.Net.Http.HttpGrammar.Year2Rule", id: "diagram_node_124", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_Year2Rule_D16FA8008887BE9FE2514BDBFCFE8E962A404557.html" },
        { name: "System.Net.Http.HttpGrammar.YearRule", id: "diagram_node_125", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_YearRule_CE43FFC51B882D128404B4A3901BA294DA2178C2.html" },
        { name: "System.Net.Http.HttpGrammar.asctimeDateRule", id: "diagram_node_126", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_asctimeDateRule_0D847B518D79C518F0C096DC45E94ADCAF7A74FE.html" },
        { name: "System.Net.Http.HttpGrammar.attributeRule", id: "diagram_node_127", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_attributeRule_A7F88A7994317774B38ED14CBF1C731711D389F3.html" },
        { name: "System.Net.Http.HttpGrammar.attrvalRule", id: "diagram_node_128", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_attrvalRule_972403C135F3E36A27B403E0515DE877C1F43909.html" },
        { name: "System.Net.Http.HttpGrammar.chunkExtNameRule", id: "diagram_node_131", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_chunkExtNameRule_4FABC01047FDA936BD6E5273041087A0EFC33886.html" },
        { name: "System.Net.Http.HttpGrammar.chunkExtValRule", id: "diagram_node_132", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_chunkExtValRule_3588BBEA2DB33BEEADD7B4A03E0BF0029CF030DD.html" },
        { name: "System.Net.Http.HttpGrammar.chunkExtensionsRule", id: "diagram_node_104", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_chunkExtensionsRule_1B4F09C9D7271C592EA0A803D27AF6F6BB843A3C.html" },
        { name: "System.Net.Http.HttpGrammar.chunkHeaderRule", id: "diagram_node_133", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_chunkHeaderRule_D70034E8C9B1EB554CE506F62B943D8DCC6232E0.html" },
        { name: "System.Net.Http.HttpGrammar.commaSeparatedFieldValueRule", id: "diagram_node_134", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_commaSeparatedFieldValueRule_D93C8DB4E004A058311FC4807941B4AC977A29BA.html" },
        { name: "System.Net.Http.HttpGrammar.ctextRule", id: "diagram_node_135", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_ctextRule_DA01293C58CB7568127F07DC3CD1323253C34CC4.html" },
        { name: "System.Net.Http.HttpGrammar.date1Rule", id: "diagram_node_136", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_date1Rule_1A2F4C09CBFAB64B781015E95AA2B523B40EB1BB.html" },
        { name: "System.Net.Http.HttpGrammar.date2Rule", id: "diagram_node_137", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_date2Rule_D2CC1CD46EFCB0E13484FABA463DF2F6BFEB911A.html" },
        { name: "System.Net.Http.HttpGrammar.date3Rule", id: "diagram_node_18", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_date3Rule_D358E55D3E90F392EC3B45C32BA3DB7C98FEA98F.html" },
        { name: "System.Net.Http.HttpGrammar.dateValueRule", id: "diagram_node_138", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_dateValueRule_FD7539FB7FD929B542BC59CC01A33320EA1B1CCD.html" },
        { name: "System.Net.Http.HttpGrammar.deltaSecondsRule", id: "diagram_node_73", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_deltaSecondsRule_15F2701439009095F087F37F908075B249188550.html" },
        { name: "System.Net.Http.HttpGrammar.fieldNameRule", id: "diagram_node_139", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_fieldNameRule_3CD8E432A9B09CBC8D9C414F139A194145E849BC.html" },
        { name: "System.Net.Http.HttpGrammar.fieldValueRule", id: "diagram_node_130", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_fieldValueRule_FAF4D757C30A0DB053BFC754D04A9C6DF0573E6A.html" },
        { name: "System.Net.Http.HttpGrammar.generalFieldValueRule", id: "diagram_node_140", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_generalFieldValueRule_A633063077A9DC33B34D99FF4D2DA01DB699D1E6.html" },
        { name: "System.Net.Http.HttpGrammar.generalParameterRule", id: "diagram_node_57", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_generalParameterRule_8D2CF9016F82521FC270D0595A722F642BD0FAAD.html" },
        { name: "System.Net.Http.HttpGrammar.httpDateRule", id: "diagram_node_142", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_httpDateRule_8C17EB87B8AB5DBA58C7335481BB037CF3967A40.html" },
        { name: "System.Net.Http.HttpGrammar.httpHeaderRule", id: "diagram_node_143", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_httpHeaderRule_C195A8116E25400D1EC420C563015935596C4F13.html" },
        { name: "System.Net.Http.HttpGrammar.httpTokenRule", id: "diagram_node_144", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_httpTokenRule_369B07C8F49D8612D1981AD568E9816B9E8FAAF6.html" },
        { name: "System.Net.Http.HttpGrammar.httpVersionRule", id: "diagram_node_145", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_httpVersionRule_ECA41B91CD27EB2890D2494223EC7D754240EED1.html" },
        { name: "System.Net.Http.HttpGrammar.mediaTypeRule", id: "diagram_node_129", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_mediaTypeRule_F61DF2FEF783D00C210D7AA988984FEAEDD85B5C.html" },
        { name: "System.Net.Http.HttpGrammar.monthRule", id: "diagram_node_146", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_monthRule_8A15272AD2C3B2EA1EE1546C07DD7A519D370EC4.html" },
        { name: "System.Net.Http.HttpGrammar.parameterRule", id: "diagram_node_33", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_parameterRule_26C7B84CB5F7222C4DB7B3B0C43CB3E3BB596963.html" },
        { name: "System.Net.Http.HttpGrammar.qdtextRule", id: "diagram_node_148", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_qdtextRule_A5981390285CB8792C3FD632128595D101765FF7.html" },
        { name: "System.Net.Http.HttpGrammar.quotedPairRule", id: "diagram_node_149", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_quotedPairRule_E7717A345D423962A07611D68DF4C077EFBC4126.html" },
        { name: "System.Net.Http.HttpGrammar.quotedStringRule", id: "diagram_node_92", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_quotedStringRule_120BD17D48DFD14EF1024B5755DE9CC10E47CD58.html" },
        { name: "System.Net.Http.HttpGrammar.reasonPhraseRule", id: "diagram_node_12", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_reasonPhraseRule_DC30F0C63FE18D36621E574EF08FF3F4B164ADE0.html" },
        { name: "System.Net.Http.HttpGrammar.rfc1123DateRule", id: "diagram_node_150", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_rfc1123DateRule_E0562B14B0541FFDC814CAB29AD0DE6B4F105584.html" },
        { name: "System.Net.Http.HttpGrammar.rfc850DateRule", id: "diagram_node_152", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_rfc850DateRule_C099CA10085407A8D77F31FD969CA5854C0ED12C.html" },
        { name: "System.Net.Http.HttpGrammar.statusCodeRule", id: "diagram_node_153", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_statusCodeRule_EAC35E5B7A81685E191137515AB2FC2572CAE467.html" },
        { name: "System.Net.Http.HttpGrammar.statusLineRule", id: "diagram_node_154", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_statusLineRule_F306DF0620B65F396F8F2CAC6F4E6E1E26E3C005.html" },
        { name: "System.Net.Http.HttpGrammar.subtypeRule", id: "diagram_node_155", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_subtypeRule_6D204658901F09A588CF00E8992A766742720E83.html" },
        { name: "System.Net.Http.HttpGrammar.timeRule", id: "diagram_node_156", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_timeRule_92453EB514D9248B126381D4E78B574431A25CF0.html" },
        { name: "System.Net.Http.HttpGrammar.typeRule", id: "diagram_node_157", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_typeRule_8383FAFA34A807B37D04466AF28F44B39D78BE43.html" },
        { name: "System.Net.Http.HttpHeaderGrammar.headerRule", id: "diagram_node_158", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_headerRule_D32964CE47F139E56BC4B565C3D655E4A58F7549.html" },
        { name: "System.Net.Http.HttpMediaTypeGrammar.mediaTypeValueRule", id: "diagram_node_159", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_mediaTypeValueRule_2D3D5639EEEA0178DB96E49EB4801130D874105F.html" },
        { name: "System.Net.Http.HttpStatusLineGrammar.statusRule", id: "diagram_node_121", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_statusRule_A5E58B70CEFE07998C3F52B22D9367B285FF373C.html" },
        { name: "System.Net.Http.UriGrammar.AbsoluteUriRule", id: "diagram_node_141", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_AbsoluteUriRule_7DE9A45C3082FFC59BA0C4F1119F2E8431C6F339.html" },
        { name: "System.Net.Http.UriGrammar.AuthorityRule", id: "diagram_node_117", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_AuthorityRule_95CEBD7C0C7EDF18A6E8DBCBB831F949A8FCA694.html" },
        { name: "System.Net.Http.UriGrammar.DomainLabelRule", id: "diagram_node_9", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_DomainLabelRule_F61CB7DFC3888FF077D3B649E3189AB813401C70.html" },
        { name: "System.Net.Http.UriGrammar.HierPartRule", id: "diagram_node_109", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_HierPartRule_D62B46A099A968396E70EFE950C01646C2BE2770.html" },
        { name: "System.Net.Http.UriGrammar.HostNameRule", id: "diagram_node_48", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_HostNameRule_A52FFC20D251790659008DEF6BAE0AF208DEC904.html" },
        { name: "System.Net.Http.UriGrammar.HostPortRule", id: "diagram_node_160", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_HostPortRule_7E1D9C2A5F01B66414C3C2405177528788990E47.html" },
        { name: "System.Net.Http.UriGrammar.NetPathRule", id: "diagram_node_28", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_NetPathRule_1F6C3C51AEB5C52718BE41A08377E559A4639A1C.html" },
        { name: "System.Net.Http.UriGrammar.PortRule", id: "diagram_node_161", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_PortRule_BE9EECB8202A8A046B58738CDF04A52E6D37DF66.html" },
        { name: "System.Net.Http.UriGrammar.RelativeUriRule", id: "diagram_node_162", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_RelativeUriRule_1AC418EE5F34AD8B96D2A0755E1DDF81E5F9666E.html" },
        { name: "System.Net.Http.UriGrammar.ServerRule", id: "diagram_node_88", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_ServerRule_034425A7C5338EE191652ECEE55357371315633A.html" },
        { name: "System.Net.Http.UriGrammar.TopLabelRule", id: "diagram_node_163", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_TopLabelRule_94FEFF98FB26DD2B84EB624B323A74FE8FB5CED1.html" },
        { name: "System.Net.Http.UriGrammar.UriReferenceRule", id: "diagram_node_164", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Net.Http/doc/class_UriReferenceRule_95587235EA13E90B32D756BD3FBB29FA198B2BDE.html" },
        { name: "System.Text.Parsing.stdlib.boolRule", id: "diagram_node_3", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_boolRule_C4BE2B41B8BD229647AD3F037FCA45B9E8E6D3E3.html" },
        { name: "System.Text.Parsing.stdlib.byteRule", id: "diagram_node_8", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_byteRule_0E3C42038DE207056BE8D50C02CA675963160E6E.html" },
        { name: "System.Text.Parsing.stdlib.charRule", id: "diagram_node_15", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_charRule_A3C8E3EAD09B1883E6F6BD376178D543F148C1C0.html" },
        { name: "System.Text.Parsing.stdlib.doubleRule", id: "diagram_node_13", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_doubleRule_6A7A4FE9F5631D53DA37A4AE3CEEDEECCDCFEAAD.html" },
        { name: "System.Text.Parsing.stdlib.escapeRule", id: "diagram_node_5", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_escapeRule_CC68EF526C4083BDC998E663A1581BA9F389123A.html" },
        { name: "System.Text.Parsing.stdlib.floatRule", id: "diagram_node_17", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_floatRule_E5AD4CA9F8078C0840EBB7AEF5E5D11DF28EBF8A.html" },
        { name: "System.Text.Parsing.stdlib.hexRule", id: "diagram_node_21", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_hexRule_A230AC982D8AF9AD987B15A19F33E93CD0B59E22.html" },
        { name: "System.Text.Parsing.stdlib.hex_literalRule", id: "diagram_node_20", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_hex_literalRule_503B8DAB2CB7BCA2AC20E1AF434A8F7B7A493463.html" },
        { name: "System.Text.Parsing.stdlib.hexbyteRule", id: "diagram_node_4", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_hexbyteRule_E670624BAB490757F84E8DD670585DEB712C0A34.html" },
        { name: "System.Text.Parsing.stdlib.hexdwordRule", id: "diagram_node_22", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_hexdwordRule_939B12A8E89B157E848D3EA2D2CCB8EF8057CDDE.html" },
        { name: "System.Text.Parsing.stdlib.hexqwordRule", id: "diagram_node_24", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_hexqwordRule_B22F84D5A1B70A0007A7E5A821525AD4C59500F9.html" },
        { name: "System.Text.Parsing.stdlib.hexwordRule", id: "diagram_node_10", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_hexwordRule_C60A06FE130C7D6A3D3E5C54EE535216AAC228F9.html" },
        { name: "System.Text.Parsing.stdlib.identifierRule", id: "diagram_node_25", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_identifierRule_6337B1033C3D83467C43058054059544FDC17572.html" },
        { name: "System.Text.Parsing.stdlib.intRule", id: "diagram_node_27", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_intRule_C9B60EB5E5E5BC242EB2AF7EE8F51555F99083F3.html" },
        { name: "System.Text.Parsing.stdlib.longRule", id: "diagram_node_29", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_longRule_77A9CD69D78EC42815C0FC1D9E923BB45F598479.html" },
        { name: "System.Text.Parsing.stdlib.numRule", id: "diagram_node_31", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_numRule_82785CA74F999E98F4880A2367A8D1DB391A41F6.html" },
        { name: "System.Text.Parsing.stdlib.octal_digitRule", id: "diagram_node_35", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_octal_digitRule_8AB21C077892BCF466CAD4D14EB7075838FA0A01.html" },
        { name: "System.Text.Parsing.stdlib.qualified_idRule", id: "diagram_node_40", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_qualified_idRule_07E8141D98216D33F6397CC34195255F1E581CB1.html" },
        { name: "System.Text.Parsing.stdlib.sbyteRule", id: "diagram_node_42", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_sbyteRule_48DC88A1CDBA68713B4A439BD9DF979DE5B2E5B5.html" },
        { name: "System.Text.Parsing.stdlib.shortRule", id: "diagram_node_44", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_shortRule_3568A99F5B94132DC6394C821D19F1C3F59780A5.html" },
        { name: "System.Text.Parsing.stdlib.stringRule", id: "diagram_node_43", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_stringRule_C15A8DD35C972FC9227397103AFDE1EA5A0926DE.html" },
        { name: "System.Text.Parsing.stdlib.uintRule", id: "diagram_node_26", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_uintRule_8C99C3EEDC7F168ECE5FA11D2130E6B9B1A3241B.html" },
        { name: "System.Text.Parsing.stdlib.ulongRule", id: "diagram_node_46", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_ulongRule_D801576710F59A4FB28AF32576733492EA31FDA4.html" },
        { name: "System.Text.Parsing.stdlib.urealRule", id: "diagram_node_47", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_urealRule_B24D8DC0257FC718085276984EC9F786E03AEE11.html" },
        { name: "System.Text.Parsing.stdlib.ushortRule", id: "diagram_node_37", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.Parsing/doc/class_ushortRule_31FEEB5245D1290D3CBCF78C21327711682D0E82.html" },
        { name: "System.Text.RegularExpressions.RegularExpressionGrammar.AlternativeExpressionRule", id: "diagram_node_56", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.RegularExpressions/doc/class_AlternativeExpressionRule_4A8E0A75B1E15D431953E1A2CC7001CE365A23BE.html" },
        { name: "System.Text.RegularExpressions.RegularExpressionGrammar.CharClassCharRule", id: "diagram_node_59", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.RegularExpressions/doc/class_CharClassCharRule_A53117A13BF1152F840E4ABF3F80BB10F64CEC38.html" },
        { name: "System.Text.RegularExpressions.RegularExpressionGrammar.CharClassRule", id: "diagram_node_60", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.RegularExpressions/doc/class_CharClassRule_4F146B0ADDD3D4BE0C73F0669E2FBBF45F3788EE.html" },
        { name: "System.Text.RegularExpressions.RegularExpressionGrammar.CharRangeRule", id: "diagram_node_50", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.RegularExpressions/doc/class_CharRangeRule_618435CFE0DB4BD0ABFBE4AFB8D7E8ED673F12F7.html" },
        { name: "System.Text.RegularExpressions.RegularExpressionGrammar.CharRule", id: "diagram_node_62", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.RegularExpressions/doc/class_CharRule_B7D4B34179E6B09B20AED1A5DC81494C02C4B5F2.html" },
        { name: "System.Text.RegularExpressions.RegularExpressionGrammar.ClassRule", id: "diagram_node_63", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.RegularExpressions/doc/class_ClassRule_CB84A917AD277E1C502ED913C6E87F636CB3D527.html" },
        { name: "System.Text.RegularExpressions.RegularExpressionGrammar.PostfixExpressionRule", id: "diagram_node_64", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.RegularExpressions/doc/class_PostfixExpressionRule_54839DB8B1E97899D69C4D4C08ADD8922DA2C262.html" },
        { name: "System.Text.RegularExpressions.RegularExpressionGrammar.PrimaryExpressionRule", id: "diagram_node_65", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.RegularExpressions/doc/class_PrimaryExpressionRule_06E24805A354F46D89064514E21FF94363E8A751.html" },
        { name: "System.Text.RegularExpressions.RegularExpressionGrammar.RegularExpressionRule", id: "diagram_node_68", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.RegularExpressions/doc/class_RegularExpressionRule_2B13F70B2A4FF7C115CB38005FA00CC5A61A8284.html" },
        { name: "System.Text.RegularExpressions.RegularExpressionGrammar.SequenceExpressionRule", id: "diagram_node_69", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Text.RegularExpressions/doc/class_SequenceExpressionRule_60D359D8CE70B6B0937DCF203E2A5BCF6525CB46.html" },
        { name: "System.XPath.XPathExpressionGrammar.AbbreviatedAbsoluteLocationPathRule", id: "diagram_node_165", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_AbbreviatedAbsoluteLocationPathRule_92A3992049E4F0845A5145599EFA4C9D40347AED.html" },
        { name: "System.XPath.XPathExpressionGrammar.AbbreviatedAxisSpecifierRule", id: "diagram_node_166", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_AbbreviatedAxisSpecifierRule_9985C45C1D68D55A812152A85CF54EAC86E5A180.html" },
        { name: "System.XPath.XPathExpressionGrammar.AbbreviatedStepRule", id: "diagram_node_167", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_AbbreviatedStepRule_8C951CF1E06BCD3DB939C6AE8AA03F6A879C4A84.html" },
        { name: "System.XPath.XPathExpressionGrammar.AbsoluteLocationPathRule", id: "diagram_node_45", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_AbsoluteLocationPathRule_EFDB7A5EC1A9715BB775BEF5C0B5F624A3CCD5DC.html" },
        { name: "System.XPath.XPathExpressionGrammar.AdditiveExprRule", id: "diagram_node_168", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_AdditiveExprRule_FBAA8379B3BC2847B1E1AD8285018BC2F9B8182C.html" },
        { name: "System.XPath.XPathExpressionGrammar.AndExprRule", id: "diagram_node_169", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_AndExprRule_8C9782E265C2753098150E085FD2D48E957EE778.html" },
        { name: "System.XPath.XPathExpressionGrammar.ArgumentRule", id: "diagram_node_170", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_ArgumentRule_922C0C6E7BC540A38535A5AF48D825404A27C9E2.html" },
        { name: "System.XPath.XPathExpressionGrammar.AxisNameRule", id: "diagram_node_171", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_AxisNameRule_4158FDC6F513104F8292679B7B386A1C5ED82535.html" },
        { name: "System.XPath.XPathExpressionGrammar.AxisSpecifierRule", id: "diagram_node_16", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_AxisSpecifierRule_8E311F6CA55AE1DE4BE1D479AF023EAA8BE03C82.html" },
        { name: "System.XPath.XPathExpressionGrammar.EqualityExprRule", id: "diagram_node_172", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_EqualityExprRule_C9D2203CE2ACA32C341339978A3B903F7EE1B9A7.html" },
        { name: "System.XPath.XPathExpressionGrammar.ExprRule", id: "diagram_node_173", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_ExprRule_B39E147CE8B00C11B8B9D67892EADA772A163CF2.html" },
        { name: "System.XPath.XPathExpressionGrammar.FilterExprRule", id: "diagram_node_115", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_FilterExprRule_C3D86FA5F3E018C8377E73008405B05CA00C721E.html" },
        { name: "System.XPath.XPathExpressionGrammar.FunctionCallRule", id: "diagram_node_174", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_FunctionCallRule_C2C18B6D02B02E5EC30F2EFAB8B617A72FB13E14.html" },
        { name: "System.XPath.XPathExpressionGrammar.FunctionNameRule", id: "diagram_node_175", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_FunctionNameRule_F678CF02EC227955E978D21F32A58E37FB448B83.html" },
        { name: "System.XPath.XPathExpressionGrammar.LiteralRule", id: "diagram_node_176", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_LiteralRule_26A36860041AFA15A8244907908832D3BA1F8B2F.html" },
        { name: "System.XPath.XPathExpressionGrammar.LocalPartRule", id: "diagram_node_147", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_LocalPartRule_E01DFD7415928B9FB50801B172F58E1F4AF80E13.html" },
        { name: "System.XPath.XPathExpressionGrammar.LocationPathRule", id: "diagram_node_177", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_LocationPathRule_09BA19EF54835D5B812935ACEFC622A4D287D581.html" },
        { name: "System.XPath.XPathExpressionGrammar.MultiplicativeExprRule", id: "diagram_node_178", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_MultiplicativeExprRule_5FC379F0C8BAB78BB3B8FD2625054CDF9D2F9AEC.html" },
        { name: "System.XPath.XPathExpressionGrammar.NCNameRule", id: "diagram_node_151", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_NCNameRule_DC9F77010BA94F33F91C62E9D903BAEADC1E3237.html" },
        { name: "System.XPath.XPathExpressionGrammar.NameTestRule", id: "diagram_node_55", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_NameTestRule_8BE8AE5731967CE07959D9B964A3B8E3FAF1C196.html" },
        { name: "System.XPath.XPathExpressionGrammar.NodeTestRule", id: "diagram_node_123", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_NodeTestRule_76AA79613231678B6EB292D2D66E9535BEAECDF4.html" },
        { name: "System.XPath.XPathExpressionGrammar.NodeTypeRule", id: "diagram_node_179", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_NodeTypeRule_4FB740942C98EFDFC5867D12F5436BFF7838F712.html" },
        { name: "System.XPath.XPathExpressionGrammar.NumberRule", id: "diagram_node_180", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_NumberRule_C2D91546E547388A7C1569EC48D773EFB161BC9A.html" },
        { name: "System.XPath.XPathExpressionGrammar.OrExprRule", id: "diagram_node_181", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_OrExprRule_5EAC07BE4733481CB9B04507055279FC7CA2B683.html" },
        { name: "System.XPath.XPathExpressionGrammar.PathExprRule", id: "diagram_node_182", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_PathExprRule_819DCCC1021E02B0E8E95C6D9431690474E94968.html" },
        { name: "System.XPath.XPathExpressionGrammar.PredicateExprRule", id: "diagram_node_183", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_PredicateExprRule_15307B6535E39EDD4A594B0E9EE3B6E151526770.html" },
        { name: "System.XPath.XPathExpressionGrammar.PredicateRule", id: "diagram_node_79", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_PredicateRule_1F145DFA11804C2F71FE14EA67FCF3A6030C2755.html" },
        { name: "System.XPath.XPathExpressionGrammar.PrefixRule", id: "diagram_node_114", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_PrefixRule_8CD8CFC8E3A42F82214431F7CCFAA22992F9B2C5.html" },
        { name: "System.XPath.XPathExpressionGrammar.PrefixedNameRule", id: "diagram_node_184", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_PrefixedNameRule_F36D92EAFE428A2CC71591E168F8AFE99FE5CC3D.html" },
        { name: "System.XPath.XPathExpressionGrammar.PrimaryExprRule", id: "diagram_node_23", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_PrimaryExprRule_646630053F6708812B83FAD6EA18D29CF8D7491F.html" },
        { name: "System.XPath.XPathExpressionGrammar.QNameRule", id: "diagram_node_185", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_QNameRule_7456D8A47FD17D9938BDF3A56C6BD4248347E64F.html" },
        { name: "System.XPath.XPathExpressionGrammar.RelationalExprRule", id: "diagram_node_186", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_RelationalExprRule_D039970BAB735CF62E2238483338F33F6640B258.html" },
        { name: "System.XPath.XPathExpressionGrammar.RelativeLocationPathRule", id: "diagram_node_187", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_RelativeLocationPathRule_A071B0DEE34F2AB67921F9EA18C4A81B1B8515BA.html" },
        { name: "System.XPath.XPathExpressionGrammar.StepRule", id: "diagram_node_188", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_StepRule_82328C83BEE878DC4C65A7550C7A146E293A6F00.html" },
        { name: "System.XPath.XPathExpressionGrammar.UnaryExprRule", id: "diagram_node_189", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_UnaryExprRule_41C649CE059D371A482CEA38ECD548C9E33ACFBD.html" },
        { name: "System.XPath.XPathExpressionGrammar.UnionExprRule", id: "diagram_node_190", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_UnionExprRule_B99ACB846B247BE79CEA6903F36837E8000C32EF.html" },
        { name: "System.XPath.XPathExpressionGrammar.UnprefixedNameRule", id: "diagram_node_191", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_UnprefixedNameRule_66945B89A94958C04F087B66D254788103B93F3F.html" },
        { name: "System.XPath.XPathExpressionGrammar.VariableReferenceRule", id: "diagram_node_192", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.XPath/doc/class_VariableReferenceRule_0D66322A95BEA8667B0A280D3397FE989C59896B.html" },
        { name: "System.Xml.XmlGrammar.AttDefRule", id: "diagram_node_6", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_AttDefRule_74FA57D4542320FD1D0D7384DF1C1EA79C7850CA.html" },
        { name: "System.Xml.XmlGrammar.AttValueRule", id: "diagram_node_70", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_AttValueRule_C5C4B335D3615A52D5946835F81713856961E5E1.html" },
        { name: "System.Xml.XmlGrammar.AttlistDeclRule", id: "diagram_node_75", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_AttlistDeclRule_3EE8B3596460E865EFF1EC32E0CCFB551EE99633.html" },
        { name: "System.Xml.XmlGrammar.AttributeRule", id: "diagram_node_39", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_AttributeRule_EE354A44CE0F99473C73EB8075BC07735429CDC4.html" },
        { name: "System.Xml.XmlGrammar.CDSectRule", id: "diagram_node_36", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_CDSectRule_7E08ECC2C1DFCAF953AF5DCDBAFBFE5B39D29545.html" },
        { name: "System.Xml.XmlGrammar.CDataRule", id: "diagram_node_76", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_CDataRule_C8B82AC473DF261084CE6A2B14ADD5D7AFD958D7.html" },
        { name: "System.Xml.XmlGrammar.CharDataRule", id: "diagram_node_72", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_CharDataRule_085F5623E1FC524C73DE6C935268CA241251DC30.html" },
        { name: "System.Xml.XmlGrammar.CharRefRule", id: "diagram_node_78", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_CharRefRule_6A683AA7F002A998E9B313FDBD472AFE9257515F.html" },
        { name: "System.Xml.XmlGrammar.CommentRule", id: "diagram_node_61", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_CommentRule_88DE8B4203A3797C8ADF3F274F96749AEECDACE0.html" },
        { name: "System.Xml.XmlGrammar.ConditionalSectRule", id: "diagram_node_81", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_ConditionalSectRule_53C51DBF90472690A50FD18D0D5CB24442516D7F.html" },
        { name: "System.Xml.XmlGrammar.ContentRule", id: "diagram_node_80", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_ContentRule_01C1164A69C637EF120A2BBE052EC6AE2B96E8FF.html" },
        { name: "System.Xml.XmlGrammar.DeclSepRule", id: "diagram_node_41", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_DeclSepRule_2EA7E3D7AEBE981FAD49F7B9FCDC50851994E972.html" },
        { name: "System.Xml.XmlGrammar.DefaultDeclRule", id: "diagram_node_34", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_DefaultDeclRule_1B974E6B203DE9B53CAE9B8D2F37EDEA9B25FA85.html" },
        { name: "System.Xml.XmlGrammar.DocTypeDeclRule", id: "diagram_node_83", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_DocTypeDeclRule_404103763FC1F00BFA35ED8265536BD4207895C9.html" },
        { name: "System.Xml.XmlGrammar.DocumentRule", id: "diagram_node_84", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_DocumentRule_3005E7B1E04511D311AA75AC616B2F5E868CAD8D.html" },
        { name: "System.Xml.XmlGrammar.ETagRule", id: "diagram_node_54", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_ETagRule_7531FBAD24AEF7EE2F43855622031A4ED4378917.html" },
        { name: "System.Xml.XmlGrammar.ElementDeclRule", id: "diagram_node_11", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_ElementDeclRule_599ABDB467CD5D06F212B1647D70AB7994638346.html" },
        { name: "System.Xml.XmlGrammar.ElementRule", id: "diagram_node_67", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_ElementRule_4BEA9AD77F12C63D9D20953597E354F129E2D0BE.html" },
        { name: "System.Xml.XmlGrammar.EncNameRule", id: "diagram_node_66", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_EncNameRule_1E0C71E51B54A798BE6FFB239F535B8548B07ACB.html" },
        { name: "System.Xml.XmlGrammar.EncodingDeclRule", id: "diagram_node_32", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_EncodingDeclRule_77B5B1C32402CFA9136EC172FE874D0EF4ED2567.html" },
        { name: "System.Xml.XmlGrammar.EntityDeclRule", id: "diagram_node_77", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_EntityDeclRule_EA7BDA8BD5BD56DA908F32620EAF2E58E9445F88.html" },
        { name: "System.Xml.XmlGrammar.EntityDefRule", id: "diagram_node_71", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_EntityDefRule_678796DE467406D45382F448E387436BB19A86DB.html" },
        { name: "System.Xml.XmlGrammar.EntityRefRule", id: "diagram_node_85", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_EntityRefRule_0A44132F06E92DE9C110BFB6B774E511ADDED93F.html" },
        { name: "System.Xml.XmlGrammar.EntityValueRule", id: "diagram_node_86", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_EntityValueRule_B8A7333936180B34CF6B332D93DA49B87AE24189.html" },
        { name: "System.Xml.XmlGrammar.ExtParsedEntRule", id: "diagram_node_38", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_ExtParsedEntRule_4EB32B54266B1BB828DF0B2610C678E5616E1561.html" },
        { name: "System.Xml.XmlGrammar.ExtSubsetDeclRule", id: "diagram_node_87", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_ExtSubsetDeclRule_55D18D498DAD965AD72541704737303644CC76D5.html" },
        { name: "System.Xml.XmlGrammar.ExtSubsetRule", id: "diagram_node_89", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_ExtSubsetRule_948BEAB8CD573C16B1B91B5611717043A0D6C4E8.html" },
        { name: "System.Xml.XmlGrammar.GEDeclRule", id: "diagram_node_90", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_GEDeclRule_2169C1573083DB817286266DAA7F550F56D6F93D.html" },
        { name: "System.Xml.XmlGrammar.IncludeSectRule", id: "diagram_node_91", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_IncludeSectRule_8596511DDC470DE5E82AA4BB775CC6FC89D36374.html" },
        { name: "System.Xml.XmlGrammar.IntSubsetRule", id: "diagram_node_93", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_IntSubsetRule_59F9ACA5BEB5559548B75A5D1637F1F28E94045C.html" },
        { name: "System.Xml.XmlGrammar.MarkupDeclRule", id: "diagram_node_94", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_MarkupDeclRule_4485F2662C6646D1361CAAA6639E2427473B469B.html" },
        { name: "System.Xml.XmlGrammar.MiscRule", id: "diagram_node_95", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_MiscRule_3FB0583A7F56DEC766AB66EDEEEF11293571A0EA.html" },
        { name: "System.Xml.XmlGrammar.NDataDeclRule", id: "diagram_node_96", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_NDataDeclRule_8675616D50DD9931132B12B454BF42E8F43E440F.html" },
        { name: "System.Xml.XmlGrammar.NameRule", id: "diagram_node_97", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_NameRule_CE90B625637B9D9C94864D2F98CE4861EA3BD116.html" },
        { name: "System.Xml.XmlGrammar.NotationDeclRule", id: "diagram_node_98", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_NotationDeclRule_D7D6E491491F689F742EAF7618A40AB3E239B627.html" },
        { name: "System.Xml.XmlGrammar.PEDeclRule", id: "diagram_node_100", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_PEDeclRule_A8AF7F379BBBDF45D2A0E9C852640ED8CCB1D5A8.html" },
        { name: "System.Xml.XmlGrammar.PEDefRule", id: "diagram_node_101", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_PEDefRule_34A5A92ED7F79DA3EF3A04792A66981C485233F3.html" },
        { name: "System.Xml.XmlGrammar.PEReferenceRule", id: "diagram_node_102", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_PEReferenceRule_DA75852BD09653FDD017ED58061CD59AD87FC97B.html" },
        { name: "System.Xml.XmlGrammar.PIRule", id: "diagram_node_30", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_PIRule_E25CFCEEDB4CFA816A51E6D06FAC09828707B04C.html" },
        { name: "System.Xml.XmlGrammar.PITargetRule", id: "diagram_node_103", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_PITargetRule_2D4A00C612CB0FB64187F2A122BDE8D3566386AF.html" },
        { name: "System.Xml.XmlGrammar.PrologRule", id: "diagram_node_82", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_PrologRule_D47116A82500D847534B40C65CEAE3B072E32ABB.html" },
        { name: "System.Xml.XmlGrammar.PubidLiteralRule", id: "diagram_node_14", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_PubidLiteralRule_7A77C5917C77A60FFCC82A391E3925B966A36336.html" },
        { name: "System.Xml.XmlGrammar.ReferenceRule", id: "diagram_node_105", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_ReferenceRule_20058B27121F4365906EE66F77BC7CA1B64BB710.html" },
        { name: "System.Xml.XmlGrammar.SDDeclRule", id: "diagram_node_107", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_SDDeclRule_FCDBA007A9A33A45137422601E8E54692139AD60.html" },
        { name: "System.Xml.XmlGrammar.SystemLiteralRule", id: "diagram_node_99", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_SystemLiteralRule_5B579AA0D00B533B884834F9DCF0B6A1DF261903.html" },
        { name: "System.Xml.XmlGrammar.TextDeclRule", id: "diagram_node_108", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_TextDeclRule_7DF2155F945AACA495FEF3888304CC62507BAF97.html" },
        { name: "System.Xml.XmlGrammar.VersionInfoRule", id: "diagram_node_110", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_VersionInfoRule_86E6A652F13CBCD37BE962CB0AFE999316FEA018.html" },
        { name: "System.Xml.XmlGrammar.XMLDeclRule", id: "diagram_node_111", level: 3, subject: false, hasDerivedNodes: false, link: "../../System.Xml/doc/class_XMLDeclRule_06CBA4384E2D398D90AE9BA7E1A16E52CEE13E04.html" }];
    drawDiagram(diagramNodes);
}

