/*  
 *    @licstart  The following is the entire license notice for the 
 *    JavaScript code in this page.
 *    
 *    Copyright (C) 2014 Marco Scarpetta
 * 
 *    The JavaScript code in this page is free software: you can
 *    redistribute it and/or modify it under the terms of the GNU
 *    General Public License (GNU GPL) as published by the Free Software
 *    Foundation, either version 3 of the License, or (at your option)
 *    any later version.  The code is distributed WITHOUT ANY WARRANTY;
 *    without even the implied warranty of MERCHANTABILITY or FITNESS
 *    FOR A PARTICULAR PURPOSE.  See the GNU GPL for more details.
 * 
 *    As additional permission under GNU GPL version 3 section 7, you
 *    may distribute non-source (e.g., minimized or compacted) forms of
 *    that code without the copy of the GNU GPL normally required by
 *    section 4, provided you include this license notice and a URL
 *    through which recipients can access the Corresponding Source.
 *
 *    @licend  The above is the entire license notice
 *    for the JavaScript code in this page.
 */

//Node class
function Node(x, y, type) {
  this.x = x;
  this.y = y;
  this.type = type;
  this.next = null;
}

Node.prototype.clone = function() {
  return new Node(this.x, this.y, this.type);
}

Node.prototype.getPosition = function() {
  return [this.x, this.y];
}

Node.prototype.equalTo = function(node) {
  return (this.x === node.x && this.y === node.y);
}

Node.prototype.indexInArray = function(array) {
  var list = [];
  for (var i=0; i<array.length; i++) {
    if (this.equalTo(array[i])) {list.push(i);}
  }
  return list;
}

Node.prototype.collide = function(array) {
  var list = [];
  for (var i=0; i<array.length; i++) {
    var node = array[i];
    var flag = false;
    while (node.next !== null) {
      var maxX = Math.max(node.x, node.next.x);
      var minX = Math.min(node.x, node.next.x);
      var maxY = Math.max(node.y, node.next.y);
      var minY = Math.min(node.y, node.next.y);
      if (this.x >= minX && this.x <= maxX && this.y >= minY && this.y <= maxY) {
	flag = true;
      }
      node = node.next;
    }
    if (flag || this.equalTo(array[i])) {list.push(i);}
  }
  return list;
}

//Snake class
function Snake(head, tail) {
  this.head = head;
  this.head.next = tail;
  this.direction = 0;
  this.points = 0;
  this.key = false;
}

Snake.prototype.clear = function() {
  this.head = new Node(10, 4, "node");
  this.head.next = new Node(3, 4, "node");
  this.direction = 0;
  this.points = 0;
  this.key = false;
}

Snake.prototype.getTail = function() {
  var node = this.head;
  while(node.next != null) {
    if (node.next.next == null) {break;}
    node = node.next;
  }
  return node;
}

Snake.prototype.changeDirection = function(newDirection) {
  var x = Math.sign(this.head.x - this.head.next.x);
  var y = Math.sign(this.head.y - this.head.next.y);
  
  if (x === 0 && (newDirection === "up" || newDirection === "down")) {return;}
  if (y === 0 && (newDirection === "left" || newDirection === "right")) {return;}
  
  this.newHead = this.head.clone();
  if (newDirection === "up") {this.newHead.y -= 1; return;}
  if (newDirection === "down") {this.newHead.y += 1; return;}
  if (newDirection === "right") {this.newHead.x += 1; return;}
  if (newDirection === "left") {this.newHead.x -= 1; return;}
  
  if (newDirection === "cw") {
    this.newHead.x -= y;
    this.newHead.y += x;
    return;
  }
  if (newDirection === "ccw") {
    this.newHead.x += y;
    this.newHead.y -= x;
    return;
  }
  
  this.newHead = null;
}

