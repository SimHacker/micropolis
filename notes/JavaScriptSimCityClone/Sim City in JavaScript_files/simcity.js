/*
 * Bugs to fix:
 *  1. A* is supposed to let you remove an item from OPEN, update its cost, and reinsert.  I dont think I handle that right now.
 *
 * In progress:
 *  1. 
 *
 * TODO:
 *  1. Abandon buildings
 *     A. If house unemployed
 *     B. If workplace unoccupied
 *  2. Model job turnover
 *     A. Randomly have a person leave a workplace
 *         -seek new job next update
 *     B. Probably required to get a fully-built city to change at all
 *  3. Figure out roads
 *     A. Extra travel distance allowed
 *     B. Traffic capacity
 */

/*
 * Possible Optimizations:
 *  1. Store list of unoccupied but zoned squares (save search for possible building sites)
 *  2. Store list of occupied squares (save search for buildings to abandon)
 */

var DO_REAL_ROADS = false;

const months = ["January", "February", "March", "April",
		"May", "June", "July", "August", "September",
		"October", "November", "December"];

const POWERPLANT_COST = 4000;
const RESIDENTIAL_COST = 10;
const COMMERCIAL_COST = 10;
const INDUSTRIAL_COST = 10;
const ROAD_COST = 10;
const POWERLINE_COST = 2;
const ROAD_TAX = 0.1;
const RESIDENTIAL_TAX = .2;
const COMMERCIAL_TAX = .2;
const INDUSTRIAL_TAX = .2;

const MAX_TRAVEL_COST = 3;
const TRAVEL_ROAD_COST = .25;
const TRAVEL_WALK_COST = 1;

const WORLD_SIZE = 32;

const CANVASWIDTH=800;
const CANVASHEIGHT=600;
const ANGLE=45 * Math.PI / 180; // if you change this, fix isVisible()
const XSCALE = 1;
const YSCALE = 0.51;
var scale = 1;
var panx = 0;
var pany = 0;

var world;
var funds;
var canvas;
var ctx;
var updateInterval;

var cursorMode;

var dragStartPoint;

// returns world position, given screen position
function invertTransforms(x, y) {
  // Do transformations, inverted

  // pan (translate)
  x -= panx;
  y -= pany;
  // translate
  y -= CANVASHEIGHT/2;
  x -= CANVASWIDTH/2;
  // scale
  y /= YSCALE*scale;
  x /= XSCALE*scale;
  // rotate
  var rotated = rotate(x, y, ANGLE);
  x = rotated[0]; y = rotated[1];
  // translate
  x -= -(world.squareSize * world.size)/2;
  y -= -(world.squareSize * world.size)/2;

  // panning?  square size?  something...
  x /= world.squareSize;
  x = parseInt(x);
  y /= world.squareSize;
  y = parseInt(y);

  return [x, y];
}

// return screen position, given world position (x, y is a corner of the square)
function transform(x, y) {
  x *= world.squareSize;
  y *= world.squareSize;

  x += -(world.squareSize * world.size)/2;
  y += -(world.squareSize * world.size)/2;

  var rotated = rotate(x, y, -ANGLE);
  x = rotated[0]; y = rotated[1];

  x *= XSCALE*scale;
  y *= YSCALE*scale;

  x += CANVASWIDTH/2;
  y += CANVASHEIGHT/2;

  x += panx;
  y += pany;
  
  return [x, y];
}

function setUpTransforms() {
  // dirty everything so the next paint does a full redraw
  var i, j;
  for (i=0; i<world.size; i++) {
    for (j=0; j<world.size; j++) {
      world.squares[j][i].dirty = true;
    }
  }

  // get back to default transforms
  ctx.restore();
  ctx.save();

  // clear the screen
  ctx.clearRect(0, 0, CANVASWIDTH, CANVASHEIGHT);

  ctx.translate(panx, pany);
  ctx.translate(CANVASWIDTH/2, CANVASHEIGHT/2);
  ctx.scale(XSCALE*scale, YSCALE*scale);
  ctx.rotate(ANGLE);
  ctx.translate(-(world.squareSize * world.size)/2, -(world.squareSize * world.size)/2);

  // caller **MUST** call draw()
}

