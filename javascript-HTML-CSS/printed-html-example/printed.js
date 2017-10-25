var printedHtml = {
  version: "0.0.99",
  dpi: 1,
  abc: "abcdefghijklmnopqrstuvwxyz",
  III: ["I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX", "X"],
  //FIXME These should be taken from css
  pageHeight: 29.7,
  pageWidth: 21,
  pageMarginTop: 2.7,
  pageMarginBottom: 2.7,
  pageMarginLeft: 1.6,
  pageMarginRight: 1.6
};

printedHtml.ABC = printedHtml.abc.toUpperCase();

//get dpi
while (!matchMedia("(max-resolution: " + printedHtml.dpi + "dpi)").matches)
  printedHtml.dpi += 1;
printedHtml.cm_per_px = 2.54 / printedHtml.dpi;

//retrieve document structure
printedHtml.structure = {};

printedHtml.xhr = function(url, ondone) {
  var xmlHttpRequest = new XMLHttpRequest();
  xmlHttpRequest.open("POST", url, true);
  xmlHttpRequest.onreadystatechange = function() {
    if (xmlHttpRequest.readyState === 4)
    {
      ondone(xmlHttpRequest.responseText);
    }
  }
  xmlHttpRequest.send();
}

printedHtml.xhr("structure.json", function(responseText){
  printedHtml.structure = JSON.parse(responseText);
});

printedHtml.clearCount = function(levelName)
{
  for (var i=0; i < printedHtml.structure[levelName].children.length; i++)
  {
    var child = printedHtml.structure[levelName].children[i];
    if (printedHtml.structure[child])
    {
      printedHtml.structure[child].count = 0;
      printedHtml.clearCount(child);
    }
  }
}

printedHtml.isInStructure = function(element)
{
  var levelName = null;

  for (var key in printedHtml.structure)
  {
    if (printedHtml.structure[key].selector)
    {
      if (printedHtml.structure[key].selector.startsWith("."))
      {
        if ("."+element.className === printedHtml.structure[key].selector)
        {
          levelName = key;
          break;
        }
      }
      else if (element.tagName === printedHtml.structure[key].selector.toUpperCase())
      {
        levelName = key;
        break;
      }
    }

    else if (element.className === key)
    {
      levelName = key;
      break;
    }
  }

  return levelName;
}

printedHtml.nextElement = function(element)
{
  if (element.dataset) element.dataset.done = "1";

  if (element.firstElementChild)
  {
    if (element.firstElementChild.dataset)
    {
      if (element.firstElementChild.dataset.done !== "1")
      {
        return element.firstElementChild;
      }
    }
  }

  if (element.nextElementSibling)
  {
    if (element.nextElementSibling.dataset)
    {
      if (element.nextElementSibling.dataset.done !== "1")
      {
        return element.nextElementSibling;
      }
    }
    else
    {
      return printedHtml.nextElement(element.nextElementSibling);
    }
  }

  if (element.parentNode !== document.body)
  {
    return printedHtml.nextElement(element.parentNode);
  }

  else
  {
    doneElements = document.querySelectorAll("[data-done='1'");
    for (var i=0; i<doneElements.length; i++)
    {
      delete doneElements[i].dataset.done;
    }
    return null;
  }
}

printedHtml.createNumbering = function()
{
  for (var level in printedHtml.structure)
  {
    printedHtml.structure[level].count = 0;
  }

  var cm_per_px = printedHtml.cm_per_px;
  var isInStructure = printedHtml.isInStructure;

  var toc = document.querySelector("#toc");

  var currentElement = document.body.firstElementChild;

  while (currentElement)
  {
    var levelName = isInStructure(currentElement);

    if (levelName)
    {
      currentLevel = printedHtml.structure[levelName];

      currentLevel.count = currentLevel.count + 1;

      printedHtml.clearCount(levelName);

      var text = currentLevel.text;
      var ref = currentLevel.ref;

      for (var level in printedHtml.structure)
      {
        var obj = printedHtml.structure[level];
        var id = obj.count || 0;
        if (obj.type == "abc") id = printedHtml.abc[id-1];
        else if (obj.type == "ABC") num = printedHtml.ABC[id-1];
        else if (obj.type == "III") num = printedHtml.III[id-1];

        text = text.replace("%"+level+"%", id);
        ref = ref.replace("%"+level+"%", id);
      }

      //caption
      var caption = null;

      if (currentLevel.captionSelector)
      {
        if ("+>: ".contains(currentLevel.captionSelector[0]))
        {
          if (currentElement.id !== "")
          {
            caption = currentElement.querySelector("#"+currentElement.id+currentLevel.captionSelector);
          }
          else if (currentElement.className !== "")
          {
              caption = currentElement.querySelector("."+currentElement.className+currentLevel.captionSelector);
          }
          else
          {
              caption = currentElement.querySelector(currentElement.tagName+currentLevel.captionSelector);
          }
        }
        else
        {
          caption = currentElement.querySelector(currentLevel.captionSelector);
        }
      }
      else
      {
        caption = currentElement;
      }

      if (caption)
      {
        if (caption.innerHTML != "")
          text = text.replace("%:%", ": ");
        else
          text = text.replace("%:%", "");

        var numberingSpan = null;

        for (var i=0; i<caption.children.length; i++)
        {
          if (caption.children[i].className === "numbering")
          {
            numberingSpan = caption.children[i];
            break;
          }
        }

        if (!numberingSpan)
        {
          numberingSpan = document.createElement("span");
          numberingSpan.className = "numbering";
          caption.insertBefore(numberingSpan, caption.childNodes[0]);
        }

        numberingSpan.innerHTML = text;
      }

      //references
      if (currentElement.id !== "")
      {
        refs = document.querySelectorAll("a[href$='#"+currentElement.id+"']");
        for (var i=0; i<refs.length; i++)
        {
          if (refs[i].innerHTML === "" || refs[i].className === "reference")
          {
            refs[i].innerHTML = ref;
            refs[i].className = "reference";
          }
        }
      }

      //toc
      var tocEntry = document.querySelector("#toc a[href='#"+currentElement.id+"']");

      if (tocEntry)
      {
        tocEntry.innerHTML = caption.innerHTML;
      }
      else if (toc && currentLevel.toc)
      {
        var title = document.createElement("div");
        title.className = levelName + "Toc";
        title.style = "display: inline-block";
        var a = document.createElement("a");
        a.href = "#" + currentElement.id;
        a.innerHTML = caption.innerHTML;
        title.appendChild(a);
        var pageNumber = document.createElement("div");
        pageNumber.dataset.id = currentElement.id;
        pageNumber.className = "pageNumber";
        pageNumber.innerHTML = "....";
        var separator = document.createElement("div");
        separator.style = "display: block; height: 5mm"
        toc.appendChild(title);
        toc.appendChild(pageNumber);
        toc.appendChild(separator);
      }

    }

    currentElement = printedHtml.nextElement(currentElement);
  }
}