Snake.prototype.update = function() {
  if (this.newHead) {
    this.newHead.next = this.head;
    this.head = this.newHead;
    this.newHead = null;
  }
  else {
    var x = Math.sign(this.head.x - this.head.next.x);
    var y = Math.sign(this.head.y - this.head.next.y);
    this.head.x += x;
    this.head.y += y;
  }
  
  var node = this.getTail();
  node.next.x += Math.sign(node.x - node.next.x);
  node.next.y += Math.sign(node.y - node.next.y);
  if (node.x == node.next.x && node.y == node.next.y) {
    delete node.next;
    node.next = null;
  }
}

Snake.prototype.grow = function() {
  var node = this.getTail();
  var x = Math.sign(node.x - node.next.x);
  var y = Math.sign(node.y - node.next.y);
  node.next.x -= x;
  node.next.y -= y;
  this.points += 1;
  document.getElementById("points").innerHTML = this.points;
}

//Map class
function Map(mode) {
  this.mode = mode;
  this.width = 32;
  this.height = 38;
  this.cellSize = 10;
  this.objects = [];
  this.eatFruits = 0;
  this.fruitsToKey = 0;
  this.levelId = null;
  this.resources = {};
  this.notLoaded = ["tail-up",true,"tail-right",true,"tail-down",true,"tail-left",true,
    "head-up",true,"head-right",true,"head-down",true,"head-left",true,
    "door",true,"fruit",true,"key",true,"rock",true,'ground',false];
  this.background = new Image();
  this.paused = true;
  this.update(this);
  this.timeout = 100;
}

Map.prototype.loadImage = function(name, resize=true) {
  var map = this;
  var loadNext = function() {
    if (map.notLoaded.length > 0) {
      var res = map.notLoaded.pop();
      var name = map.notLoaded.pop();
      map.loadImage(name, res);
    }
    else {map.allLoaded();}
  }
  var image = new Image();
  if (resize) {
    var canvas = document.createElement('canvas');
    canvas.width = this.cellSize;
    canvas.height = this.cellSize;
    
    image.onload = function() {
      canvas.getContext("2d").clearRect(0,0,map.cellSize,map.cellSize);
      canvas.getContext("2d").drawImage(image,0,0,map.cellSize,map.cellSize);
      image.onload = loadNext;
      image.src = canvas.toDataURL("image/png");
    };
  }
  else {
    image.onload = loadNext;
  }
  image.src = "data/"+name+".png";
  this.resources[name] = image;
}

Map.prototype.loadResources = function(allLoaded) {
  if (this.notLoaded.length > 0) {
    var res = this.notLoaded.pop();
    var name = this.notLoaded.pop();
    this.loadImage(name, res);
    this.allLoaded = allLoaded;
  }
  else {
    allLoaded();
  }
}