function init() {
  canvas = document.getElementById("theCanvas");
  canvas.onmousedown = handleMouseDown;
  canvas.onmousemove = handleMouseMove;
  canvas.onmouseup = handleMouseUp;
  ctx = canvas.getContext("2d");

  cursorMode = null;
  dragStartPoint = null;
  buildWorld();
  funds = new Funds();
  funds.money = 10000;
  ctx.save(); // must do this once before calling setUpTransforms() the first time
  setUpTransforms();
  draw();
  updateInterval = setInterval(update, 1000);

  // add key listeners
  window.onkeypress = keyHandler;
}

function keyHandler(event) {
  if (event.keyCode == 39) // right
    panx -= 200*XSCALE;
  else if (event.keyCode == 37) // left
    panx += 200*XSCALE;
  else if (event.keyCode == 38) // up
    pany += 200*YSCALE;
  else if (event.keyCode == 40) // down
    pany -= 200*YSCALE;
  else if (event.keyCode == 36) // home
    zoom(1.5);
  else if (event.keyCode == 35) // end
    zoom(1/1.5);
  else {
    //alert(event.charCode + " " + event.keyCode);
    return;
  }
  setUpTransforms();
  draw();
  event.stopPropagation();
}

function update() {
  //window.status = "update!";
  world.iteration++;
  checkAllSquares();
  earnMoney((world.iteration % 12 == 0));
  draw();
}

function earnMoney(yearEnd) {
  var i; var j;
  for (i=0; i<world.size; i++) {
    for (j=0; j<world.size; j++) {
      var sq = world.squares[j][i];
      // handle cost to maintain roads
      // TODO: police, fire, school, etc
      // handle income from taxes
      
      if (sq.road) {
	funds.roadCost += ROAD_TAX;
      }
      // income from taxes
      if (sq.building) {
	if (sq.zone == "r")
	  funds.tax += RESIDENTIAL_TAX;
	else if (sq.zone == "c")
	  funds.tax += COMMERCIAL_TAX;
	else if (sq.zone == "i")
	  funds.tax += INDUSTRIAL_TAX;
      }
    }
  }
  if (yearEnd) {
    funds.money -= parseInt(funds.roadCost);
    funds.money += parseInt(funds.tax);
    funds.roadCost = 0;
    funds.tax = 0;
  }
}

function zoom(factor) {
  scale *= factor;
  panx *= factor;
  pany *= factor;
  setUpTransforms();
  draw();
}

function checkAllSquares() {
  var i;
  var j;
  var k;
  var l;
  for (i=0; i<world.size; i++) {
    for (j=0; j<world.size; j++) {
      var sq = world.squares[j][i];
      var b = sq.building;
      if (!b && !sq.zone) {
	continue; // nothing here to abandon, can't build here either
      }
      else if (b && !b.abandoned && sq.zone) { // zone check b/c can't abandon power plants
	// handle reasons to abandon a building
	// are we powered?
	// is there work within range?  (may want to store "work" / "home" pointers to optimize check
	//    once we found a place, and only redo search if that place goes away
	if (!sq.checkPower()) {
	  // abandon the building
	  const ABANDON_NO_POWER_PROBABILITY = 0.5;
	  if (Math.random() < ABANDON_NO_POWER_PROBABILITY) {
	    b.abandoned = true;
	  }
	}
	// TODO: handle abandon due to low employment vs no employment
	if (b.workplace.length + b.house.length == 0) {// no employment
	  const ABANDON_NO_WORK_PROBABILITY = 0.5;
	  if (Math.random() < ABANDON_NO_WORK_PROBABILITY)
	    b.abandoned = true;
	}

	if (b.abandoned) {
	  b.abandon();
	}
      }
      else if (sq.zone && (!b || (b && b.abandoned))) {
	// handle reasons to add a new building
	// are we powered?
	var isSatisfied = 1.0;

	if (!sq.checkPower())
	  isSatisfied *= 0.0;
	var wp = sq.checkEmployment("build new", sq.zone);
	if (!wp)
	  isSatisfied *= 0.805;

	// TODO: check for job / house / roads
	if (isSatisfied * Math.random() > 0.8) {
	  if (!b) {
	    b = new Building();
	    b.multiSquareRoot = b;
	    b.multiSquareArray.push(b);
	  }
	  b.abandoned = false;
	  b.square = sq;
	  b.occupancy = 5; // 5 residents or 5 employees TODO: make not a constant, make multiple people
	  sq.building = b;
	  sq.powered = true; // guaranteed; we don't build w/o power
	  sq.addPower();
	  sq.powerline = false;
	  sq.dirty = true;
	  
	  switch (sq.zone) {
	  case "r":
	    b.type = 0;
	    //	    if (wp)
	    //	      wp.house[wp.house.length] = b;
	    break;
	  case "c":
	    b.type = 1;
	    //	    if (wp)
	    //	      wp.workplace[wp.workplace.length] = b;
	    break;
	  case "i":
	    b.type = 2;
	    //	    if (wp)
	    //	      wp.workplace[wp.workplace.length] = b;
	    break;
	  default:
	    alert("unknown zone!");
	    break;
	  }
	  if (wp) {
	    sq.findJobs(wp);
	  }
	}
      }
    }
  }
}

