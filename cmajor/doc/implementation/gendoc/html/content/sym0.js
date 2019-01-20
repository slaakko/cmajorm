function drawTreeNodes(nodes) {
   var maxTextWidth = 0;
   var maxTextHeight = 0;
   var maxLevel = 0;
   var xspace = 8;
   var yspace = 8;
   var rectXSpace = 40;
   var rectYSpace = 20;
   var svg = document.getElementById("treeDiagram");
   var n = nodes.length;
   for (var i = 0; i < n; ++i) {
       var node = nodes[i];
       var nodeLevel = node.level;
       if (nodeLevel > maxLevel) {
           maxLevel = nodeLevel;
       }
       var rectElement = document.createElementNS('http://www.w3.org/2000/svg', 'rect');
       rectElement.setAttribute("id", "rect" + i.toString());
       svg.appendChild(rectElement);
       var textElement = document.createElementNS('http://www.w3.org/2000/svg', 'text');
       if (node.id && node.id != "") {
           textElement.setAttribute("id", node.id);
       } else {
           textElement.setAttribute("id", "text" + i.toString());
       }
       textElement.innerHTML = node.name;
       var hasLink = node.link && node.link != "";
       if (hasLink) {
           if (node.link.indexOf(".html") == -1) {
               textElement.targetId = node.link;
               textElement.onclick = function() {
                   var targetTextElement = document.getElementById(this.targetId);
                   if (targetTextElement) {
                       var h = targetTextElement.getBBox().y - this.getBBox().y;
                       window.scrollBy(0, h);
                   }
               };
               svg.appendChild(textElement);
           } else {
               var linkElement = document.createElementNS('http://www.w3.org/2000/svg', 'a');
               linkElement.setAttribute("href", node.link);
               linkElement.appendChild(textElement);
               svg.appendChild(linkElement);
           }
       } else {
           svg.appendChild(textElement);
       }
       var bb = textElement.getBBox();
       var textWidth = bb.width;
       var textHeight = bb.height;
       node.textWidth = textWidth;
       node.textHeight = textHeight;
       if (textWidth > maxTextWidth) {
           maxTextWidth = textWidth;
       }
       if (textHeight > maxTextHeight) {
           maxTextHeight = textHeight;
       }
   }
   var rectWidth = maxTextWidth + 2 * xspace;
   var rectHeight = maxTextHeight + 2 * yspace;
   var totalWidth = rectWidth + maxLevel * rectXSpace;
   var totalHeight = n * (rectHeight + rectYSpace);
   svg.setAttribute("width", totalWidth.toString());
   svg.setAttribute("height", totalHeight.toString());
   var levelNode = [];
   for (var i = 0; i < n; ++i) {
       var node = nodes[i];
       var level = node.level;
       if (level == levelNode.length) {
           levelNode.push(node);
       }
       else {
           levelNode[level] = node;
       }
       var rectX = level * rectXSpace;
       var rectY = i * (rectHeight + rectYSpace);
       var levelX = rectX + rectXSpace / 2;
       var levelY = rectY + rectHeight;
       node.x = levelX;
       node.y = levelY;
       var textWidth = node.textWidth;
       var textHeight = node.textHeight;
       var textX = rectX + (rectWidth - textWidth) / 2;
       var textY = (rectY + rectHeight - yspace / 2) - (rectHeight - textHeight) / 2;
       var rectElement = document.getElementById("rect" + i.toString());
       rectElement.setAttribute("x", rectX.toString());
       rectElement.setAttribute("y", rectY.toString());
       rectElement.setAttribute("width", rectWidth.toString());
       rectElement.setAttribute("height", rectHeight.toString());
       rectElement.setAttribute("fill", "white");
       rectElement.setAttribute("stroke", "black");
       var textId = "text" + i.toString();
       if (node.id && node.id != "") {
           textId = node.id;
       }
       var textElement = document.getElementById(textId);
       textElement.setAttribute("x", textX.toString());
       textElement.setAttribute("y", textY.toString());
       if (level > 0) {
           var x1 = rectX;
           var y1 = rectY + rectHeight / 2;
           var x2 = levelNode[level - 1].x;
           var y2 = levelNode[level - 1].y;
           var horLineElement = document.createElementNS('http://www.w3.org/2000/svg', 'line');
           horLineElement.setAttribute("x1", x1.toString());
           horLineElement.setAttribute("y1", y1.toString());
           horLineElement.setAttribute("x2", x2.toString());
           horLineElement.setAttribute("y2", y1.toString());
           horLineElement.setAttribute("stroke", "black");
           svg.appendChild(horLineElement);
           var verLineElement = document.createElementNS('http://www.w3.org/2000/svg', 'line');
           verLineElement.setAttribute("x1", x2.toString());
           verLineElement.setAttribute("y1", y1.toString());
           verLineElement.setAttribute("x2", x2.toString());
           verLineElement.setAttribute("y2", y2.toString());
           verLineElement.setAttribute("stroke", "black");
           svg.appendChild(verLineElement);
       }
   }
}
function drawTree() {
 var nodes = [
  { name: "symbolTable", id: "", level: 0, link: "class_SymbolTable_ED1EDE715C5A9505E6FD6DBB1CB6B668AB12B460.html" },
  { name: "symbols", id: "", level: 1, link: "" },
  { name: "NamespaceSymbol", id: "", level: 2, link: "class_NamespaceSymbol_BCCF6423401B565F4ACE6EB96887A998E2535933.html" },
  { name: "FunctionGroupSymbol(Partition)", id: "", level: 3, link: "class_FunctionGroupSymbol_ADD9128234E8CBDCA551DECFEB138ABA2C75FD11.html" },
  { name: "FunctionSymbol(Partition)", id: "", level: 4, link: "class_FunctionSymbol_5AC1C26A9572181B3E31247997BF40453B22EDEF.html" },
  { name: "ParameterSymbol(intList)", id: "", level: 5, link: "class_ParameterSymbol_EFEA20B84930B4A79D869DA6EFE9F58679F053A6.html" },
  { name: "ParameterSymbol(left)", id: "", level: 5, link: "class_ParameterSymbol_EFEA20B84930B4A79D869DA6EFE9F58679F053A6.html" },
  { name: "ParameterSymbol(right)", id: "", level: 5, link: "class_ParameterSymbol_EFEA20B84930B4A79D869DA6EFE9F58679F053A6.html" },
  { name: "DeclarationBlock", id: "", level: 5, link: "class_DeclarationBlock_585311538A78AC7083BB3F8BA178ADC04C211148.html" },
  { name: "LocalVariableSymbol(i)", id: "", level: 6, link: "class_LocalVariableSymbol_F5A9F74EEFC0001129616C5EEBCFE3B2DFC74FCF.html" },
  { name: "LocalVariableSymbol(j)", id: "", level: 6, link: "class_LocalVariableSymbol_F5A9F74EEFC0001129616C5EEBCFE3B2DFC74FCF.html" },
  { name: "LocalVariableSymbol(pivot)", id: "", level: 6, link: "class_LocalVariableSymbol_F5A9F74EEFC0001129616C5EEBCFE3B2DFC74FCF.html" },
  { name: "DeclarationBlock", id: "", level: 6, link: "class_DeclarationBlock_585311538A78AC7083BB3F8BA178ADC04C211148.html" },
  { name: "DeclarationBlock", id: "", level: 7, link: "class_DeclarationBlock_585311538A78AC7083BB3F8BA178ADC04C211148.html" },
  { name: "FunctionGroupSymbol(QuickSort)", id: "", level: 3, link: "class_FunctionGroupSymbol_ADD9128234E8CBDCA551DECFEB138ABA2C75FD11.html" },
  { name: "FunctionSymbol(QuickSort)", id: "", level: 4, link: "class_FunctionSymbol_5AC1C26A9572181B3E31247997BF40453B22EDEF.html" },
  { name: "ParameterSymbol(intList)", id: "", level: 5, link: "class_ParameterSymbol_EFEA20B84930B4A79D869DA6EFE9F58679F053A6.html" },
  { name: "ParameterSymbol(left)", id: "", level: 5, link: "class_ParameterSymbol_EFEA20B84930B4A79D869DA6EFE9F58679F053A6.html" },
  { name: "ParameterSymbol(right)", id: "", level: 5, link: "class_ParameterSymbol_EFEA20B84930B4A79D869DA6EFE9F58679F053A6.html" },
  { name: "DeclarationBlock", id: "", level: 5, link: "class_DeclarationBlock_585311538A78AC7083BB3F8BA178ADC04C211148.html" },
  { name: "LocalVariableSymbol(index)", id: "", level: 6, link: "class_LocalVariableSymbol_F5A9F74EEFC0001129616C5EEBCFE3B2DFC74FCF.html" },
  { name: "DeclarationBlock", id: "", level: 6, link: "class_DeclarationBlock_585311538A78AC7083BB3F8BA178ADC04C211148.html" },
  { name: "DeclarationBlock", id: "", level: 6, link: "class_DeclarationBlock_585311538A78AC7083BB3F8BA178ADC04C211148.html" },
  { name: "FunctionSymbol(QuickSort)", id: "", level: 4, link: "class_FunctionSymbol_5AC1C26A9572181B3E31247997BF40453B22EDEF.html" },
  { name: "ParameterSymbol(intList)", id: "", level: 5, link: "class_ParameterSymbol_EFEA20B84930B4A79D869DA6EFE9F58679F053A6.html" },
  { name: "DeclarationBlock", id: "", level: 5, link: "class_DeclarationBlock_585311538A78AC7083BB3F8BA178ADC04C211148.html" },
  { name: "DeclarationBlock", id: "", level: 6, link: "class_DeclarationBlock_585311538A78AC7083BB3F8BA178ADC04C211148.html" },
  { name: "FunctionGroupSymbol(Print)", id: "", level: 3, link: "class_FunctionGroupSymbol_ADD9128234E8CBDCA551DECFEB138ABA2C75FD11.html" },
  { name: "FunctionSymbol(Print)", id: "", level: 4, link: "class_FunctionSymbol_5AC1C26A9572181B3E31247997BF40453B22EDEF.html" },
  { name: "ParameterSymbol(intList)", id: "", level: 5, link: "class_ParameterSymbol_EFEA20B84930B4A79D869DA6EFE9F58679F053A6.html" },
  { name: "DeclarationBlock", id: "", level: 5, link: "class_DeclarationBlock_585311538A78AC7083BB3F8BA178ADC04C211148.html" },
  { name: "LocalVariableSymbol(first)", id: "", level: 6, link: "class_LocalVariableSymbol_F5A9F74EEFC0001129616C5EEBCFE3B2DFC74FCF.html" },
  { name: "FunctionGroupSymbol(main)", id: "", level: 3, link: "class_FunctionGroupSymbol_ADD9128234E8CBDCA551DECFEB138ABA2C75FD11.html" },
  { name: "FunctionSymbol(main)", id: "", level: 4, link: "class_FunctionSymbol_5AC1C26A9572181B3E31247997BF40453B22EDEF.html" },
  { name: "DeclarationBlock", id: "", level: 5, link: "class_DeclarationBlock_585311538A78AC7083BB3F8BA178ADC04C211148.html" },
  { name: "LocalVariableSymbol(intList)", id: "", level: 6, link: "class_LocalVariableSymbol_F5A9F74EEFC0001129616C5EEBCFE3B2DFC74FCF.html" },
  { name: "LocalVariableSymbol(n)", id: "", level: 6, link: "class_LocalVariableSymbol_F5A9F74EEFC0001129616C5EEBCFE3B2DFC74FCF.html" },
  { name: "DeclarationBlock", id: "", level: 6, link: "class_DeclarationBlock_585311538A78AC7083BB3F8BA178ADC04C211148.html" },
  { name: "LocalVariableSymbol(i)", id: "", level: 7, link: "class_LocalVariableSymbol_F5A9F74EEFC0001129616C5EEBCFE3B2DFC74FCF.html" },
  { name: "DeclarationBlock", id: "", level: 7, link: "class_DeclarationBlock_585311538A78AC7083BB3F8BA178ADC04C211148.html" },
  { name: "types", id: "", level: 1, link: "" }];
 drawTreeNodes(nodes);
}