Map.prototype.loadLevel = function(levelId) {
  var canvas = document.getElementById('gameCanvas');
  var ctx = canvas.getContext("2d");
  this.objects = []
  this.snake.clear();
  this.eatFruits = 0;
  
  if (levelId === "survival") {
    this.mode = "survival";
    var pattern = ctx.createPattern(this.resources["ground"], "repeat");
    ctx.fillStyle = pattern;
    ctx.fillRect(0,0,this.cellSize*this.width,this.cellSize*this.height);
  }
  else {
    this.mode = "levels";
    this.levelId = levelId;
    var level = levels[levelId];
    this.fruitsToKey = level.fruitsToKey;
    for (var i=0; i<level.rocks.length; i++) {
      var rock = new Node(level.rocks[i][0][0], level.rocks[i][0][1], 'rock');
      var last = rock;
      for (var j=0; j<level.rocks[i].length; j++) {
	last.next = new Node(level.rocks[i][j][0], level.rocks[i][j][1], 'rock');
	last = last.next;
      }
      this.objects.push(rock);
    }
    this.objects.push(new Node(level.door[0], level.door[1], 'door'));
    
    //render background
    var pattern = ctx.createPattern(this.resources["ground"], "repeat");
    ctx.fillStyle = pattern;
    ctx.fillRect(0,0,this.cellSize*this.width,this.cellSize*this.height);
    
    var pattern = ctx.createPattern(this.resources["rock"], "repeat");
    for (var i in this.objects) {
      if (this.objects[i].type == "rock") {
	var currentNode = this.objects[i];
	var nextNode = this.objects[i].next;
	
	if (nextNode == null) {
	  ctx.fillStyle = pattern;
	  ctx.fillRect(this.objects[i].x*this.cellSize,this.objects[i].y*this.cellSize,this.cellSize,this.cellSize);
	}
	while (nextNode != null) {
	  var x = Math.min(currentNode.x, nextNode.x)*this.cellSize;
	  var y = Math.min(currentNode.y, nextNode.y)*this.cellSize;
	  var width = (Math.abs(currentNode.x - nextNode.x)+1)*this.cellSize;
	  var height = (Math.abs(currentNode.y - nextNode.y)+1)*this.cellSize;
	  ctx.fillStyle = pattern;
	  ctx.fillRect(x,y,width,height);
	  currentNode = nextNode;
	  nextNode = currentNode.next;
	}
      }
      else if (this.objects[i].type == "door") {
	ctx.drawImage(this.resources['door'],this.objects[i].x*this.cellSize,this.objects[i].y*this.cellSize);
      }
    }
  }
  var map = this;
  this.background.onload = function() {map.paused = false;};
  this.background.src = canvas.toDataURL("image/png");
}

Map.prototype.addFruit = function() {
  var fruit;
  do {
    fruit = new Node(Math.floor(Math.random()*this.width), Math.floor(Math.random()*this.height), "fruit");
  }
  while(fruit.collide(this.objects).length > 0)
  
  if (this.mode === "levels" && this.eatFruits === this.fruitsToKey) {
    fruit.type = "key";
  }
  this.objects.push(fruit);
  this.eatFruits += 1;
}


Map.prototype.render = function() {
  var canvas = document.getElementById('gameCanvas');
  var ctx = canvas.getContext("2d");

  //background
  ctx.drawImage(this.background,0,0);

  //snake
  //var rects = [];
  var currentNode = this.snake.head;
  var nextNode = this.snake.head.next;
  
  var headX = Math.sign(this.snake.head.x - this.snake.head.next.x);
  var headY = Math.sign(this.snake.head.y - this.snake.head.next.y);
  
  var node = this.snake.getTail();
  var tailX = Math.sign(node.x - node.next.x);
  var tailY = Math.sign(node.y - node.next.y);
  
  var padding = Math.floor(this.cellSize/5);
  
  while (nextNode != null) {
    var x = Math.min(currentNode.x, nextNode.x)*this.cellSize;
    var y = Math.min(currentNode.y, nextNode.y)*this.cellSize;
    var width = (Math.abs(currentNode.x - nextNode.x)+1)*this.cellSize;
    var height = (Math.abs(currentNode.y - nextNode.y)+1)*this.cellSize;
    //rects.push([x,y,width,height]);
    ctx.fillStyle = '#0a0';
    ctx.fillRect(x+padding,y+padding,width-2*padding,height-2*padding);
    currentNode = nextNode;
    nextNode = currentNode.next;
  }
  
  var orientation;
  if (headX===0) {if (headY===1) {orientation = "down";} else {orientation="up";}}
  else if (headY===0) {if (headX===1) {orientation = "right";} else {orientation="left";}}
  ctx.drawImage(this.resources["head-"+orientation],this.snake.head.x*this.cellSize,this.snake.head.y*this.cellSize);
  
  if (tailX===0) {if (tailY===-1) {orientation = "down";} else {orientation="up";}}
  else if (tailY===0) {if (tailX===-1) {orientation = "right";} else {orientation="left";}}
  ctx.drawImage(this.resources["tail-"+orientation],node.next.x*this.cellSize,node.next.y*this.cellSize);
  
  /*var lineWidth = Math.floor(this.cellSize/3);
  for (var i in rects) {
    ctx.fillStyle = '#060';
    ctx.fillRect(rects[i][0]+lineWidth,rects[i][1]+lineWidth,rects[i][2]-2*lineWidth,rects[i][3]-2*lineWidth);
  }*/
  
  //other objects
  for (var i in this.objects) {
    if (this.objects[i].type == "fruit") {
      ctx.drawImage(this.resources['fruit'],this.objects[i].x*this.cellSize,this.objects[i].y*this.cellSize);
    }
    else if (this.objects[i].type == "key") {
      ctx.drawImage(this.resources['key'],this.objects[i].x*this.cellSize,this.objects[i].y*this.cellSize);
    }
  }
}