function rotate(x, y, angle) {
/*
  [x']   [cos  sin  0] [x]
  [y'] = [-sin cos  0] [y]
  [w']   [0    0    1] [w]
*/
  var x2 = Math.cos(angle)*x + Math.sin(angle)*y;
  var y2 = -Math.sin(angle)*x + Math.cos(angle)*y;
  return [x2, y2];
}

function pointForMouseEvent(e) {
  var bo = document.getBoxObjectFor(canvas);
  var x = e.clientX - bo.x + document.body.scrollLeft;
  var y = e.clientY - bo.y + document.body.scrollTop;

  var transformed = invertTransforms(x, y);
  x = transformed[0]; y = transformed[1];

  return [x, y];
}

function min(a, b) { return (a < b) ? a : b; }
function max(a, b) { return (a > b) ? a : b; }

function handleMouseDown(e) {
  dragStartPoint = pointForMouseEvent(e);
}

function handleMouseMove(e) {
  var i;
  var j;
  var blockSelectionMode = dragStartPoint && (cursorMode == "residential" ||
					      cursorMode == "commercial" || 
					      cursorMode == "industrial");

  var oldSelection = [];
  var newSelection = [];
  for (i=0; i<world.size; i++)
    for (j=0; j<world.size; j++) {
      if (world.squares[j][i].selected) {
	oldSelection.push([i, j]);
	if (!blockSelectionMode) {
	  world.squares[j][i].selected = false;
	  world.squares[j][i].dirty = true;
	}
	else { // optimization for large selections
	  world.squares[j][i].selected = 1;
	}
      }
    }
  // draw an indicator showing what squares will be affected
  if (dragStartPoint) {
    var dragStopPoint = pointForMouseEvent(e);
    if (cursorMode == "residential" ||
	cursorMode == "commercial" || 
	cursorMode == "industrial") {
      var istart = min(dragStartPoint[0], dragStopPoint[0]);
      var jstart = min(dragStartPoint[1], dragStopPoint[1]);
      var istop = max(dragStartPoint[0], dragStopPoint[0]);
      var jstop = max(dragStartPoint[1], dragStopPoint[1]);
      for (i = istart; i<=istop; i++) {
	for (j = jstart; j<=jstop; j++) {
	  if (i<0 || j<0 || i>=world.size || j>=world.size)
	    continue; // ugh...need a cleaner way
	  world.squares[j][i].selected++;
	  newSelection.push([i, j]);
	}
      }
      draw();
    }
    else if (cursorMode == "powerline" ||
	     cursorMode == "road") {
      line(dragStartPoint[0], dragStartPoint[1],
	   dragStopPoint[0], dragStopPoint[1],
	   "cursor");
    }
  }
  else {
    var point = pointForMouseEvent(e);
    if (cursorMode == "powerplant") {
      for (i = point[0]; i < world.size && i < point[0]+4; i++)
	for (j = point[1]; j < world.size && j < point[1]+4; j++)
	  plot(i, j, "cursor");
      draw();
    }
    else {
      plot(point[0], point[1], "cursor");
      draw();
    }
  }
  if (blockSelectionMode) {
    var tmpSquare;
    var tmpCoords;
    for (i=0; i<oldSelection.length; i++) {
      tmpCoords = oldSelection[i];
      var x = tmpCoords[0];
      var y = tmpCoords[1];
      tmpSquare = world.squares[y][x];
      if (tmpSquare.selected == 1) { // it was selected, but isn't now or it'd be 2)
	tmpSquare.dirty = true;
	tmpSquare.selected = false;
      }
    }
    for (i=0; i<newSelection.length; i++) {
      tmpCoords = newSelection[i];
      var x = tmpCoords[0];
      var y = tmpCoords[1];
      tmpSquare = world.squares[y][x];
      if (tmpSquare.selected == 1) { // it was NOT selected, but is now or it'd be 2)
	tmpSquare.dirty = true;
	tmpSquare.selected = true;
      }
    }
  }
}

