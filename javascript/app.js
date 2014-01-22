$().ready(function($) {

  var game = new MicropolisGame();
  game.init();

});

function MicropolisGame() {

    this.simulationSpeed = 1; // simulation speed (frames per second)
    this.simulationDay = 0; // day number in simulation
    this.simulationYear = 1950; // year number in su
    this.animationSpeed = 8; // animation speed (frames per second)
    this.gameView = null; // container for game view
    this.canvas = null; // DOM pointer to canvas element
    this.context = null; // DOM pointer to context of canvas element
    this.mapXSize = 128; // map x size in tile block size, ie 64 with a tile size of 16px means 64*16px
    this.mapYSize = 128; // map y size in tile block size
    this.tileXSize = 16; // tile x size in pixels
    this.tileYSize = 16; // tile y size in pixels
    this.tileDirectory = "images/tiles/",
    this.tiles = new Array();
    this.gameMap = new Array();
    this.toolContainer = "#micropolis-tools ul.tools";
    this.canvasContainer = "micropolis-game";
    this.selectedGameObjectUrlName = null;
    this.currentCursorImageData = null; // image data for the zone size template that follows the cursor when a zone is selected
    this.previousCursorImageData = null; // and for the previous frame (so it can be reversed)
    this.time = 0;
    this.fTime = -1.0;
    this.noiseGenerator = new ClassicalNoise();
    this.terrainComplexity = 32;

    //minimap
    this.miniMapCanvas = document.getElementById('micropolis-minimap');
    this.miniMapCanvasWidth = this.mapXSize;
    this.miniMapCanvasHeight = this.mapYSize;
    this.miniMapCtx = this.miniMapCanvas.getContext("2d");
    this.miniMapCanvasData = this.miniMapCtx.getImageData(0, 0, this.miniMapCanvasWidth, this.miniMapCanvasHeight);

    this.drawMiniMapPixel = function(x, y, r, g, b, a) {
        var index = (x + y * self.miniMapCanvasWidth) * 4;

        self.miniMapCanvasData.data[index + 0] = r;
        self.miniMapCanvasData.data[index + 1] = g;
        self.miniMapCanvasData.data[index + 2] = b;
        self.miniMapCanvasData.data[index + 3] = a;
    }

    this.updateMiniMapCanvas = function() {
        self.miniMapCtx.putImageData(self.miniMapCanvasData, 0, 0);
    }

    this.gameObjects = new Array(
      { 
          _id: -1,
          type: "bulldozer",
          name: "bulldozer",
          urlname: "bulldozer",
          destroyer: true,
          icon: "images/icons/bulldozer.png",
          destroyToUrlName: "ground",
          buildOverlayColor: "#FF0000",
          xSize: 1,
          ySize: 1
      },
      { 
          _id: 0,
          type: "water",
          name: "water",
          urlname: "water",
          population: 0,
          hasPower: 0,
          buildable: false,
          allowBuilding: false,
          xSize: 1,
          ySize: 1,
          xPos: 0,
          yPos: 0,
          bulldozable: false,
          conductElectricity: false,
          canBurn: false,
          defaultQuery: {},
          framesets: [
            { attrs: { },
              tilesets: [
                ["micropolis_tile_0002.png"]
              ]
            }
          ]
      },
      { 
          _id: 1,
          type: "ground",
          name: "Ground",
          urlname: "ground",
          population: 0,
          hasPower: 0,
          buildable: false,
          allowBuilding: true,
          xSize: 1,
          ySize: 1,
          xPos: 0,
          yPos: 0,
          defaultQuery: {},
          framesets: [
            { attrs: { },
              tilesets: [
                ["micropolis_tile_0000.png"]
              ]
            }
          ]
      },
      { 
          _id: 2,
          type: "trees",
          name: "Trees",
          urlname: "trees",
          population: 0,
          hasPower: 0,
          buildable: false,
          allowBuilding: true,
          xSize: 1,
          ySize: 1,
          xPos: 0,
          yPos: 0,
          defaultQuery: {},
          framesets: [
            { attrs: { },
              tilesets: [
                ["micropolis_tile_0037.png"]
              ]
            }
          ]
      },
      { 
          _id: 3,
          type: "zone-commercial",
          name: "Commercial Zone",
          urlname: "commercialzone",
          icon: "images/icons/commercial.png",
          population: 0,
          hasPower: 0,
          pollutionLevel: 0,
          landValue: 0,
          stages: 0,
          buildable: true,
          buildOverlayColor: "#0000FF",
          allowBuilding: false,
          xSize: 3,
          ySize: 3,
          xPos: 0,
          yPos: 0,
          onPopulationGrowth: function(populationData) {},
          onPopulationDecrease: function(populationData) {},
          onPollutionGrowth: function(pollutionData) {},
          onPollutionDecrease: function(pollutionData) {},
          onLandValueGrowth: function(landValueData) {},
          onLandValueDecrease: function(landValueData) {},
          defaultQuery: { population: 0 },
          framesets: [
            { attrs: { population: 0 },
              tilesets: [
                ["micropolis_tile_0423.png", "micropolis_tile_0424.png", "micropolis_tile_0425.png"], ["micropolis_tile_0426.png", "micropolis_tile_0427.png", "micropolis_tile_0428.png"], ["micropolis_tile_0429.png", "micropolis_tile_0430.png", "micropolis_tile_0431.png"]
              ]
            }
          ]
      },
      { 
          _id: 4,
          type: "zone-residential",
          name: "Residential Zone",
          urlname: "residentialzone",
          icon: "images/icons/residential.png",
          population: 0,
          hasPower: 0,
          pollutionLevel: 0,
          landValue: 0,
          stages: 0,
          buildable: true,
          buildOverlayColor: "#00FF00",
          allowBuilding: false,
          xSize: 3,
          ySize: 3,
          xPos: 0,
          yPos: 0,
          onPopulationGrowth: function(populationData) {},
          onPopulationDecrease: function(populationData) {},
          onPollutionGrowth: function(pollutionData) {},
          onPollutionDecrease: function(pollutionData) {},
          onLandValueGrowth: function(landValueData) {},
          onLandValueDecrease: function(landValueData) {},
          defaultQuery: { population: 0 },
          framesets: [
            { attrs: { population: 0 },
              tilesets: [
                ["micropolis_tile_0240.png", "micropolis_tile_0241.png", "micropolis_tile_0242.png"], ["micropolis_tile_0243.png", "micropolis_tile_0244.png", "micropolis_tile_0245.png"], ["micropolis_tile_0246.png", "micropolis_tile_0247.png", "micropolis_tile_0248.png"]
              ]
            }
          ]
      },
      { 
          _id: 5,
          type: "zone-industrial",
          name: "Industrial Zone",
          urlname: "industrialzone",
          icon: "images/icons/industrial.png",
          population: 0,
          hasPower: 0,
          pollutionLevel: 0,
          landValue: 0,
          stages: 0,
          buildable: true,
          buildOverlayColor: "#00FFFF",
          allowBuilding: false,
          xSize: 3,
          ySize: 3,
          xPos: 0,
          yPos: 0,
          onPopulationGrowth: function(populationData) {},
          onPopulationDecrease: function(populationData) {},
          onPollutionGrowth: function(pollutionData) {},
          onPollutionDecrease: function(pollutionData) {},
          onLandValueGrowth: function(landValueData) {},
          onLandValueDecrease: function(landValueData) {},
          defaultQuery: { population: 0 },
          framesets: [
            { attrs: { population: 0 },
              tilesets: [
                ["micropolis_tile_0612.png", "micropolis_tile_0613.png", "micropolis_tile_0614.png"], ["micropolis_tile_0615.png", "micropolis_tile_0616.png", "micropolis_tile_0617.png"], ["micropolis_tile_0618.png", "micropolis_tile_0619.png", "micropolis_tile_0620.png"]
              ]
            }
          ]
      },
      { 
          _id: 6,
          type: "zone-park",
          name: "Park Zone",
          urlname: "parkzone",
          icon: "images/icons/park.png",
          population: 0,
          hasPower: 0,
          pollutionLevel: 0,
          landValue: 0,
          stages: 0,
          buildable: true,
          buildOverlayColor: "#00FF00",
          allowBuilding: false,
          xSize: 1,
          ySize: 1,
          xPos: 0,
          yPos: 0,
          onPopulationGrowth: function(populationData) {},
          onPopulationDecrease: function(populationData) {},
          onPollutionGrowth: function(pollutionData) {},
          onPollutionDecrease: function(pollutionData) {},
          onLandValueGrowth: function(landValueData) {},
          onLandValueDecrease: function(landValueData) {},
          defaultQuery: {},
          framesets: [
            { attrs: { },
              tilesets: [
                ["micropolis_tile_0840.png"]
              ]
            }
          ]
      },
      { 
          _id: 6,
          type: "zone-road",
          name: "Road",
          urlname: "road",
          icon: "images/icons/road.png",
          population: 0,
          hasPower: 0,
          pollutionLevel: 0,
          landValue: 0,
          stages: 0,
          buildable: true,
          buildOverlayColor: "#000000",
          allowBuilding: false,
          xSize: 1,
          ySize: 1,
          xPos: 0,
          yPos: 0,
          onPopulationGrowth: function(populationData) {},
          onPopulationDecrease: function(populationData) {},
          onPollutionGrowth: function(pollutionData) {},
          onPollutionDecrease: function(pollutionData) {},
          onLandValueGrowth: function(landValueData) {},
          onLandValueDecrease: function(landValueData) {},
          defaultQuery: { north: 0, east: 0, south: 0, west: 0 },
          connectAdjacentTiles: true,
          framesets: [
            { attrs: { north: 0, east: 0, south: 0, west: 0 },
              tilesets: [
                ["micropolis_tile_0066.png"]
              ]
            },
            { attrs: { north: 0, east: 0, south: 0, west: 1 },
              tilesets: [
                ["micropolis_tile_0066.png"]
              ]
            },
            { attrs: { north: 0, east: 1, south: 0, west: 0 },
              tilesets: [
                ["micropolis_tile_0066.png"]
              ]
            },
            { attrs: { north: 0, east: 1, south: 0, west: 1 },
              tilesets: [
                ["micropolis_tile_0066.png"]
              ]
            },
            { attrs: { north: 0, east: 0, south: 1, west: 0 },
              tilesets: [
                ["micropolis_tile_0067.png"]
              ]
            },
            { attrs: { north: 1, east: 0, south: 0, west: 0 },
              tilesets: [
                ["micropolis_tile_0067.png"]
              ]
            },
            { attrs: { north: 1, east: 0, south: 1, west: 0 },
              tilesets: [
                ["micropolis_tile_0067.png"]
              ]
            },
            { attrs: { north: 1, east: 1, south: 0, west: 0 },
              tilesets: [
                ["micropolis_tile_0068.png"]
              ]
            },
            { attrs: { north: 0, east: 1, south: 1, west: 0 },
              tilesets: [
                ["micropolis_tile_0069.png"]
              ]
            },
            { attrs: { north: 0, east: 0, south: 1, west: 1 },
              tilesets: [
                ["micropolis_tile_0070.png"]
              ]
            },
            { attrs: { north: 1, east: 0, south: 0, west: 1 },
              tilesets: [
                ["micropolis_tile_0071.png"]
              ]
            },
            { attrs: { north: 1, east: 1, south: 0, west: 1 },
              tilesets: [
                ["micropolis_tile_0072.png"]
              ]
            },
            { attrs: { north: 1, east: 1, south: 1, west: 0 },
              tilesets: [
                ["micropolis_tile_0073.png"]
              ]
            },
            { attrs: { north: 0, east: 1, south: 1, west: 1 },
              tilesets: [
                ["micropolis_tile_0074.png"]
              ]
            },
            { attrs: { north: 1, east: 0, south: 1, west: 1 },
              tilesets: [
                ["micropolis_tile_0075.png"]
              ]
            },
            { attrs: { north: 1, east: 1, south: 1, west: 1 },
              tilesets: [
                ["micropolis_tile_0076.png"]
              ]
            }
          ]
      },
      { 
          _id: 6,
          type: "zone-powerline",
          name: "Power line",
          urlname: "powerline",
          icon: "images/icons/powerlines.png",
          population: 0,
          hasPower: 0,
          pollutionLevel: 0,
          landValue: 0,
          stages: 0,
          buildable: true,
          buildOverlayColor: "#000000",
          allowBuilding: false,
          xSize: 1,
          ySize: 1,
          xPos: 0,
          yPos: 0,
          onPopulationGrowth: function(populationData) {},
          onPopulationDecrease: function(populationData) {},
          onPollutionGrowth: function(pollutionData) {},
          onPollutionDecrease: function(pollutionData) {},
          onLandValueGrowth: function(landValueData) {},
          onLandValueDecrease: function(landValueData) {},
          defaultQuery: { north: 0, east: 0, south: 0, west: 0 },
          connectAdjacentTiles: true,
          framesets: [
            { attrs: { north: 0, east: 0, south: 0, west: 0 },
              tilesets: [
                ["micropolis_tile_0210.png"]
              ]
            },
            { attrs: { north: 0, east: 0, south: 0, west: 1 },
              tilesets: [
                ["micropolis_tile_0210.png"]
              ]
            },
            { attrs: { north: 0, east: 1, south: 0, west: 0 },
              tilesets: [
                ["micropolis_tile_0210.png"]
              ]
            },
            { attrs: { north: 0, east: 1, south: 0, west: 1 },
              tilesets: [
                ["micropolis_tile_0210.png"]
              ]
            },
            { attrs: { north: 0, east: 0, south: 1, west: 0 },
              tilesets: [
                ["micropolis_tile_0211.png"]
              ]
            },
            { attrs: { north: 1, east: 0, south: 0, west: 0 },
              tilesets: [
                ["micropolis_tile_0211.png"]
              ]
            },
            { attrs: { north: 1, east: 0, south: 1, west: 0 },
              tilesets: [
                ["micropolis_tile_0211.png"]
              ]
            },
            { attrs: { north: 1, east: 1, south: 0, west: 0 },
              tilesets: [
                ["micropolis_tile_0212.png"]
              ]
            },
            { attrs: { north: 0, east: 1, south: 1, west: 0 },
              tilesets: [
                ["micropolis_tile_0213.png"]
              ]
            },
            { attrs: { north: 0, east: 0, south: 1, west: 1 },
              tilesets: [
                ["micropolis_tile_0214.png"]
              ]
            },
            { attrs: { north: 1, east: 0, south: 0, west: 1 },
              tilesets: [
                ["micropolis_tile_0215.png"]
              ]
            },
            { attrs: { north: 1, east: 1, south: 0, west: 1 },
              tilesets: [
                ["micropolis_tile_0216.png"]
              ]
            },
            { attrs: { north: 1, east: 1, south: 1, west: 0 },
              tilesets: [
                ["micropolis_tile_0217.png"]
              ]
            },
            { attrs: { north: 0, east: 1, south: 1, west: 1 },
              tilesets: [
                ["micropolis_tile_0218.png"]
              ]
            },
            { attrs: { north: 1, east: 0, south: 1, west: 1 },
              tilesets: [
                ["micropolis_tile_0219.png"]
              ]
            },
            { attrs: { north: 1, east: 1, south: 1, west: 1 },
              tilesets: [
                ["micropolis_tile_0220.png"]
              ]
            }
          ]
      }
    );

    var self = this;

    /**
     * Return the image data for a game object
     * @param {String} gameObjectUrlName The game object's url name
     * @param {Object} frameSearchQuery Search query to find which frame's tiles to render (in JSON format)
     * @returns {Array} Array containing image elements, x and y coordinates to be passed to drawGameObject and finally the gameObjectResult
     */
    this.returnGameObjectImageData = function(gameObjectUrlName, frameSearchQuery) {
      
      var gameObjectResult = $.grep(self.gameObjects, function(e) { return e.urlname === gameObjectUrlName }); // return gameobject based on url name
      if(gameObjectResult.length > 0) { // only execute if we have a result
        
        var framesetResult = $.grep(gameObjectResult[0].framesets, function(e) { if(_.isEqual(e.attrs, frameSearchQuery)) return true; }); // return frameset based on set attributes (population etc.)

        if(framesetResult.length > 0) { // only execute if we have a result

          var frames = new Array(); // container for frame image information

          // go through the (possibly animated) tile sets for this frame set and push the images in the frames array
          _.each(framesetResult[0].tilesets, function(element, index, list) { 
            frames.push(element);
          });

          return new Array(frames, gameObjectResult);
        }
        else {
          console.log('returnGameObjectImageData could not find framesets');
          return false; // we have no framesets to return
        }

      }
      else {
        console.log('returnGameObjectImageData could not find gameObject');
        return false; // we have no gameobject
      }

    }

    this.getMapCoordinates = function(posX, posY, tileX, tileY, center) {

      if(center) {
        var mapXPos = Math.ceil((posX / self.tileXSize) - tileX);
        var mapYPos = Math.ceil((posY / self.tileYSize) - tileY);
      }
      else {
        var mapXPos = Math.floor((posX / self.tileXSize) - tileX);
        var mapYPos = Math.floor((posY / self.tileYSize) - tileY);
      }
      return [mapYPos, mapXPos];
    }


    this.normalizeNeighbouringTiles = function(affectedTiles) {

      for(var i = 0; i < affectedTiles.length; i++) {

          var tile = affectedTiles[i];

          if(tile.gameObject.connectAdjacentTiles) {

            // fetch adjacent tiles that are the same as the tile in this location and normalize them
            var adjacentTiles = self.getAdjacentTiles(tile.gameObject, tile.mapX, tile.mapY);

            _.each(adjacentTiles, function(item, tileIndex) {
              if(item != null) {
                self.drawGameObject(self.returnGameObjectImageData(adjacentTiles[tileIndex].gameObject.urlname, self.getAdjacentTileAttributes(tile.gameObject, adjacentTiles[tileIndex].mapX, adjacentTiles[tileIndex].mapY)), adjacentTiles[tileIndex].mapX * self.tileXSize, adjacentTiles[tileIndex].mapY * self.tileYSize, false, true);
              }
            });

            // normalize this tile
            self.drawGameObject(self.returnGameObjectImageData(tile.gameObject.urlname, self.getAdjacentTileAttributes(tile.gameObject, tile.mapX, tile.mapY)), tile.mapX * self.tileXSize, tile.mapY * self.tileYSize, false, true);

          }
      }

    }

    this.getAdjacentTiles = function(gameObject, x, y) {

      var north = null;
      var west = null;
      var east = null;
      var south = null;

      if(self.gameMap[y - 1][x].gameObject.urlname == gameObject.urlname) {
        north = self.gameMap[y - 1][x];
      }
      if(self.gameMap[y][x - 1].gameObject.urlname == gameObject.urlname) {
        west = self.gameMap[y][x - 1];
      }
      if(self.gameMap[y][x + 1].gameObject.urlname == gameObject.urlname) {
        east = self.gameMap[y][x + 1];
      }
      if(self.gameMap[y + 1][x].gameObject.urlname == gameObject.urlname) {
        south = self.gameMap[y + 1][x];
      }

      var ret = { 'north': north, 'west': west, 'east': east, 'south': south };
      return ret;

    }

    this.getAdjacentTileAttributes = function(gameObject, x, y) {

      var north = 0;
      var west = 0;
      var east = 0;
      var south = 0;

      if(self.gameMap[y - 1][x].gameObject.urlname == gameObject.urlname) {
        north = 1;
      }
      if(self.gameMap[y][x - 1].gameObject.urlname == gameObject.urlname) {
        west = 1;
      }
      if(self.gameMap[y][x + 1].gameObject.urlname == gameObject.urlname) {
        east = 1;
      }
      if(self.gameMap[y + 1][x].gameObject.urlname == gameObject.urlname) {
        south = 1;
      }

      var ret = { 'north': north, 'west': west, 'east': east, 'south': south };
      return ret;

    }

    this.drawGameObject = function(input, posX, posY, interpolate, forceUpdate) {

      var frames = input[0];
      var gameObjectResult = input[1];

      var tileMap = new Array();
      var canPlaceObject = true;

      var affectedTiles = new Array();

      // y coordinates
      for(var tileY = 0; tileY < frames.length; tileY++) {

        tileMap[tileY] = new Array();

        // x coordinates
        for(var tileX = 0; tileX < frames[tileY].length; tileX++) {

          // store a reference to the gameobject in the game map along with its tile number to be drawn for this position
          if(frames.length > 1) {
            var mapCoords = self.getMapCoordinates(posX, posY, tileX, tileY, true);
          }
          else {
            var mapCoords = self.getMapCoordinates(posX, posY, tileX, tileY, false);
          }

          var mapXPos = mapCoords[1];
          var mapYPos = mapCoords[0];

          // first check if there's something that we can't build upon on this tile
          if(self.gameMap[mapYPos][mapXPos] === undefined || self.gameMap[mapYPos][mapXPos].gameObject.allowBuilding == true || forceUpdate == true) {
            tileMap[tileY][tileX] = { gameObject: gameObjectResult[0], tileX: tileX, tileY: tileY, mapX: mapXPos, mapY: mapYPos };
          }
          else {
            canPlaceObject = false;
          }

        }

      }

      if(canPlaceObject == true) {

        // y coordinates
        for(var tileY = 0; tileY < frames.length; tileY++) {

          // x coordinates
          for(var tileX = 0; tileX < frames[tileY].length; tileX++) {

            if(frames.length > 1) {
              var mapCoords = self.getMapCoordinates(posX, posY, tileX, tileY, true);
            }
            else {
              var mapCoords = self.getMapCoordinates(posX, posY, tileX, tileY, false);
            }

            var mapXPos = mapCoords[1];
            var mapYPos = mapCoords[0];


            // set the map position to this gameobject with the tile x/y offset
            self.gameMap[mapYPos][mapXPos] = tileMap[tileY][tileX];
            self.gameMap[mapYPos][mapXPos].gameObject.xPos = mapXPos;
            self.gameMap[mapYPos][mapXPos].gameObject.yPos = mapYPos;

            affectedTiles.push(self.gameMap[mapYPos][mapXPos]);

            var img = new Image();

            // tile location in the frame array is inverse to the current tile number
            var imgTileX = frames[tileY].length - tileX - 1;
            var imgTileY = frames[tileX].length - tileY - 1;

            img.src = self.tileDirectory+frames[imgTileY][imgTileX];

            // spread the tiles around the center of the click
            img.tileXPos = mapXPos * self.tileXSize;
            img.tileYPos = mapYPos * self.tileYSize;

            self.drawMiniMapPixel(mapXPos, mapYPos, 255, 255, 0, 1);
            self.updateMiniMapCanvas();

            img.onload = function() {
              self.context.drawImage(this, this.tileXPos, this.tileYPos);
            };


          }

        }


      }

    return affectedTiles;

    }

    /**
     * Initialize and render the game canvas
     */
    this.init = function() {
      
      self.canvas = document.getElementById(self.canvasContainer);
      self.context = this.canvas.getContext('2d');

      $(self.canvas).attr('width', window.innerWidth - $(self.canvas).position().left);
      $(self.canvas).attr('height', window.innerHeight);
      //$(self.canvas).css({'margin-top': '-'+window.innerHeight+'px'});

      $('#micropolis-minimap').attr('width', self.mapXSize);
      $('#micropolis-minimap').attr('height', self.mapYSize);

      // Initialize the game map array
      self.newMap(1024);

      // Initialize tools
      _.each(self.gameObjects, function(e) {
        if(e.buildable == true || e.destroyer == true) {
          self.addTool(e);
        }
      });

      $('#terrain-complexity').change(function (e) {
        self.terrainComplexity = $(this).val();
        self.newMap();
      });

      $('#terrain-regenerate').click(function(e) {
        e.preventDefault();
        self.newMap();
      });

      var addObject = function(e) {
        
        var gameObjectResult = $.grep(self.gameObjects, function(e) { return e.urlname === self.selectedGameObjectUrlName }); // return gameobject based on url name

        if(gameObjectResult[0].destroyer) {
          var destroyToBlockResult = $.grep(self.gameObjects, function(e) { return e.urlname === gameObjectResult[0].destroyToUrlName }); // return destroy to gameobject based on url name
          var affectedTiles = self.drawGameObject(self.returnGameObjectImageData(destroyToBlockResult[0].urlname, destroyToBlockResult[0].defaultQuery), e.pageX - $('#'+self.canvasContainer).position().left, e.pageY, true, true);
          self.normalizeNeighbouringTiles(affectedTiles);
          $('#micropolis-audio-plopdoze')[0].play();
        }
        else {
          var affectedTiles = self.drawGameObject(self.returnGameObjectImageData(self.selectedGameObjectUrlName, gameObjectResult[0].defaultQuery), e.pageX - $('#'+self.canvasContainer).position().left, e.pageY, true, false);
          self.normalizeNeighbouringTiles(affectedTiles);
          $('#micropolis-audio-plopok')[0].play();
        }

      }

      // bind mouse click events
      $(self.canvas).mousedown(function (e) {

        // if a tool is selected do something with it
        if(self.selectedGameObjectUrlName != null) {
          addObject(e);
          $(this).bind('mousemove', addObject);
        }

        // scroll event
        else {
          $(this).mousemove(function (mme) {
          });
        }

      }).mouseup(function (e) {
        $(this).unbind('mousemove', addObject);
      });

      var moveMiniMap = function(e) {
        $('#micropolis-minimap-container').offset({ top: e.pageY, left: e.pageX - self.mapXSize / 2 }); 
      }

      $('#micropolis-minimap-container').mousedown(function (e) {
        $(document).bind('mousemove', moveMiniMap);
      });

      $('#micropolis-minimap-container').mouseup(function (e) {
        $(document).unbind('mousemove', moveMiniMap);
      });

    }

    this.newMap = function(seed) {

      if(seed === undefined) seed = (Math.random() * 255) / 255;
      else seed = (Math.random() * seed) / seed;

      for(yIndex = 0; yIndex < self.mapYSize; yIndex++) {
        self.gameMap[yIndex] = [];
        for(xIndex = 0; xIndex < self.mapXSize; xIndex++) {
          var height = self.noiseGenerator.noise(xIndex / self.terrainComplexity, yIndex / self.terrainComplexity, seed) * 10;
          if(height < -0.5) tile = 'water';
          else if(height < 2.5) tile = 'ground';
          else tile = 'trees';
          self.drawGameObject(self.returnGameObjectImageData(tile, { }), xIndex * self.tileXSize, yIndex * self.tileYSize, false, true);
        }
      }

    }

    /**
     * 
     */

    this.addTool = function(gameObject) {
      $(self.toolContainer).append('<li data-urlname="'+gameObject.urlname+'"><img src="'+gameObject.icon+'" /></li>');
      $(self.toolContainer+' li[data-urlname='+gameObject.urlname+'] img').click(function(e) {

        if($(this).parent().hasClass('selected')) {
          $(this).parents('ul').find('li').removeClass('selected');
          self.selectedGameObjectUrlName = null;
          self.previousCursorImageData = null;
          self.currentCursorImageData = null;
          $('#'+self.canvasContainer).unbind('mousemove');
          $('#micropolis-tool-overlay').hide();
        }
        else {

          $('#micropolis-tools').mousemove(function (e) {
              $('#micropolis-tool-overlay').hide();
          });

          $('#'+self.canvasContainer).mousemove(function (e) {
            if(e.pageX <= $(this).position().left || e.pageY <= $(this).position().top) {
              $('#micropolis-tool-overlay').hide();
            }
            else {
              var coords = self.getMapCoordinates(e.pageX - $(this).position().left, e.pageY, 0, 0, false);
              if(gameObject.xSize > 1) {
                var left = ($(this).position().left + coords[1] * self.tileXSize) - (Math.floor(gameObject.xSize / 2) * self.tileXSize);
                var top = coords[0] * self.tileYSize - (Math.floor(gameObject.ySize / 2) * self.tileYSize);
              }
              else {
                var left = $(this).position().left + coords[1] * self.tileXSize;
                var top = coords[0] * self.tileYSize;
              }

              $('#micropolis-tool-overlay').show().css({'background-color': gameObject.buildOverlayColor, 'width': gameObject.xSize * self.tileXSize, 'height': gameObject.ySize * self.tileYSize, 'left': left, 'top': top});
            }
          });

          $(this).parents('ul').find('li').removeClass('selected');
          $(this).parents().addClass('selected');
          self.selectedGameObjectUrlName = gameObject.urlname;
        }

      });
    }

}