Map.prototype.check = function() {
  //check if snake hits itself
  var i=1;
  var node = this.snake.head;
  while (node.next !== null) {
    if (i>3) {
      var maxX = Math.max(node.x, node.next.x);
      var minX = Math.min(node.x, node.next.x);
      var maxY = Math.max(node.y, node.next.y);
      var minY = Math.min(node.y, node.next.y);
      if (this.snake.head.x >= minX && this.snake.head.x <= maxX && this.snake.head.y >= minY && this.snake.head.y <= maxY) {
	return 1;
      }
    }
    node = node.next;
    i+=1;
  }
  //check if snake hits the border
  if (this.snake.head.x < 0 || this.snake.head.x > this.width-1 || this.snake.head.y < 0 || this.snake.head.y > this.height-1) {
    return 1;
  }
  //check if snake hits something
  var list = this.snake.head.collide(this.objects);
  for (var i=0; i<list.length; i++) {
    if (this.objects[list[i]].type === "rock") {
      return 1;
    }
    if (this.mode === "levels") {
      if (this.objects[list[i]].type === "door" && this.snake.key === true) {
        return 2;
      }
      if (this.objects[list[i]].type === "key") {
        this.objects.pop(list[i]);
	this.snake.key = true;
	return 0;
      }
    }
    if (this.objects[list[i]].type === "fruit") {
      this.objects.pop(list[i]);
      this.addFruit();
      this.snake.grow();
    }
  }
  return 0;
}

Map.prototype.update = function(map) {
  if (!map.paused) {
    map.snake.update();
    var state = map.check();
    switch (state) {
      case 0: 
	map.render();
	break;
      case 1: 
	map.paused = true
	setTimeout(gameOver, 200, map);
	break;
      case 2:
	map.paused = true;
	levelCleared(map);
	break;
    }
  }
  setTimeout(map.update, map.timeout, map);
}

function gameOver(map) {
  document.querySelector('#game').className = 'currentToLeft';
  document.querySelector('#game-over').className = 'current';
  document.querySelector('#level-cleared-h1').style.display = "none";
  document.querySelector('#game-over-h1').style.display = "";
  if (map.mode === "survival") {
    if (localStorage.getItem("high-scores") == null) {localStorage['high-scores']='[]'}
    var highScores = JSON.parse(localStorage.getItem('high-scores'));
    var i=0;
    while (i<highScores.length) {
      if (map.snake.points > highScores[i].points) {break;}
      i++
    }
    if (i<10) {
      document.querySelector('#game-over-form').style.display = "";
      document.querySelector('#btn-gameover-save-score').style.display = "";
      document.querySelector('#btn-gameover-save-score').onclick = function () {
	highScores.splice(i, 0, {
	  'name': document.querySelector('#high-scores-name').value,
	  'points': map.snake.points
	});
	while (highScores.length > 10) {highScores.pop();}
	localStorage['high-scores'] = JSON.stringify(highScores);
	displayHighScores();
	document.querySelector('#game-over').className = 'currentToLeft';
	document.querySelector('#high-scores').className = 'current';
      }
    }
    else {
      document.querySelector('#game-over-form').style.display = "none";
      document.querySelector('#btn-gameover-save-score').style.display = "none";
    }
    document.querySelector('#btn-gameover-try-again').style.display = "none";
    
  }
  else if (map.mode === "levels") {
    document.querySelector('#game-over-form').style.display = "none";
    document.querySelector('#btn-gameover-save-score').style.display = "none";
    document.querySelector('#btn-gameover-try-again').style.display = "";
    document.querySelector('#btn-gameover-try-again').onclick = function () {
      startGame(map, 'levels', map.levelId);
      document.querySelector('#game').className = 'leftToCurrent';
      document.querySelector('#game-over').className = 'right';
    }
  }
  document.querySelector('#btn-gameover-next-level').style.display = "none";
  
}