function handleMouseUp(e) {
  var i; var j;
  for (i=0; i<world.size; i++)
    for (j=0; j<world.size; j++) {
      if (world.squares[j][i].selected) {
	world.squares[j][i].selected = false;
	world.squares[j][i].dirty = true;
      }
    }
  if (!dragStartPoint) {
    return; // must have started dragging offscreen
  }
  var dragStopPoint = pointForMouseEvent(e);

  if (cursorMode == "residential" ||
      cursorMode == "commercial" || 
      cursorMode == "industrial") {
    var z;
    var cost = 0;
    if (cursorMode == "residential") {
      z = "r";
      cost = RESIDENTIAL_COST;
    }
    else if (cursorMode == "commercial") {
      z = "c";
      cost = COMMERCIAL_COST;
    }
    else if (cursorMode == "industrial") {
      z = "i";
      cost = INDUSTRIAL_COST;
    }
    else
      alert("impossible cursor mode!");

    // handle a box
    var i;
    var j;
    var istart = min(dragStartPoint[0], dragStopPoint[0]);
    var jstart = min(dragStartPoint[1], dragStopPoint[1]);
    var istop = max(dragStartPoint[0], dragStopPoint[0]);
    var jstop = max(dragStartPoint[1], dragStopPoint[1]);
    for (j = jstart; j<=jstop; j++) {
      if (j < 0 || j >= world.size)
	continue;
      for (i = istart; i<=istop; i++) {
	if (i < 0 || i >= world.size)
	  continue;
	if (!world.squares[j][i].road && !world.squares[j][i].building) {
	  if (funds.money < cost)
	    break;
	  funds.money -= cost;
	  world.squares[j][i].zone = z;
	  world.squares[j][i].dirty = true;
	}
      }
    }
    draw();
  }
  else if (cursorMode == "powerline" ||
	   cursorMode == "road") {
    // handle a line
    line(dragStartPoint[0], dragStartPoint[1],
	 dragStopPoint[0], dragStopPoint[1],
	 cursorMode);
  }
  else if (cursorMode == "powerplant" ||
	   cursorMode == "bulldozer" || cursorMode == "center") {
    // point
    var x = dragStopPoint[0];
    var y = dragStopPoint[1];

    if (cursorMode == "powerplant") {
      var dontBuild = false;
      if (x+4 <= world.size && y+4 <= world.size) {
	for (i = x; i < x+4; i++)
	  for (j = y; j < y+4; j++) {
	    if (world.squares[j][i].building || world.squares[j][i].road)
	      dontBuild = true;
	  }
	if (funds.money < POWERPLANT_COST)
	  dontBuild = true;
	if (!dontBuild) {
	  funds.money -= POWERPLANT_COST;
	  var ppRoot;
	  for (i = x; i < x+4; i++)
	    for (j = y; j < y+4; j++) {
	      var pp = new Building();
	      if (i == x && y == j)
		ppRoot = pp;
	      pp.abandoned = false;
	      pp.square = world.squares[j][i];
	      pp.occupancy = 0; //nobody works at the power plant
	      pp.type = 3;
	      world.squares[j][i].zone = null;
	      world.squares[j][i].building = pp;
	      world.squares[j][i].generator = true;
	      world.squares[j][i].addPower();
	      world.squares[j][i].dirty = true;
	      pp.multiSquareRoot = ppRoot;
	      ppRoot.multiSquareArray.push(pp);
	    }
	}
      }
    }
    else if (cursorMode == "bulldozer") {
      world.squares[y][x].bulldoze();
    }
    else if (cursorMode == "center") {
      var bo = document.getBoxObjectFor(canvas);
      var screenx = e.clientX - bo.x + document.body.scrollLeft;
      var screeny = e.clientY - bo.y + document.body.scrollTop;
      panx = panx - (screenx - (CANVASWIDTH/2));
      pany = pany - (screeny - (CANVASHEIGHT/2));
      setUpTransforms();
      // draw() *must* follow this
    }
    draw();
  }
  else if (cursorMode == null) {
    // do nothing, no cursor
  }
  else {
    // wtf?
    alert("Unknown cursor mode!");
  }

  dragStartPoint = null;
}

function setMode(mode) {
  cursorMode = mode;
}