printedHtml.createPageNumbers = function()
{
  var cm_per_px = printedHtml.cm_per_px;
  var isInStructure = printedHtml.isInStructure;
  var pageHeight = printedHtml.pageHeight-printedHtml.pageMarginTop-printedHtml.pageMarginBottom;

  var toc = document.querySelector("#toc");

  var getPage = function(top)
  {
    var page = top/pageHeight;

    if (parseInt(page) === page)
    {
      return page+1;
    }
    else
    {
      return Math.ceil(page);
    }
  }

  var currentElement = document.body.firstElementChild;
  currentElement.dataset.extraHeight = 0;

  while (currentElement)
  {
    var style = window.getComputedStyle(currentElement);

    //extra height due to move to next page of an element
    var currentExtraHeight = parseFloat(currentElement.dataset.extraHeight) || 0;

    var currentTop = currentElement.offsetTop*cm_per_px+currentExtraHeight;

    var cmHeight = currentElement.offsetHeight*cm_per_px;

    var freeHeight = getPage(currentTop)*pageHeight-currentTop;

    if ((freeHeight < cmHeight) && (currentElement.tagName === "IMG" || style.pageBreakInside === "avoid"))
    {
      var currentExtraHeight = cmHeight-freeHeight+currentExtraHeight;
      currentElement.dataset.extraHeight = currentExtraHeight;
    }

    var currentTop = currentElement.offsetTop*cm_per_px+currentExtraHeight;

    currentElement.dataset.pageNumber = getPage(currentTop);

    //calc extra height due to page breaks after elements
    if (currentElement.nextElementSibling)
    {
      if (currentElement.nextElementSibling.dataset)
      {
        if (style.pageBreakAfter === "always")
        {
          var nextHeight = currentElement.nextElementSibling.offsetTop*cm_per_px+
            +currentExtraHeight;
          currentElement.nextElementSibling.dataset.extraHeight =
            getPage(nextHeight)*pageHeight-nextHeight+currentExtraHeight;
        }
        else
        {
          currentElement.nextElementSibling.dataset.extraHeight = currentExtraHeight;
        }
      }
    }

    if (currentElement.firstElementChild && currentElement.firstElementChild.dataset)
    {
      currentElement.firstElementChild.dataset.extraHeight = currentExtraHeight;
    }

    delete currentElement.dataset.extraHeight;

    currentElement = printedHtml.nextElement(currentElement);
  }

  //update page numbers on toc
  if (toc)
  {
    var pageNumbers = toc.querySelectorAll(".pageNumber");
    for (var i=0; i<pageNumbers.length; i++)
    {
      var element = document.getElementById(pageNumbers[i].dataset.id);
      if (element)
      {
        var pointWidth = pageNumbers[i].offsetWidth/4;
        pageNumbers[i].innerHTML = " "+element.dataset.pageNumber;
        var titleWidth = pageNumbers[i].previousElementSibling.offsetWidth+
          pageNumbers[i].previousElementSibling.offsetLeft;
        var pageWidth = (printedHtml.pageWidth-printedHtml.pageMarginRight)/cm_per_px;
        var points = Math.floor((pageWidth-titleWidth-pageNumbers[i].offsetWidth)/pointWidth);
        var text = " ";
        for (var j=0; j<points-2; j++)
        {
          text = "."+text;
        }
        pageNumbers[i].innerHTML = text+element.dataset.pageNumber;
      }
    }
  }
}

printedHtml.init = function()
{
  printedHtml.createNumbering();
  printedHtml.createPageNumbers();
}

printedHtml.initOnLoad = function()
{
  window.addEventListener("load", printedHtml.init);
}