function displayHighScores() {
  var highScores = JSON.parse(localStorage.getItem('high-scores'));
  if (highScores == null) {return;}
  var list = document.querySelector('#high-scores-list');
  while (list.firstChild) {
    list.removeChild(list.firstChild);
  }
  for (var i in highScores) {
    var li = document.createElement("li");
    list.appendChild(li);
    
    var pName = document.createElement("p");
    pName.appendChild(document.createTextNode(highScores[i].name));
    
    var pPoints = document.createElement("p");
    pPoints.appendChild(document.createTextNode(highScores[i].points));
    
    li.appendChild(pName);
    li.appendChild(pPoints);
  }
}

function levelCleared(map) {
  if (map.levelId > localStorage.getItem('level')) {
    localStorage['level'] = map.levelId;
    loadLevels(map);
  }
  document.querySelector('#game').className = 'currentToLeft';
  document.querySelector('#game-over').className = 'current';
  document.querySelector('#level-cleared-h1').style.display = "";
  document.querySelector('#game-over-h1').style.display = "none";
  document.querySelector('#game-over-form').style.display = "none";
  document.querySelector('#btn-gameover-save-score').style.display = "none";
  document.querySelector('#btn-gameover-try-again').style.display = "none";
  if (levels[parseInt(map.levelId)+1]) {
    document.querySelector('#btn-gameover-next-level').style.display = "";
    document.querySelector('#btn-gameover-next-level').onclick = function () {
      startGame(map, 'levels', parseInt(map.levelId)+1);
      document.querySelector('#game').className = 'leftToCurrent';
      document.querySelector('#game-over').className = 'right';
    }
  }
  else {
    document.querySelector('#btn-gameover-next-level').style.display = "none";
    document.querySelector('#btn-gameover-next-level').onclick = null;
  }
}

function startGame(map, mode, levelId) {
  document.querySelector('#btn-a').hidden = false;
  document.querySelector('#btn-b').hidden = false;
  document.querySelector('#btn-paused-back-to-menu').hidden = true;
  document.querySelector('#btn-play').hidden = true;
  map.timeout = 350-50*(localStorage["speed"] || 1);
  
  document.getElementById("points").innerHTML = 0;
  
  var ctx = document.getElementById('gameCanvas').getContext('2d');
  ctx.clearRect(0,0,map.cellSize*map.width,map.cellSize*map.height);
  ctx.drawImage(document.querySelector('#loading'), (map.width*map.cellSize-128)/2, (map.height*map.cellSize-160)/2);
  
  map.loadResources(function() {
    map.loadLevel(levelId);
    map.addFruit();
  });
}

function loadLevels(map) {
  levelsList = document.querySelector('#levels-list');
  while (levelsList.firstChild) {
    levelsList.removeChild(levelsList.firstChild);
  }
  var maxLevel = parseInt(localStorage.getItem('level') || 0);
  for (var key in levels) {
    headerText = document.createTextNode(key);
    var p = document.createElement('p');
    p.appendChild(headerText);
    var li = document.createElement('li');
    levelsList.appendChild(li);
    var a = document.createElement('a');
    li.appendChild(a);
    var img = document.createElement('img');
    if (key <= maxLevel) {img.style.background="#0f0";}
    else if (key == maxLevel+1) {img.style.background="#ff0";}
    else {img.style.background="#f00";}
    var aside = document.createElement('aside');
    aside.appendChild(img);
    a.appendChild(aside);
    a.appendChild(p);
    a.href = '#';
    a.addEventListener ('click', (function (key) { return function (e) {
      document.querySelector('#select-level').className = 'currentToLeft';
      document.querySelector('#game').className = 'current';
      startGame(map, "levels", key);
    };})(key));
    
    if (key > maxLevel+1) {
      li.setAttribute('aria-disabled', "true");
    }
  }
}