function buildWorld() {
  world = new World();
  world.iteration = 0;
  world.size = WORLD_SIZE;
  world.canvasWidth = CANVASWIDTH;
  world.canvasHeight = CANVASHEIGHT;
  world.squares = [];
  var j;
  for (j=0; j<world.size; j++) {
    world.squares[j] = [];
    var i;
    for (i=0; i<world.size; i++) {
      var sq = new Square();
      sq.x = i;
      sq.y = j;
      sq.dirty = true;
      sq.generator = false;
      sq.powered = false;
      sq.powerline = false;
      sq.road = false;
      sq.zone = null;
      sq.building = null;
      sq.selected = false;
      world.squares[j][i] = sq;
    }
  }
}

function plot(x, y, task) {
  if (!(x >= 0 && x < world.size && y >= 0 && y < world.size)) 
    return; // mouse is off the world

  if (task != "cursor") {
    if (task == "road") {
      if (!world.squares[y][x].building) {
	if (funds.money > ROAD_COST) {
	  funds.money -= ROAD_COST;
	  world.squares[y][x].dirty = true;
	  world.squares[y][x].zone = null;
	  world.squares[y][x].road = true;
	}
      }
    }
    else if (task == "powerline") {
      if (!world.squares[y][x].building) {
	if (funds.money > POWERLINE_COST) {
	  funds.money -= POWERLINE_COST;
	  world.squares[y][x].powerline = true;
	  world.squares[y][x].dirty = true;
	  if (world.squares[y][x].checkPower())
	    world.squares[y][x].addPower(); // power up & notify neighbors
	}
      }
    }
  }
  else {
    world.squares[y][x].selected = true;
    world.squares[y][x].dirty = true;
  }
}

function line(x1, y1, x2, y2, task) {
  var steep;
  var temp;
  steep = Math.abs(y2-y1) > Math.abs(x2-x1);
  if (steep) {
    temp = x1;
    x1 = y1;
    y1 = temp;
    temp = x2;
    x2 = y2;
    y2 = temp;
  }
  if (x1 > x2) {
    temp = x1;
    x1 = x2;
    x2 = temp;
    temp = y1;
    y1 = y2;
    y2 = temp;
  }
  var dx = x2 - x1;
  var dy = Math.abs(y2 - y1);
  var error = 0;
  var derror = dy / dx;
  var y = y1;
  var ystep;
  if (y1 < y2)
    ystep = 1;
  else
    ystep = -1;

  var x;
  for (x=x1; x<=x2; x++) {
    if (steep)
      plot(y, x, task);
    else
      plot(x, y, task);
    error += derror;
    if (error > .5) {
      y += ystep;
      if (steep)
        plot(y, x, task);
      else
        plot(x, y, task);
      error--;
    }
  }
  draw();
}

/// HACK: the min/max x/y need to be searched in case the rotation is different
function isVisible(square) {
  var x = square.x;
  var y = square.y;

  var p1 = transform(x,   y  );
  var p2 = transform(x+1, y  );
  var p3 = transform(x,   y+1);
  var p4 = transform(x+1, y+1);

  //window.status = p1 + "  " + p2 + "  " + p3 + "  " + p4;
  var minx = p3[0];
  var maxx = p2[0];
  var miny = p1[1];
  var maxy = p4[1];

  var rv = (minx <= CANVASWIDTH && maxx >= 0
	    && miny <= CANVASHEIGHT && maxy >= 0);

  //window.status = minx+", "+miny+"    "+rv;

  return rv;
}

