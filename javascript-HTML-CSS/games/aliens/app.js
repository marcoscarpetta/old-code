/*  
 *	@licstart  The following is the entire license notice for the 
 *    JavaScript code in this page.
 *    
 *    Copyright (C) 2013  Marco Scarpetta
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

function gameOver(points) {
  jaws.game_loop.pause();
  
  if (localStorage.getItem("record") == null) {localStorage['record']=0}
  
  document.getElementById('scored').innerHTML = "You scored "+points+" points!";
  if (parseInt(localStorage.getItem("record")) < points) {
    document.getElementById('record').innerHTML = "New record!";
		localStorage['record']=points;
  }
  else {
    document.getElementById('record').innerHTML = "Record: "+localStorage.getItem("record");
  }

  document.querySelector('#game').className = 'right';
  document.querySelector('[data-position="current"]').className = 'current';
}
  
var last_points = 0;
function gameState() {
  
  var direction = null;
  var bullets = new jaws.SpriteList();
  var enemies = new jaws.SpriteList();
  var missiles = new jaws.SpriteList();
  var gifts = new jaws.SpriteList();
  var health_sprite_sheet = new jaws.SpriteSheet({image: "health.png",
    frame_size: [80,10] });
  var health_sprite = new jaws.Sprite({image: "health.png",x:10, y:5,
    anchor: "top_left"});
  var life_sprite = new jaws.Sprite({image: "lifes.png",x:0, y:5,
    anchor: "top_left"});
  var exp_anim = new jaws.Animation({sprite_sheet: 'exp.png',
    frame_size: [30,30], frame_duration: 50});
  exp_anim.bounce = true;
  var anim = new jaws.Animation({sprite_sheet: 'ship.png', frame_size: [80,30],
    frame_duration: 50});
  anim.bounce = true;
  var ship_exp = anim.slice(4,6);
  
  function new_ship() {
    var Ship = new jaws.Sprite({x:10, y:10, anchor: "top_left"});
    Ship.anim_default = anim.slice(0,4);
    Ship.setImage(Ship.anim_default.next());
    Ship.init = function(){
      this.health = 5;
      this.can_fire=1;
      this.weapon = 0;
      this.bullets = 1;
      this.side_bullets = 0;
      this.speed = 5;
      this.shield = 0;
      this.shield_sprite = new jaws.Sprite({image: "shield_sprite.png",
	x:10, y:10, anchor: "top_left"});
    }
    Ship.init();
    Ship.lifes = 3;
    Ship.points = 0;
    Ship.aliens = 0;
    return Ship;
  }
  
  function new_static(image, x, y, xspeed, yspeed) {
    var obj = new jaws.Sprite({image: image, x: x, y: y, anchor: "top_left"});
    obj.xspeed = xspeed;
    obj.yspeed = yspeed;
    obj.update = function() {this.x+=this.xspeed;this.y+=this.yspeed;};
    return obj
  }
  
  var life = function (ship) {ship.lifes += 1}
  var care = function (ship) {ship.health = 5}
  var due = function (ship){if (ship.bullets<3){ship.bullets+=1}}
  var tre = function (ship){ship.side_bullets = 1}
  var shield = function (ship) {
    ship.shield = 1;
    setTimeout(function(ship) {ship.shield=0}, 10000, ship);
  }
  var gifts_list = [
  ['care.png', care],
  ['life.png', life],
  ['due.png', due],
  ['tre.png', tre],
  ['shield.png', shield],
  ];
  
  var create_gift = function() {
    setTimeout(create_gift,10000+Math.round(Math.random()*20000));
    var py = 0;
    do {py=Math.round(Math.random()*jaws.height)}
    while (py<0||py>jaws.height-30);
    var i = Math.floor(Math.random()*gifts_list.length)
    var gift = new_static(gifts_list[i][0],jaws.width,py,-7,0);
    gift.taken = gifts_list[i][1]
    gift.exp_anim = exp_anim.slice(0,6)
    gifts.push(gift);
  }
  
  var total_enemies = 0
  function next_enemy(type){
    switch(type){
      case 0: return Math.round(Math.random()*100*total_enemies); break;
      case 1: return Math.round(Math.random()*65*total_enemies); break;
      case 2: return Math.round(500+Math.random()*(100000*(1/total_enemies)));
      break;
    }
  }
  
  var enemies_types = [
  ['',1],
  ['1',5],
  ['2',10]
  ]
  var create_enemy = function(type) {
    total_enemies += 1;
    setTimeout(create_enemy, next_enemy(type), type);
    if (!paused){
      var py = 0;
      do {py=Math.round(Math.random()*jaws.height)}
      while (py<0||py>jaws.height-30-(type==2)*20);
      var enemy = new_static("enemy"+enemies_types[type][0]+'.png',
			     jaws.width, py, -2, 0);
      enemy.health = enemies_types[type][1];
      enemy.exp_anim = exp_anim.slice(0, 6);
      enemies.push(enemy);
    }
  }
  
  
  function create_bullet(ship) {
    switch (ship.bullets) {
      case 1: var d_y=[0]; break;
      case 2: var d_y=[-5,5]; break;
      case 3: var d_y=[-10,0,10]; break;
    }
    for (var i=0;i<d_y.length;i++) {
      var bullet = new_static("bullet.png", ship.x+ship.width, 
			      ship.y+ship.height/2+d_y[i], 15, 0);
      bullets.push(bullet);
    }
    if (ship.side_bullets) {
      var ys=[-5,5];
      for (var i=0;i<ys.length;i++) {
	var bullet = new_static("bullet.png", ship.x+ship.width,
				ship.y+ship.height/2, 15, ys[i]);
	bullets.push(bullet);
      }
    }
  }
  
  var player;
  var paused = false;
  
  var create_missile = function() {
    setTimeout(create_missile,Math.round(Math.random()*3000));
    if (!paused) {
      var py = 0;
      do {py=Math.round(Math.random()*jaws.height)}
      while (py<0||py>jaws.height-30);
      var enemy = new_static("bomb.png", jaws.width, player.y, -15, 0);
      missiles.push(enemy);
    }
  }
  
  this.hit = function() {
    player.health -= 1;
    if (player.health<1) {
      if (player.lifes>0) {
	player.lifes -= 1;
	player.init();
      }
      else {
	gameOver(player.points);
      }
    }
  }
  
  this.setup = function() {
		if (navigator.userAgent.contains('Mobile')) {
			var onTouch = function(event) {
				direction = (event.touches[0].pageY<jaws.height/2) ? 1 : 0;
				event.preventDefault();
			}
			jaws.canvas.addEventListener("touchstart", onTouch, false);
			jaws.canvas.addEventListener("touchmove", onTouch, false);
			jaws.canvas.addEventListener("touchend",
																	 function(event) {
																		 direction = null;
																		 event.preventDefault();
																	 },
																false);
		}
		else {
			window.addEventListener("keydown",
			function(e){
			  switch(e.keyCode){
			    case 38: direction = 1; break; //up
					case 40: direction = 0; break; //down							
			    //case 32:  break; //spacebar
					//case 80:  break; //p
			    default: break;
			  }
			},
			false);
			window.addEventListener("keyup", function(e){direction = null;}, false);
		}
		
    player = new_ship();
    setTimeout(create_enemy, 100, 0);
    setTimeout(create_enemy, 50000, 1);
    setTimeout(create_enemy, 120000, 2);
    create_missile(player);
    setTimeout(create_gift,5000);
  }
  
  this.update = function() {
    if (!paused) {
      player.setImage(player.anim_default.next());
      if(direction === 0) {
	if (player.y+player.speed<jaws.height-player.height) {
	  player.y += player.speed;
	}
      }
      if(direction === 1) {
	if (player.y-player.speed>0) {
	  player.y -= player.speed;
	}
      }
      player.shield_sprite.y = player.y;
      if(player.can_fire) {
	player.can_fire=0;
	setTimeout(function() {player.can_fire=1}, 100);
	create_bullet(player);
      }
      var list = jaws.collideOneWithMany(player, missiles);
      for (var i=0; i<list.length; i++) {
	missiles.remove(list[i]);
	if (player.shield == 0) {
	  this.hit();
	  player.anim_default = ship_exp;
	  setTimeout(function() {player.anim_default=anim.slice(0, 4)}, 500);
	}
      }
      list = jaws.collideOneWithMany(player, gifts);
      for (var i=0;i<list.length;i++) {
	list[i].taken(player);
	gifts.remove(list[i]);
      }
      var del = function(item) {
	enemies.remove(item);
      }
      list = jaws.collideOneWithMany(player, enemies);
      for (var i=0;i<list.length;i++) {
	if (list[i].health > 0) {
	  list[i].health = 0;
	  list[i].xspeed = 0;
	  player.points += 1;
	  setTimeout(del, 600, list[i]);
	  if (player.shield == 0) {
	    this.hit();
	    player.anim_default = ship_exp;
	    setTimeout(function() {player.anim_default=anim.slice(0,4)}, 500);
	  }
	}
      }
      gifts.deleteIf(jaws.isOutsideCanvas);
      gifts.update();
      enemies.deleteIf(function(item) {
	if (jaws.isOutsideCanvas(item)) {
	  player.aliens+=1;
	  if (player.aliens>9) {
	    gameOver(player.points);
	  }
	  return true;
	}
	else {return false}
      });
      missiles.update();
      bullets.deleteIf(jaws.isOutsideCanvas);
      bullets.update();
      enemies.forEach(function (element, index, array) {
	if (element.health<1) {
	  element.setImage(element.exp_anim.next());
	  element.health-=1;
	}
      });
      missiles.deleteIf(jaws.isOutsideCanvas);
      enemies.update();
      var col=jaws.collideManyWithMany(bullets, enemies);
      for (var i=0; i<col.length; i++) {
	if (col[i][1].health>0) {
	  bullets.remove(col[i][0]);
	}
	col[i][1].health -= 1;
	if (col[i][1].health==0) {
	  player.points+=1;
	  setTimeout(del, 600, col[i][1]);
	}
	if (col[i][1].health<1) {
	  col[i][1].xspeed = 0;
	}
      }
      /* This is to destroy gifts when hit by bullets
       * var col=jaws.collideManyWithMany(bullets,gifts);
       * for (var i=0; i<col.length; i++)
       * {
       *  bullets.remove(col[i][0]);
       *  gifts.remove(col[i][1]);
    }
    */
    }
  }
  
  this.draw = function() {
    var grd = jaws.context.createLinearGradient(0, 0, jaws.width, 0);
    grd.addColorStop(0,"#caeaff");
    grd.addColorStop(1,"#67dbf1");
    jaws.context.fillStyle=grd;
    jaws.context.fillRect(0, 0, jaws.width, jaws.height);
    if (player.shield==1) {player.shield_sprite.draw()}
    player.draw();
    missiles.draw();
    bullets.draw()
    enemies.draw()
    gifts.draw();
    jaws.context.font = "10pt terminal";
    jaws.context.lineWidth = 10;
    jaws.context.fillStyle =  "Black";
    var w = jaws.context.measureText("Points: "+player.points);
    health_sprite.x = w.width+10;
    health_sprite.setImage(health_sprite_sheet.frames[player.health-1]);
    jaws.context.fillText("Points: "+player.points, 0, 15);
    health_sprite.draw();
    for (var i=0; i<player.lifes; i++) {
      life_sprite.x = w.width+100+15*i;
      life_sprite.draw();
    }
    jaws.context.fillText("Aliens to invasion: "+(10-player.aliens),
			  100+15*player.lifes+w.width, 15);
    if (paused) {
      jaws.context.font = "bold 20pt terminal";
      jaws.context.lineWidth = 10;
      jaws.context.fillStyle = "Black";
      jaws.context.strokeStyle =  "rgba(200,200,200,0.0)";
      jaws.context.fillText("Paused", jaws.width/2, jaws.height/2);
    }
  }
}