document.body.onload = function() {
  var canvas = document.getElementById('gameCanvas');
  
  if (navigator.userAgent.contains('Mobile')) {
    console.log(screen.width,screen.height);
    var screenWidth = Math.min(screen.width, screen.height);
    var screenHeight = Math.max(screen.width, screen.height);
    var cellSize = Math.floor(Math.min(screenWidth/32, screenHeight/38))*devicePixelRatio;
    canvas.width = cellSize*32;
    canvas.height = cellSize*38;
    canvas.style.width = Math.floor(canvas.width/devicePixelRatio)+'px';
    canvas.style.height = Math.floor(canvas.height/devicePixelRatio)+'px';
    console.log(canvas.width,canvas.height);
    var size = Math.min(screenWidth/3, screenHeight-Math.floor(canvas.height/devicePixelRatio));
    document.querySelector('#btn-a').style.height = size+'px';
    document.querySelector('#btn-a').style.width = size+'px';
    document.querySelector('#btn-b').style.height = size+'px';
    document.querySelector('#btn-b').style.width = size+'px';
    document.querySelector('#btn-paused-back-to-menu').style.height = size+'px';
    document.querySelector('#btn-paused-back-to-menu').style.width = size+'px';
    document.querySelector('#btn-play').style.height = size+'px';
    document.querySelector('#btn-play').style.width = size+'px';
    document.querySelector('#commands').style.fontSize = (0.2*size)+'px';
    document.querySelector('#points').style.fontSize = (0.7*size)+'px';
  }
  else {
    canvas.width = 320;
    canvas.height = 380;
    
    canvas.parentNode.style.textAlign = "center";
    
    window.addEventListener("keydown",
			    function (e) {
			      switch (e.keyCode) {
				case 37: case 39: case 38:  case 40:
				case 32: e.preventDefault(); break;
				default: break;
			      }
			    },
			    false);
  }
  
  var canvas = document.getElementById('gameCanvas')
  var snake = new Snake(new Node(10, 4, "node"), new Node(3, 4, "node"));
  var map = new Map("survival");
  map.snake = snake;
  map.width = 32;
  map.height = 38;
  map.cellSize = canvas.width/map.width;
  
  document.querySelector('#btn-a').addEventListener ('touchstart', function () {
    snake.changeDirection("cw");
  });
  
  document.querySelector('#btn-b').addEventListener ('touchstart', function () {
    snake.changeDirection("ccw");
  });
  
  window.addEventListener("keydown", function (e) {
    switch (e.keyCode) {
      case 37: snake.changeDirection("left"); break;
      case 38: snake.changeDirection("up"); break;
      case 39: snake.changeDirection("right"); break;
      case 40: snake.changeDirection("down"); break;
      default: break;
    }
  }, false);
  
  var firstPoint = null;
  canvas.addEventListener("touchstart", function (e) {
        firstPoint = [e.touches[0].pageX, e.touches[0].pageY];
    }, false);
  canvas.addEventListener("touchend", function (e) {
        dx = e.changedTouches[0].pageX - firstPoint[0];
        dy = e.changedTouches[0].pageY - firstPoint[1];
        
        if (Math.abs(dx) > Math.abs(dy)) {
          if (dx > 0) {snake.changeDirection("right");}
          if (dx < 0) {snake.changeDirection("left");}
        }
        else {
          if (dy > 0) {snake.changeDirection("down");}
          if (dy < 0) {snake.changeDirection("up");}
        }
    }, false);
  
  /*canvas.addEventListener('mousedown', function () {
    if (map.paused) {
      document.querySelector('#btn-a').hidden = false;
      document.querySelector('#btn-b').hidden = false;
      document.querySelector('#btn-paused-back-to-menu').hidden = true;
      document.querySelector('#btn-play').hidden = true;
      map.paused = false;
    }
    else {
      document.querySelector('#btn-a').hidden = true;
      document.querySelector('#btn-b').hidden = true;
      document.querySelector('#btn-paused-back-to-menu').hidden = false;
      document.querySelector('#btn-play').hidden = false;
      map.paused = true;
    }
  });*/
  
  document.querySelector('#btn-play').addEventListener('click', function () {
    document.querySelector('#btn-a').hidden = false;
    document.querySelector('#btn-b').hidden = false;
    document.querySelector('#btn-paused-back-to-menu').hidden = true;
    document.querySelector('#btn-play').hidden = true;
    map.paused = false;
  });
  
  loadLevels(map);
  
  //game
  document.querySelector('#btn-survival').addEventListener ('click', function () {
    document.querySelector('#game').className = 'current';
    document.querySelector('[data-position="current"]').className = 'left';
    startGame(map, "survival", "survival");
  });
  document.querySelector('#btn-levels').addEventListener ('click', function () {
    document.querySelector('#select-level').className = 'current';
    document.querySelector('[data-position="current"]').className = 'left';
  });
    document.querySelector('#btn-select-level-back').addEventListener ('click', function () {
    document.querySelector('#select-level').className = 'right';
    document.querySelector('[data-position="current"]').className = 'current';
  });
  document.querySelector('#btn-paused-back-to-menu').addEventListener ('click', function () {
    document.querySelector('[data-position="current"]').className = 'current';
    document.querySelector('#game').className = 'right';
  });
  
  //game
  document.querySelector('#btn-back-to-menu').addEventListener ('click', function () {
    document.querySelector('[data-position="current"]').className = 'current';
    document.querySelector('#game-over').className = 'right';
  });
  
  //settings
  document.querySelector('#speed').children[(localStorage['speed'] || 1)-1].selected = true;
  document.querySelector('#btn-settings').addEventListener ('click', function () {
    document.querySelector('#settings').className = 'current';
    document.querySelector('[data-position="current"]').className = 'left';
  });
  document.querySelector('#btn-settings-back').addEventListener ('click', function () {
    document.querySelector('#settings').className = 'right';
    document.querySelector('[data-position="current"]').className = 'current';
  });
  document.querySelector('#speed').onchange = function(e) {
    localStorage["speed"] = document.querySelector('#speed').selectedOptions[0].value;
  }
  
  //high scores
  document.querySelector('#high-scores-name-reset').addEventListener ('mousedown', function () {
    document.querySelector('#high-scores-name').value = '';
  });
  document.querySelector('#btn-high-scores').addEventListener ('click', function () {
    displayHighScores();
    document.querySelector('#high-scores').className = 'current';
    document.querySelector('[data-position="current"]').className = 'left';
  });
  document.querySelector('#btn-high-scores-back').addEventListener ('click', function () {
    document.querySelector('#high-scores').className = 'right';
    document.querySelector('[data-position="current"]').className = 'current';
  });
  
  //help
  document.querySelector('#btn-help').addEventListener ('click', function () {
    document.querySelector('#help').className = 'current';
    document.querySelector('[data-position="current"]').className = 'left';
  });
  document.querySelector('#btn-help-back').addEventListener ('click', function () {
    document.querySelector('#help').className = 'right';
    document.querySelector('[data-position="current"]').className = 'current';
  });
  
  //about
  document.querySelector('#btn-about').addEventListener ('click', function () {
    document.querySelector('#about').className = 'current';
    document.querySelector('[data-position="current"]').className = 'left';
  });
  document.querySelector('#btn-about-back').addEventListener ('click', function () {
    document.querySelector('#about').className = 'right';
    document.querySelector('[data-position="current"]').className = 'current';
  });
}