function draw() {
  var i;
  var j;
  var draws = 0;
  // update funds, month
  document.getElementById("month").innerHTML = months[world.iteration % 12] + " " + (1900 + parseInt((world.iteration/12)));
  document.getElementById("funds").innerHTML = funds.money;
  // draw buildings
  for (i=0; i < world.size; i++) {
    for (j=0; j < world.size; j++) {
      var sq = world.squares[j][i];
      // paint is RIDICULOUSLY expensive!  avoid it if possible
      // by only painting dirty squares
      if (!sq.dirty || !isVisible(sq))
	continue;
      draws++;
      sq.dirty = false;
      // black square outline
      ctx.strokeStyle = "rgba(0,0,0,.5)";

      // solid square color for zone
      switch(sq.zone) {
      case "r": {
	ctx.fillStyle = "rgb(0,200,0)";
	break;
      }
      case "c": {
	ctx.fillStyle = "rgb(0,0,200)";
	break;
      }
      case "i": {
	ctx.fillStyle = "rgb(200,200,0)";
	break;
      }
      default: {
	ctx.fillStyle = "rgb(100,100,0)";
	break;
      }
      }
      // road is solid square for now
      if (sq.road) {
	ctx.fillStyle = "rgb(50,50,50)";
      }
      ctx.fillRect(i*world.squareSize+.5, j*world.squareSize+.5, world.squareSize, world.squareSize);
      ctx.strokeRect(i*world.squareSize+.5, j*world.squareSize+.5, world.squareSize, world.squareSize);

      if (sq.road && sq.powerline) {
	ctx.save();
	ctx.strokeStyle = "rgb(100,100,0)";
	ctx.beginPath();
	ctx.moveTo((i+.5)*world.squareSize, (j)*world.squareSize);
	ctx.lineTo((i+.5)*world.squareSize, (j+1)*world.squareSize);
	ctx.moveTo((i)*world.squareSize, (j+.5)*world.squareSize);
	ctx.lineTo((i+1)*world.squareSize, (j+.5)*world.squareSize);
	ctx.stroke();
	ctx.restore();
      }


      // images for buildings & powerlines
      var img = null;
      if (sq.building) {
	switch(sq.building.type) {
	case 0:
	  img = document.getElementById("resIcon");
	  break;
	case 1:
	  img = document.getElementById("comIcon");
	  break;
	case 2:
	  img = document.getElementById("indIcon");
	  break;
	case 3:
	  img = document.getElementById("ppIcon");
	  break;
	default:
	  alert("unknown building!");
	  break;
	}
      }
      else if (sq.powerline && !sq.road) {
	img = document.getElementById("plIcon");
      }
      if (img)
	ctx.drawImage(img, i*world.squareSize+.5, j*world.squareSize+.5, 15, 15);
      if (sq.building && sq.building.abandoned) {
	ctx.fillStyle = "rgba(0,0,0,.5)";
	ctx.fillRect(i*world.squareSize+.5, j*world.squareSize+.5, world.squareSize, world.squareSize);
      }
      if (sq.selected) {
	ctx.fillStyle = "rgba(255,255,255,.5)";
	ctx.fillRect(i*world.squareSize+.5, j*world.squareSize+.5, world.squareSize, world.squareSize);
      }
      if (!sq.powered && (sq.powerline || sq.building)) {
	ctx.fillStyle = "rgba(255,255,0,.5)";
	ctx.fillRect(i*world.squareSize, j*world.squareSize, world.squareSize, world.squareSize);
      }
    }
  }
  window.status = "Painted " + draws + " squares";
}


///////////////////////////////
///// OBJECT DECLARATIONS /////
///////////////////////////////

function World() {
  this.iteration = null; // month counter
  this.size = null; // integer
  this.squares = null;

  this.canvasWidth = null;
  this.canvasHeight = null;

  this.squareSize = 15; // squares are this many pixels on a side
}

function Square() {
  this.x = null;
  this.y = null;
  this.dirty = null; // true, false, indicates whether paint is necessary
  this.generator = null; // true, false
  this.powered = null; // true, false
  this.powerline = null; // true, false
  this.road = null; // true, false
  this.zone = null; // null, "r", "c", "i"
  this.building = null; // a Building
  this.selected = null; // draw cursor indicator?
  this.searching = null; // boolean used for various recursive searches
}

Square.prototype.toString = function Square_toString() {
  return "["+this.x + ", " + this.y+"]";
}

Square.prototype.addPower = function Square_addPower() {
  var i;
  var j;
  for (j=0; j<world.size; j++) {
    for (i=0; i<world.size; i++) {
      world.squares[j][i].searching = false;
    }
  }
  this.addPowerHelper(true);
};

Square.prototype.addPowerHelper = function Square_addPowerHelper(first) {
  var i;
  var j;
  if (this.powered && !first) // are we already powered?
      return;
  if (!this.building && !this.powerline) // do we conduct electricity?
    return;

  this.powered = true;
  this.dirty = true;

  var x = this.x;
  var y = this.y;
  var minx = max(0, x-1);
  var maxx = min(x+1, world.size-1);
  var miny = max(0, y-1);
  var maxy = min(y+1, world.size-1);
  world.squares[miny][x].addPowerHelper(false);
  world.squares[maxy][x].addPowerHelper(false);
  world.squares[y][minx].addPowerHelper(false);
  world.squares[y][maxx].addPowerHelper(false);
};

