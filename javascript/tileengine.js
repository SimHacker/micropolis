////////////////////////////////////////////////////////////////////////
// tileengine.js
// Don Hopkins


////////////////////////////////////////////////////////////////////////


(function() {


    ////////////////////////////////////////////////////////////////////////
    // Classes


    function TileEngine()
    {
    }


    window.TileEngine = TileEngine;

    var p = TileEngine.prototype;
    p.constructor = TileEngine;


    ////////////////////////////////////////////////////////////////////////
    // Global constants.


    ////////////////////////////////////////////////////////////////////////
    // Methods.


    p.init = function init() {

        // Canvas to draw on.
        this.canvas = null; // canvas
        this.canvasContext = null; // 2d context

        // Where to draw on the canvas.
        this.canvasX = 0; // pixels
        this.canvasY = 0; // pixels
        this.canvasWidth = 0; // pixels
        this.canvasHeight = 0; // pixels

        // Which tiles to draw in the canvas.
        this.canvasTileX = 0; // pixels
        this.canvasTileY = 0; // pixels
        this.canvasTileRows = 0; // tiles
        this.canvasTileColumns = 0; // tiles

        // Tile images.
        this.tileWidth = 0; // pixels
        this.tileHeight = 0; // pixels

        // Tile atlas.
        this.tileAtlas = null; // image
        this.tileAtlasColumns = 0; // tiles
        this.tileAtlasRows = 0; // tiles

        // Tiles array.
        this.tiles = null; // array
        this.tilesWidth = 0; // tiles
        this.tilesHeight = 0; // tiles
        this.tilesBase = 0; // tile offset
        this.tilesColumnStride = 0; // tile offset
        this.tilesRowStride = 0; // tile offset

        // How to transform values.
        this.tileShift = 0; // integer shift
        this.tileMask = -1; // integer mask
        this.tileFunction = null; // function or null
        this.tileMap = null; // dictionary or null

    };


    p.initCanvas = function initCanvas(canvas, canvasX, canvasy, canvasWidth, canvasHeight) {

        this.canvas = canvas;
        this.canvasContext = this.canvas.getContext("2d");
        this.canvasX = canvasX;
        this.canvasY = canvasY;
        this.canvasWidth = canvasWidth;
        this.canvasHeight = canvasHeight;

    };


    p.initTileAtlas = function initTileAtlas(tileAtlas, tileAtlasColumns, tileAtlasRows, tileWidth, tileHeight) {

        this.tileAtlas = tileAtlas;
        this.tileAtlasColumns = tileAtlasColumns;
        this.tileAtlasRows = tileAtlasRows;
        this.tileWidth = tileWidth;
        this.tileHeight = tileHeight;

    };


    p.initTiles = function initTiles(tiles, tilesWidth, tilesHeight, tilesBase, tilesColumnStride, tilesRowStride) {

        this.tiles = tiles;
        this.tilesWidth = tilesWidth;
        this.tilesHeight = tilesHeight;
        this.tilesBase = tilesBase;
        this.tilesColumnStride = tilesColumnStride;
        this.tilesRowStride = tilesRowStride;

    };
        

    p.initTileTransform = function initTileTransform(tileShift, tileMask, tileFunction, tileMap) {
        this.tileShift = tileShift;
        this.tileMask = tileMask;
        this.tileFunction = tileFunction;
        this.tileMap = tileMap;
    };


})();


////////////////////////////////////////////////////////////////////////