jaws.onload = function() {
	var canvas = document.getElementById('gameCanvas');
	
	if (navigator.userAgent.contains('Mobile')) {
		canvas.width = Math.max(screen.width, screen.height);
		canvas.height = Math.min(screen.width, screen.height);
	}
	else {
		canvas.width = 800;
		canvas.height = 600;
		
		canvas.parentNode.style.textAlign = "center";
		
		window.addEventListener("keydown",
			function(e){
			  switch(e.keyCode){
			    case 37: case 39: case 38:  case 40:
			    case 32: e.preventDefault(); break;
			    default: break;
			  }
			},
			false);
	}
  
  
  
	//game
	document.querySelector('#btn-new-game').addEventListener ('click', function () {
		document.querySelector('#game').className = 'current';
		document.querySelector('[data-position="current"]').className = 'left';
		jaws.switchGameState(gameState);
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
  
  document.getElementById('record').innerHTML = "Record: "+localStorage.getItem("record");
  
  jaws.assets.setRoot("data/");
  jaws.assets.add(
    ["lifes.png",
    "health.png",
    "shield_sprite.png",
    "shield.png",
    "care.png",
    "life.png",
    "due.png",
    "tre.png",
    "bomb.png",
    "ship.png",
    "bullet.png",
    'enemy.png',
    'enemy1.png',
    'enemy2.png',
    'exp.png']);
  jaws.start(gameState);
  jaws.game_loop.pause();
}