Square.prototype.checkPower = function Square_checkPower() {
  if (this.powered)
    return true;
  var x = this.x;
  var y = this.y;
  var minx = max(0, x-1);
  var maxx = min(x+1, world.size-1);
  var miny = max(0, y-1);
  var maxy = min(y+1, world.size-1);

  // don't actually set this.powered = true because empty squares could check for power
  // but opt not to build, and we dont want empty squares to report power to neighbors
  if (world.squares[miny][x].powered)
    return true;
  if (world.squares[maxy][x].powered)
    return true;
  if (world.squares[y][minx].powered)
    return true;
  if (world.squares[y][maxx].powered)
    return true;

  return false;
};

Square.prototype.powerDisconnect = function Square_powerDisconnect() {
  var x = this.x;
  var y = this.y;
  if (x > 0)
    world.squares[y][x-1].powerDisconnectHelper();
  if (x < world.size-1)
    world.squares[y][x+1].powerDisconnectHelper();
  if (y > 0)
    world.squares[y-1][x].powerDisconnectHelper();
  if (y < world.size-1)
    world.squares[y+1][x].powerDisconnectHelper();
};

Square.prototype.powerDisconnectHelper = function Square_powerDisconnectHelper() {
  var i; var j;
  for (i=0; i<world.size; i++) {
    for (j=0; j<world.size; j++) {
      world.squares[j][i].searching = false;
    }
  }

  // we could have been supplying neighbors
  var visitedArray = [this];
  this.searching = true;
  
  for (i=0; i<visitedArray.length; i++) {
    var sq = visitedArray[i];

    if (sq.generator) {
      return; // don't disconnect anybody - this whole network is powered
    }
    
    // check if we don't conduct
    if (!sq.building && !sq.powerline) {
      continue; // can't pass power between our neighbors
    }
    
    // add neighbors of cell to array
    var x = sq.x;
    var y = sq.y;
    if (x > 0)
      if (!world.squares[y][x-1].searching) {
	visitedArray.push(world.squares[y][x-1]);
	world.squares[y][x-1].searching = true;
      }
    if (x < world.size-1)
      if (!world.squares[y][x+1].searching) {
	visitedArray.push(world.squares[y][x+1]);
	world.squares[y][x+1].searching = true;
      }
    if (y > 0)
      if (!world.squares[y-1][x].searching) {
	visitedArray.push(world.squares[y-1][x]);
	world.squares[y-1][x].searching = true;
      }
    if (y < world.size-1)
      if (!world.squares[y+1][x].searching) {
	visitedArray.push(world.squares[y+1][x]);
	world.squares[y+1][x].searching = true;
      }
  }

  // we must have hit every cell in the list and not found power.  kill power to this whole network
  for (i=0; i<visitedArray.length; i++) {
    visitedArray[i].powered = false;
    visitedArray[i].dirty = true;
  }
};

// true if there's a job within some distance
Square.prototype.checkEmployment = function Square_checkEmployment(str, zone) {
  if (DO_REAL_ROADS) {
  var closed = {};
  var q = new Tree();
  q.insert(new TreeNode([this], 0));
  while (q.hasNodes()) {
    var p = q.removeFirst();
    //alert(p.cost);
    if (p.cost > MAX_TRAVEL_COST)
      return false; // cheapest is too expensive.  no solution :(
    var x = p.item[p.item.length-1]; // last node on the path
    if (closed[x]) { // redundant given check below on insert?
      continue;
    }
    var sq = x;
    if (sq.building &&
	(sq.zone == "r" && (this.zone == "c" || this.zone == "i") ||
	 (this.zone == "r" && (sq.zone == "c" || sq.zone == "i")))) {
      if (sq.building.workplace.length + sq.building.house.length < sq.building.occupancy) { // not full
	return sq.building;
      }
    }
    closed[x] = true;
    var minx = max(x.x - 1, 0);
    var maxx = min(x.x + 1, world.size-1);
    var miny = max(x.y - 1, 0);
    var maxy = min(x.y + 1, world.size-1);
    for (var i = minx; i <= maxx; i++) {
      for (var j = miny; j <= maxy; j++) {
	sq = world.squares[j][i];
	if (!closed[sq]) {
	  var newPath = Array.apply(null, p.item); // copy the array.  <sayrer> new Array().concat(oldArray) works too
	  newPath.push(sq); // add the new square to the path
	  var newcost = p.cost + (sq.road ? TRAVEL_ROAD_COST : TRAVEL_WALK_COST);
	  q.insert(new TreeNode(newPath, newcost));
	}
      }
    }
  }
  return false;
  }


  var x = this.x;
  var y = this.y;
  var minx = max(0, x-6);
  var maxx = min(x+6, world.size-1);
  var miny = max(0, y-6);
  var maxy = min(y+6, world.size-1);
  var i;
  var j;

  for (i=minx; i<maxx; i++) {
    for (j=miny; j<maxy; j++) {
      var sq = world.squares[j][i];
      if (sq.building) {
	if ((sq.zone == "r" && (this.zone == "c" || this.zone == "i")) ||
	    (this.zone == "r" && (sq.zone == "c" || sq.zone == "i"))) {
	  if (sq.building.workplace.length + sq.building.house.length < sq.building.occupancy) { // not full
	    //if (sq.zone == "r")
	    //alert("found a place with " + sq.building.workplace.length + "+" + sq.building.house.length + " people busy of " + sq.building.occupancy + "possible [" + str + "]" + " at " + i+", "+j + " while searching from zone " + zone);
	    return sq.building;
	  }
	  else {
	    //alert("full building :(");
	  }
	}
      }
    }
  }
  return false;

};

// for all occupants, find jobs (or houses)
// argument is a target, if the caller already called checkEmployment
Square.prototype.findJobs = function Square_findJobs(wp) {
  var b = this.building;
  var o = b.occupancy;
  var emp = (this.zone == "r") ? (b.workplace.length) : (b.house.length);
  var unemp = o - emp; // how many people need a place

  var i;
  for (i=0; i<unemp; i++) {
    var sq2;
    if (wp && (wp.workplace.length + wp.house.length < wp.occupancy)) // is there cached workplace with idle citizens?
      sq2 = wp;
    else
      sq2 = this.checkEmployment("findJobs");
    if (!sq2)
      break; // no more jobs

    const JOB_ACCEPTANCE_PROBABILITY = 0.25;
    if (Math.random() > JOB_ACCEPTANCE_PROBABILITY)
      continue; // don't always accept the top-leftmost work place

    if (this.zone == "r") { // found a job
      b.workplace[b.workplace.length] = sq2;
      sq2.house[sq2.house.length] = b;
      if (sq2.abandoned) {
	sq2.abandoned = false;
	sq2.square.dirty = true;
      }
    }
    else { // found a home
      b.house[b.house.length] = sq2;
      sq2.workplace[sq2.workplace.length] = b;
      if (sq2.abandoned) {
	sq2.abandoned = false;
	sq2.square.dirty = true;
      }
    }
  }
};

Square.prototype.bulldoze = function Square_bulldoze() {
  if (this.building) {
    var i; var j;
    var b = this.building.multiSquareRoot;
    for (i = 0; i < b.multiSquareArray.length; i++) {
      b.multiSquareArray[i].abandon();
      b.multiSquareArray[i].square.building = null;
      b.multiSquareArray[i].square.generator = false;
      b.multiSquareArray[i].square.powered = false;
      b.multiSquareArray[i].square.powerline = false;
      b.multiSquareArray[i].square.road = false;
      b.multiSquareArray[i].square.powerDisconnect();
      b.multiSquareArray[i].square.dirty = true;
    }
    return;
  }
  this.generator = false;
  this.powered = false;
  this.powerline = false;
  this.road = false;
  this.powerDisconnect();
  this.dirty = true;  
};

var Buildings = ["house", //0
		 "office", //1
		 "factory", //2
		 "powerplant"]; //3

function Building() {
  this.type = null; // ID into Buildings array
  this.occupancy = null; // maximum occupants
  this.workplace = []; // if residential, list of workplaces of residents
  this.house = []; // if commercial / industrial, list of houses of employees
  this.square = null;
  this.abandoned = null;
  this.multiSquareRoot = null; // points to top left square of building
  this.multiSquareArray = []; // contains all building objects that make up a building (incl. root)
}

Building.prototype.abandon = function Building_abandon() {
  // notify employers they lost an employee
  this.square.dirty = true;
  var k;
  var l;
  for (k=0; k<this.workplace.length; k++) {
    var place = this.workplace[k];
    for (l=0; l<place.house.length; l++) {
      if (place.house[l] == this) {
	place.house.splice(l, 1);
	l--; // removed an item
      }
    }
  }
  // notify houses they lost a job
  for (k=0; k<this.house.length; k++) {
    var place = this.house[k];
    for (l=0; l<place.workplace.length; l++) {
      if (place.workplace[l] == this) {
	place.workplace.splice(l, 1);
	l--; // removed an item
      }
    }
  }
  // remove our residents from our list
  this.workplace = [];
  this.house = [];

};

function Funds() {
  this.money = 0;
  this.tax = 0;
  this.roadCost = 0;
}
