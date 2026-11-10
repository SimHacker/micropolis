////////////////////////////////////////////////////////////////////////
// micropolis.js
// Don Hopkins


(function() {


    ////////////////////////////////////////////////////////////////////////
    // Classes


    ////////////////////////////////////////////////////////////////////////
    // Class Map
    //
    // A generic class for maps in the Micropolis game.
    // A map is assumed to cover a 2D grid.
    // A block of positions may be clustered in (blockSize x blockSize)
    // squares, and represented by a single data value.


    function Map(worldWidth, worldHeight, blockShift, dataSize, arrayType) {

        this.worldWidth = worldWidth;
        this.worldHeight = worldHeight;
        this.blockShift = blockShift;
        this.blockSize = 1 << this.blockShift;
        this.width = (this.worldWidth + this.blockSize - 1) << this.blockShift;
        this.height = (this.worldHeight + this.blockSize - 1) << this.blockShift;
        this.dataSize = dataSize;
        this.arrayType = arrayType;

        this.data = 
            new this.arrayType(
                new ArrayBuffer(
                    this.width * this.height * this.dataSize));

    }


    // Constructor for maps of 1 byte per 1x1 tile.
    Map.makeMap_Uint8_1x1 = function makeMap_Uint8_1x1(width, height) {
        return new Map(
            width,
            height,
            0,
            1,
            Uint8Array);
    };


    // Constructor for maps of 1 byte per 2x2 tile cluster.
    Map.makeMap_Uint8_2x2 = function makeMap_Uint8_2x2(width, height) {
        return new Map(
            width,
            height,
            1,
            1,
            Uint8Array);
    };


    // Constructor for maps of 1 byte per 4x4 tile cluster.
    Map.makeMap_Uint8_4x4 = function makeMap_Uint8_4x4(width, height) {
        return new Map(
            width,
            height,
            2,
            1,
            Uint8Array);
    };


    // Constructor for maps of 2 bytes per 1x1 tile.
    Map.makeMap_Uint16_1x1 = function makeMap_Uint16_1x1(width, height) {
        return new Map(
            width,
            height,
            0,
            2,
            Uint16Array);
    };


    // Constructor for maps of 2 bytes per 8x8 tile cluster.
    Map.makeMap_Uint16_8x8 = function makeMap_Uint16_8x8(width, height) {
        return new Map(
            width,
            height,
            3,
            2,
            Uint16Array);
    };


    // Constructor for maps of 2 bytes per 8x8 tile cluster.
    Map.makeMap_Int16_8x8 = function makeMap_Int16_8x8(width, height) {
        return new Map(
            width,
            height,
            3,
            2,
            Int16Array);
    };


    // Fill the map with the given value.
    Map.prototype.fill = function fill(value) {

        for (var i = 0, len = this.width * this.height;
             i < len;
             i++) {
            this.data[i] = value;
        }

    };


    // Clear the map with zeros.
    Map.prototype.clear = function clear() {

        this.fill(0);

    };


    // Copy the values from another map.
    Map.prototype.copy = function copy(map) {

        for (var i = 0, len = this.width * this.height;
             i < len;
             i++) {
            this.data[i] = map.data[i];
        }

    };


    // Filter the values of the map with a function, which takes x, y and
    // the value, and returns null for no change, or a number to change
    // the value.
    Map.prototype.filter = function filter(filterFunction) {

        var data = this.data;

        for (var y = 0, index = 0;
             y < this.height;
             y++) {

            for (var x = 0;
                 x < this.width;
                 x++, index++) {

                var value = data[index];

                var newValue = filterFunction(x, y, value);
                if (newValue !== null) {
                    data[index] = newValue;
                }

            }

        }

    };


    // Set a value in the map at x, y in map coordinates.
    Map.prototype.set = function set(x, y, val) {

        this.data[x + (y + this.width)] = val;

    };


    // Get a value from the map at x, y in map coordinates.
    Map.prototype.get = function get(x, y) {

        return this.data[x + (y + this.width)];

    };


    // Return true if x, y in map coordinates is on the map.
    Map.prototype.onMap = function onMap(x, y) {

        return (
            (x >= 0) &&
            (x < this.width) &&
            (y >= 0) &&
            (y < this.height));

    };


    // Set a value in the map at x, y in world coordinates.
    Map.prototype.worldSet = function worldSet(x, y, val) {

        this.set(
            Math.floor(x / this.blockShift),
            Math.floor(y / this.blockShift),
            val);

    };


    // Get a value from the map at x, y in world coordinates.
    Map.prototype.worldGet = function worldGet(x, y) {

        return this.get(
            Math.floor(x / this.blockShift),
            Math.floor(y / this.blockShift));

    };


    // Return true of x, y in world coordinates is on the map.
    Map.prototype.worldOnMap = function worldOnMap(x, y) {

        return this.onMap(
            Math.floor(x / this.blockShift),
            Math.floor(y / this.blockShift));

    };


    // Return the map data.
    Map.prototype.getData = function getData() {

        return this.data;

    };


    ////////////////////////////////////////////////////////////////////////
    // Class ToolEffects


    // Class for storing effects of applying a tool to the world.
    //
    // When applying a tool, two things change:
    //  - The world map.
    //  - The funds of the player.
    //  - Messages sent to the player and the front-end.
    //  - Sounds played for the player.
    //
    // The funds gives a decision problem. To decide whether the tool can be
    // applied, you need to know the cost. To know the cost you need to know the
    // exact changes being made.
    // The simplest way to compute the exact changes is to simply apply the tool to
    // the world. This holds especially when tools get stacked on top of each
    // other.
    //
    // This class provides an easy way out, greatly simplifying the problem.
    // All tools do not modify the world directly, but instead put their results
    // in an instance of this class, thus collecting all the modifications.
    // After the whole operation is 'done', the ToolEffects instance can tell the
    // precise cost and what has been changed in the world. At that moment, the
    // yes/no decision can be made, and the effects can be copied to the real map
    // and funds.


    function ToolEffects(micropolis)
    {

        this.micropolis = micropolis;
        this.clear();

    }


    ToolEffects.prototype.clear = function clear() {

        this.cost = 0;
        this.modifications = {};
        this.messages = [];

    };


    ToolEffects.prototype.getCost = function getCost() {

        return this.cost;

    };


    ToolEffects.prototype.addCost = function addCost(amount) {

        this.cost += amount;

    };


    ToolEffects.prototype.addMessage = function addMessage(message, params) {

        this.messages.push([message, params]);

    };


    ToolEffects.prototype.modifyWorld = function modifyWorld() {

        this.micropolis.spend(this.cost);

        for (var key in this.modifications) {

            var modification = this.modifications[key];

            this.micropolis.setTile(
                modification.x,
                modification.y,
                modification.tile);

        }

        for (var i = 0, n = this.messages.length;
             i < n;
             i++) {

            var messageAndParams = this.messages[i];
            var message = messageAndParams[0];
            var params = messageAndParams[1];

            switch (message) {

                case 'makeSound':
                    this.micropolis.makeSound.apply(this. params);
                    break;

                case 'doTool':
                    this.micropolis.doTool.apply(this. params);
                    break;

                default:
                    break;

            }

        }

        this.clear();

    };


    ToolEffects.prototype.modifyIfEnoughFunding = function modifyIfEnoughFunding() {

        if (this.micropolis.totalFonds <= this.cost) {
            return false;
        }

        this.modifyWorld();
        return true;

    };


    ToolEffects.prototype.getTile = function getTile(x, y) {

        var key = x + ',' + y;
        var modification = this.modifications[key];

        if (modification) {
            return modification.tile;
        }

        return this.micropolis.getTile(x, y);

    };


    ToolEffects.prototype.setTile = function setTile(x, y, tile) {

        var key = x + ',' + y;

        this.modifications[key] = {
            x: x,
            y: y,
            tile: tile
        };

    };


    ////////////////////////////////////////////////////////////////////////
    // Class BuildingProperties


    // Properties of a building with respect to its construction.


    function BuildingProperties(sizeX, sizeY, baseTile, tool, toolName, animated)
    {
        this.sizeX = sizeX;
        this.sizeY = sizeY;
        this.baseTile = baseTile;
        this.tool = tool;
        this.toolName = toolName;
        this.animated = animated;
    }


    ////////////////////////////////////////////////////////////////////////
    // Class Micropolis


    function Micropolis()
    {
        this.init();
    }


    window.Micropolis = Micropolis;

    var p = Micropolis.prototype;
    p.constructor = Micropolis;


    ////////////////////////////////////////////////////////////////////////
    // Global constants.


    var DIRT = 0;
    var RIVER = 2;
    var REDGE = 3;
    var CHANNEL = 4;
    var FIRSTRIVEDGE = 5;
    var LASTRIVEDGE = 20;
    var WATER_LOW = 2;
    var WATER_HIGH = 20;
    var TREEBASE = 21;
    var WOODS_LOW = 21;
    var LASTTREE = 36;
    var WOODS = 37;
    var UNUSED_TRASH1 = 38;
    var UNUSED_TRASH2 = 39;
    var WOODS_HIGH = 39;
    var WOODS2 = 40;
    var WOODS3 = 41;
    var WOODS4 = 42;
    var WOODS5 = 43;
    var RUBBLE = 44;
    var LASTRUBBLE = 47;
    var FLOOD = 48;
    var LASTFLOOD = 51;
    var RADTILE = 52;
    var UNUSED_TRASH3 = 53;
    var UNUSED_TRASH4 = 54;
    var UNUSED_TRASH5 = 55;
    var FIRE = 56;
    var FIREBASE = 56;
    var LASTFIRE = 63;
    var HBRIDGE = 64;
    var ROADBASE = 64;
    var VBRIDGE = 65;
    var ROADS = 66;
    var ROADS2 = 67;
    var ROADS3 = 68;
    var ROADS4 = 69;
    var ROADS5 = 70;
    var ROADS6 = 71;
    var ROADS7 = 72;
    var ROADS8 = 73;
    var ROADS9 = 74;
    var ROADS10 = 75;
    var INTERSECTION = 76;
    var HROADPOWER = 77;
    var VROADPOWER = 78;
    var BRWH = 79;
    var LTRFBASE = 80;
    var BRWV = 95;
    var BRWXXX1 = 111;
    var BRWXXX2 = 127;
    var BRWXXX3 = 143;
    var HTRFBASE = 144;
    var BRWXXX4 = 159;
    var BRWXXX5 = 175;
    var BRWXXX6 = 191;
    var LASTROAD = 206;
    var BRWXXX7 = 207;
    var HPOWER = 208;
    var VPOWER = 209;
    var LHPOWER = 210;
    var LVPOWER = 211;
    var LVPOWER2 = 212;
    var LVPOWER3 = 213;
    var LVPOWER4 = 214;
    var LVPOWER5 = 215;
    var LVPOWER6 = 216;
    var LVPOWER7 = 217;
    var LVPOWER8 = 218;
    var LVPOWER9 = 219;
    var LVPOWER10 = 220;
    var RAILHPOWERV = 221;
    var RAILVPOWERH = 222;
    var POWERBASE = 208;
    var LASTPOWER = 222;
    var UNUSED_TRASH6 = 223;
    var HRAIL = 224;
    var VRAIL = 225;
    var LHRAIL = 226;
    var LVRAIL = 227;
    var LVRAIL2 = 228;
    var LVRAIL3 = 229;
    var LVRAIL4 = 230;
    var LVRAIL5 = 231;
    var LVRAIL6 = 232;
    var LVRAIL7 = 233;
    var LVRAIL8 = 234;
    var LVRAIL9 = 235;
    var LVRAIL10 = 236;
    var HRAILROAD = 237;
    var VRAILROAD = 238;
    var RAILBASE = 224;
    var LASTRAIL = 238;
    var ROADVPOWERH = 239;
    var RESBASE = 240;
    var FREEZ = 244;
    var HOUSE = 249;
    var LHTHR = 249;
    var HHTHR = 260;
    var RZB = 265;
    var HOSPITALBASE = 405;
    var HOSPITAL = 409;
    var CHURCHBASE = 414;
    var CHURCH0BASE = 414;
    var CHURCH = 418;
    var CHURCH0 = 418;
    var COMBASE = 423;
    var COMCLR = 427;
    var CZB = 436;
    var COMLAST = 609;
    var INDBASE = 612;
    var INDCLR = 616;
    var LASTIND = 620;
    var IND1 = 621;
    var IZB = 625;
    var IND2 = 641;
    var IND3 = 644;
    var IND4 = 649;
    var IND5 = 650;
    var IND6 = 676;
    var IND7 = 677;
    var IND8 = 686;
    var IND9 = 689;
    var PORTBASE = 693;
    var PORT = 698;
    var LASTPORT = 708;
    var AIRPORTBASE = 709;
    var RADAR = 711;
    var AIRPORT = 716;
    var COALBASE = 745;
    var POWERPLANT = 750;
    var LASTPOWERPLANT = 760;
    var FIRESTBASE = 761;
    var FIRESTATION = 765;
    var POLICESTBASE = 770;
    var POLICESTATION = 774;
    var STADIUMBASE = 779;
    var STADIUM = 784;
    var FULLSTADIUM = 800;
    var NUCLEARBASE = 811;
    var NUCLEAR = 816;
    var LASTZONE = 826;
    var LIGHTNINGBOLT = 827;
    var HBRDG0 = 828;
    var HBRDG1 = 829;
    var HBRDG2 = 830;
    var HBRDG3 = 831;
    var HBRDG_END = 832;
    var RADAR0 = 832;
    var RADAR1 = 833;
    var RADAR2 = 834;
    var RADAR3 = 835;
    var RADAR4 = 836;
    var RADAR5 = 837;
    var RADAR6 = 838;
    var RADAR7 = 839;
    var FOUNTAIN = 840;
    var INDBASE2 = 844;
    var TELEBASE = 844;
    var TELELAST = 851;
    var SMOKEBASE = 852;
    var TINYEXP = 860;
    var SOMETINYEXP = 864;
    var LASTTINYEXP = 867;
    var TINYEXPLAST = 883;
    var COALSMOKE1 = 916;
    var COALSMOKE2 = 920;
    var COALSMOKE3 = 924;
    var COALSMOKE4 = 928;
    var FOOTBALLGAME1 = 932;
    var FOOTBALLGAME2 = 940;
    var VBRDG0 = 948;
    var VBRDG1 = 949;
    var VBRDG2 = 950;
    var VBRDG3 = 951;
    var NUKESWIRL1 = 952;
    var NUKESWIRL2 = 953;
    var NUKESWIRL3 = 954;
    var NUKESWIRL4 = 955;
    var CHURCH1BASE = 956;
    var CHURCH1 = 960;
    var CHURCH2BASE = 965;
    var CHURCH2 = 969;
    var CHURCH3BASE = 974;
    var CHURCH3 = 978;
    var CHURCH4BASE = 983;
    var CHURCH4 = 987;
    var CHURCH5BASE = 992;
    var CHURCH5 = 996;
    var CHURCH6BASE = 1001;
    var CHURCH6 = 1005;
    var CHURCH7BASE = 1010;
    var CHURCH7 = 1014;
    var CHURCH7LAST = 1018;
    var TILE_COUNT = 1024;
    var TILE_INVALID = -1;

    var MICROPOLIS_EDITION = 'Micropolis-JS';
    var MICROPOLIS_VERSION = '0.9';

    var SAVE_FILE_BYTES = 27120;

    var WORLD_W = 120;
    var WORLD_H = 100;

    var WORLD_W_2 = Math.floor((WORLD_W + 1) / 2);
    var WORLD_H_2 = Math.floor((WORLD_H + 1) / 2);

    var WORLD_W_4 = Math.floor((WORLD_W + 3) / 4);
    var WORLD_H_4 = Math.floor((WORLD_H + 3) / 4);

    var WORLD_W_8 = Math.floor((WORLD_W + 7) / 8);
    var WORLD_H_8 = Math.floor((WORLD_H + 7) / 8);

    var BITS_PER_TILE = 16;
    var BYTES_PER_TILE = 2;

    var EDITOR_TILE_SIZE = 16;

    var PASSES_PER_CITYTIME = 16;
    var CITYTIMES_PER_MONTH = 4;
    var CITYTIMES_PER_YEAR = CITYTIMES_PER_MONTH * 12;

    var HISTORY_LENGTH = 480;
    var MISC_HISTORY_LENGTH = 240;
    var HISTORY_COUNT = 120;

    var MAX_TRAFFIC_DISTANCE = 30;
    var MAX_ROAD_EFFECT = 32;

    var MAX_POLICE_STATION_EFFECT = 1000;
    var MAX_FIRE_STATION_EFFECT = 1000;

    var RES_VALVE_RANGE = 2000;
    var COM_VALVE_RANGE = 1500;
    var IND_VALVE_RANGE = 1500;

    var COAL_POWER_STRENGTH = 700;
    var NUCLEAR_POWER_STRENGTH = 2000;

    var CENSUS_FREQUENCY_10 = 4;
    var CENSUS_FREQUENCY_120 = CENSUS_FREQUENCY_10 * 12;

    var TAX_FREQUENCY = 48;

    var EXTERNAL_MARKET_INIT = 6.0;
    var EXTERNAL_MARKET_DEFAULT = 4.0;

    var LEVEL_FUNDS_EASY = 20000;
    var LEVEL_FUNDS_MEDIUM = 10000;
    var LEVEL_FUNDS_HARD = 5000;

    var VALID_CITY_NAME_CHARACTERS = 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_';

    var ISLAND_RADIUS = 18;

    var TRAIN_GROOVE_X = -39;
    var TRAIN_GROOVE_Y = 6;

    var BUS_GROOVE_X = -39;
    var BUS_GROOVE_Y = 6;

    var CONNECTTILE_FIX = 0;
    var CONNECTTILE_BULLDOZE = 1;
    var CONNECTTILE_ROAD = 2;
    var CONNECTTILE_RAILROAD = 3;
    var CONNECTTILE_WIRE = 4;

    var TOOL_RESIDENTIAL = 0;
    var TOOL_COMMERCIAL = 1;
    var TOOL_INDUSTRIAL = 2;
    var TOOL_FIRESTATION = 3;
    var TOOL_POLICESTATION = 4;
    var TOOL_QUERY = 5;
    var TOOL_WIRE = 6;
    var TOOL_BULLDOZER = 7;
    var TOOL_RAILROAD = 8;
    var TOOL_ROAD = 9;
    var TOOL_STADIUM = 10;
    var TOOL_PARK = 11;
    var TOOL_SEAPORT = 12;
    var TOOL_COALPOWER = 13;
    var TOOL_NUCLEARPOWER = 14;
    var TOOL_AIRPORT = 15;
    var TOOL_NETWORK = 16;
    var TOOL_WATER = 17;
    var TOOL_LAND = 18;
    var TOOL_FOREST = 19;
    var TOOL_COUNT = 20;
    var TOOL_FIRST = 0;
    var TOOL_LAST = 19;

    var TOOLRESULT_NO_MONEY = -2;
    var TOOLRESULT_NEED_BULLDOZE = -1;
    var TOOLRESULT_FAILED = 0;
    var TOOLRESULT_OK = 1;

    var HISTORY_TYPE_RES = 0;
    var HISTORY_TYPE_COM = 1;
    var HISTORY_TYPE_IND = 2;
    var HISTORY_TYPE_MONEY = 3;
    var HISTORY_TYPE_CRIME = 4;
    var HISTORY_TYPE_POLLUTION = 5;
    var HISTORY_TYPE_COUNT = 6;

    var HISTORY_SCALE_SHORT = 0;
    var HISTORY_SCALE_LONG = 1;
    var HISTORY_SCALE_COUNT = 2;

    var MAP_TYPE_ALL = 0;
    var MAP_TYPE_RES = 1;
    var MAP_TYPE_COM = 2;
    var MAP_TYPE_IND = 3;
    var MAP_TYPE_POWER = 4;
    var MAP_TYPE_ROAD = 5;
    var MAP_TYPE_POPULATION_DENSITY = 6;
    var MAP_TYPE_RATE_OF_GROWTH = 7;
    var MAP_TYPE_TRAFFIC_DENSITY = 8;
    var MAP_TYPE_POLLUTION = 9;
    var MAP_TYPE_CRIME = 10;
    var MAP_TYPE_LAND_VALUE = 11;
    var MAP_TYPE_FIRE_RADIUS = 12;
    var MAP_TYPE_POLICE_RADIUS = 13;
    var MAP_TYPE_DYNAMIC = 14;
    var MAP_TYPE_COUNT = 15;

    var SPRITE_NOTUSED = 0;
    var SPRITE_TRAIN = 1;
    var SPRITE_HELICOPTER = 2;
    var SPRITE_AIRPLANE = 3;
    var SPRITE_SHIP = 4;
    var SPRITE_MONSTER = 5;
    var SPRITE_TORNADO = 6;
    var SPRITE_EXPLOSION = 7;
    var SPRITE_BUS = 8;
    var SPRITE_COUNT = 9;

    var TILEMASK_ZONE = 0x0400;
    var TILEMASK_ANIMATED = 0x0800;
    var TILEMASK_BULLDOZABLE = 0x1000;
    var TILEMASK_BURNABLE = 0x2000;
    var TILEMASK_CONDUCTIVE = 0x4000;
    var TILEMASK_POWER = 0x8000;
    var TILEMASK_ALL = TILEMASK_ZONE | TILEMASK_ANIMATED | TILEMASK_BULLDOZABLE | TILEMASK_BURNABLE | TILEMASK_CONDUCTIVE | TILEMASK_POWER;
    var TILEMASK_TILE = 0x03ff;

    var SCENARIO_NONE = 0;
    var SCENARIO_DULLSVILLE = 1;
    var SCENARIO_SAN_FRANCISCO = 2;
    var SCENARIO_HAMBURG = 3;
    var SCENARIO_BERN = 4;
    var SCENARIO_TOKYO = 5;
    var SCENARIO_DETROIT = 6;
    var SCENARIO_BOSTON = 7;
    var SCENARIO_RIO = 8;
    var SCENARIO_COUNT = 9;

    var ZONE_COMMERCIAL = 0;
    var ZONE_INDUSTRIAL = 1;
    var ZONE_RESIDENTIAL = 2;
    var ZONE_NUM_DESTINATIONS = 3;

    var PROBLEM_CRIME = 0;
    var PROBLEM_POLLUTION = 1;
    var PROBLEM_HOUSING = 2;
    var PROBLEM_TAXES = 3;
    var PROBLEM_TRAFFIC = 4;
    var PROBLEM_UNEMPLOYMENT = 5;
    var PROBLEM_FIRE = 6;
    var PROBLEM_NUMPROBLEMS = 7;
    var PROBLEM_COUNT = 10;
    var PROBLEM_COMPLAINTS = 4;

    var CITYCLASS_VILLAGE = 0;
    var CITYCLASS_TOWN = 1;
    var CITYCLASS_CITY = 2;
    var CITYCLASS_CAPITAL = 3;
    var CITYCLASS_METROPOLIS = 4;
    var CITYCLASS_MEGALOPOLIS = 5;
    var CITYCLASS_NUM_CITIES = 6;

    var LEVEL_EASY = 0;
    var LEVEL_MEDIUM = 1;
    var LEVEL_HARD = 2;
    var LEVEL_COUNT = 3;
    var LEVEL_FIRST = 0;
    var LEVEL_LAST  = 2;

    var DIR_INVALID = 0;
    var DIR_NORTH = 1;
    var DIR_NORTH_EAST = 2;
    var DIR_EAST = 3;
    var DIR_SOUTH_EAST = 4;
    var DIR_SOUTH = 5;
    var DIR_SOUTH_WEST = 6;
    var DIR_WEST = 7;
    var DIR_NORTH_WEST = 8;
    var DIR_BEGIN = 1;
    var DIR_END = 9;

    var directionDX = [
        0,  // DIR_INVALID
        0,  // DIR_NORTH
        -1, // DIR_NORTH_EAST
        -1, // DIR_EAST
        -1, // DIR_SOUTH_EAST
        0,  // DIR_SOUTH
        1,  // DIR_SOUTH_WEST
        1,  // DIR_WEST
        1   // DIR_NORTH_WEST
    ];

    var directionDY = [
        0,  // DIR_INVALID
        -1, // DIR_NORTH
        -1, // DIR_NORTH_EAST
        0,  // DIR_EAST
        1,  // DIR_SOUTH_EAST
        1,  // DIR_SOUTH
        1,  // DIR_SOUTH_WEST
        0,  // DIR_WEST
        -1  // DIR_NORTH_WEST
    ];

    var editingToolCost = [
         100,    100,    100,    500, // res, com, ind, fire
         500,      0,      5,      1, // police, query, wire, bulldozer
          20,     10,   5000,     10, // rail, road, stadium, park
        3000,   3000,   5000,  10000, // seaport, coal, nuclear, airport
         100,      0,      0,      0  // network, water, land, forest
    ];

    var editingToolSize = [
        3, 3, 3, 3, // res, com, ind, fire
        3, 1, 1, 1, // police, query, wire, bulldozer
        1, 1, 4, 1, // rail, road, stadium, park
        4, 4, 4, 6, // seaport, coal, nuclear, airport
        1, 1, 1, 1  // network, water, land, forest
    ];

    var treeTable = [
        0,  0,  0,  34,
        0,  0,  36, 35,
        0,  32, 0,  33,
        30, 31, 29, 37
    ];

    var dx = [
        0,  1,  0, -1
    ];

    var dy = [
        -1,  0,  1,  0
    ];

    var speedPowerScan = [
        2, 4, 5
    ];

    var speedPollutionTerrainLandValueScan = [
        2, 7, 17
    ];

    var speedCrimeScan = [
        1, 8, 18
    ];

    var speedPopulationDensityScan = [
        1, 9, 19
    ];

    var speedFireAnalysis = [
        1, 10, 20
    ];

    // Bigger numbers reduce chance of nuclear melt down.
    var meltDownTable = [
        30000, 20000, 10000
    ];

    // Disaster delay table for each scenario
    var disasterWaitTable = [
        0,          // No scenario (free playing)
        2,          // Dullsville (boredom)
        10,         // San francisco (earth quake)
        4 * 10,     // Hamburg (fire bombs)
        20,         // Bern (traffic)
        3,          // Tokyo (scary monster)
        5,          // Detroit (crime)
        5,          // Boston (nuclear meltdown)
        2 * 48      // Rio (flooding)
    ];

    // Time to wait before score calculation for each scenario
    var scoreWaitTable = [
        0,          // No scenario (free playing)
        30 * 48,    // Dullsville (boredom)
        5 * 48,     // San francisco (earth quake)
        5 * 48,     // Hamburg (fire bombs)
        10 * 48,    // Bern (traffic)
        5 * 48,     // Tokyo (scary monster)
        10 * 48,    // Detroit (crime)
        5 * 48,     // Boston (nuclear meltdown)
        10 * 48     // Rio (flooding)
    ];

    var taxTable = [
        200, 150, 120, 100, 80, 50, 30, 0, -10, -40, -100,
        -150, -200, -250, -300, -350, -400, -450, -500, -550, -600
    ];

    var externalMarketParamTable = [
        1.2, 1.1, 0.98
    ];

    var rLevels = [
        0.7, 0.9, 1.2
    ];

    var fLevels = [
        1.4, 1.2, 0.8
    ];

    var coalSmokeDX = [
        1, 2, 1, 2
    ];

    var coalSmokeDY = [
        -1, -1, 0, 0
    ];

    var directionTable = [
        0, 3, 2, 1, 3, 4, 5, 7, 6, 5, 7, 8, 1
    ];

    var hDX = [
        -2, 2, -2, -1, 0, 1, 2
    ];

    var hDY = [
        -1, -1, 0, 0, 0, 0, 0
    ];

    var vDX = [
        0, 1, 0, 0, 0, 0, 1
    ];

    var vDY = [
        -2, -2, -1, 0, 1, 2, 2
    ];

    var animatedTiles = [
        0,    1,    2,    3,    4,    5,    6,    7,    8,    9,    10,   11,   12,   13,   14,   15,
        16,   17,   18,   19,   20,   21,   22,   23,   24,   25,   26,   27,   28,   29,   30,   31,
        32,   33,   34,   35,   36,   37,   38,   39,   40,   41,   42,   43,   44,   45,   46,   47,
        48,   49,   50,   51,   52,   53,   54,   55,
        // Fire
                                                        57,   58,   59,   60,   61,   62,   63,   56,
        // No Traffic
        64,   65,   66,   67,   68,   69,   70,   71,   72,   73,   74,   75,   76,   77,   78,   79,
        // Light Traffic
        128,  129,  130,  131,  132,  133,  134,  135,  136,  137,  138,  139,  140,  141,  142,  143,
        80,   81,   82,   83,   84,   85,   86,   87,   88,   89,   90,   91,   92,   93,   94,   95,
        96,   97,   98,   99,   100,  101,  102,  103,  104,  105,  106,  107,  108,  109,  110,  111,
        112,  113,  114,  115,  116,  117,  118,  119,  120,  121,  122,  123,  124,  125,  126,  127,
        // Heavy Traffic
        192,  193,  194,  195,  196,  197,  198,  199,  200,  201,  202,  203,  204,  205,  206,  207,
        144,  145,  146,  147,  148,  149,  150,  151,  152,  153,  154,  155,  156,  157,  158,  159,
        160,  161,  162,  163,  164,  165,  166,  167,  168,  169,  170,  171,  172,  173,  174,  175,
        176,  177,  178,  179,  180,  181,  182,  183,  184,  185,  186,  187,  188,  189,  190,  191,
        // Wires and Rails
        208,  209,  210,  211,  212,  213,  214,  215,  216,  217,  218,  219,  220,  221,  222,  223,
        224,  225,  226,  227,  228,  229,  230,  231,  232,  233,  234,  235,  236,  237,  238,  239,
        // Residential
        240,  241,  242,  243,  244,  245,  246,  247,  248,  249,  250,  251,  252,  253,  254,  255,
        256,  257,  258,  259,  260,  261,  262,  263,  264,  265,  266,  267,  268,  269,  270,  271,
        272,  273,  274,  275,  276,  277,  278,  279,  280,  281,  282,  283,  284,  285,  286,  287,
        288,  289,  290,  291,  292,  293,  294,  295,  296,  297,  298,  299,  300,  301,  302,  303,
        304,  305,  306,  307,  308,  309,  310,  311,  312,  313,  314,  315,  316,  317,  318,  319,
        320,  321,  322,  323,  324,  325,  326,  327,  328,  329,  330,  331,  332,  333,  334,  335,
        336,  337,  338,  339,  340,  341,  342,  343,  344,  345,  346,  347,  348,  349,  350,  351,
        352,  353,  354,  355,  356,  357,  358,  359,  360,  361,  362,  363,  364,  365,  366,  367,
        368,  369,  370,  371,  372,  373,  374,  375,  376,  377,  378,  379,  380,  381,  382,  383,
        384,  385,  386,  387,  388,  389,  390,  391,  392,  393,  394,  395,  396,  397,  398,  399,
        400,  401,  402,  403,  404,  405,  406,  407,  408,  409,  410,  411,  412,  413,  414,  415,
        416,  417,  418,  419,  420,  421,  422,
        // Commercial
                                                  423,  424,  425,  426,  427,  428,  429,  430,  431,
        432,  433,  434,  435,  436,  437,  438,  439,  440,  441,  442,  443,  444,  445,  446,  447,
        448,  449,  450,  451,  452,  453,  454,  455,  456,  457,  458,  459,  460,  461,  462,  463,
        464,  465,  466,  467,  468,  469,  470,  471,  472,  473,  474,  475,  476,  477,  478,  479,
        480,  481,  482,  483,  484,  485,  486,  487,  488,  489,  490,  491,  492,  493,  494,  495,
        496,  497,  498,  499,  500,  501,  502,  503,  504,  505,  506,  507,  508,  509,  510,  511,
        512,  513,  514,  515,  516,  517,  518,  519,  520,  521,  522,  523,  524,  525,  526,  527,
        528,  529,  530,  531,  532,  533,  534,  535,  536,  537,  538,  539,  540,  541,  542,  543,
        544,  545,  546,  547,  548,  549,  550,  551,  552,  553,  554,  555,  556,  557,  558,  559,
        560,  561,  562,  563,  564,  565,  566,  567,  568,  569,  570,  571,  572,  573,  574,  575,
        576,  577,  578,  579,  580,  581,  582,  583,  584,  585,  586,  587,  588,  589,  590,  591,
        592,  593,  594,  595,  596,  597,  598,  599,  600,  601,  602,  603,  604,  605,  606,  607,
        608,  609,  610,  611,
        // Industrial
                                612,  613,  614,  615,  616,  617,  618,  619,  620,  852,  622,  623,
        624,  625,  626,  627,  628,  629,  630,  631,  632,  633,  634,  635,  636,  637,  638,  639,
        640,  884,  642,  643,  888,  645,  646,  647,  648,  892,  896,  651,  652,  653,  654,  655,
        656,  657,  658,  659,  660,  661,  662,  663,  664,  665,  666,  667,  668,  669,  670,  671,
        672,  673,  674,  675,  900,  904,  678,  679,  680,  681,  682,  683,  684,  685,  908,  687,
        688,  912,  690,  691,  692,
        // SeaPort
                                      693,  694,  695,  696,  697,  698,  699,  700,  701,  702,  703,
        704,  705,  706,  707,  708,
        // AirPort
                        // 832 was previous value of 711, to start radar
                        // animation, but now we break the link and the
                        // simulator switches the tiles.
                                      709,  710,  711,  712,  713,  714,  715,  716,  717,  718,  719,
        720,  721,  722,  723,  724,  725,  726,  727,  728,  729,  730,  731,  732,  733,  734,  735,
        736,  737,  738,  739,  740,  741,  742,  743,  744,
        // Coal Power
                                                              745,  746,  916,  920,  749,  750,  924,
        928,  753,  754,  755,  756,  757,  758,  759,  760,
        // Fire Dept
                                                              761,  762,  763,  764,  765,  766,  767,
        768,  769,  770,  771,  772,  773,  774,  775,  776,  777,  778,
        // Stadium
                                                                          779,  780,  781,  782,  783,
        784,  785,  786,  787,  788,  789,  790,  791,  792,  793,  794,
        // Stadium Anims
                                                                          795,  796,  797,  798,  799,
        800,  801,  802,  803,  804,  805,  806,  807,  808,  809,  810,
        // Nuclear Power
                                                                          811,  812,  813,  814,  815,
        816,  817,  818,  819,  952,  821,  822,  823,  824,  825,  826,
        // Power Out + Bridges
                                                                          827,  828,  829,  830,  831,
        //  Radar  dish
        833,  834,  835,  836,  837,  838,  839,  832,
        // Fountain / Flag
                                                        841,  842,  843,  840,  845,  846,  847,  848,
        849,  850,  851,  844,  853,  854,  855,  856,  857,  858,  859,  852,
        // Zone Destruct and Rubblize
                                                                                861,  862,  863,  864,
        865,  866,  867,  867,
        // Totally Unsure
                                868,  869,  870,  871,  872,  873,  874,  875,  876,  877,  878,  879,
        880,  881,  882,  883,
        // Smoke Stacks
                                885,  886,  887,  884,  889,  890,  891,  888,  893,  894,  895,  892,
        897,  898,  899,  896,  901,  902,  903,  900,  905,  906,  907,  904,  909,  910,  911,  908,
        913,  914,  915,  912,  917,  918,  919,  916,  921,  922,  923,  920,  925,  926,  927,  924,
        929,  930,  931,  928,
        // Stadium Playfield
                                933,  934,  935,  936,  937,  938,  939,  932,  941,  942,  943,  944,
        945,  946,  947,  940,
        // Bridge Up
                                948,  949,  950,  951,
        // Nuclear Swirl
                                                        953,  954,  955,  952,
        // Churches
                                                                                956,  957,  958,  959,
        960,  961,  962,  963,  964,  965,  966,  967,  968,  969,  970,  971,  972,  973,  974,  975, 
        976,  977,  978,  979,  980,  981,  982,  983,  984,  985,  986,  987,  988,  989,  990,  991, 
        992,  993,  994,  995,  996,  997,  998,  999,  1000, 1001, 1002, 1003, 1004, 1005, 1006, 1007, 
        1008, 1009, 1010, 1011, 1012, 1013, 1014, 1015, 1016, 1017, 1018, 1019, 1020, 1021, 1022, 1023
    ];


    var roadTable = [
        ROADS,      ROADS2,     ROADS,      ROADS3,
        ROADS2,     ROADS2,     ROADS4,     ROADS8,
        ROADS,      ROADS6,     ROADS,      ROADS7,
        ROADS5,     ROADS10,    ROADS9,     INTERSECTION
    ];


    var railTable = [
        LHRAIL,     LVRAIL,     LHRAIL,     LVRAIL2,
        LVRAIL,     LVRAIL,     LVRAIL3,    LVRAIL7,
        LHRAIL,     LVRAIL5,    LHRAIL,     LVRAIL6,
        LVRAIL4,    LVRAIL9,    LVRAIL8,    LVRAIL10
    ];


    var wireTable = [
        LHPOWER,    LVPOWER,    LHPOWER,    LVPOWER2,
        LVPOWER,    LVPOWER,    LVPOWER3,   LVPOWER7,
        LHPOWER,    LVPOWER5,   LHPOWER,    LVPOWER6,
        LVPOWER4,   LVPOWER9,   LVPOWER8,   LVPOWER10
    ];


    var bigRiverMatrix = [
        [ 0,          0,          0,          REDGE,      REDGE,      REDGE,      0,          0,          0      ],
        [ 0,          0,          REDGE,      RIVER,      RIVER,      RIVER,      REDGE,      0,          0      ],
        [ 0,          REDGE,      RIVER,      RIVER,      RIVER,      RIVER,      RIVER,      REDGE,      0      ],
        [ REDGE,      RIVER,      RIVER,      RIVER,      RIVER,      RIVER,      RIVER,      RIVER,     REDGE   ],
        [ REDGE,      RIVER,      RIVER,      RIVER,      CHANNEL,    RIVER,      RIVER,      RIVER,     REDGE   ],
        [ REDGE,      RIVER,      RIVER,      RIVER,      RIVER,      RIVER,      RIVER,      RIVER,     REDGE   ],
        [ 0,          REDGE,      RIVER,      RIVER,      RIVER,      RIVER,      RIVER,      REDGE,     0       ],
        [ 0,          0,          REDGE,      RIVER,      RIVER,      RIVER,      REDGE,          0,     0       ],
        [ 0,          0,          0,          REDGE,      REDGE,      REDGE,          0,          0,     0       ]
    ];


    var smallRiverMatrix = [
        [       0,          0,      REDGE,      REDGE,          0,        0 ],
        [       0,      REDGE,      RIVER,      RIVER,      REDGE,        0 ],
        [   REDGE,      RIVER,      RIVER,      RIVER,      RIVER,        REDGE ],
        [   REDGE,      RIVER,      RIVER,      RIVER,      RIVER,        REDGE ],
        [       0,      REDGE,      RIVER,      RIVER,      REDGE,        0 ],
        [       0,          0,      REDGE,      REDGE,          0,
                0 ]
    ];


    var densityTable = [
        ROADBASE, LTRFBASE, HTRFBASE
    ];


    var hBridgeTable = [
        HBRDG1 | TILEMASK_BULLDOZABLE,
        HBRDG3 | TILEMASK_BULLDOZABLE,
        HBRDG0 | TILEMASK_BULLDOZABLE,
        RIVER,
        BRWH | TILEMASK_BULLDOZABLE,
        RIVER,
        HBRDG2 | TILEMASK_BULLDOZABLE
    ];



    var hBridgeTable2 = [
        RIVER,
        RIVER,
        HBRIDGE | TILEMASK_BULLDOZABLE,
        HBRIDGE | TILEMASK_BULLDOZABLE,
        HBRIDGE | TILEMASK_BULLDOZABLE,
        HBRIDGE | TILEMASK_BULLDOZABLE,
        HBRIDGE | TILEMASK_BULLDOZABLE
    ];


    var vBridgeTable = [
        VBRDG0 | TILEMASK_BULLDOZABLE,
        VBRDG1 | TILEMASK_BULLDOZABLE,
        RIVER,
        BRWV | TILEMASK_BULLDOZABLE,
        RIVER,
        VBRDG2 | TILEMASK_BULLDOZABLE,
        VBRDG3 | TILEMASK_BULLDOZABLE
    ];


    var vBridgeTable2 = [
        VBRIDGE | TILEMASK_BULLDOZABLE,
        RIVER,
        VBRIDGE | TILEMASK_BULLDOZABLE,
        VBRIDGE | TILEMASK_BULLDOZABLE,
        VBRIDGE | TILEMASK_BULLDOZABLE,
        VBRIDGE | TILEMASK_BULLDOZABLE,
        RIVER
    ];


    var coalSmokeTable = [
        COALSMOKE1, COALSMOKE2, COALSMOKE3, COALSMOKE4
    ];


    // Clear, Water, Trees, Rubble,
    // Flood, Radioactive Waste, Fire, Road,
    // Power, Rail, Residential, Commercial,
    // Industrial, Port, AirPort, Coal Power,
    // Fire Department, Police Department, Stadium, Nuclear Power,
    // Draw Bridge, Radar Dish, Fountain, Industrial,
    // 49er's 38  Bears 3, Draw Bridge, Ur 238, Unknown
    var statusZones = [
        DIRT, RIVER, TREEBASE, RUBBLE,
        FLOOD, RADTILE, FIRE, ROADBASE,
        POWERBASE, RAILBASE, RESBASE, COMBASE,
        INDBASE, PORTBASE, AIRPORTBASE, COALBASE,
        FIRESTBASE, POLICESTBASE, STADIUMBASE, NUCLEARBASE,
        // FIXME: I think HBRDG_END should be HBRDG0...?
        HBRDG0, RADAR0, FOUNTAIN, INDBASE2,
        FOOTBALLGAME1, VBRDG0, NUKESWIRL1, CHURCH1BASE,
        9999 // a huge number
    ];


    var residentialZoneBuilding =
        new BuildingProperties(
            3,
            3,
            RESBASE,
            TOOL_RESIDENTIAL,
            'Res',
            false);

    var commercialZoneBuilding =
        new BuildingProperties(
            3,
            3,
            COMBASE,
            TOOL_COMMERCIAL,
            'Com',
            false);

    var industrialZoneBuilding =
        new BuildingProperties(
            3,
            3,
            INDBASE,
            TOOL_INDUSTRIAL,
            'Ind',
            false);

    var policeStationBuilding =
        new BuildingProperties(
            3,
            3,
            POLICESTBASE,
            TOOL_POLICESTATION,
            'Pol',
            false);

    var fireStationBuilding =
        new BuildingProperties(
            3,
            3,
            FIRESTBASE,
            TOOL_FIRESTATION,
            'Fire',
            false);

    var stadiumBuilding =
        new BuildingProperties(
            4,
            4,
            STADIUMBASE,
            TOOL_STADIUM,
            'Stad',
            false);

    var coalPowerBuilding =
        new BuildingProperties(
            4,
            4,
            COALBASE,
            TOOL_COALPOWER,
            'Coal',
            false);

    var nuclearPowerBuilding =
        new BuildingProperties(
            4,
            4,
            NUCLEARBASE,
            TOOL_NUCLEARPOWER,
            'Nuc',
            true);

    var seaportBuilding =
        new BuildingProperties(
            4,
            4,
            PORTBASE,
            TOOL_SEAPORT,
            'Seap',
            false);

    var airportBuilding =
        new BuildingProperties(
            6,
            6,
            AIRPORTBASE,
            TOOL_AIRPORT,
            'Airp',
            false);


    ////////////////////////////////////////////////////////////////////////
    // Methods.


    p.init = function init() {

        this.roadTotal = 0;
        this.railTotal = 0;
        this.firePop = 0;
        this.resPop = 0;
        this.comPop = 0;
        this.indPop = 0;
        this.totalPop = 0;
        this.totalPopLast = 0;
        this.resZonePop = 0;
        this.comZonePop = 0;
        this.indZonePop = 0;
        this.totalZonePop = 0;
        this.hospitalPop = 0;
        this.churchPop = 0;
        this.faith = 0;
        this.stadiumPop = 0;
        this.policeStationPop = 0;
        this.fireStationPop = 0;
        this.coalPowerPop = 0;
        this.nuclearPowerPop = 0;
        this.seaportPop = 0;
        this.airportPop = 0;
        this.crimeAverage = 0;
        this.pollutionAverage = 0;
        this.landValueAverage = 0;
        this.cityTime = 0;
        this.cityMonth = 0;
        this.cityYear = 0;
        this.startingYear = 0;
        this.resHist10Max = 0;
        this.resHist120Max = 0;
        this.comHist10Max = 0;
        this.comHist120Max = 0;
        this.indHist10Max = 0;
        this.indHist120Max = 0;
        this.censusChanged = false;
        this.roadSpend = 0;
        this.policeSpend = 0;
        this.fireSpend = 0;
        this.roadFund = 0;
        this.policeFund = 0;
        this.fireFund = 0;
        this.roadEffect = 0;
        this.policeEffect = 0;
        this.fireEffect = 0;
        this.taxFund = 0;
        this.cityTax = 0;
        this.taxFlag = 0;
        this.needHospital = 0;
        this.needChurch = 0;
        this.mapBase = null;
        this.roadPercent = 0.0;
        this.policePercent = 0.0;
        this.firePercent = 0.0;
        this.roadValue = 0;
        this.policeValue = 0;
        this.fireValue = 0;
        this.mustDrawBudget = false;
        this.floodCount = 0;
        this.cityYes = 0;
        this.cityPop = 0;
        this.cityPopDelta = 0;
        this.cityAssessedValue = 0;
        this.cityClass = 0;
        this.cityScore = 0;
        this.cityScoreDelta = 0;
        this.trafficAverage = 0;
        this.terrainTreeLevel = 0;
        this.terrainLakeLevel = 0;
        this.terrainCurveLevel = 0;
        this.terrainCreateIsland = 0;
        this.generatedMapSeed = 0;
        this.historyInitialized = 0;
        this.graph10Max = 0;
        this.graph120Max = 0;
        this.simLoops = 0;
        this.simPasses = 0;
        this.simPass = 0;
        this.simPaused = false;
        this.simPausedSpeed = 0;
        this.tickTimer = null;
        this.tickDelay = 100;
        this.tilesAnimated = true;
        this.doAnimation = true;
        this.doMessages = false;
        this.doNotices = true;
        this.cityFileName = '';
        this.cityName = '';
        this.citySaveData = null;
        this.cityPopLast = 0;
        this.categoryLast = null;
        this.autoGoto = false;
        this.powerStack = [];
        this.nextRandom = 0;
        this.newMap = false;
        this.cityCenterX = 0;
        this.cityCenterY = 0;
        this.pollutionMaxX = 0;
        this.pollutionMaxY = 0;
        this.crimeMaxX = 0;
        this.crimeMaxY = 0;
        this.donDither = 0;
        this.valveFlag = false;
        this.crimeRamp = 0;
        this.pollutionRamp = 0;
        this.resCap = false;
        this.comCap = false;
        this.indCap = false;
        this.cashFlow = 0;
        this.externalMarket = 0;
        this.disasterEvent = SCENARIO_NONE;
        this.disasterWait = 0;
        this.scoreType = SCENARIO_NONE;
        this.scoreWait = 0;
        this.poweredZoneCount = 0;
        this.unpoweredZoneCount = 0;
        this.newPower = false;
        this.cityTaxAverage = 0;
        this.simCycle = 0;
        this.phaseCycle = 0;
        this.speedCycle = 0;
        this.doInitialEval = false;
        this.mapSerial = 0;
        this.resValve = 0;
        this.comValve = 0;
        this.indValve = 0;
        this.spriteList = null;
        this.absDist = 0;
        this.spriteCycle = 0;
        this.totalFunds = 0;
        this.autoBulldoze = true;
        this.autoBudget = true;
        this.messageTimeLast = 0;
        this.gameLevel = this.LEVEL_EASY;
        this.initSimLoad = 0;
        this.scenario = SCENARIO_NONE;
        this.simSpeed = 0;
        this.simSpeedMeta = 0;
        this.enableSound = true;
        this.enableDisasters = true;
        this.messageNumber = 0;
        this.evalChanged = false;
        this.callbackHook = null;
        this.callbackData = null;
        this.userData = null;
        this.curMapStack = [];
        this.trafficMaxX = 0;
        this.trafficMaxY = 0;
        this.mustUpdateFunds = false;
        this.mustUpdateOptions = false;
        this.cityTimeLast = 0;
        this.cityYearLast = 0;
        this.cityMonthLast = 0;
        this.totalFundsLast = 0;
        this.fresLast = 0;
        this.comLast = 0;
        this.indLast = 0;
        this.errorMessage = '';
        this.resources = {};
        this.problemVotes =
            this.makeArray_Uint16(
                PROBLEM_COUNT);
        this.problemOrder =
            this.makeArray_Uint16(
                PROBLEM_COMPLAINTS);
        this.newMapFlags =
            this.makeArray_Uint16(
                MAP_TYPE_COUNT);
        this.globalSprites =
            new Array(SPRITE_COUNT);

        this.loadResources();
        this.initMapArrays();
        this.simInit();

    };


    p.initMapArrays = function initMapArrays() {

        this.map =
            Map.makeMap_Uint16_1x1(
                WORLD_W,
                WORLD_H);

        this.populationDensityMap = 
            Map.makeMap_Uint8_2x2(
                WORLD_W,
                WORLD_H);

        this.trafficDensityMap = 
            Map.makeMap_Uint8_2x2(
                WORLD_W,
                WORLD_H);

        this.pollutionDensityMap = 
            Map.makeMap_Uint8_2x2(
                WORLD_W,
                WORLD_H);

        this.landValueMap = 
            Map.makeMap_Uint8_2x2(
                WORLD_W,
                WORLD_H);

        this.crimeRateMap = 
            Map.makeMap_Uint8_2x2(
                WORLD_W,
                WORLD_H);

        this.terrainDensityMap = 
            Map.makeMap_Uint8_4x4(
                WORLD_W,
                WORLD_H);

        this.tempMap1 = 
            Map.makeMap_Uint8_2x2(
                WORLD_W,
                WORLD_H);

        this.tempMap2 = 
            Map.makeMap_Uint8_2x2(
                WORLD_W,
                WORLD_H);

        this.tempMap3 = 
            Map.makeMap_Uint8_4x4(
                WORLD_W,
                WORLD_H);

        this.tempMap4 = 
            Map.makeMap_Uint16_8x8(
                WORLD_W,
                WORLD_H);

        this.powerGridMap = 
            Map.makeMap_Uint8_1x1(
                WORLD_W,
                WORLD_H);

        this.rateOfGrowthMap = 
            Map.makeMap_Int16_8x8(
                WORLD_W,
                WORLD_H);

        this.fireStationMap = 
            Map.makeMap_Uint16_8x8(
                WORLD_W,
                WORLD_H);

        this.fireStationEffectMap = 
            Map.makeMap_Uint16_8x8(
                WORLD_W,
                WORLD_H);

        this.policeStationMap = 
            Map.makeMap_Uint16_8x8(
                WORLD_W,
                WORLD_H);

        this.policeStationEffectMap = 
            Map.makeMap_Uint16_8x8(
                WORLD_W,
                WORLD_H);

        this.comRateMap = 
            Map.makeMap_Uint16_8x8(
                WORLD_W,
                WORLD_H);

        this.resHist =
            this.makeArray_Uint16(
                HISTORY_LENGTH);

        this.comHist =
            this.makeArray_Uint16(
                HISTORY_LENGTH);

        this.indHist =
            this.makeArray_Uint16(
                HISTORY_LENGTH);

        this.moneyHist =
            this.makeArray_Uint16(
                HISTORY_LENGTH);

        this.pollutionHist =
            this.makeArray_Uint16(
                HISTORY_LENGTH);

        this.crimeHist =
            this.makeArray_Uint16(
                HISTORY_LENGTH);

        this.miscHist =
            this.makeArray_Uint16(
                HISTORY_LENGTH);

        dynamicData =
            this.makeArray_Uint32(
                MAP_TYPE_COUNT);

        this.spriteList = [];
    };


    p.simInit = function simInit() {

        this.mustUpdateOptions = true; // Update options displayed at user
        this.scenario = SCENARIO_NONE;
        this.startingYear = 1900;
        this.simPasses = 1;
        this.simPass = 0;
        this.cityTime = 50;
        this.setAutoBulldoze(true);
        this.setAutoBudget(true);
        this.setAutoGoto(true);
        this.setEnableSound(true);
        this.setEnableDisasters(true);
        this.simSpeed = 3;
        this.simPaused = true;
        this.simPausedSpeed = 3;
        this.simLoops = 0;
        this.initSimLoad = 2;
        this.changeEval();
        this.initMapArrays();
        this.initGraphs();
        this.initFundingLevel();
        this.doResetMapState();
        this.doResetEditorState();
        this.clearMap();
        this.initWillStuff();
        this.setCityTax(7);
        this.setFunds(5000);
        this.setGameLevelFunds(LEVEL_EASY);
        this.setPasses(1);

    };


    p.destroy = function destroy() {
    };


    ////////////////////////////////////////////////////////////////////////
    // Utilities


    p.clamp = function clamp(val, lower, upper)
    {
        if (val < lower) {
            return lower;
        }

        if (val > upper) {
            return upper;
        }

        return val;
    };


    p.makeArray_Uint8 = function makeArray_Uint8(count)
    {
        return new Uint8Array(
            new ArrayBuffer(
                count));
    };


    p.makeArray_Uint16 = function makeArray_Uint16(count)
    {
        return new Uint16Array(
            new ArrayBuffer(
                count << 1));
    };


    p.makeArray_Uint32 = function makeArray_Uint32(count)
    {
        return new Uint32Array(
            new ArrayBuffer(
                count << 2));
    };


    p.animateTiles = function animateTiles() {

        for (var y = 0;
             y < WORLD_H;
             y++) {

            for (var x = 0;
                 x < WORLD_W;
                 x++) {

                var tile = this.getTile(x, y);
                if (tile & TILEMASK_ANIMATED) {
                    var flags = tile & TILEMASK_ALL;
                    tile &= TILEMASK_TILE;
                    tile = animatedTiles[tile];
                    tile |= flags;
                    this.setTile(x, y);
                }

            }

        }

    };


    p.initFundingLevel = function initFundingLevel() {
        this.firePercent = 1.0;
        this.fireValue = 0;
        this.policePercent = 1.0;
        this.policeValue = 0;
        this.roadPercent = 1.0;
        this.roadValue = 0;
        this.mustDrawBudget = true;
    };


    p.doBudget = function doBudget() {
        this.doBudgetNow(false);
    };


    p.doBudgetFromMenu = function doBudgetFromMenu() {
        this.doBudgetNow(true);
    };


    p.doBudgetNow = function doBudgetNow(fromMenu) {

        var fireInt = Math.floor(this.fireFund * this.firePercent);
        var policeInt = Math.floor(this.policeFund * this.policePercent);
        var roadInt = Math.floor(this.roadFund * this.roadPercent);

        var total = fireInt + policeInt + roadInt;
        var yumDuckets = this.taxFund + this.totalFunds;
        var moreDough = 0;

        if (yumDuckets > total) {

            // Enough yumDuckets to fully fund fire, police and road.

            this.fireValue = fireInt;
            this.policeValue = policeInt;
            this.roadValue = roadInt;

            /// @todo Why are we not subtracting from yumDuckets what we
            /// spend, like the code below is doing?

        } else if (total > 0) {

            // Not enough yumDuckets to fund everything.
            // First spend on roads, then on fire, then on police.

            if (yumDuckets > roadInt) {

                // Enough yumDuckets to fully fund roads.

                this.roadValue = roadInt;
                yumDuckets -= roadInt;

                if (yumDuckets > fireInt) {

                    // Enough yumDuckets to fully fund fire.

                    this.fireValue = fireInt;
                    yumDuckets -= fireInt;

                    if (yumDuckets > policeInt) {

                        // Enough yumDuckets to fully fund police.
                        // Hey what are we doing here? Should never get here.
                        // We tested for yumDuckets > total above
                        // (where total = fireInt + policeInt + roadInt),
                        // so this should never happen.

                        this.policeValue = policeInt;
                        yumDuckets -= policeInt;

                    } else {

                        // Fuly funded roads and fire.
                        // Partially fund police.

                        this.policeValue = yumDuckets;

                        if (yumDuckets > 0) {

                            // Scale back police percentage to available cash.

                            this.policePercent = yumDuckets / this.policeFund;

                        } else {

                            // Exactly nothing left, so scale back police percentage to zero.

                            this.policePercent = 0.0;

                        }

                    }

                } else {

                    // Not enough yumDuckets to fully fund fire.

                    this.fireValue = yumDuckets;

                    // No police after funding roads and fire.

                    this.policeValue = 0;
                    this.policePercent = 0.0;

                    if (yumDuckets > 0) {

                        // Scale back fire percentage to available cash.

                        this.firePercent =
                            yumDuckets / this.fireFund;

                    } else {

                        // Exactly nothing left, so scale back fire percentage to zero.

                        this.firePercent = 0.0;

                    }

                }

            } else {

                // Not enough yumDuckets to fully fund roads.

                this.roadValue = yumDuckets;

                // No fire or police after funding roads.

                this.fireValue = 0;
                this.policeValue = 0;
                this.firePercent = 0.0;
                this.policePercent = 0.0;

                if (yumDuckets > 0) {

                    // Scale back road percentage to available cash.

                    this.roadPercent = yumDuckets / this.roadFund;

                } else {

                    // Exactly nothing left, so scale back road percentage to zero.

                    this.roadPercent = 0.0;

                }

            }

        } else {

            // Zero funding, so no values but full percentages.

            this.fireValue = 0;
            this.policeValue = 0;
            this.roadValue = 0;
            this.firePercent = 1.0;
            this.policePercent = 1.0;
            this.roadPercent = 1.0;

        }

        if (!this.autoBudget || fromMenu) {

            if (!fromMenu) {

                this.fireSpend = this.fireValue;
                this.policeSpend = this.policeValue;
                this.roadSpend = this.roadValue;

                total = this.fireSpend + this.policeSpend + this.roadSpend;

                moreDough = this.taxFund - total;
                this.spend(-moreDough);

            }

            this.mustDrawBudget = true;
            this.doUpdateHeads();

        } else { // autoBudget & !fromMenu

            // FIXME: Not sure yumDuckets is the right value here. It gets the
            // amount spent subtracted from it above in some cases, but not if
            // we are fully funded. I think we want to use the original value
            // of yumDuckets, which is taxFund + totalFunds.

            if (yumDuckets > total) {

                moreDough = this.taxFund - total;
                this.spend(-moreDough);

                this.fireSpend = this.fireFund;
                this.policeSpend = this.policeFund;
                this.roadSpend = this.roadFund;

                this.mustDrawBudget = true;
                this.doUpdateHeads();

            } else {

                this.setAutoBudget(false); // Force autobudget.
                this.mustUpdateOptions = true;
                this.sendMessage('NO_MONEY', null, null, false, true);

                if (!fromMenu) {

                    this.fireSpend = this.fireValue;
                    this.policeSpend = this.policeValue;
                    this.roadSpend = this.roadValue;

                    total = this.fireSpend + this.policeSpend + this.roadSpend;

                    moreDough = this.taxFund - total;
                    this.spend(-moreDough);

                }

                this.mustDrawBudget = true;
                this.doUpdateHeads();

            }

        }

    };


    p.updateBudget = function updateBudget() {

        if (this.mustDrawBudget) {
            this.doUpdateBudget();
            this.mustDrawBudget = false;
        }

    };


    p.setCityTax = function setCityTax(tax) {

        this.cityTax = tax;
        this.doUpdateTaxRate();

    };


    p.neutralizeRoad = function neutralizeRoad(tile) {

        if (tile >= 64 && tile <= 207) {
            tile = (tile & 0x000F) + 64;
        }

        return tile;

    };


    p.connectTile = function connectTile(x, y, cmd, effects) {

        var result = TOOLRESULT_OK;

        // Make sure the array subscripts are in bounds.
        if (!this.testBounds(x, y)) {
            return TOOLRESULT_FAILED;
        }

        // Perform auto-doze if appropriate.
        switch (cmd) {

        case CONNECTTILE_ROAD:
        case CONNECTTILE_RAILROAD:
        case CONNECTTILE_WIRE:

            // Silently skip auto-bulldoze if no money.
            if (this.autoBulldoze) {

                var mapVal = effects.getTile(x, y);

                if (mapVal & TILEMASK_BULLDOZABLE) {
                    mapVal &= TILEMASK_TILE;
                    mapVal = this.neutralizeRoad(mapVal);

                    // Maybe this should check TILEMASK_BULLDOZABLE instead of checking tile values?
                    if (((mapVal >= TINYEXP) &&
                         (mapVal <= LASTTINYEXP)) ||
                        ((mapVal < HBRIDGE) &&
                         (mapVal !== DIRT))) {

                        effects.addCost(1);

                        effects.setTile(x, y, DIRT);

                    }
                }
            }
            break;

        default:
            // Do nothing.
            break;

        }

        // Perform the command.
        switch (cmd) {

        case CONNECTTILE_FIX: // Fix zone.
            this.fixZone(x, y, effects);
            break;

        case CONNECTTILE_BULLDOZE: // Bulldoze zone.
            result = this.layDoze(x, y, effects);
            this.fixZone(x, y, effects);
            break;

        case CONNECTTILE_ROAD: // Lay road.
            result = this.layRoad(x, y, effects);
            this.fixZone(x, y, effects);
            break;

        case CONNECTTILE_RAILROAD: // Lay railroad.
            result = this.layRail(x, y, effects);
            this.fixZone(x, y, effects);
            break;

        case CONNECTTILE_WIRE: // Lay wire.
            result = this.layWire(x, y, effects);
            this.fixZone(x, y, effects);
            break;

        default:
            break;

        }

        return result;

    };


    p.layDoze = function layDoze(x, y, effects) {

        var tile = effects.getTile(x, y);

        if (!(tile & TILEMASK_BULLDOZABLE)) {
            return TOOLRESULT_FAILED; // Check dozeable bit.
        }

        tile &= TILEMASK_TILE;
        tile = this.neutralizeRoad(tile);

        switch (tile) {
        case HBRIDGE:
        case VBRIDGE:
        case BRWV:
        case BRWH:
        case HBRDG0:
        case HBRDG1:
        case HBRDG2:
        case HBRDG3:
        case VBRDG0:
        case VBRDG1:
        case VBRDG2:
        case VBRDG3:
        case HPOWER:
        case VPOWER:
        case HRAIL:
        case VRAIL: // Dozing over water, replace with water.
            effects.setTile(x, y, RIVER);
            break;

        default: // Dozing on land, replace with land. aSimple, eh?
            effects.setTile(x, y, DIRT);
            break;

        }

        effects.addCost(1); // Costs $1.00.

        return TOOLRESULT_OK;
    };


    p.layRoad = function layRoad(x, y, effects) {

        var cost = 10;
        var tile = effects.getTile(x, y) & TILEMASK_TILE;

        switch (tile) {

        case DIRT:

            effects.setTile(x, y, ROADS | TILEMASK_BULLDOZABLE | TILEMASK_BURNABLE);

            break;

        case RIVER: // Road on water.
        case REDGE:
        case CHANNEL: // Check how to build bridges, if possible.

            cost = 50;

            if (x < (WORLD_W - 1)) {
                tile = effects.getTile(x + 1, y) & TILEMASK_TILE;
                tile = this.neutralizeRoad(tile);
                if ((tile === VRAILROAD) ||
                    (tile === HBRIDGE) ||
                    ((tile >= ROADS) &&
                     (tile <= HROADPOWER))) {
                    effects.setTile(x, y, HBRIDGE | TILEMASK_BULLDOZABLE);
                    break;
                }
            }

            if (x > 0) {
                tile = effects.getTile(x - 1, y) & TILEMASK_TILE;
                tile = this.neutralizeRoad(tile);
                if ((tile === VRAILROAD) ||
                    (tile === HBRIDGE) ||
                    ((tile >= ROADS) &&
                     (tile <= INTERSECTION))) {
                    effects.setTile(x, y, HBRIDGE | TILEMASK_BULLDOZABLE);
                    break;
                }
            }

            if (y < (WORLD_H - 1)) {
                tile = effects.getTile(x, y + 1) & TILEMASK_TILE;
                tile = this.neutralizeRoad(tile);
                if ((tile === HRAILROAD) ||
                    (tile === VROADPOWER) ||
                    ((tile >= VBRIDGE) &&
                     (tile <= INTERSECTION))) {
                    effects.setTile(x, y, VBRIDGE | TILEMASK_BULLDOZABLE);
                    break;
                }
            }

            if (y > 0) {
                tile = effects.getTile(x, y - 1) & TILEMASK_TILE;
                tile = this.neutralizeRoad(tile);
                if ((tile === HRAILROAD) ||
                    (tile === VROADPOWER) ||
                    ((tile >= VBRIDGE) &&
                     (tile <= INTERSECTION))) {
                    effects.setTile(x, y, VBRIDGE | TILEMASK_BULLDOZABLE);
                    break;
                }
            }

            // Can't do road.
            return TOOLRESULT_FAILED;

        case LHPOWER: // Road on power

            effects.setTile(x, y, VROADPOWER | TILEMASK_CONDUCTIVE | TILEMASK_BURNABLE | TILEMASK_BULLDOZABLE);

            break;

        case LVPOWER: // Road on power #2

            effects.setTile(x, y, HROADPOWER | TILEMASK_CONDUCTIVE | TILEMASK_BURNABLE | TILEMASK_BULLDOZABLE);

            break;

        case LHRAIL: // Road on rail

            effects.setTile(x, y, HRAILROAD | TILEMASK_BURNABLE | TILEMASK_BULLDOZABLE);

            break;

        case LVRAIL: // Road on rail #2

            effects.setTile(x, y, VRAILROAD | TILEMASK_BURNABLE | TILEMASK_BULLDOZABLE);

            break;

        default: // Can't do road

            return TOOLRESULT_FAILED;

        }

        effects.addCost(cost);

        return TOOLRESULT_OK;
    };


    p.layRail = function layRail(x, y, effects) {

        var cost = 20;
        var tile = effects.getTile(x, y) & TILEMASK_TILE;

        tile = this.neutralizeRoad(tile);

        switch (tile) {

        case DIRT: // Rail on dirt.

            effects.setTile(x, y, LHRAIL | TILEMASK_BULLDOZABLE | TILEMASK_BURNABLE);

            break;

        case RIVER: // Rail on water.
        case REDGE:
        case CHANNEL: // Check how to build underwater tunnel, if possible.

            cost = 100;

            if (x < (WORLD_W - 1)) {
                tile = effects.getTile(x + 1, y) & TILEMASK_TILE;
                tile = this.neutralizeRoad(tile);
                if ((tile === RAILHPOWERV) ||
                    (tile === HRAIL) ||
                    ((tile >= LHRAIL) &&
                     (tile <= HRAILROAD))) {
                    effects.setTile(x, y, HRAIL | TILEMASK_BULLDOZABLE);
                    break;
                }
            }

            if (x > 0) {
                tile = effects.getTile(x - 1, y) & TILEMASK_TILE;
                tile = this.neutralizeRoad(tile);
                if ((tile === RAILHPOWERV) ||
                    (tile === HRAIL) ||
                    ((tile > VRAIL) &&
                     (tile < VRAILROAD))) {
                    effects.setTile(x, y, HRAIL | TILEMASK_BULLDOZABLE);
                    break;
                }
            }

            if (y < (WORLD_H - 1)) {
                tile = effects.getTile(x, y + 1) & TILEMASK_TILE;
                tile = this.neutralizeRoad(tile);
                if ((tile === RAILVPOWERH) ||
                    (tile === VRAILROAD) ||
                    ((tile > HRAIL) &&
                     (tile < HRAILROAD))) {
                    effects.setTile(x, y, VRAIL | TILEMASK_BULLDOZABLE);
                    break;
                }
            }

            if (y > 0) {
                tile = effects.getTile(x, y - 1) & TILEMASK_TILE;
                tile = this.neutralizeRoad(tile);
                if ((tile === RAILVPOWERH) ||
                    (tile === VRAILROAD) ||
                    ((tile > HRAIL) &&
                     (tile < HRAILROAD))) {
                    effects.setTile(x, y, VRAIL | TILEMASK_BULLDOZABLE);
                    break;
                }
            }

            // Can't do rail.
            return TOOLRESULT_FAILED;

        case LHPOWER: // Rail on power.
            effects.setTile(x, y, RAILVPOWERH | TILEMASK_CONDUCTIVE | TILEMASK_BURNABLE | TILEMASK_BULLDOZABLE);
            break;

        case LVPOWER: // Rail on power #2.
            effects.setTile(x, y, RAILHPOWERV | TILEMASK_CONDUCTIVE | TILEMASK_BURNABLE | TILEMASK_BULLDOZABLE);
            break;

        case ROADS: // Rail on road.
            effects.setTile(x, y, VRAILROAD | TILEMASK_BURNABLE | TILEMASK_BULLDOZABLE);
            break;

        case ROADS2: // Rail on road #2.
            effects.setTile(x, y, HRAILROAD | TILEMASK_BURNABLE | TILEMASK_BULLDOZABLE);
            break;

        default: // Can't do rail.
            return TOOLRESULT_FAILED;

        }

        effects.addCost(cost);

        return TOOLRESULT_OK;
    };


    p.layWire = function layWire(x, y, effects) {

        var cost = 5;
        var tile = effects.getTile(x, y) & TILEMASK_TILE;

        tile = this.neutralizeRoad(tile);

        switch (tile) {

        case DIRT: // Wire on dirt.

            effects.setTile(x, y, LHPOWER | TILEMASK_CONDUCTIVE | TILEMASK_BURNABLE | TILEMASK_BULLDOZABLE);

            break;

        case RIVER: // Wire on water.
        case REDGE:
        case CHANNEL: // Check how to lay underwater wire, if possible.

            cost = 25;

            if (x < (WORLD_W - 1)) {
                tile = effects.getTile(x + 1, y);
                if (tile & TILEMASK_CONDUCTIVE) {
                    tile &= TILEMASK_TILE;
                    tile = this.neutralizeRoad(tile);
                    if ((tile !== HROADPOWER) &&
                        (tile !== RAILHPOWERV) &&
                        (tile !== HPOWER)) {
                        effects.setTile(x, y, VPOWER | TILEMASK_CONDUCTIVE | TILEMASK_BULLDOZABLE);
                        break;
                    }
                }
            }

            if (x > 0) {
                tile = effects.getTile(x - 1, y);
                if (tile & TILEMASK_CONDUCTIVE) {
                    tile &= TILEMASK_TILE;
                    tile = this.neutralizeRoad(tile);
                    if ((tile !== HROADPOWER) &&
                        (tile !== RAILHPOWERV) &&
                        (tile !== HPOWER)) {
                        effects.setTile(x, y, VPOWER | TILEMASK_CONDUCTIVE | TILEMASK_BULLDOZABLE);
                        break;
                    }
                }
            }

            if (y < (WORLD_H - 1)) {
                tile = effects.getTile(x, y + 1);
                if (tile & TILEMASK_CONDUCTIVE) {
                    tile &= TILEMASK_TILE;
                    tile = this.neutralizeRoad(tile);
                    if ((tile !== VROADPOWER) &&
                        (tile !== RAILVPOWERH) &&
                        (tile !== VPOWER)) {
                        effects.setTile(x, y, HPOWER | TILEMASK_CONDUCTIVE | TILEMASK_BULLDOZABLE);
                        break;
                    }
                }
            }

            if (y > 0) {
                tile = effects.getTile(x, y - 1);
                if (tile & TILEMASK_CONDUCTIVE) {
                    tile &= TILEMASK_TILE;
                    tile = this.neutralizeRoad(tile);
                    if ((tile !== VROADPOWER) &&
                        (tile !== RAILVPOWERH) &&
                        (tile !== VPOWER)) {
                        effects.setTile(x, y, HPOWER | TILEMASK_CONDUCTIVE | TILEMASK_BULLDOZABLE);
                        break;
                    }
                }
            }

            // Can't do wire.
            return TOOLRESULT_FAILED;

        case ROADS: // Wire on road.
            effects.setTile(x, y, HROADPOWER | TILEMASK_CONDUCTIVE | TILEMASK_BURNABLE | TILEMASK_BULLDOZABLE);
            break;

        case ROADS2: // Wire on Road #2.
            effects.setTile(x, y, VROADPOWER | TILEMASK_CONDUCTIVE | TILEMASK_BURNABLE | TILEMASK_BULLDOZABLE);
            break;

        case LHRAIL: // Wire on rail.
            effects.setTile(x, y, RAILHPOWERV | TILEMASK_CONDUCTIVE | TILEMASK_BURNABLE | TILEMASK_BULLDOZABLE);
            break;

        case LVRAIL: // Wire on rail #2.
            effects.setTile(x, y, RAILVPOWERH | TILEMASK_CONDUCTIVE | TILEMASK_BURNABLE | TILEMASK_BULLDOZABLE);
            break;

        default: // Can't do wire.
            return TOOLRESULT_FAILED;

        }

        effects.addCost(cost);

        return TOOLRESULT_OK;
    };


    p.fixZone = function fixZone(x, y, effects) {

        this.fixSingle(x, y, effects);

        if (y > 0) {
            this.fixSingle(x, y - 1, effects);
        }

        if (x < (WORLD_W - 1)) {
            this.fixSingle(x + 1, y, effects);
        }

        if (y < (WORLD_H - 1)) {
            this.fixSingle(x, y + 1, effects);
        }

        if (x > 0) {
            this.fixSingle(x - 1, y, effects);
        }

    };


    p.fixSingle = function fixSingle(x, y, effects) {

        var adjTile = 0;

        var tile = effects.getTile(x, y) & TILEMASK_TILE;

        tile = this.neutralizeRoad(tile);

        if ((tile >= ROADS) &&
            (tile <= INTERSECTION)) { // Cleanup road.

            if (y > 0) {
                tile = effects.getTile(x, y - 1) & TILEMASK_TILE;
                tile = this.neutralizeRoad(tile);
                if (((tile === HRAILROAD) ||
                     ((tile >= ROADBASE) &&
                      (tile <= VROADPOWER))) &&
                    (tile !== HROADPOWER) &&
                    (tile !== VRAILROAD) &&
                    (tile !== ROADBASE)) {
                    adjTile |= 0x0001;
                }
            }

            if (x < (WORLD_W - 1)) {
                tile = effects.getTile(x + 1, y) & TILEMASK_TILE;
                tile = this.neutralizeRoad(tile);
                if (((tile === VRAILROAD) ||
                     ((tile >= ROADBASE) &&
                      (tile <= VROADPOWER))) &&
                    (tile !== VROADPOWER) &&
                    (tile !== HRAILROAD) &&
                    (tile !== VBRIDGE)) {
                    adjTile |= 0x0002;
                }
            }

            if (y < (WORLD_H - 1)) {
                tile = effects.getTile(x, y + 1) & TILEMASK_TILE;
                tile = this.neutralizeRoad(tile);
                if (((tile === HRAILROAD) ||
                     ((tile >= ROADBASE) &&
                      (tile <= VROADPOWER))) &&
                    (tile !== HROADPOWER) &&
                    (tile !== VRAILROAD) &&
                    (tile !== ROADBASE)) {
                    adjTile |= 0x0004;
                }
            }

            if (x > 0) {
                tile = effects.getTile(x - 1, y) & TILEMASK_TILE;
                tile = this.neutralizeRoad(tile);
                if (((tile === VRAILROAD) ||
                     ((tile >= ROADBASE) &&
                      (tile <= VROADPOWER))) &&
                    (tile !== VROADPOWER) &&
                    (tile !== HRAILROAD) &&
                    (tile !== VBRIDGE)) {
                    adjTile |= 0x0008;
                }
            }

            effects.setTile(x, y, roadTable[adjTile] | TILEMASK_BULLDOZABLE | TILEMASK_BURNABLE);

            return;
        }

        if ((tile >= LHRAIL) &&
            (tile <= LVRAIL10)) { // Cleanup rail.

            if (y > 0) {
                tile = effects.getTile(x, y - 1) & TILEMASK_TILE;
                tile = this.neutralizeRoad(tile);
                if ((tile >= RAILHPOWERV) &&
                    (tile <= VRAILROAD) && 
                    (tile !== RAILHPOWERV) &&
                    (tile !== HRAILROAD) &&
                    (tile !== HRAIL)) {
                    adjTile |= 0x0001;
                }
            }

            if (x < (WORLD_W - 1)) {
                tile = effects.getTile(x + 1, y) & TILEMASK_TILE;
                tile = this.neutralizeRoad(tile);
                if ((tile >= RAILHPOWERV) &&
                    (tile <= VRAILROAD) &&
                    (tile !== RAILVPOWERH) &&
                    (tile !== VRAILROAD) &&
                    (tile !== VRAIL)) {
                    adjTile |= 0x0002;
                }
            }

            if (y < (WORLD_H - 1)) {
                tile = effects.getTile(x, y + 1) & TILEMASK_TILE;
                tile = this.neutralizeRoad(tile);
                if ((tile >= RAILHPOWERV) &&
                    (tile <= VRAILROAD) &&
                    (tile !== RAILHPOWERV) &&
                    (tile !== HRAILROAD) &&
                    (tile !== HRAIL)) {
                    adjTile |= 0x0004;
                }
            }

            if (x > 0) {
                tile = effects.getTile(x - 1, y) & TILEMASK_TILE;
                tile = this.neutralizeRoad(tile);
                if ((tile >= RAILHPOWERV) &&
                    (tile <= VRAILROAD) &&
                    (tile !== RAILVPOWERH) &&
                    (tile !== VRAILROAD) &&
                    (tile !== VRAIL)) {
                    adjTile |= 0x0008;
                }
            }

            effects.setTile(x, y, railTable[adjTile] | TILEMASK_BULLDOZABLE | TILEMASK_BURNABLE);

            return;
        }

        if ((tile >= LHPOWER) &&
            (tile <= LVPOWER10)) { // Cleanup wire.

            if (y > 0) {
                tile = effects.getTile(x, y - 1);
                if (tile & TILEMASK_CONDUCTIVE) {
                    tile &= TILEMASK_TILE;
                    tile = this.neutralizeRoad(tile);
                    if ((tile !== VPOWER) &&
                        (tile !== VROADPOWER) &&
                        (tile !== RAILVPOWERH)) {
                        adjTile |= 0x0001;
                    }
                }
            }

            if (x < (WORLD_W - 1)) {
                tile = effects.getTile(x + 1, y);
                if (tile & TILEMASK_CONDUCTIVE) {
                    tile &= TILEMASK_TILE;
                    tile = this.neutralizeRoad(tile);
                    if ((tile !== HPOWER) &&
                        (tile !== HROADPOWER) &&
                        (tile !== RAILHPOWERV)) {
                        adjTile |= 0x0002;
                    }
                }
            }

            if (y < (WORLD_H - 1)) {
                tile = effects.getTile(x, y + 1);
                if (tile & TILEMASK_CONDUCTIVE) {
                    tile &= TILEMASK_TILE;
                    tile = this.neutralizeRoad(tile);
                    if ((tile !== VPOWER) &&
                        (tile !== VROADPOWER) &&
                        (tile !== RAILVPOWERH)) {
                        adjTile |= 0x0004;
                    }
                }
            }

            if (x > 0) {
                tile = effects.getTile(x - 1, y);
                if (tile & TILEMASK_CONDUCTIVE) {
                    tile &= TILEMASK_TILE;
                    tile = this.neutralizeRoad(tile);
                    if ((tile !== HPOWER) &&
                        (tile !== HROADPOWER) &&
                        (tile !== RAILHPOWERV)) {
                        adjTile |= 0x0008;
                    }
                }
            }

            effects.setTile(x, y, wireTable[adjTile] | TILEMASK_BULLDOZABLE | TILEMASK_BURNABLE | TILEMASK_CONDUCTIVE);

            return;
        }

    };


    p.doDisasters = function doDisasters() {

        // Chance of disasters at level 0 1 2.
        var disasterChance = [
            10 * 48, // Game level 0
            5 * 48,  // Game level 1
            60 // Game level 2
        ];

        if (this.floodCount) {
            this.floodCount--;
        }

        if (this.disasterEvent !== SCENARIO_NONE) {
            this.scenarioDisaster();
        }

        if (!this.enableDisasters) { // Disasters have been disabled
            return;
        }

        var x = this.gameLevel;

        if (x > LEVEL_LAST) {
            x = LEVEL_EASY;
        }

        if (!this.getRandom(disasterChance[x])) {

            switch (this.getRandom(8)) {

            case 0:
            case 1:
                this.setFire();  // 2/9 chance a fire breaks out.
                break;

            case 2:
            case 3:
                this.makeFlood(); // 2/9 chance for a flood.
                break;

            case 4:
                // 1/9 chance nothing happens (was airplane crash,
                // which EA removed after 9/11, and requested it be
                // removed from this code)
                break;

            case 5:
                this.makeTornado(); // 1/9 chance tornado
                break;

            case 6:
                this.makeEarthquake(); // 1/9 chance earthquake
                break;

            case 7:
            case 8:
                // 2/9 chance a scary monster arrives in a dirty town
                if (this.pollutionAverage > 60) {
                    this.makeMonster();
                }
                break;

            default:
                break;

            }

        }

    };


    p.scenarioDisaster = function scenarioDisaster() {

        switch (this.disasterEvent) {
        case SCENARIO_DULLSVILLE:
            break;

        case SCENARIO_SAN_FRANCISCO:
            if (this.disasterWait === 1) {
                this.makeEarthquake();
            }
            break;

        case SCENARIO_HAMBURG:
            if ((this.disasterWait % 10) === 0) {
                this.makeFireBombs();
            }
            break;

        case SCENARIO_BERN:
            break;

        case SCENARIO_TOKYO:
            if (this.disasterWait === 1) {
                this.makeMonster();
            }
            break;

        case SCENARIO_DETROIT:
            break;

        case SCENARIO_BOSTON:
            if (this.disasterWait === 1) {
                this.makeMeltdown();
            }
            break;

        case SCENARIO_RIO:
            if ((this.disasterWait % 24) === 0) {
                this.makeFlood();
            }
            break;

        default:
            break; // Never used
        }

        if (this.disasterWait > 0) {
            this.disasterWait--;
        } else {
            this.disasterEvent = SCENARIO_NONE;
        }
    };


    p.makeMeltdown = function makeMeltdown() {

        for (var x = 0;
             x < (WORLD_W - 1);
             x++) {

            for (var y = 0;
                 y < (WORLD_H - 1);
                 y++) {

                if ((this.getTile(x, y) & TILEMASK_TILE) === NUCLEAR) {
                    this.doMeltdown(x, y);
                    return;
                }

            }

        }

    };


    p.fireBomb = function fireBomb() {

        var crashX = this.getRandom(WORLD_W - 1);
        var crashY = this.getRandom(WORLD_H - 1);

        this.makeExplosion(crashX, crashY);

        this.sendMessage('FIREBOMBING', crashX, crashY, true, true);

    };


    p.makeFireBombs = function makeFireBombs() {

        var count = 2 + (this.getRandom16() & 1);

        while (count > 0) {
            this.fireBomb();
            count--;
        }

        // TODO: Schedule periodic fire bombs over time, every few ticks.

    };


    p.randomFire = function randomFire() {

        return (
            (FIRE +
             (this.getRandom16() & 7)) |
                TILEMASK_ANIMATED);

    };


    p.randomRubble = function randomRubble() {

        return (
            (RUBBLE +
             (this.getRandom16() & 3)) |
                TILEMASK_BULLDOZABLE);

    };


    p.makeEarthquake = function makeEarthquake() {

        // Strength/duration of the earthquake.
        var strength = this.getRandom(700) + 300;

        this.sendMessage('EARTHQUAKE', this.cityCenterX, this.cityCenterY, true, true);
        this.makeSound('city', 'ExplosionLow', null, null); // Make the sound all over.

        this.doEarthquake(strength);

        for (var z = 0;
             z < strength;
             z++)  {
            var x = this.getRandom(WORLD_W - 1);
            var y = this.getRandom(WORLD_H - 1);

            if (this.vulnerable(this.getTile(x, y))) {

                if ((z & 0x03) !== 0) { // 3 of 4 times reduce to rubble
                    this.setTile(x, y, this.randomRubble());
                } else {
                    // 1 of 4 times start fire
                    this.setTile(x, y, this.randomFire());
                }
            }
        }
    };


    p.setFire = function setFire() {

        var x = this.getRandom(WORLD_W - 1);
        var y = this.getRandom(WORLD_H - 1);
        var z = this.getTile(x, y);

        if ((z & TILEMASK_ZONE) === 0) {

            z = z & TILEMASK_TILE;

            if ((z > LHTHR) &&
                (z < LASTZONE)) {

                this.setTile(x, y, this.randomFire());
                this.sendMessage('FIRE_REPORTED', x, y, true, true);

            }

        }

    };


    p.makeFire = function makeFire() {

        for (var t = 0;
             t < 40;
             t++)  {

            var x = this.getRandom(WORLD_W - 1);
            var y = this.getRandom(WORLD_H - 1);
            var z = this.getTile(x, y);

            if ((!(z & TILEMASK_ZONE)) &&
                (z & TILEMASK_BURNABLE)) {

                z = z & TILEMASK_TILE;

                if ((z > TREEBASE) &&
                    (z < LASTZONE)) {

                    this.setTile(x, y, this.randomFire());
                    this.sendMessage('FIRE_REPORTED', x, y, true, true);

                    return;

                }

            }

        }

    };


    p.vulnerable = function vulnerable(tem) {

        var tem2 = tem & TILEMASK_TILE;

        if ((tem2 < RESBASE) ||
            (tem2 > LASTZONE) ||
            ((tem & TILEMASK_ZONE) !== 0)) {

            return false;

        }

        return true;

    };


    p.makeFlood = function makeFlood() {


        for (var z = 0;
             z < 300;
             z++) {

            var x = this.getRandom(WORLD_W - 1);
            var y = this.getRandom(WORLD_H - 1);
            var c = this.getTile(x, y) & TILEMASK_TILE;

            if ((c > CHANNEL) &&
                (c <= WATER_HIGH)) { // if riveredge

                for (var t = 0;
                     t < 4;
                     t++) {

                    var xx = x + dx[t];
                    var yy = y + dy[t];

                    if (this.testBounds(xx, yy)) {

                        c = this.getTile(xx, yy);

                        // Tile is floodable.
                        if ((c === DIRT) ||
                            ((c & (TILEMASK_BULLDOZABLE | TILEMASK_BURNABLE)) ===
                             (TILEMASK_BULLDOZABLE | TILEMASK_BURNABLE))) {

                            this.setTile(xx, yy, FLOOD);
                            this.floodCount = 30;
                            this.sendMessage('FLOODING_REPORTED', xx, yy, true, true);

                            return;

                        }

                    }

                }

            }

        }

    };


    p.doFlood = function doFlood(x, y) {

        if (this.floodCount > 0) {

            // Flood is not over yet
            for (var z = 0;
                 z < 4;
                 z++) {

                if ((this.getRandom16() & 7) === 0) { // 12.5% chance

                    var xx = x + dx[z];
                    var yy = y + dy[z];

                    if (this.testBounds(xx, yy)) {

                        var c = this.getTile(xx, yy);
                        var t = c & TILEMASK_TILE;

                        if (((c & TILEMASK_BURNABLE) === TILEMASK_BURNABLE) ||
                            (c === DIRT) ||
                            ((t >= WOODS5) &&
                             (t < FLOOD))) {

                            if ((c & TILEMASK_ZONE) === TILEMASK_ZONE) {

                                this.fireZone(xx, yy, c);
                            }

                            this.setTile(xx, yy, FLOOD + this.getRandom(2));

                        }

                    }

                }

            }

        } else {

            if ((this.getRandom16() & 15) === 0) { // 1/16 chance

                this.setTile(x, y,DIRT);

            }

        }

    };


    p.cityEvaluation = function cityEvaluation() {

        if (this.totalPop > 0) {

            var problemTable = []; // Score for each problem, higher the more severe the problem is.

            for (var z = 0;
                 z < PROBLEM_COUNT;
                 z++) {

                problemTable.push(0);

            }

            this.getAssessedValue();
            this.doPopNum();
            this.doProblems(problemTable);
            this.getScore(problemTable);
            this.doVotes(); // How well is the mayor doing?
            this.changeEval();

        } else {

            this.evalInit();
            this.cityYes = 50; // No population => no voting. Let's say 50/50.
            this.changeEval();

        }

    };


    p.evalInit = function evalInit() {

        this.cityYes = 0;
        this.cityPop = 0;
        this.cityPopDelta = 0;
        this.cityAssessedValue = 0;
        this.cityClass = CITYCLASS_VILLAGE;
        this.cityScore = 500;
        this.cityScoreDelta = 0;

        var i;

        for (i = 0;
             i < PROBLEM_COUNT;
             i++) {

            this.problemVotes[i] = 0;

        }

        for (i = 0;
             i < PROBLEM_COMPLAINTS;
             i++) {

            this.problemOrder[i] = PROBLEM_NUMPROBLEMS;

        }

    };


    p.getAssessedValue = function getAssessedValue() {

        var z = 0;

        z = this.roadTotal * 5;
        z += this.railTotal * 10;
        z += this.policeStationPop * 1000;
        z += this.fireStationPop * 1000;
        z += this.hospitalPop * 400;
        z += this.stadiumPop * 3000;
        z += this.seaportPop * 5000;
        z += this.airportPop * 10000;
        z += this.coalPowerPop * 3000;
        z += this.nuclearPowerPop * 6000;

        this.cityAssessedValue = z * 1000;

    };


    p.doPopNum = function doPopNum() {

        var oldCityPop = this.cityPop;

        this.cityPop = this.getPopulation();

        if (this.oldCityPop === -1) {
            oldCityPop = this.cityPop;
        }

        this.cityPopDelta = this.cityPop - oldCityPop;
        this.cityClass = this.getCityClass(this.cityPop);

    };


    p.getPopulation = function getPopulation() {

        var pop =
                (this.resPop + (this.comPop + this.indPop) * 8) * 20;

        return pop;

    };


    p.getCityClass = function getCityClass(cityPop) {

        var cityClassification = CITYCLASS_VILLAGE;

        if (this.cityPopulation > 2000) {
            cityClassification = CITYCLASS_TOWN;
        }
        if (this.cityPopulation > 10000) {
            cityClassification = CITYCLASS_CITY;
        }
        if (this.cityPopulation > 50000) {
            cityClassification = CITYCLASS_CAPITAL;
        }
        if (this.cityPopulation > 100000) {
            cityClassification = CITYCLASS_METROPOLIS;
        }
        if (this.cityPopulation > 500000) {
            cityClassification = CITYCLASS_MEGALOPOLIS;
        }

        return cityClassification;

    };


    p.doProblems = function doProblems(problemTable) {

        var problemTaken = []; // Which problems are taken?
        var z;

        for (z = 0;
             z < PROBLEM_COUNT;
             z++) {

            problemTaken[z] = false;
            problemTable[z] = 0;

        }

        problemTable[PROBLEM_CRIME]        = this.crimeAverage;                // Crime
        problemTable[PROBLEM_POLLUTION]    = this.pollutionAverage;            // Pollution
        problemTable[PROBLEM_HOUSING]      = this.landValueAverage * 7 / 10;   // Housing
        problemTable[PROBLEM_TAXES]        = this.cityTax * 10;                // Taxes
        problemTable[PROBLEM_TRAFFIC]      = this.getTrafficAverage();         // Traffic
        problemTable[PROBLEM_UNEMPLOYMENT] = this.getUnemployment();           // Unemployment
        problemTable[PROBLEM_FIRE]         = this.getFireSeverity();           // Fire
        this.voteProblems(problemTable);

        for (z = 0;
             z < PROBLEM_COMPLAINTS;
             z++) {

            // Find biggest problem not taken yet.
            var maxVotes = 0;
            var bestProblem = PROBLEM_NUMPROBLEMS;

            for (var i = 0;
                 i < PROBLEM_NUMPROBLEMS;
                 i++) {

                if ((this.problemVotes[i] > maxVotes) &&
                    (!problemTaken[i])) {

                    bestProblem = i;
                    maxVotes = this.problemVotes[i];

                }

            }

            // bestProblem === PROBLEM_NUMPROBLEMS means no problem found
            this.problemOrder[z] = bestProblem;
            if (bestProblem < PROBLEM_NUMPROBLEMS) {
                problemTaken[bestProblem] = true;
            }

            // else: No problem found.
            //       Repeating the procedure will give the same result.
            //       Optimize by filling all remaining entries, and breaking out

        }

    };


    p.voteProblems = function voteProblems(problemTable) {

        for (var z = 0;
             z < PROBLEM_COUNT;
             z++) {

            this.problemVotes[z] = 0;

        }

        var problem = 0; // Problem to vote for
        var voteCount = 0; // Number of votes
        var loopCount = 0; // Number of attempts

        while ((voteCount < 100) &&
               (loopCount < 600)) {

            if (this.getRandom(300) < problemTable[problem]) {
                this.problemVotes[problem]++;
                voteCount++;
            }

            problem++;

            if (problem > PROBLEM_COUNT) {
                problem = 0;
            }

            loopCount++;

        }

    };


    p.getTrafficAverage = function getTrafficAverage() {

        var trafficTotal = 0;
        var count = 1;

        for (var x = 0;
             x < WORLD_W;
             x += this.landValueMap.blockSize) {

            for (var y = 0;
                 y < WORLD_H;
                 y += this.landValueMap.blockSize) {

                if (this.landValueMap.worldGet(x, y) > 0) {

                    trafficTotal += this.trafficDensityMap.worldGet(x, y);
                    count++;

                }

            }

        }

        this.trafficAverage = Math.floor((trafficTotal / count) * 2.4);

        return this.trafficAverage;

    };


    p.getUnemployment = function getUnemployment() {

        var b = (this.comPop + this.indPop) * 8;

        if (b === 0) {
            return 0;
        }

        // Ratio total people / working. At least 1.
        var r = this.resPop / b;

        b = Math.floor((r - 1) * 255); // (r - 1) is the fraction unemployed people

        return Math.min(b, 255);

    };


    p.getFireSeverity = function getFireSeverity() {

        return Math.min(this.firePop * 5, 255);

    };


    p.getScore = function getScore(problemTable) {

        var cityScoreLast = this.cityScore;
        var x = 0;

        for (var z = 0;
             z < PROBLEM_NUMPROBLEMS;
             z++) {

            x += problemTable[z]; // Add 7 probs.

        }

        // @todo Should this expression depend on PROBLEM_NUMPROBLEMS?

        x = x / 3; // 7 + 2 average.
        x = Math.min(x, 256);

        z = this.clamp((256 - x) * 4, 0, 1000);

        if (this.resCap) {
            z = Math.floor(z * 0.85);
        }

        if (this.comCap) {
            z = Math.floor(z * 0.85);
        }

        if (this.indCap) {
            z = Math.floor(z * 0.85);
        }

        if (this.roadEffect < MAX_ROAD_EFFECT)  {
            z -= MAX_ROAD_EFFECT - this.roadEffect;
        }

        if (this.policeEffect < MAX_POLICE_STATION_EFFECT) {
            // 10.0001 = 10000.1 / 1000, 1/10.0001 is about 0.1
            z = Math.floor(z * (0.9 + (this.policeEffect / (10.0001 * MAX_POLICE_STATION_EFFECT))));
        }

        if (this.fireEffect < MAX_FIRE_STATION_EFFECT) {
            // 10.0001 = 10000.1 / 1000, 1/10.0001 is about 0.1
            z = Math.floor(z * (0.9 + (this.fireEffect / (10.0001 * MAX_FIRE_STATION_EFFECT))));
        }

        if (this.resValve < -1000) {
            z = Math.floor(z * 0.85);
        }

        if (this.comValve < -1000) {
            z = Math.floor(z * 0.85);
        }

        if (this.indValve < -1000) {
            z = Math.floor(z * 0.85);
        }

        var sm = 1.0;
        if ((this.cityPop === 0) ||
            (this.cityPopDelta === 0)) {

            sm = 1.0; // there is nobody or no migration happened

        } else if (this.cityPopDelta === this.cityPop) {

            sm = 1.0; // city sprang into existence or doubled in size

        } else if (this.cityPopDelta > 0) {

            sm = (this.cityPopDelta / this.cityPop) + 1.0;

        } else if (this.cityPopDelta < 0) {

            sm = 0.95 + (this.cityPopDelta / (this.cityPop - this.cityPopDelta));

        }

        z = Math.floor(z * sm);

        z -= this.getFireSeverity() - this.cityTax; // Dec score for fires and taxes.

        var tm = this.unpoweredZoneCount + this.poweredZoneCount; // Dec score for unpowered zones.

        if (tm > 0.0) {
            z = Math.floor(z * (this.poweredZoneCount / tm));
        }

        z = this.clamp(z, 0, 1000);

        this.cityScore = (this.cityScore + z) / 2;

        this.cityScoreDelta = this.cityScore - cityScoreLast;

    };


    p.doVotes = function doVotes() {

        this.cityYes = 0;

        for (var z = 0;
             z < 100;
             z++) {

            if (this.getRandom(1000) < this.cityScore) {

                this.cityYes++;

            }

        }

    };


    p.changeEval = function changeEval() {

        this.evalChanged = true;

    };


    p.scoreDoer = function scoreDoer() {

        if (this.evalChanged) {
            this.evalChanged = false;
            this.doUpdateEvaluation();
        }

    };


    p.countProblems = function countProblems() {

        var i;
        for (i = 0;
             i < PROBLEM_COMPLAINTS;
             i++) {

            if (this.problemOrder[i] === PROBLEM_NUMPROBLEMS) {
                break;
            }
        }

        return i;

    };


    p.getProblemNumber = function getProblemNumber(i) {

        if ((i < 0) ||
            (i >= PROBLEM_COMPLAINTS) ||
            (this.problemOrder[i] === PROBLEM_NUMPROBLEMS)) {

            return -1;

        } else {

            return this.problemOrder[i];

        }

    };


    p.getProblemVotes = function getProblemVotes(i) {

        if ((i < 0) ||
            (i >= PROBLEM_COMPLAINTS) ||
            (this.problemOrder[i] === PROBLEM_NUMPROBLEMS)) {

            return -1;

        } else {

            return this.problemVotes[this.problemOrder[i]];

        }

    };


    p.swapShort = function swapShort(val) {

        return (
            ((val & 0x00ff) << 8) |
                ((val & 0xff00) >> 8));

    };


    p.swapShorts = function swapShorts(buf) {

        for (var i = 0, len = buf.length;
             i < len;
             i++) {

            buf[i] = this.swapShort(buf[i]);

        }

    };


    p.halfSwapLong = function halfSwapLong(val) {

        return (
            ((val & 0x0000ffff) << 16) |
                ((val & 0xffff0000) >> 16));

    };


    p.halfSwapLongs = function halfSwapLongs(buf) {

        for (var i = 0, len = buf.length;
             i < len;
             i++) {

            buf[i] = this.halfSwapLong(buf[i]);

        }

    };


    p.loadShorts = function loadShorts(f, buf) {

        var fileData = f.data;
        var fileLength = f.length;
        var fileOffset = f.offset;
        var bufLen = buf.len;
        var bufByteLen = bufLen * 2;
        var bufByteEnd = fileOffset + bufByteLen;

        if (bufByteEnd > fileLength) {
            return false;
        }


        for (var i = 0;
             i < bufLen;
             i++) {

            buf[i] =
                fileData[fileOffset + (i * 2)] |
                (fileData[fileOffset + (i * 2) + 1] << 8);

        }

        f.offset += bufByteLen;

        return true;

    };


    p.saveShorts = function saveShorts(f, buf) {

        var fileData = f.data;
        var fileLength = f.length;
        var fileOffset = f.offset;
        var bufLen = buf.len;
        var bufByteLen = bufLen * 2;
        var bufByteEnd = fileOffset + bufByteLen;

        if (bufByteEnd > fileLength) {
            return false;
        }


        for (var i = 0;
             i < bufLen;
             i++) {

            var val = buf[i];

            fileData[fileOffset + (i * 2)] = val & 0x00ff;
            fileData[fileOffset + (i * 2) + 1] = (val >> 8) & 0x00ff;

        }

        f.offset += bufByteLen;

        return true;

    };


    p.loadMiscShort = function loadMiscShort(offset) {

        return this.miscHist[offset];

    };


    p.loadMiscLong = function loadMiscLong(offset) {

        var val0 =
                this.miscHist[offset];

        var val1 =
                this.miscHist[offset + 1];

        return (
            ((val0 & 0xffff) << 16) |
                (val1 & 0xffff));

    };


    p.saveMiscShort = function saveMiscShort(offset, val) {

        this.miscHist[offset] = val;

    };


    p.saveMiscLong = function saveMiscLong(offset, val) {

        var val0 = (val >> 16) & 0xffff;
        var val1 = val & 0xffff;

        this.miscHist[offset] = val0;
        this.miscHist[offset + 1] = val1;

    };


    p.loadData = function loadData(data) {

        var f = {
            data: data,
            length: data.length,
            offset: 0
        };

        if (data.length !== SAVE_FILE_BYTES) {
            this.errorMessage +=
            'Saved city file length ' +
                data.length +
                ' is not the expected length ' +
                SAVE_FILE_BYTES +
                ' bytes.\n';
            return false;
        }

        if (!this.loadShorts(f, this.resHist)) {
            this.errorMessage += 'Could not load saved city file resHist.\n';
            return false;
        }

        if (!this.loadShorts(f, this.comHist)) {
            this.errorMessage += 'Could not load saved city file comHist.\n';
            return false;
        }

        if (!this.loadShorts(f, this.indHist)) {
            this.errorMessage += 'Could not load saved city file indHist.\n';
            return false;
        }

        if (!this.loadShorts(f, this.crimeHist)) {
            this.errorMessage += 'Could not load saved city file crimeHist.\n';
            return false;
        }

        if (!this.loadShorts(f, this.pollutionHist)) {
            this.errorMessage += 'Could not load saved city file pollutionHist.\n';
            return false;
        }

        if (!this.loadShorts(f, this.moneyHist)) {
            this.errorMessage += 'Could not load saved city file moneyHist.\n';
            return false;
        }

        if (!this.loadShorts(f, this.miscHist)) {
            this.errorMessage += 'Could not load saved city file miscHist.\n';
            return false;
        }

        if (!this.loadShorts(f, this.map.getData())) {
            this.errorMessage += 'Could not load saved city file map.\n';
            return false;
        }

        return true;
    };


    p.loadFile = function loadFile(fileName, data) {

        errorMessage = '';

        if (data === null) {
            this.errorMessage +=
            'Could not read saved city file "' +
                fileName +
                '".';
            this.doNotLoadCity();
            return false;

        }

        if (!this.loadData(data)) {
            this.errorMessage +=
            'Could not load saved city file "' +
                fileName +
                '".';
            this.doNotLoadCity();
            return false;
        }

        this.setFunds(
            this.loadMiscLong(50));

        this.cityTime =
            Math.max(
                0,
                this.loadMiscLong(8));

        this.setAutoBulldoze(
            this.loadMiscShort(52) !== 0);

        this.setAutoBudget(
            this.loadMiscShort(53) !== 0);

        this.setAutoGoto(
            this.loadMiscShort(54) !== 0);

        this.setEnableSound(
            this.loadMiscShort(55) !== 0);

        this.setCityTax(
            this.loadMiscShort(56));

        this.setSpeed(
            this.loadMiscShort(57));

        this.changeCensus();
        this.mustUpdateOptions = true;

        this.policePercent =
            this.loadMiscLong(58) / 65536.0;

        this.firePercent =
            this.loadMiscLong(60) / 65536.0;

        this.roadPercent =
            this.loadMiscLong(62) / 65536.0;

        // If the tax is nonsensical, set it to a reasonable value.
        if ((this.cityTax > 20) ||
            (this.cityTax < 0)) {
            this.setCityTax(7);
        }

        // If the speed is nonsensical, set it to a reasonable value.
        if ((this.simSpeed < 0) ||
            (this.simSpeed > 3)) {
            this.setSpeed(3);
        }

        this.setSpeed(this.simSpeed);

        this.setPasses(1);
        this.initFundingLevel();

        // Set the scenario id to 0.
        this.initWillStuff();
        this.scenario = SCENARIO_NONE;
        this.initSimLoad = 1;
        this.doInitialEval = false;
        this.prepareToStart();

        this.cityFileName = fileName;
        this.citySaveData = data;

        this.doLoadCity();

        return true;
    };


    p.saveData = function saveData(data) {

        var f = {
            data: data,
            length: data.length,
            offset: 0
        };

        if (data.length !== SAVE_FILE_BYTES) {
            this.errorMessage +=
            'Saved city data length ' +
                data.length +
                ' is not the expected length ' +
                SAVE_FILE_BYTES +
                ' bytes.\n';
            return false;
        }

        if (!this.loadShorts(f, this.resHist)) {
            this.errorMessage += 'Could not load saved city file resHist.\n';
            return false;
        }

        if (!this.loadShorts(f, this.comHist)) {
            this.errorMessage += 'Could not load saved city file comHist.\n';
            return false;
        }

        if (!this.loadShorts(f, this.indHist)) {
            this.errorMessage += 'Could not load saved city file indHist.\n';
            return false;
        }

        if (!this.loadShorts(f, this.crimeHist)) {
            this.errorMessage += 'Could not load saved city file crimeHist.\n';
            return false;
        }

        if (!this.loadShorts(f, this.pollutionHist)) {
            this.errorMessage += 'Could not load saved city file pollutionHist.\n';
            return false;
        }

        if (!this.loadShorts(f, this.moneyHist)) {
            this.errorMessage += 'Could not load saved city file moneyHist.\n';
            return false;
        }

        if (!this.loadShorts(f, this.miscHist)) {
            this.errorMessage += 'Could not load saved city file miscHist.\n';
            return false;
        }

        if (!this.loadShorts(f, this.map.getData())) {
            this.errorMessage += 'Could not load saved city file map.\n';
            return false;
        }

        return true;
    };


    p.saveFile = function saveFile(fileName) {

        errorMessage = '';

        var data =
            this.makeArray_Uint8(
                SAVE_FILE_BYTES);

        var f = {
            data: data,
            length: data.length,
            offset: 0
        };

        this.saveMiscLong(
            100, this.totalFunds);

        this.saveMiscLong(
            16, this.cityTime);

        this.saveMiscShort(
            104, this.autoBulldoze ? 1 : 0);

        this.saveMiscShort(
            106, this.autoBudget ? 1 : 0);

        this.saveMiscShort(
            108, this.autoGoto ? 1 : 0);

        this.saveMiscShort(
            110, this.enableSound ? 1 : 0);

        this.saveMiscShort(
            112, this.cityTax);

        this.saveMiscShort(
            114, this.simSpeed);

        this.saveMiscLong(
            116, Math.floor(this.policePercent * 65536.0));

        this.saveMiscLong(
            120, Math.floor(this.firePercent * 65536.0));

        this.saveMiscLong(
            124, Math.floor(this.roadPercent * 65536.0));

        data = this.saveData(f);

        if (data === null) {
            this.errorMessage +=
            'Could not save city.\n';
            this.doNotSaveCity();
            return null;
        }

        this.cityFileName = fileName;
        this.citySaveData= f.data;

        this.doSaveCity();

        return f.data;
    };


    p.loadScenario = function loadScenario(scenario) {

        this.cityFileName = '';

        this.setGameLevel(LEVEL_EASY);

        if ((scenario < SCENARIO_DULLSVILLE) ||
            (scenario > SCENARIO_RIO)) {
            scenario = SCENARIO_DULLSVILLE;
        }

        var cityName = '';
        var resourceName = '';

        switch (scenario) {

        case SCENARIO_DULLSVILLE:
            cityName = 'Dullsville';
            resourceName = 'snro.111';
            this.cityTime = ((1900 - 1900) * 48) + 2;
            this.setFunds(5000);
            break;

        case SCENARIO_SAN_FRANCISCO:
            cityName = 'San Francisco';
            resourceName = 'snro.222';
            this.cityTime = ((1906 - 1900) * 48) + 2;
            this.setFunds(20000);
            break;

        case SCENARIO_HAMBURG:
            cityName = 'Hamburg';
            resourceName = 'snro.333';
            this.cityTime = ((1944 - 1900) * 48) + 2;
            this.setFunds(20000);
            break;

        case SCENARIO_BERN:
            cityName = 'Bern';
            resourceName = 'snro.444';
            this.cityTime = ((1965 - 1900) * 48) + 2;
            this.setFunds(20000);
            break;

        case SCENARIO_TOKYO:
            cityName = 'Tokyo';
            resourceName = 'snro.555';
            this.cityTime = ((1957 - 1900) * 48) + 2;
            this.setFunds(20000);
            break;

        case SCENARIO_DETROIT:
            cityName = 'Detroit';
            resourceName = 'snro.666';
            this.cityTime = ((1972 - 1900) * 48) + 2;
            this.setFunds(20000);
            break;

        case SCENARIO_BOSTON:
            cityName = 'Boston';
            resourceName = 'snro.777';
            this.cityTime = ((2010 - 1900) * 48) + 2;
            this.setFunds(20000);
            break;

        case SCENARIO_RIO:
            cityName = 'Rio de Janeiro';
            resourceName = 'snro.888';
            this.cityTime = ((2047 - 1900) * 48) + 2;
            this.setFunds(20000);
            break;

        default:
            break;

        }

        this.setCleanCityName(cityName);
        this.setSpeed(3);
        this.setCityTax(7);

        var fileName =
                'res:' + resourceName;
        var data = 
                this.getResource(resourceName);

        if (data === null) {
            this.doNotLoadCity();
            this.doNotLoadScenario();
            return false;
        }

        if (!this.loadCity(
            fileName,
            data)) {
            this.doNotLoadScenario();
            return false;
        }

        this.initWillStuff();
        this.initFundingLevel();
        this.updateFunds();
        this.initSimLoad = 1;
        this.doInitialEval = false;
        this.prepareToStart();

        this.doLoadScenario();

        return true;

    };


    p.loadCityFromURL = function loadCityFromURL(url, success, failure) {

        var self = this;
        var xhr = new XMLHttpRequest();

        xhr.open("GET", url, true);
        xhr.responseType = "arraybuffer";

        xhr.onload = function(event) {

            var arrayBuffer = xhr.response;

            if (arrayBuffer) {

                var byteArray =
                    new Uint8Array(arrayBuffer);

                var result =
                    self.loadFile(url, byteArray);

                if (result) {
                    success();
                } else {
                    console.log("Error loading city:", self.errorMessage);
                    failure();
                }

            } else {

                failure();

            }

        }

        xhr.send(null);

    };


    p.generateRandomMap = function generateRandomMap() {

        this.generateMapFromSeed(
            this.getRandom16());

    };


    p.generateMapFromSeed = function generateMapFromSeed(seed) {

        this.cityFileName = '';

        this.generatedMapSeed = seed;

        this.seedRandom(this.generatedMapSeed);

        // Construct land.
        if (this.terrainCreateIsland < 0) {
            if (this.getRandom(100) < 10) { // chance that island is generated.
                this.makeIsland();
                return;
            }
        }

        if (this.terrainCreateIsland === 1) {
            this.makeNakedIsland();
        } else {
            this.clearMap();
        }

        // Lay a river.
        if (this.terrainCurveLevel !== 0) {

            var terrainXStart = 40 + this.getRandom(WORLD_W - 80);
            var terrainYStart = 33 + this.getRandom(WORLD_H - 67);

            this.doRivers(terrainXStart, terrainYStart);
        }

        // Lay a few lakes.
        if (this.terrainLakeLevel !== 0) {
            this.makeLakes();
        }

        this.smoothRiver();

        // And add trees.
        if (this.terrainTreeLevel !== 0) {
            this.doTrees();
        }

        this.scenario = SCENARIO_NONE;
        this.cityTime = 0;
        this.initSimLoad = 2;
        this.doInitialEval = false;

        this.initWillStuff();
        this.prepareToStart();
        this.updateFunds();
        this.simUpdate();
        this.doResetMapState();
        this.doResetEditorState();

        this.doGenerateMap(this.generatedMapSeed);

    };


    p.clearMap = function clearMap() {

        this.map.fill(
            DIRT);

    };


    p.clearUnnatural = function clearUnnatural() {

        var self = this;

        this.map.filter(
            function(x, y, value) {
                return ((value > WOODS) ? DIRT : null);
            });

    };


    p.makeNakedIsland = function makeNakedIsland() {

        var self = this;
        var islandRadius = ISLAND_RADIUS;

        this.map.filter(
            function(x, y, value) {

                var water =
                    (x < 5) ||
                    (x >= (self.WORLD_W - 5)) ||
                    (y < 5) ||
                    (y >= (self.WORLD_H - 5));

                return water ? RIVER : DIRT;

            });

        for (var x = 0;
             x < (WORLD_W - 5);
             x += 2) {

            this.plopBRiver(
                x,
                this.getERandom(islandRadius));

            this.plopBRiver(
                x,
                ((WORLD_H - 10) -
                 this.getERandom(islandRadius)));

            this.plopSRiver(
                x,
                0);

            this.plopSRiver(
                x,
                WORLD_H - 6);

        }

        for (var y = 0;
             y < WORLD_H - 5;
             y += 2) {

            this.plopBRiver(
                this.getERandom(islandRadius),
                y);

            this.plopBRiver(
                ((WORLD_W - 10) -
                 this.getERandom(islandRadius)),
                y);

            this.plopSRiver(
                0,
                y);

            this.plopSRiver(
                WORLD_W - 6,
                y);

        }

    };


    p.makeIsland = function makeIsland() {

        this.makeNakedIsland();
        this.smoothRiver();
        this.doTrees();

    };


    p.makeLakes = function makeLakes() {

        var numLakes =
            (this.terrainLakeLevel < 0) ? this.getRandom(10) : (this.terrainLakeLevel / 2);

        while (numLakes > 0) {
            var x = this.getRandom(WORLD_W - 21) + 10;
            var y = this.getRandom(WORLD_H - 20) + 10;

            this.makeSingleLake(x, y);

            numLakes--;
        }

    };


    p.makeSingleLake = function makeSingleLake(x, y) {

        var numPlops = this.getRandom(12) + 2;

        while (numPlops > 0) {

            x = x + this.getRandom(12) - 6;
            y = y + this.getRandom(12) - 6;

            if (this.getRandom(4)) {
                this.plopSRiver(x, y);
            } else {
                this.plopBRiver(x, y);
            }

            numPlops--;
        }

    };


    p.treeSplash = function treeSplash(xloc, yloc) {

        var numTrees =
            50 +
            this.getRandom(
                (this.terrainTreeLevel < 0) ? 150 : (100 + (terrainTreeLevel * 2)));

        var x = xloc;
        var y = yloc;

        while (numTrees > 0) {

            var dir = DIR_NORTH + this.getRandom(7);

            x += directionDX[dir];
            y += directionDY[dir];

            if (!this.testBounds(x, y)) {
                return;
            }

            if ((this.getTile(x, y) & TILEMASK_TILE) === DIRT) {
                this.setTile(x, y, WOODS | TILEMASK_BURNABLE | TILEMASK_BULLDOZABLE);
            }

            numTrees--;

        }

    };


    p.doTrees = function doTrees() {

        var amount =
            (this.terrainTreeLevel < 0) ? (this.getRandom(100) + 50) : (this.terrainTreeLevel + 3);

        for (var x = 0;
             x < amount;
             x++) {

            var xloc = this.getRandom(WORLD_W - 1);
            var yloc = this.getRandom(WORLD_H - 1);

            this.treeSplash(xloc, yloc);

        }

        this.smoothTrees();
        this.smoothTrees();

    };


    p.smoothRiver = function smoothRiver() {

        var self = this;
        var table = [
            13 | TILEMASK_BULLDOZABLE,   13 | TILEMASK_BULLDOZABLE,     17 | TILEMASK_BULLDOZABLE,     15 | TILEMASK_BULLDOZABLE,
            5  | TILEMASK_BULLDOZABLE,   2,                                 19 | TILEMASK_BULLDOZABLE,     17 | TILEMASK_BULLDOZABLE,
            9  | TILEMASK_BULLDOZABLE,   11 | TILEMASK_BULLDOZABLE,     2,                                 13 | TILEMASK_BULLDOZABLE,
            7  | TILEMASK_BULLDOZABLE,   9  | TILEMASK_BULLDOZABLE,     5  | TILEMASK_BULLDOZABLE,     2
        ];

        this.map.filter(
            function(x, y, value) {

                if (value !== REDGE) {
                    return null;
                }

                var bitIndex = 0;

                for (var z = 0;
                     z < 4;
                     z++) {

                    bitIndex = bitIndex << 1;

                    var xTemp = x + dx[z];
                    var yTemp = y + dy[z];

                    if (self.testBounds(xTemp, yTemp)) {

                        var tileTemp = self.getTile(xTemp, yTemp) & self.TILEMASK_TILE;

                        if ((tileTemp !== DIRT) &&
                            ((tileTemp < WOODS_LOW) ||
                             (tileTemp > WOODS_HIGH))) {

                            bitIndex++;

                        }
                    }

                }

                var temp = table[bitIndex & 15];

                if ((temp !== RIVER) &&
                    self.getRandom(1)) {
                    temp++;
                }

                return temp;

            });

    };


    p.isTree = function isTree(tile) {

        tile &= TILEMASK_TILE;

        return ((tile >= WOODS_LOW) &&
                (tile <= WOODS_HIGH));

    };


    p.smoothTrees = function smoothTrees() {

        var self = this;

        this.map.filter(
            function(x, y, value) {

                if (!self.isTree(value)) {
                    return null;
                }

                return this.getSmoothTreesAt(x, y, value);

            });
    };



    p.getSmoothTreesAt = function getSmoothTreesAt(x, y, tile) {

        if (!this.isTree(x, y)) {
            return null;
        }

        var bitIndex = 0;
        for (var z = 0;
             z < 4;
             z++) {

            bitIndex = bitIndex << 1;

            var xTemp = x + dx[z];
            var yTemp = y + dy[z];

            if (this.testBounds(xTemp, yTemp) &&
                this.isTree(this.getTile(xTemp, yTemp))) {

                bitIndex++;

            }

        }

        var temp = treeTable[bitIndex & 15];

        if (temp) {

            if (temp !== WOODS) {
                if ((x + y) & 1) {
                    temp = temp - 8;
                }
            }

            return temp | TILE_BULLDOZABLE | TILE_BURNABLE;

        } else {

            return temp;

        }

    };


    p.smoothTreesAt = function smoothTreesAt(x, y, preserve, effects) {

        var tile = effects.getTile(x, y);
        var newTile = this.getSmoothTreesAt(x, y, tile);

        if (((newTile !== 0) &&
             (newTile !== null)) ||
            !preserve) {
            effects.setTile(x, y, newTile);
        }

        effects.modifyWorld();

    };


    p.doRivers = function doRivers(x, y) {

        // Global direction of the river
        var riverDir =
                DIR_NORTH + (this.getRandom(3) * 2);

        this.doBRiver(
            x,
            y,
            riverDir,
            riverDir);

        riverDir =
            this.rotate180(
                riverDir);

        var terrainDir =
                this.doBRiver(
                    x,
                    y,
                    riverDir,
                    riverDir);

        riverDir =
            DIR_NORTH +
            (this.getRandom(3) * 2);

        this.doSRiver(
            x,
            y,
            riverDir,
            terrainDir);

    };


    p.doBRiver = function doBRiver(x, y, riverDir, terrainDir) {

        var rate1, rate2;

        if (this.terrainCurveLevel < 0) {
            rate1 = 100;
            rate2 = 200;
        } else {
            rate1 = this.terrainCurveLevel + 10;
            rate2 = this.terrainCurveLevel + 100;
        }

        while (this.testBounds(x + 4, y + 4)) {
            this.plopBRiver(x, y);
            if (this.getRandom(rate1) < 10) {
                terrainDir = riverDir;
            } else {
                if (this.getRandom(rate2) > 90) {
                    terrainDir = this.rotate45(terrainDir);
                }
                if (this.getRandom(rate2) > 90) {
                    terrainDir = this.rotate45(terrainDir, 7);
                }
            }

            x += this.directionDX[terrainDir];
            y += directionDY[terrainDir];

        }

        return terrainDir;

    };


    p.doSRiver = function doSRiver(riverPos, riverDir, terrainDir) {

        var rate1, rate2;

        if (this.terrainCurveLevel < 0) {
            rate1 = 100;
            rate2 = 200;
        } else {
            rate1 = this.terrainCurveLevel + 10;
            rate2 = this.terrainCurveLevel + 100;
        }

        while (this.testBounds(x + 3, y + 3)) {

            this.plopSRiver(x, y);

            if (this.getRandom(rate1) < 10) {
                terrainDir = riverDir;
            } else {
                if (this.getRandom(rate2) > 90) {
                    terrainDir = this.rotate45(terrainDir);
                }
                if (this.getRandom(rate2) > 90) {
                    terrainDir = this.rotate45(terrainDir, 7);
                }
            }

            x += this.directionDX[terrainDir];
            y += directionDY[terrainDir];

        }

        return terrainDir;

    };


    p.putOnMap = function putOnMap(tile, x, y) {

        if (tile === 0) {
            return;
        }

        if (!this.testBounds(x, y)) {
            return;
        }

        var temp = this.getTile(x, y) & TILEMASK_TILE;

        if ((temp !== DIRT) &&
            (((temp === RIVER) &&
              (tile !== CHANNEL)) ||
             (temp === CHANNEL))) {

            return;

        }

        this.setTile(x, y, tile);

    };


    p.plopBRiver = function plopBRiver(x, y) {

        for (var xx = 0;
             xx < 9;
             xx++) {

            for (var yy = 0;
                 yy < 9;
                 yy++) {

                this.putOnMap(
                    bigRiverMatrix[yy][xx],
                    x + xx,
                    y + yy);

            }

        }

    };


    p.plopSRiver = function plopSRiver(x, y) {

        for (var xx = 0;
             xx < 6;
             xx++) {

            for (var yy = 0;
                 yy < 6;
                 yy++) {

                this.putOnMap(
                    smallRiverMatrix[yy][xx],
                    x + xx,
                    y + yy);

            }

        }

    };


    p.smoothWater = function smoothWater() {

        var x;
        var y;
        var dir;
        var tile;

        for (x = 0;
             x < WORLD_W;
             x++) {

            for (y = 0;
                 y < WORLD_H;
                 y++) {

                tile =
                    this.getTile(x, y) & TILEMASK_TILE;

                // If tile is water:
                if ((tile >= WATER_LOW) &&
                    (tile <= WATER_HIGH)) {

                    for (dir = DIR_BEGIN;
                         dir < DIR_END;
                         dir = this.increment90(dir)) {

                        // If getting a tile off-map, condition below fails.
                        tile = this.getTileFromMap(x, y, dir, WATER_LOW);

                        // If nearest object is not water:
                        if ((tile < WATER_LOW) ||
                            (tile > WATER_HIGH)) {

                            this.setTile(x, y, REDGE); // Set river edge.

                            break; // Continue with next tile.

                        }

                    }

                }

            }

        }

        for (x = 0;
             x < WORLD_W;
             x++) {

            for (y = 0;
                 y < WORLD_H;
                 y++) {

                tile = this.getTile(x, y) & TILEMASK_TILE;

                // If water which is not a channel:
                if ((this.tile !== CHANNEL) &&
                    (tile >= WATER_LOW) &&
                    (tile <= WATER_HIGH)) {

                    var makeRiver = true; // Make tile a river.

                    for (dir = DIR_BEGIN;
                         dir < DIR_END;
                         dir = this.increment90(dir)) {

                        // If getting a tile off-map, condition below fails.
                        tile = this.getTileFromMap(x, y, dir, WATER_LOW);

                        // If nearest object is not water:
                        if ((tile < WATER_LOW) ||
                            (tile > WATER_HIGH)) {

                            makeRiver = false;

                            break;

                        }

                    }

                    if (makeRiver) {

                        this.setTile(x, y, RIVER); // Make it a river.

                    }

                }

            }

        }

        for (x = 0;
             x < WORLD_W;
             x++) {

            for (y = 0; y < WORLD_H;
                 y++) {


                tile =
                    this.getTile(x, y) & TILEMASK_TILE;

                // If woods:
                if ((tile >= WOODS_LOW) &&
                    (tile <= WOODS_HIGH)) {

                    for (dir = DIR_BEGIN;
                         dir < DIR_END;
                         dir = this.increment90(dir)) {

                        // If getting a tile off-map, condition below fails.
                        tile = this.getTileFromMap(x, y, dir, TILE_INVALID);

                        if ((tile === RIVER) ||
                            (tile === CHANNEL)) {
                            this.setTile(x, y, REDGE); // Make it water's edge.

                            break;

                        }

                    }

                }

            }

        }

    };


    p.drawMonth = function drawMonth(hist, s, scale) {

        for (var x = 0;
             x < 120;
             x++) {

            var val = Math.floor(hist[x] * scale);

            s[119 - x] = this.clamp(val, 0, 255);

        }

    };


    p.changeCensus = function changeCensus() {

        this.censusChanged = true;

    };


    p.graphDoer = function graphDoer() {

        if (this.censusChanged) {
            this.censusChanged = false;
            this.doUpdateHistory();
        }

    };


    p.initGraphs = function initGraphs() {

        if (!this.historyInitialized) {
            this.historyInitialized = true;
        }

    };


    p.initGraphMax = function initGraphMax() {

        var x;

        this.resHist10Max = 0;
        this.comHist10Max = 0;
        this.indHist10Max = 0;

        for (x = 118;
             x >= 0;
             x--) {

            if (this.resHist[x] < 0) {
                this.resHist[x] = 0;
            }
            if (this.comHist[x] < 0) {
                this.comHist[x] = 0;
            }
            if (this.indHist[x] < 0) {
                this.indHist[x] = 0;
            }

            this.resHist10Max = Math.max(this.resHist10Max, this.resHist[x]);
            this.comHist10Max = Math.max(this.comHist10Max, this.comHist[x]);
            this.indHist10Max = Math.max(this.indHist10Max, this.indHist[x]);

        }

        this.graph10Max =
            Math.max(
                this.resHist10Max,
                this.comHist10Max,
                this.indHist10Max);

        this.resHist120Max = 0;
        this.comHist120Max = 0;
        this.indHist120Max = 0;

        for (x = 238;
             x >= 120;
             x--) {

            if (this.resHist[x] < 0) {
                this.resHist[x] = 0;
            }
            if (this.comHist[x] < 0) {
                this.comHist[x] = 0;
            }
            if (this.indHist[x] < 0) {
                this.indHist[x] = 0;
            }

            this.resHist120Max = Math.max(this.resHist120Max, this.resHist[x]);
            this.comHist120Max = Math.max(this.comHist120Max, this.comHist[x]);
            this.indHist120Max = Math.max(this.indHist120Max, this.indHist[x]);

        }

        this.graph120Max =
            Math.max(
                this.resHist120Max,
                this.comHist120Max,
                this.indHist120Max);

    };


    p.getHistoryData = function getHistoryData(historyType) {

        switch (historyType) {

            case HISTORY_TYPE_RES:
                return this.resHist;

            case HISTORY_TYPE_COM:
                return this.comHist;

            case HISTORY_TYPE_IND:
                return this.indHist;

            case HISTORY_TYPE_MONEY:
                return this.moneyHist;

            case HISTORY_TYPE_CRIME:
                return this.crimeHist;

            case HISTORY_TYPE_POLLUTION:
                return this.pollutionHist;

            default:
                return null;

        }

    };


    p.getHistoryRange = function getHistoryRange(historyScale) {

        switch (historyScale) {

            case HISTORY_SCALE_SHORT:
                return 0;

            case HISTORY_SCALE_LONG:
                return 120;

            default:
                return null;

        }

    };


    p.getHistoryRange = function getHistoryRange(historyType, historyScale) {

        var history = this.getHistoryData(historyType);
        var offset = this.getHistoryOffset(historyScale);

        if ((history === null) ||
            (offset === null)) {
            return [0, 0];
        }

        var minVal = 32000;
        var maxVal = -32000;

        for (var i = 0;
             i < this.historyType.HISTORY_COUNT;
             i++) {

            var val = history[i + offset];

            minVal = Math.min(val, minVal);
            maxVal = Math.max(val, maxVal);

        }

        return [minVal, maxVal];

    };


    p.getHistory = function getHistory(historyType, historyScale, historyIndex) {

        var history = this.getHistoryData(historyType);
        var offset = this.getHistoryOffset(historyScale);

        if ((history === null) ||
            (offset === null) ||
            (historyIndex < 0) ||
            (historyIndex >= this.historyType.HISTORY_COUNT)) {
            return 0;
        }

        var result = history[historyIndex + offset];

        return result;

    };


    p.setHistory = function setHistory(historyType, historyScale, historyIndex, historyValue) {

        var history = this.getHistoryData(historyType);
        var offset = this.getHistoryOffset(historyScale);

        if ((history === null) ||
            (offset === null) ||
            (historyIndex < 0) ||
            (historyIndex >= this.historyType.HISTORY_COUNT)) {
            return;
        }

        history[historyIndex + offset] = historyValye;

    };


    p.initWillStuff = function initWillStuff() {

        this.randomlySeedRandom();
        this.initGraphMax();
        this.destroyAllSprites();

        this.roadEffect = MAX_ROAD_EFFECT;
        this.policeEffect = MAX_POLICE_STATION_EFFECT;
        this.fireEffect = MAX_FIRE_STATION_EFFECT;
        this.cityScore = 500;
        this.cityPop = -1;
        this.cityTimeLast = -1;
        this.cityYearLast = -1;
        this.cityMonthLast = -1;
        this.totalFundsLast = -1;
        this.resLast = this.comLast = this.indLast = -999999;
        this.roadFund = 0;
        this.policeFund = 0;
        this.fireFund = 0;
        this.valveFlag = true;
        this.disasterEvent = SCENARIO_NONE;
        this.taxFlag = false;
        this.categoryLast = null;
        this.cityPopLast = 0;

        this.populationDensityMap.clear();
        this.trafficDensityMap.clear();
        this.pollutionDensityMap.clear();
        this.landValueMap.clear();
        this.crimeRateMap.clear();
        this.terrainDensityMap.clear();
        this.rateOfGrowthMap.clear();
        this.comRateMap.clear();
        this.policeStationMap.clear();
        this.policeStationEffectMap.clear();
        this.fireStationMap.clear();
        this.fireStationEffectMap.clear();

        this.doNewGame();
        this.doUpdateHeads();

    };


    p.getMicropolisAppName = function getMicropolisAppName() {

        return this.getMicroplisEdition() + this.getMicroplisVersion();

    };


    p.getMicropolisEdition = function getMicropolisEdition() {

        return MICROPOLIS_EDITION;

    };


    p.getMicropolisVersion = function getMicropolisVersion() {

        return MICROPOLIS_VERSION;

    };


    p.simUpdate = function simUpdate() {

        if (this.simSpeed) {
            this.tilesAnimated = false;
        }

        this.doUpdateHeads();
        this.graphDoer();
        this.updateBudget();
        this.scoreDoer();

    };


    p.simLoop = function simLoop(doSim) {

        if (doSim) {
            this.simFrame();
        }

        this.moveObjects();
        //this.doSimRobots();

        this.simLoops++;

    };


    p.startTicking = function startTicking() {

        this.stopTicking();

        this.tickTimer = 
            setTimeout(
                $.proxy(this.handleTick, this),
                this.tickDelay);

    };


    p.stopTicking = function stopTicking() {

        if (!this.tickTimer) {
            return;
        }

        clearTimeout(
            this.tickTimer);
        this.tickTimer = null;

    };


    p.handleTick = function handleTick() {

        this.simTick();

        if (!this.paused) {
            this.startTicking();
        }

    };


    p.simTick = function simTick() {

        //console.log("Micropolis tick:", "simSpeed", this.simSpeed);

        if (this.simSpeed) {

            for (this.simPass = 0;
                 this.simPass < this.simPasses;
                 this.simPass++) {

                this.simLoop(true);

            }

        }

        this.simUpdate();

    };


    p.sendMessages = function sendMessages() {

        // Running a scenario, and waiting it to 'end' so we can give a score
        if ((this.scenario > SCENARIO_NONE) &&
            (this.scoreType > SCENARIO_NONE) &&
            (this.scoreWait > 0)) {

            this.scoreWait--;

            if (this.scoreWait === 0) {
                this.doScenarioScore(this.scoreType);
            }

        }

        this.checkGrowth();

        this.totalZonePop =
            this.resZonePop +
            this.comZonePop +
            this.indZonePop;

        var powerPop =
            this.nuclearpowerPop +
            this.coalpowerPop;

        switch (this.cityTime & 63) {

            case 1:
                if ((this.totalZonePop / 4) >= this.resZonePop) {
                    this.sendMessage('NEED_MORE_RESIDENTIAL', null, null, false, true);
                }
                break;

            case 5:
                if ((this.totalZonePop / 8) >= this.comZonePop) {
                    this.sendMessage('NEED_MORE_COMMERCIAL', null, null, false, true);
                }
                break;

            case 10:
                if ((this.totalZonePop / 8) >= this.indZonePop) {
                    this.sendMessage('NEED_MORE_INDUSTRIAL', null, null, false, true);
                }
                break;

            case 14:
                if ((this.totalZonePop > 10) &&
                    (this.totalZonePop * 2) > this.roadTotal) {
                    this.sendMessage('NEED_MORE_ROADS', null, null, false, true);
                }
                break;

            case 18:
                if ((this.totalZonePop > 50) &&
                    (this.totalZonePop > this.railTotal)) {
                    this.sendMessage('NEED_MORE_RAILS', null, null, false, true);
                }
                break;

            case 22:
                if ((this.totalZonePop > 10) &&
                    (this.powerPop === 0)) {
                    this.sendMessage('NEED_ELECTRICITY', null, null, false, true);
                }
                break;

            case 26:
                if ((this.resPop > 500) &&
                    (this.stadiumPop === 0)) {
                    this.sendMessage('NEED_STADIUM', null, null, false, true);
                    this.resCap = true;
                } else {
                    this.resCap = false;
                }
                break;

            case 28:
                if ((this.indPop > 70) &&
                    (this.seaportPop === 0)) {
                    this.sendMessage('NEED_SEAPORT', null, null, false, true);
                    this.indCap = true;
                } else {
                    this.indCap = false;
                }
                break;

            case 30:
                if ((this.comPop > 100) &&
                    (this.airportPop === 0)) {
                    this.sendMessage('NEED_AIRPORT', null, null, false, true);
                    this.comCap = true;
                } else {
                    this.comCap = false;
                }
                break;

            case 32:
                // Dec score for unpowered zones.
                var tm =
                    this.unpoweredZoneCount + this.poweredZoneCount;
                if (tm > 0) {
                    if ((this.poweredZoneCount / tm) < 0.7) {
                        this.sendMessage('BLACKOUTS_REPORTED', null, null, false, true);
                    }
                }
                break;

            case 35:
                if (this.pollutionAverage > 60) {
                    this.sendMessage('HIGH_POLLUTION', null, null, true, true);
                }
                break;

            case 42:
                if (this.crimeAverage > 100) {
                    this.sendMessage('HIGH_CRIME', null, null, true, true);
                }
                break;

            case 45:
                if ((this.totalPop > 60) &&
                    (this.fireStationPop === 0)) {
                    this.sendMessage('NEED_FIRE_STATION', null, null, false, true);
                }
                break;

            case 48:
                if ((this.totalPop > 60) &&
                    (this.policeStationPop === 0)) {
                    this.sendMessage('NEED_POLICE_STATION', null, null, false, true);
                }
                break;

            case 51:
                if (this.cityTax > 12) {
                    this.sendMessage('TAX_TOO_HIGH', null, null, false, true);
                }
                break;

            case 54:
                // If roadEffect < 5/8 of max effect
                if ((this.roadEffect < (5 * MAX_ROAD_EFFECT / 8)) &&
                    (this.roadTotal > 30)) {
                    this.sendMessage('ROAD_NEEDS_FUNDING', null, null, false, true);
                }
                break;

            case 57:
                // If fireEffect < 0.7 of max effect
                if ((this.fireEffect < (7 * MAX_FIRE_STATION_EFFECT / 10)) &&
                    (this.totalPop > 20)) {
                    this.sendMessage('FIRE_STATION_NEEDS_FUNDING', null, null, false, true);
                }
                break;

            case 60:
                // If policeEffect < 0.7 of max effect
                if ((this.policeEffect < (7 * MAX_POLICE_STATION_EFFECT / 10)) &&
                    (this.totalPop > 20)) {
                    this.sendMessage('POLICE_NEEDS_FUNDING', null, null, false, true);
                }
                break;

            case 63:
                if (this.trafficAverage > 60) {
                    this.sendMessage('TRAFFIC_JAMS', null, null, true, true);
                }
                break;

            default:
                break;

        }

    };


    p.checkGrowth = function checkGrowth() {

        if ((this.cityTime & 3) !== 0) {
            return;
        }

        var category = null;
        var thisCityPop = this.getPopulation();

        if (this.cityPopLast > 0) {

            var lastClass = this.getCityClass(this.cityPopLast);
            var newClass = this.getCityClass(thisCityPop);

            if (lastClass !== newClass) {

                // Switched class, find appropiate message.
                switch (newClass) {

                    case CITYCLASS_VILLAGE:
                        // Don't mention it.
                        break;

                    case CITYCLASS_TOWN:
                        category = 'REACHED_TOWN';
                        break;

                    case CITYCLASS_CITY:
                        category = 'REACHED_CITY';
                        break;

                    case CITYCLASS_CAPITAL:
                        category = 'REACHED_CAPITAL';
                        break;

                    case CITYCLASS_METROPOLIS:
                        category = 'REACHED_METROPOLIS';
                        break;

                    case CITYCLASS_MEGALOPOLIS:
                        category = 'REACHED_MEGALOPOLIS';
                        break;

                    default:
                        break;

                }
            }
        }

        if ((category > 0) &&
            (category !== this.categoryLast)) {

            this.sendMessage(category, null, null, true, true);

            this.categoryLast = category;
        }

        this.cityPopLast = thisCityPop;

    };


    p.doScenarioScore = function doScenarioScore(scenario) {

        var message = 'SCENARIO_LOST'; // You lose.

        switch (scenario) {

            case SCENARIO_DULLSVILLE:
                if (this.cityClass >= CITYCLASS_METROPOLIS) {
                    message = 'SCENARIO_WON';
                }
                break;

            case SCENARIO_SAN_FRANCISCO:
                if (this.cityClass >= CITYCLASS_METROPOLIS) {
                    message = 'SCENARIO_WON';
                }
                break;

            case SCENARIO_HAMBURG:
                if (this.cityClass >= CITYCLASS_METROPOLIS) {
                    message = 'SCENARIO_WON';
                }
                break;

            case SCENARIO_BERN:
                if (this.trafficAverage < 80) {
                    message = 'SCENARIO_WON';
                }
                break;

            case SCENARIO_TOKYO:
                if (this.cityScore > 500) {
                    message = 'SCENARIO_WON';
                }
                break;

            case SCENARIO_DETROIT:
                if (this.crimeAverage < 60) {
                    message = 'SCENARIO_WON';
                }
                break;

            case SCENARIO_BOSTON:
                if (this.cityScore > 500) {
                    message = 'SCENARIO_WON';
                }
                break;

            case SCENARIO_RIO:
                if (this.cityScore > 500) {
                    message = 'SCENARIO_WON';
                }
                break;

            default:
                break;

        }

        this.sendMessage(message, null, null, true, true);

        if (message === 'SCENARIO_LOST') {
            this.doLoseScenario();
        } else {
            this.doWinScenario();
        }

    };


    p.sendMessage = function sendMessage(messageID, x, y, picture, important) {

        this.makeMessageSound(messageID, x, y);
        this.doSendMessage(messageID, x, y, picture, important);

    };


    p.makeMessageSound = function makeMessageSound(messageID, x, y) {

        switch (messageID) {

            case 'TRAFFIC_JAMS':
                if (this.getRandom(5) === 1) {
                    this.makeSound('city', 'HonkHonkMed', x, y);
                } else if (this.getRandom(5) === 1) {
                    this.makeSound('city', 'HonkHonkLow', x, y);
                } else if (this.getRandom(5) === 1) {
                    this.makeSound('city', 'HonkHonkHigh', x, y);
                }
                break;

            case 'HIGH_CRIME':
            case 'FIRE_REPORTED':
            case 'TORNADO_SIGHTED':
            case 'EARTHQUAKE':
            case 'PLANE_CRASHED':
            case 'SHIP_CRASHED':
            case 'TRAIN_CRASHED':
            case 'HELICOPTER_CRASHED':
                this.makeSound('city', 'Siren', x, y);
                break;

            case  'MONSTER_SIGHTED':
                this.makeSound('city', 'Monster', x, y);
                break;

            case 'FIREBOMBING':
                this.makeSound('city', 'ExplosionLow', x, y);
                this.makeSound('city', 'Siren', x, y);
                break;

            case 'NUCLEAR_MELTDOWN':
                this.makeSound('city', 'ExplosionHigh', x, y);
                this.makeSound('city', 'ExplosionLow', x, y);
                this.makeSound('city', 'Siren', x, y);
                break;

            case 'RIOTS_REPORTED':
                this.makeSound('city', 'Siren', x, y);
                break;

            default:
                break;

        }

    };


    p.testForConductive = function testForConductive(x, y, testDir) {

        x += this.directionDX[testDir];
        y += directionDY[testDir];

        if (this.map.onMap(x, y)) {

            var tile = this.getTile(x, y);

            if (((tile & TILEMASK_CONDUCTIVE) !== 0) &&
                !powerGridMap.worldGet(x, y)) {
                return true;
            }

        }

        return false;

    };


    p.doPowerScan = function doPowerScan() {

        // Clear power map.
        this.powerGridMap.clear();

        // Power that the combined coal and nuclear power plants can deliver.
        var maxPower =
                (this.coalPowerPop * COAL_POWER_STRENGTH) +
                (this.nuclearPowerPop * NUCLEAR_POWER_STRENGTH);

        var numPower = 0; // Amount of power used.

        while (this.powerStack.length > 0) {

            var pos = this.pullPowerStack();
            var x = pos[0];
            var y = pos[1];
            var anyDir = DIR_INVALID;

            do {

                numPower++;

                if (numPower > maxPower) {
                    this.sendMessage('NOT_ENOUGH_POWER', null, null, false, true);
                    return;
                }

                if (anyDir !== DIR_INVALID) {

                    x += this.directionDX[anyDir];
                    y += directionDY[anyDir];

                }

                this.powerGridMap.worldSet(x, y, 1);

                var conNum = 0;
                var dir = DIR_BEGIN;

                while ((dir < DIR_END) &&
                       (conNum < 2)) {

                    if (this.testForConductive(x, y, dir)) {
                        conNum++;
                        anyDir = dir;
                    }

                    dir = this.increment90(dir);
                }
                if (conNum > 1) {
                    this.pushPowerStack([x, y]);
                }

            } while (conNum);

        }

    };


    p.pushPowerStack = function pushPowerStack(pos) {

        this.powerStack.push(pos);

    };


    p.pullPowerStack = function pullPowerStack() {

        return this.powerStack.pop();

    };


    p.simRandom = function simRandom() {

        this.nextRandom = Math.floor(this.nextRandom * 1103515245 + 12345) & 0xffffffff;

        return (this.nextRandom & 0xffff00) >> 8;

    };


    p.getRandom = function getRandom(range) {

        range++; /// @bug Increment may cause range overflow.
        var maxMultiple = Math.floor(0xffff / range) * range;

        var rnum;
        do {
            rnum = this.getRandom16();
        } while (rnum >= maxMultiple);

        return (rnum % range);

    };


    p.getRandom16 = function getRandom16() {

        return this.simRandom() & 0x0000ffff;

    };


    p.getRandom16Signed = function getRandom16Signed() {

        var i = this.getRandom16();

        if (i > 0x7fff) {
            i = 0x7fff - i;
        }

        return i;

    };


    p.getERandom = function getERandom(limit) {

        var z = this.getRandom(limit);
        var x = this.getRandom(limit);

        return Math.min(z, x);

    };


    p.randomlySeedRandom = function randomlySeedRandom() {

        var seed =
                Math.floor(
                    (Date.now() *
                     Math.random()) +
                        (Math.random() * 0xffff));

        this.seedRandom(seed);

    };


    p.seedRandom = function seedRandom(seed) {

        this.nextRandom = seed;

    };


    p.loadResources = function loadResources() {

        // TODO: load resources.

        // this.resources
        // this.strings

    };


    p.getResource = function getResource(name) {

        return this.resources[name];

    };


    p.getIndString = function getIndString(str, id, num) {

        return this.strings[str + '_' + id][num];

    };

    p.smoothStationMap = function smoothStationMap(map) {

        this.tempMap4.copy(map);

        for (var x = 0;
             x < this.tempMap4.width;
             x++) {

            for (var y = 0; 
                 y < this.tempMap4.height;
                 y++) {

                var edge = 0;

                if (x > 0) {
                    edge += this.tempMap4.get(x - 1, y);
                }

                if (x < this.tempMap4.width - 1) {
                    edge += this.tempMap4.get(x + 1, y);
                }

                if (y > 0) {
                    edge += this.tempMap4.get(x, y - 1);
                }

                if (y < this.tempMap4.height - 1) {
                    edge += this.tempMap4.get(x, y + 1);
                }

                edge = this.tempMap4.get(x, y) + edge / 4;

                map.set(x, y, edge / 2);
            }

        }

    };


    p.fireAnalysis = function fireAnalysis() {

        this.smoothStationMap(this.fireStationMap);
        this.smoothStationMap(this.fireStationMap);
        this.smoothStationMap(this.fireStationMap);

        this.fireStationEffectMap.copy(this.fireStationMap);

        this.newMapFlags[MAP_TYPE_FIRE_RADIUS] = 1;
        this.newMapFlags[MAP_TYPE_DYNAMIC] = 1;

    };


    p.populationDensityScan = function populationDensityScan() {

        // sets: populationDensityMap, , , comRateMap

        this.tempMap1.clear();

        var xTotal = 0;
        var yTotal = 0;
        var zTotal = 0;

        for (var x = 0;
             x < WORLD_W;
             x++) {

            for (var y = 0;
                 y < WORLD_H;
                 y++) {

                var mapValue = this.getTile(x, y);

                if (mapValue & TILEMASK_ZONE) {

                    var mapTile =
                            mapValue & TILEMASK_TILE;
                    var pop =
                            Math.min(
                                this.getPopulationDensity(x, y, mapTile) * 8,
                                254);

                    this.tempMap1.worldSet(x, y, pop & 0xff);

                    xTotal += x;
                    yTotal += y;
                    zTotal++;

                }

            }

        }

        this.doSmooth1(); // tempMap1 -> tempMap2
        this.doSmooth2(); // tempMap2 -> tempMap1
        this.doSmooth1(); // tempMap1 -> tempMap2

        // Copy tempMap2 to populationDensityMap, multiplying by 2

        var self = this;

        this.populationDensityMap.filter(
            function(x, y, value) {
                return self.tempMap2.get(x, y) * 2;
            });

        this.computeComRateMap(); // Compute the comRateMap.


        // Compute new city center.
        if (zTotal > 0) {

            // Find center of mass for city.
            this.cityCenterX = xTotal / zTotal;
            this.cityCenterY = tTotal / Ztotal;

        } else {
            // If pop === 0 then center of map is city center.
            this.cityCenterX = WORLD_W / 2;
            this.cityCenterY = WORLD_H / 2;
        }

        // Set flags for updated maps.
        this.newMapFlags[MAP_TYPE_POPULATION_DENSITY] = 1;
        this.newMapFlags[MAP_TYPE_RATE_OF_GROWTH] = 1;
        this.newMapFlags[MAP_TYPE_DYNAMIC] = 1;

    };


    p.getPopulationDensity = function getPopulationDensity(x, y, tile) {

        if (tile === FREEZ) {
            return this.doFreePop(x, y);
        }

        if (tile < COMBASE) {
            return this.getResZonePop(tile);
        }

        if (tile < INDBASE) {
            return this.getComZonePop(tile) * 8;
        }

        if (tile < PORTBASE) {
            this.getIndZonePop(tile) * 8;
        }

        return 0;

    };


    p.pollutionTerrainLandValueScan = function pollutionTerrainLandValueScan() {

        /// Does pollution, terrain, land value.


        // tempMap3 is a map of development density, smoothed into terrainMap.
        this.tempMap3.clear();

        var pollutionLevel = 0;
        var landValueTotal = 0;
        var landValueNumber = 0;

        var x;
        var y;

        for (x = 0;
             x < this.landValueMap.width;
             x++) {

            for (y = 0;
                 y < this.landValueMap.height; y++) {

                pollutionLevel = 0;

                var landValueFlag = false;
                var worldX = x * 2;
                var worldY = y * 2;

                for (var mx = worldX;
                     mx <= worldX + 1;
                     mx++) {

                    for (var my = worldY;
                         my <= worldY + 1;
                         my++) {

                        var tile = this.getTile(mx, my) & TILEMASK_TILE;

                        if (tile) {

                            if (tile < RUBBLE) {

                                // Increment terrain memory.
                                var value = this.tempMap3.get(x >> 1, y >> 1);

                                this.tempMap3.set(x >> 1, y >> 1, value + 15);

                                continue;
                            }

                            pollutionLevel +=
                                this.getPollutionValue(tile);

                            if (tile >= ROADBASE) {

                                landValueFlag = true;

                            }

                        }

                    }

                }

                pollutionLevel =
                    Math.min(pollutionLevel, 255);

                this.tempMap1.set(x, y, pollutionLevel);

                if (landValueFlag) { // Land value equation.

                    var distance =
                        34 -
                        (this.getCityCenterDistance(worldX, worldY) / 2);

                    distance =
                        distance << 2;

                    distance +=
                        this.terrainDensityMap.get(x >> 1, y >> 1);

                    distance -=
                        this.pollutionDensityMap.get(x, y);

                    if (this.crimeRateMap.get(x, y) > 190) {
                        distance -= 20;
                    }

                    distance =
                        this.clamp(distance, 1, 250);

                    this.landValueMap.set(
                        x, y, distance);

                    landValueTotal += 
                        distance;

                    landValueNumber++;

                } else {

                    this.landValueMap.set(x, y, 0);

                }

            }

        }

        if (landValueNumber > 0) {

            this.landValueAverage =
                Math.floor(landValueTotal / landValueNumber);

        } else {

            this.landValueAverage = 0;

        }

        this.doSmooth1(); // tempMap1 -> tempMap2
        this.doSmooth2(); // tempMap2 -> tempMap1

        var pollutionMax = 0;
        var pollutionNumber = 0;
        var pollutionTotal = 0;

        for (x = 0;
             x < WORLD_W;
             x += this.pollutionDensityMap.blockSize) {

            for (y = 0;
                 y < WORLD_H;
                 y += this.pollutionDensityMap.blockSize)  {

                z = this.tempMap1.worldGet(x, y);

                this.pollutionDensityMap.worldSet(x, y, z);

                if (z) { // Get pollute average/

                    pollutionNumber++;
                    pollutionTotal += z;

                    // Find max pollutiom for monster.

                    if ((z > pollutionMax) ||
                        ((z === pollutionMax) &&
                         (this.getRandom16() & 3) === 0)) {

                        pollutionMax = z;
                        this.pollutionMaxX = x;
                        this.pollutionMaxY = y;

                    }

                }

            }

        }

        if (pollutionNumber) {

            this.pollutionAverage =
                Math.floor(pollutionTotal / pollutionNumber);

        } else {

            this.pollutionAverage = 0;

        }

        this.smoothTerrain();

        this.newMapFlags[MAP_TYPE_POLLUTION] = 1;
        this.newMapFlags[MAP_TYPE_LAND_VALUE] = 1;
        this.newMapFlags[MAP_TYPE_DYNAMIC] = 1;

    };


    p.getPollutionValue = function getPollutionValue(tile) {

        if (tile < POWERBASE) {

            if (tile >= HTRFBASE) {
                return 75; // Heavy traffic.
            }

            if (tile >= LTRFBASE) {
                return 50; // light traffic.
            }

            if (tile < ROADBASE) {

                if (tile > FIREBASE) {
                    return 90;
                }

                if (tile >= RADTILE) {
                    return 255; // Radioactivity.
                }

            }

            return 0;
        }

        if (tile <= LASTIND) {
            return 0;
        }

        if (tile < PORTBASE) {
            return 50; // Industrial.
        }

        if (tile <= LASTPOWERPLANT) {
            return 100; // prt, aprt, cpp.
        }

        return 0;

    };


    p.getCityCenterDistance = function getCityCenterDistance(x, y) {


        var xDistance;
        if (x > this.cityCenterX) {
            xDistance = x - this.cityCenterX;
        } else {
            xDistance = this.cityCenterX - x;
        }

        var yDistance;
        if (y > this.cityCenterY) {
            yDisance = y - this.cityCenterY;
        } else {
            yDistance = this.cityCenterY - y;
        }

        return Math.min(xDistance + yDistance, 64);

    };


    p.crimeScan = function crimeScan() {

        this.smoothStationMap(this.policeStationMap);
        this.smoothStationMap(this.policeStationMap);
        this.smoothStationMap(this.policeStationMap);

        var totz = 0;
        var numz = 0;
        var cmax = 0;

        for (var x = 0;
             x < WORLD_W;
             x += this.crimeRateMap.blockSize) {

            for (var y = 0;
                 y < WORLD_H;
                 y += this.crimeRateMap.blockSize) {

                var z =
                    this.landValueMap.worldGet(x, y);

                if (z > 0) {

                    ++numz;

                    z =
                        128 - z;

                    z +=
                        this.populationDensityMap.worldGet(x, y);

                    z =
                        Math.min(z, 300);

                    z -=
                        this.policeStationMap.worldGet(x, y);

                    z = 
                        this.clamp(z, 0, 250);

                    this.crimeRateMap.worldSet(x, y, z);

                    totz += z;

                    // Update new crime hot-spot
                    if ((z > cmax) ||
                        ((z === cmax) &&
                         ((this.getRandom16() & 3) === 0))) {

                        cmax = z;
                        this.crimeMaxX = x;
                        this.crimeMaxY = y;

                    }

                } else {

                    this.crimeRateMap.worldSet(x, y, 0);

                }

            }

        }

        if (numz > 0) {

            this.crimeAverage = totz / numz;

        } else {

            this.crimeAverage = 0;

        }

        this.policeStationEffectMap.set(this.policeStationMap);

        this.newMapFlags[MAP_TYPE_CRIME] = 1;
        this.newMapFlags[MAP_TYPE_POLICE_RADIUS] = 1;
        this.newMapFlags[MAP_TYPE_DYNAMIC] = 1;

    };


    p.smoothTerrain = function smoothTerrain() {

        if (this.donDither & 1) {

            var dir = 1;
            var x;
            var y;
            var z;
            var val;

            for (x = 0, y = 0, z = 0;
                 x < this.terrainDensityMap.width;
                 x++) {

                for (;
                     (y !== terrainDensityMap.MAP_H) && (y !== -1);
                     y += dir) {

                    z +=
                        this.tempMap3.get((x === 0) ? x : (x - 1), y) +
                        this.tempMap3.get((x === (this.terrainDensityMap.width - 1)) ? x : (x + 1), y) +
                        this.tempMap3.get(x, (y === 0) ? (0) : (y - 1)) +
                        this.tempMap3.get(x, (y === (this.terrainDensityMap.width - 1)) ? y : (y + 1)) +
                        (this.tempMap3.get(x, y) << 2);

                    val = Math.floor(z / 8);

                    terrainDensityMap.set(x, y, val);

                    z &= 0x7;

                }

                dir = -dir;
                y += dir;

            }

        } else {

            for (x = 0;
                 x < this.terrainDensityMap.width;
                 x++) {

                for (y = 0;
                     y < this.terrainDensityMap.height;
                     y++) {

                    z = 0;

                    if (x > 0) {
                        z += this.tempMap3.get(x - 1, y);
                    }

                    if (x < (this.terrainDensityMap.width - 1)) {
                        z += this.tempMap3.get(x + 1, y);
                    }

                    if (y > 0) {
                        z += this.tempMap3.get(x, y - 1);
                    }

                    if (y < (this.terrainDensityMap.height - 1)) {
                        z += this.tempMap3.get(x, y + 1);
                    }

                    val =
                        Math.floor(((z / 4) + this.tempMap3.get(x, y)) / 2);

                    this.terrainDensityMap.set(x, y, val);

                }

            }

        }

    };


    p.smoothDitherMap = function smoothDitherMap(srcMap, destMap, ditherFlag) {

        var dir = 1;
        var x;
        var y;
        var z;

        if (ditherFlag) {

            for (x = 0, y = 0, z = 0;
                 x < srcMap.width;
                 x++) {

                for (;
                     (y !== srcMap.height) && (y !== -1);
                     y += dir) {

                    z +=
                        srcMap.get((x === 0) ? x : (x - 1), y) +
                        srcMap.get((x === srcMap.width - 1) ? x : (x + 1), y) +
                        srcMap.get(x, (y === 0) ? (0) : (y - 1)) +
                        srcMap.get(x, (y === (srcMap.height - 1)) ? y : (y + 1)) +
                        srcMap.get(x, y);

                    val = Math.floor(z / 4) & 0xff;

                    destMap.set(x, y, val);

                    z &= 3;

                }

                dir = -dir;
                y += dir;

            }

        } else {

            for (x = 0;
                 x < srcMap.width;
                 x++) {

                for (y = 0;
                     y < srcMap.height;
                     y++) {

                    z = 0;

                    if (x > 0) {
                        z += srcMap.get(x - 1, y);
                    }

                    if (x < srcMap.width - 1) {
                        z += srcMap.get(x + 1, y);
                    }

                    if (y > 0) {
                        z += srcMap.get(x, y - 1);
                    }

                    if (y < (srcMap.height - 1)) {
                        z += srcMap.get(x, y + 1);
                    }

                    z = (z + srcMap.get(x, y)) >> 2;

                    if (z > 255) {
                        z = 255;
                    }

                    destMap.set(x, y, z);

                }

            }

        }

    };


    p.doSmooth1 = function doSmooth1() {

        this.smoothDitherMap(this.tempMap1, this.tempMap2, this.donDither & 2);

    };


    p.doSmooth2 = function doSmooth2() {

        this.smoothDitherMap(this.tempMap2, this.tempMap1, this.donDither & 4);

    };


    p.computeComRateMap = function computeComRateMap() {

        for (var x = 0;
             x < this.comRateMap.width;
             x++) {

            for (var y = 0;
                 y < this.comRateMap.height;
                 y++) {

                var z =
                    Math.floor(
                        this.getCityCenterDistance(
                            x * 8,
                            y * 8) / 2); // 0..32

                z = z * 4; // 0..128
                z = 64 - z; // 64..-64

                this.comRateMap.set(x, y, z);

            }

        }

    };


    p.simFrame = function simFrame() {


        if (this.simSpeed === 0) {
            return;
        }

        if (++this.speedCycle > 1023) {
            this.speedCycle = 0;
        }

        if ((this.simSpeed === 1) &&
            ((this.speedCycle % 5) !== 0)) {
            return;
        }

        if ((this.simSpeed === 2) &&
            ((this.speedCycle % 3) !== 0)) {
            return;
        }

        this.simulate();

    };


    p.simulate = function simulate() {

        var speedIndex =
            this.clamp(this.simSpeed, 0, 2);

        // The simulator has 16 different phases, which we cycle through
        // according to phaseCycle, which is incremented and wrapped at
        // the end of this switch.

        if (this.initSimLoad) {
            this.phaseCycle = 0;
        } else {
            this.phaseCycle &= 15;
        }

        switch (this.phaseCycle)  {

            case 0:

                this.simCycle++;

                if (this.simCycle > 1023) {
                    this.simCycle = 0; // This is cosmic!
                }

                if (this.doInitialEval) {
                    this.doInitialEval = false;
                    this.cityEvaluation();
                }

                this.cityTime++;
                this.cityTaxAverage += this.cityTax;

                if (!(this.simCycle & 1)) {
                    this.setValves();
                }

                this.clearCensus();

                break;

            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:

                // Scan 1/8 of the map for each of the 8 phases 1..8:
                this.mapScan(
                    (this.phaseCycle - 1) * WORLD_W / 8,
                    this.phaseCycle * WORLD_W / 8);

                break;

            case 9:
                if ((this.cityTime % CENSUS_FREQUENCY_10) === 0) {
                    this.take10Census();
                }

                if ((this.cityTime % CENSUS_FREQUENCY_120) === 0) {
                    this.take120Census();
                }

                if ((this.cityTime % TAX_FREQUENCY) === 0)  {
                    this.collectTax();
                    this.cityEvaluation();
                }

                break;

            case 10:

                if (!(this.simCycle % 5)) {
                    this.decRateOfGrowthMap();
                }

                this.decTrafficMap();

                this.newMapFlags[MAP_TYPE_TRAFFIC_DENSITY] = 1;
                this.newMapFlags[MAP_TYPE_ROAD] = 1;
                this.newMapFlags[MAP_TYPE_ALL] = 1;
                this.newMapFlags[MAP_TYPE_RES] = 1;
                this.newMapFlags[MAP_TYPE_COM] = 1;
                this.newMapFlags[MAP_TYPE_IND] = 1;
                this.newMapFlags[MAP_TYPE_DYNAMIC] = 1;

                this.sendMessages();

                break;

            case 11:

                if ((this.simCycle % speedPowerScan[speedIndex]) === 0) {
                    this.doPowerScan();
                    this.newMapFlags[MAP_TYPE_POWER] = 1;
                    this.newPower = true;
                }

                break;

            case 12:

                if ((this.simCycle % speedPollutionTerrainLandValueScan[speedIndex]) === 0) {
                    this.pollutionTerrainLandValueScan();
                }

                break;

            case 13:

                if ((this.simCycle % speedCrimeScan[speedIndex]) === 0) {
                    this.crimeScan();
                }

                break;

            case 14:

                if ((this.simCycle % speedPopulationDensityScan[speedIndex]) === 0) {
                    this.populationDensityScan();
                }

                break;

            case 15:

                if ((this.simCycle % speedFireAnalysis[speedIndex]) === 0) {
                    this.fireAnalysis();
                }

                this.doDisasters();

                break;

            default:
                break;

        }

        // Go on the the next phase.
        this.phaseCycle = (this.phaseCycle + 1) & 15;

    };


    p.prepareToStart = function prepareToStart() {

        this.phaseCycle = 0;
        this.simCycle = 0;

        if (this.initSimLoad === 2) {
            // If new city.
            this.initSimMemory();
        }

        if (this.initSimLoad === 1) {
            // If city just loaded.
            this.simLoadInit();
        }

        this.setValves();
        this.clearCensus();
        this.mapScan(0, WORLD_W);
        this.doPowerScan();
        this.pollutionTerrainLandValueScan();
        this.crimeScan();
        this.populationDensityScan();
        this.fireAnalysis();
        this.invalidateMaps();

        this.newPower = true;
        this.newMap = true;
        this.censusChanged = true;
        this.totalPop = 1;
        this.doInitialEval = true;

    };


    p.doNilPower = function doNilPower() {

        for (var x = 0;
             x < WORLD_W;
             x++) {

            for (var y = 0;
                 y < WORLD_H;
                 y++) {

                var tile = this.getTile(x, y);

                if (tile & TILEMASK_ZONE) {

                    this.setZonePower(x, y);

                }

            }

        }

    };


    p.decTrafficMap = function decTrafficMap() {

        // Tends to empty trafficDensityMap.
        for (var x = 0;
             x < WORLD_W;
             x += this.trafficDensityMap.blockSize) {

            for (var y = 0;
                 y < WORLD_H;
                 y += this.trafficDensityMap.blockSize) {

                var z = this.trafficDensityMap.worldGet(x, y);

                if (z === 0) {
                    continue;
                }

                if (z <= 24) {
                    this.trafficDensityMap.worldSet(x, y, 0);
                    continue;
                }

                if (z > 200) {
                    this.trafficDensityMap.worldSet(x, y, z - 34);
                } else {
                    this.trafficDensityMap.worldSet(x, y, z - 24);
                }

            }

        }

    };


    p.decRateOfGrowthMap = function decRateOfGrowthMap() {

        // Tends to empty rateOfGrowthMap.

        for (var x = 0;
             x < this.rateOfGrowthMap.width;
             x++) {

            for (var y = 0;
                 y < this.rateOfGrowthMap.height;
                 y++) {

                var z = this.rateOfGrowthMap.get(x, y);

                if (z === 0) {
                    continue;
                }

                if (z > 0) {
                    z--;
                    z = this.clamp(z, -200, 200);
                    this.rateOfGrowthMap.set(x, y, z);
                    continue;
                }

                if (z < 0)  {
                    z++;
                    z = this.clamp(z, -200, 200);
                    this.rateOfGrowthMap.set(x, y, z);
                }

            }

        }

    };


    p.initSimMemory = function initSimMemory() {

        this.setCommonInits();

        for (var x = 0;
             x < 240;
             x++)  {

            this.resHist[x] = 0;
            this.comHist[x] = 0;
            this.indHist[x] = 0;
            this.moneyHist[x] = 128;
            this.crimeHist[x] = 0;
            this.pollutionHist[x] = 0;

        }

        this.crimeRamp = 0;
        this.pollutionRamp = 0;
        this.totalPop = 0;
        this.resValve = 0;
        this.comValve = 0;
        this.indValve = 0;
        this.resCap = false; // Do not block residential growth
        this.comCap = false; // Do not block commercial growth
        this.indCap = false; // Do not block industrial growth

        this.externalMarket = EXTERNAL_MARKET_INIT;
        this.disasterEvent = SCENARIO_NONE;
        this.scoreType = SCENARIO_NONE;

        // This clears powermem.
        this.powerStack = [];
        this.doPowerScan();
        this.newPower = true;

        this.initSimLoad = 0;

    };


    p.simLoadInit = function simLoadInit() {

        this.externalMarket = this.miscHist[1];
        this.resPop = this.miscHist[2];
        this.comPop = this.miscHist[3];
        this.indPop = this.miscHist[4];
        this.resValve = this.miscHist[5];
        this.comValve = this.miscHist[6];
        this.indValve = this.miscHist[7];
        this.crimeRamp = this.miscHist[10];
        this.pollutionRamp = this.miscHist[11];
        this.landValueAverage = this.miscHist[12];
        this.crimeAverage = this.miscHist[13];
        this.pollutionAverage = this.miscHist[14];
        this.gameLevel = this.miscHist[15];

        if (this.cityTime < 0) {
            this.cityTime = 0;
        }

        if (!this.externalMarket) {
            this.externalMarket = EXTERNAL_MARKET_DEFAULT;
        }

        // Set game level
        if ((this.gameLevel > LEVEL_LAST) ||
            (this.gameLevel < LEVEL_FIRST)) {
            this.gameLevel = LEVEL_FIRST;
        }

        this.setGameLevel(this.gameLevel);

        this.setCommonInits();

        // Load cityClass
        this.cityClass = this.miscHist[16];
        if ((this.cityClass > CITYCLASS_MEGALOPOLIS) ||
            (this.cityClass < CITYCLASS_VILLAGE)) {
            this.cityClass = CITYCLASS_VILLAGE;
        }

        this.cityScore = this.miscHist[17];
        if (this.cityScore > 999 || this.cityScore < 1) {
            this.cityScore = 500;
        }

        this.resCap = false;
        this.comCap = false;
        this.indCap = false;

        this.cityTaxAverage = (this.cityTime % 48) * 7;

        // Set power map.
        /// @todo What purpose does this serve? Weird...
        this.powerGridMap.fill(1);
        this.doNilPower();

        if (this.scenario >= SCENARIO_COUNT) {
            this.scenario = SCENARIO_NONE;
        }

        if (this.scenario !== SCENARIO_NONE) {
            this.disasterEvent = this.scenario;
            this.disasterWait = disasterWaitTable[this.disasterEvent];
            this.scoreType = this.disasterEvent;
            this.scoreWait = scoreWaitTable[this.disasterEvent];
        } else {
            this.disasterEvent = SCENARIO_NONE;
            this.disasterWait = 0;
            this.scoreType = SCENARIO_NONE;
            this.scoreWait = 0;
        }

        this.roadEffect = MAX_ROAD_EFFECT;
        this.policeEffect = MAX_POLICE_STATION_EFFECT;
        this.fireEffect = MAX_FIRE_STATION_EFFECT;
        this.initSimLoad = 0;

    };


    p.setCommonInits = function setCommonInits() {

        this.evalInit();
        this.roadEffect = MAX_ROAD_EFFECT;
        this.policeEffect = MAX_POLICE_STATION_EFFECT;
        this.fireEffect = MAX_FIRE_STATION_EFFECT;
        this.taxFlag = false;
        this.taxFund = 0;

    };


    p.setValves = function setValves() {

        /// @todo Make configurable parameters.
        var resPopDenom = 8;
        var birthRate = 0.02;
        var laborBaseMax = 1.3;
        var internalMarketDenom = 3.7;
        var projectedIndPopMin = 5.0;
        var resRatioDefault = 1.3;
        var resRatioMax = 2;
        var comRatioMax = 2;
        var indRatioMax = 2;
        var taxMax = 20;
        var taxTableScale = 600;

        this.miscHist[1] = Math.floor(this.externalMarket);
        this.miscHist[2] = this.resPop;
        this.miscHist[3] = this.comPop;
        this.miscHist[4] = this.indPop;
        this.miscHist[5] = this.resValve;
        this.miscHist[6] = this.comValve;
        this.miscHist[7] = this.indValve;
        this.miscHist[10] = this.crimeRamp;
        this.miscHist[11] = this.pollutionRamp;
        this.miscHist[12] = this.landValueAverage;
        this.miscHist[13] = this.crimeAverage;
        this.miscHist[14] = this.pollutionAverage;
        this.miscHist[15] = this.gameLevel;
        this.miscHist[16] = this.cityClass;
        this.miscHist[17] = this.cityScore;

        var normalizedResPop = this.resPop / resPopDenom;
        this.totalPopLast = this.totalPop;
        this.totalPop = Math.floor(normalizedResPop + this.comPop + this.indPop);

        var employment =
            (this.resPop > 0) ? ((this.comHist[1] + this.indHist[1]) / normalizedResPop) : 1;

        var migration =
            normalizedResPop * (employment - 1);

        var births =
            normalizedResPop * birthRate;

        var projectedResPop =
            normalizedResPop + migration + births;

        // Compute laborBase
        var temp =
            this.comHist[1] + this.indHist[1];
        var laborBase =
            (temp > 0.0) ? (resHist[1] / temp) : 1;

        laborBase = this.clamp(laborBase, 0.0, laborBaseMax);

        var internalMarket =
            (normalizedResPop + this.comPop + this.indPop) / internalMarketDenom;

        var projectedComPop =
            internalMarket * laborBase;

        var projectedIndPop =
            this.indPop *
            laborBase *
            externalMarketParamTable[this.gameLevel];

        projectedIndPop = 
            Math.max(projectedIndPop, projectedIndPopMin);

        var resRatio;
        if (normalizedResPop > 0) {
            resRatio = projectedResPop / normalizedResPop; // projected -vs- actual.
        } else {
            resRatio = resRatioDefault;
        }
        resRatio = Math.min(resRatio, resRatioMax);

        var comRadio;
        if (this.comPop > 0) {
            comRatio = projectedComPop / this.comPop;
        } else {
            comRatio = projectedComPop;
        }
        comRatio = Math.min(comRatio, comRatioMax);

        var indRatio;
        if (this.indPop > 0) {
            indRatio = projectedIndPop / indPop;
        } else {
            indRatio = projectedIndPop;
        }
        resRatio = Math.min(indRatio, indRatioMax);

        // Global tax and game level effects.
        var z = Math.min(this.cityTax + this.gameLevel, taxMax);

        resRatio = ((resRatio - 1) * taxTableScale) + taxTable[z];
        comRatio = ((comRatio - 1) * taxTableScale) + taxTable[z];
        indRatio = ((indRatio - 1) * taxTableScale) + taxTable[z];

        // Ratios are velocity changes to valves.
        this.resValve =
            this.clamp(this.resValve + resRatio, -RES_VALVE_RANGE, RES_VALVE_RANGE);
        this.comValve =
            this.clamp(this.comValve + comRatio, -COM_VALVE_RANGE, COM_VALVE_RANGE);
        this.indValve =
            this.clamp(this.indValve + indRatio, -IND_VALVE_RANGE, IND_VALVE_RANGE);

        if (this.resCap &&
            (this.resValve > 0)) {
            this.resValve = 0; // Need a stadium, so cap resValve.
        }

        if (this.comCap &&
            (this.comValve > 0)) {
            this.comValve = 0; // Need a airport, so cap comValve.
        }

        if (this.indCap &&
            (this.indValve > 0)) {
            this.indValve = 0; // Need an seaport, so cap indValve.
        }

        this.valveFlag = true;

    };


    p.clearCensus = function clearCensus() {

        this.poweredZoneCount = 0;
        this.unpoweredZoneCount = 0;
        this.firePop = 0;
        this.roadTotal = 0;
        this.railTotal = 0;
        this.resPop = 0;
        this.comPop = 0;
        this.indPop = 0;
        this.resZonePop = 0;
        this.comZonePop = 0;
        this.indZonePop = 0;
        this.hospitalPop = 0;
        this.churchPop = 0;
        this.policeStationPop = 0;
        this.fireStationPop = 0;
        this.stadiumPop = 0;
        this.coalPowerPop = 0;
        this.nuclearPowerPop = 0;
        this.seaportPop = 0;
        this.airportPop = 0;
        this.powerStack = []; // Reset before Mapscan.

        this.fireStationMap.clear();
        //this.fireStationEffectMap.clear(); // Added in rev293
        this.policeStationMap.clear();
        //this.policeStationEffectMap.clear(); // Added in rev293

    };


    p.take10Census = function take10Census() {

        // TODO: Make configurable parameters.
        var resPopDenom = 8;
        var x;

        // Put census in historical graphs and scroll data.
        this.resHist10Max = 0;
        this.comHist10Max = 0;
        this.indHist10Max = 0;

        for (x = 118;
             x >= 0;
             x--) {

            this.resHist10Max = Math.max(this.resHist10Max, this.resHist[x]);
            this.comHist10Max = Math.max(this.comHist10Max, this.comHist[x]);
            this.indHist10Max = Math.max(this.indHist10Max, this.indHist[x]);

            this.resHist[x + 1] = this.resHist[x];
            this.comHist[x + 1] = this.comHist[x];
            this.indHist[x + 1] = this.indHist[x];
            this.crimeHist[x + 1] = this.crimeHist[x];
            this.pollutionHist[x + 1] = this.pollutionHist[x];
            this.moneyHist[x + 1] = this.moneyHist[x];

        }

        this.graph10Max = this.resHist10Max;
        this.graph10Max = Math.max(this.graph10Max, this.comHist10Max);
        this.graph10Max = Math.max(this.graph10Max, this.indHist10Max);

        this.resHist[0] = this.resPop / resPopDenom;
        this.comHist[0] = this.comPop;
        this.indHist[0] = this.indPop;

        this.crimeRamp += (this.crimeAverage - this.crimeRamp) / 4;
        this.crimeHist[0] = Math.min(this.crimeRamp, 255);

        this.pollutionRamp += (this.pollutionAverage - this.pollutionRamp) / 4;
        this.pollutionHist[0] = Math.min(this.pollutionRamp, 255);

        x = (this.cashFlow / 20) + 128; // Scale to 0..255.
        this.moneyHist[0] = this.clamp(x, 0, 255);

        this.changeCensus();

        var resPopScaled = this.resPop >> 8;

        if (this.hospitalPop < resPopScaled) {
            this.needHospital = 1;
        }

        if (this.hospitalPop > resPopScaled) {
            this.needHospital = -1;
        }

        if (this.hospitalPop === resPopScaled) {
            this.needHospital = 0;
        }

        var faithfulPop = this.resPopScaled + this.faith;

        if (this.churchPop < faithfulPop) {
            this.needChurch = 1;
        }

        if (this.churchPop > faithfulPop) {
            this.needChurch = -1;
        }

        if (this.churchPop === faithfulPop) {
            this.needChurch = 0;
        }

    };


    p.take120Census = function take120Census() {

        // TODO: Make configurable parameters.
        var resPopDenom = 8;

        // Long term graphs.
        this.resHist120Max = 0;
        this.comHist120Max = 0;
        this.indHist120Max = 0;

        for (var x = 238;
             x >= 120;
             x--)  {

            this.resHist120Max = Math.max(this.resHist120Max, this.resHist[x]);
            this.comHist120Max = Math.max(this.comHist120Max, this.comHist[x]);
            this.indHist120Max = Math.max(this.indHist120Max, this.indHist[x]);

            this.resHist[x + 1] = this.resHist[x];
            this.comHist[x + 1] = this.comHist[x];
            this.indHist[x + 1] = this.indHist[x];
            this.crimeHist[x + 1] = this.crimeHist[x];
            this.pollutionHist[x + 1] = this.pollutionHist[x];
            this.moneyHist[x + 1] = this.moneyHist[x];

        }

        this.graph120Max = this.resHist120Max;
        this.graph120Max = Math.max(this.graph120Max, this.comHist120Max);
        this.graph120Max = Math.max(this.graph120Max, this.indHist120Max);

        this.resHist[120] = this.resPop / resPopDenom;
        this.comHist[120] = this.comPop;
        this.indHist[120] = this.indPop;
        this.crimeHist[120] = this.crimeHist[0] ;
        this.pollutionHist[120] = this.pollutionHist[0];
        this.moneyHist[120] = this.moneyHist[0];
        this.changeCensus();

    };


    p.collectTax = function collectTax() {

        this.cashFlow = 0;

        // @todo Apparently taxFlag is never set to true in MicropolisEngine
        // or the TCL code, so this always runs.
        // @todo Check old Mac code to see if it's ever set, and why.

        if (this.taxFlag) {
            return;
        }

        // If the Tax port is clear.

        /// @todo Do something with z? Check old Mac code to see if it's used.
        var z =
            Math.floor(this.cityTaxAverage / 48);

        this.cityTaxAverage = 0;

        this.policeFund =
            this.policeStationPop * 100;

        this.fireFund =
            this.fireStationPop * 100;

        this.roadFund =
            Math.floor(
                (this.roadTotal + (this.railTotal * 2)) *
                rLevels[this.gameLevel]);

        this.taxFund =
            ((this.totalPop * this.landValueAverage) / 120) *
            this.cityTax *
            fLevels[this.gameLevel];

        if (this.totalPop > 0) {

            // There are people to tax.
            this.cashFlow =
                this.taxFund -
                (this.policeFund + this.fireFund + this.roadFund);

            this.doBudget();

        } else {

            // Nobody lives here.

            this.roadEffect =
                MAX_ROAD_EFFECT;

            this.policeEffect =
                MAX_POLICE_STATION_EFFECT;

            this.fireEffect =
                MAX_FIRE_STATION_EFFECT;

        }

    };


    p.updateFundEffects = function updateFundEffects() {

        // Compute road effects of funding.
        this.roadEffect =
            MAX_ROAD_EFFECT;

        if (this.roadFund > 0) {

            // Multiply with funding fraction.
            this.roadEffect =
                Math.floor(
                    this.roadEffect * this.roadSpend / this.roadFund);

        }

        // Compute police station effects of funding.
        this.policeEffect =
            MAX_POLICE_STATION_EFFECT;

        if (this.policeFund > 0) {

            // Multiply with funding fraction.
            this.policeEffect = 
                Math.floor(
                    policeEffect * policeSpend / policeFund);

        }

        // Compute fire station effects of funding.
        fireEffect = MAX_FIRE_STATION_EFFECT;
        if (fireFund > 0) {

            // Multiply with funding fraction.
            fireEffect =
                Math.floor(
                    fireEffect * fireSpend / fireFund);

        }

        this.mustDrawBudget = true;

    };


    p.mapScan = function mapScan(x1, x2) {

        for (var x = x1;
             x < x2;
             x++) {

            for (var y = 0;
                 y < WORLD_H;
                 y++) {

                var mapVal = this.getTile(x, y);

                if (mapVal === DIRT) {
                    continue;
                }

                var tile = mapVal & TILEMASK_TILE; // Mask off status bits.

                if (tile < FLOOD) {
                    continue;
                }

                // tile >= FLOOD

                if (tile < ROADBASE) {

                    if (tile >= FIREBASE) {

                        this.firePop++;

                        if (!(this.getRandom16() & 3)) {
                            this.doFire(x, y); // 1 in 4 times.
                        }

                        continue;
                    }

                    if (tile < RADTILE) {
                        doFlood(x, y);
                    } else {
                        this.doRadTile(x, y);
                    }

                    continue;
                }

                if (this.newPower &&
                    (mapVal & TILEMASK_CONDUCTIVE)) {

                    // Copy TILEMASK_POWER from powerGridMap
                    this.setZonePower(x, y);

                }

                if ((tile >= ROADBASE) &&
                    (tile < POWERBASE)) {
                    this.doRoad(x, y);
                    continue;
                }

                if (mapVal & TILEMASK_ZONE) { // Process Zones.
                    this.doZone(x, y);
                    continue;
                }

                if ((tile >= RAILBASE) &&
                    (tile < RESBASE)) {
                    this.doRail(x, y);
                    continue;
                }

                if ((tile >= SOMETINYEXP) &&
                    (tile <= LASTTINYEXP)) {
                    // Clear AniRubble.
                    this.setTile(x, y, this.randomRubble());
                }

            }

        }

    };


    p.doRail = function doRail(x, y) {

        this.railTotal++;

        this.generateTrain(x, y);

        if (this.roadEffect < (15 * MAX_ROAD_EFFECT / 16)) {

            // roadEffect < 15/16 of max road, enable deteriorating rail
            if (!(this.getRandom16() & 511)) {

                var curValue = this.getTile(x, y);

                if (!(curValue & TILEMASK_CONDUCTIVE)) {

                    // Otherwise the '(getRandom16() & 31)' makes no sense
                    if (this.roadEffect < (this.getRandom16() & 31)) {
                        var tile = curValue & TILEMASK_TILE;
                        if (tile < (RAILBASE + 2)) {
                            this.setTile(x, y, RIVER);
                        } else {
                            this.setTile(x, y, this.randomRubble());
                        }

                        return;

                    }

                }

            }

        }

    };


    p.doRadTile = function doRadTile(x, y) {

        if ((this.getRandom16() & 4095) === 0) {
            this.setTile(x, y, DIRT); // Radioactive decay.
        }

    };


    p.doRoad = function doRoad(x, y) {

        this.roadTotal++;

        var mapValue = this.getTile(x, y);
        var tile = mapValue & TILEMASK_TILE;

        // this.generateBus(x, y);

        if (this.roadEffect < (15 * MAX_ROAD_EFFECT / 16)) {

            // roadEffect < 15/16 of max road, enable deteriorating road

            if ((this.getRandom16() & 511) === 0) {

                if (!(mapValue & TILEMASK_CONDUCTIVE)) {

                    if (this.roadEffect < (this.getRandom16() & 31)) {

                        if (((tile & 15) < 2) ||
                            ((tile & 15) === 15)) {

                            this.setTile(x, y, RIVER);

                        } else {

                            this.setTile(x, y, this.randomRubble());

                        }

                        return;

                    }

                }

            }

        }

        if ((mapValue & TILEMASK_BURNABLE) === 0) { // If bridge.

            this.roadTotal += 4; // Bridge counts as 4 road tiles

            if (this.doBridge(x, y, tile)) {
                return;
            }

        }

        var tden;
        if (tile < LTRFBASE) {
            tden = 0;
        } else if (tile < HTRFBASE) {
            tden = 1;
        } else {
            this.roadTotal++; // Heavy traffic counts as 2 roads.
            tden = 2;
        }

        var trafficDensity =
            this.trafficDensityMap.worldGet(x, y) >> 6;

        if (trafficDensity > 1) {
            trafficDensity--;
        }

        if (tden !== trafficDensity) { // tden 0..2.
            var z =
                 ((tile - ROADBASE) & 15) +
                densityTable[trafficDensity];

            z |=
                mapValue & (TILEMASK_ALL - TILEMASK_ANIMATED);

            if (trafficDensity > 0) {
                z |= TILEMASK_ANIMATED;
            }

            this.setTile(x, y, z);

        }

    };


    p.doBridge = function doBridge(x, y, tile) {

        var z;
        var xx;
        var yy;
        var mpTemp;

        if (tile === BRWV) { // Vertical bridge close.

            if ((!(this.getRandom16() & 3)) &&
                (this.getBoatDistance(x, y) > 340)) {

                for (z = 0;
                     z < 7;
                     z++) { // Close bridge.

                    xx = x + vDX[z];
                    yy = y + vDY[z];

                    if (this.testBounds(xx, yy)) {

                        if ((this.getTile(xx, yy) & TILEMASK_TILE) ===
                            (this.vBridgeTabble[z] & TILEMASK_TILE)) {

                            this.setTile(xx, yy, vBridgeTable2[z]);

                        }

                    }

                }

            }

            return true;

        }

        if (tile === BRWH) { // Horizontal bridge close.

            if ((!(this.getRandom16() & 3)) &&
                (this.getBoatDistance(pos) > 340)) {

                for (z = 0;
                     z < 7;
                     z++) { // Close bridge.

                    xx = x + hDX[z];
                    yy = y + hDY[z];

                    if (this.testBounds(xx, yx)) {

                        if ((this.getTile(xx, yy) & TILEMASK_TILE) ===
                            (hBridgeTable[z] & TILEMASK_TILE)) {

                            this.setTile(xx, yy, hBridgeTable2[z]);

                        }

                    }

                }

            }

            return true;

        }

        if ((this.getBoatDistance(x, y) < 300) ||
            (!(this.getRandom16() & 7))) {

            if (tile & 1) {

                if (x < (WORLD_W - 2)) {

                    if (this.getTile(x + 1, y) === CHANNEL) { // Vertical open.

                        for (z = 0; z < 7; z++) {

                            xx = x + this.vDx[z];
                            yy = y + this.vDy[z];

                            if (this.testBounds(xx, yy)) {

                                mpTemp = this.getTile(xx, yy);

                                if ((mpTemp === CHANNEL) ||
                                    ((mpTemp & 15) === (tis.vBridgeTable2[z] & 15))) {
                                    this.setTile(xx, yy, vBridgeTable[z]);
                                }

                            }

                        }

                        return true;

                    }

                }

                return false;

            } else {

                if (y > 0) {

                    if (this.getTile(x, y - 1) === CHANNEL) {

                        /// Horizontal open.
                        for (z = 0;
                             z < 7;
                             z++) {

                            xx = x + hDX[z];
                            yy = y + hDY[z];

                            if (this.testBounds(xx, yy)) {

                                mpTemp = this.getTile(xx, yy);

                                if (((mpTemp & 15) === (hBridgeTable2[z] & 15)) ||
                                    (mpTemp === CHANNEL)) {

                                    this.setTile(xx, yy, hBridgeTable[z]);

                                }

                            }

                        }

                        return true;

                    }

                }

                return false;

            }

        }

        return false;

    };


    p.getBoatDistance = function getBoatDistance(x, y) {

        var dist = 99999;
        var mx = (x * 16) + 8;
        var my = (y * 16) + 8;

        for (var spriteIndex = 0, spriteCount = this.spriteList.length;
             spriteIndex < spriteCount;
             spriteIndex++) {

            var sprite = this.spriteList[spriteIndex];

            if ((sprite !== null) &&
                (sprite.type === SPRITE_SHIP) &&
                (sprite.frame !== 0)) {

                var spriteDist =
                    Math.abs(sprite.x + sprite.xHot - mx) +
                    Math.abs(sprite.y + sprite.yHot - my);

                dist = Math.min(dist, spriteDist);
            }

        }

        return dist;

    };


    p.doFire = function doFire(x, y) {

        // Try to set neighbouring tiles on fire as well.

        var z;

        for (z = 0;
             z < 4;
             z++) {

            if ((this.getRandom16() & 7) === 0) {

                var xx = x + dx[z];
                var yy = y + dy[z];

                if (testBounds(xx, yy)) {

                    var tile = this.getTile(xx, yy);

                    if (!(tile & TILEMASK_BURNABLE)) {
                        continue;
                    }

                    if (tile & TILEMASK_ZONE) {

                        // Neighbour is a zone and burnable.
                        this.fireZone(xx, yy, tile);

                        if ((tile & TILEMASK_TILE) > IZB) { // Explode.
                              this.makeExplosionAt((xx * 16) + 8, (yy * 16) + 8);
                        }
                    }

                    this.setTile(xx, yy, this.randomFire());

                }

            }

        }

        // Compute likelyhood of fire running out of fuel.
        var rate = 10; // Likelyhood of extinguishing (bigger means less chance).

        z = this.fireStationEffectMap.worldGet(x, y);

        if (z > 0) {

            rate = 3;

            if (z > 20) {
                rate = 2;
            }

            if (z > 100) {
                rate = 1;
            }

        }

        // Decide whether to put out the fire.

        if (this.getRandom(rate) === 0) {
            this.setTile(x, y, this.randomRubble());
        }

    };


    p.fireZone = function fireZone(x, y, tile) {

        var value =
            this.rateOfGrowthMap.worldGet(x, y);

        value =
            this.clamp(value - 20, -200, 200);

        this.rateOfGrowthMap.worldSet(x, y, value);

        tile =
            tile & TILEMASK_TILE;

        var xyMax;
        if (tile < PORTBASE) {
            xyMax = 2;
        } else {
            if (tile === AIRPORT) {
                xyMax = 5;
            } else {
                xyMax = 4;
            }
        }

        // Make remaining tiles of the zone bulldozable
        for (var xx = -1;
             xx < xyMax;
             xx++) {

            for (var yy = -1;
                 yy < xyMax;
                 yy++) {

                var xxx = x + xx;
                var yyy = y + yy;

                if (!this.testBounds(xxx, yyy)) {
                    continue;
                }

                var newTile = this.getTile(xxx, yyy);

                if ((newTile & TILEMASK_TILE) >=
                    ROADBASE) {

                    var bullTile =
                        newTile | TILEMASK_BULLDOZABLE;

                    this.setTile(xxx, yyy, bullTile);

                }

            }

        }

    };


    p.repairZone = function repairZone(x, y, zCent, zSize) {

        var tile = zCent - 2 - zSize;

        // y and x loops one position shifted to compensate for the center-tile position.
        for (var yy = -1;
             yy < zSize - 1;
             yy++) {

            for (var xx = -1;
                 xx < zSize - 1;
                 xx++) {

                var xxx = x + xx;
                var yyy = y + yy;

                tile++;

                if (this.testBounds(xxx, yyy)) {

                    var mapValue = this.getTile(xxx, yyy);

                    if ((mapValue & TILEMASK_ZONE) ||
                        (mapValue & TILEMASK_ANIMATED)) {
                        continue;
                    }

                    var mapTile = mapValue & TILEMASK_TILE;

                    if ((mapTile < RUBBLE) ||
                        (mapTile >= ROADBASE)) {

                        this.setTile(xxx, yyy, tile | TILEMASK_CONDUCTIVE | TILEMASK_BURNABLE);

                    }

                }

            }

        }

    };


    p.doSpecialZone = function doSpecialZone(x, y, powered) {

        var foundPos;
        var foundX;
        var foundY;
        var z;
        var tile =
            this.getTile(x, y) & TILEMASK_TILE;

        switch (tile) {

            case POWERPLANT:

                this.coalPowerPop++;

                if ((this.cityTime & 7) === 0) {
                    this.repairZone(x, y, POWERPLANT, 4);
                }

                this.pushPowerStack([x, y]);
                this.coalSmoke(x, y);

                return;

            case NUCLEAR:

                if (this.enableDisasters &&
                    !this.getRandom(this.meltdownTable[this.gameLevel])) {
                    this.doMeltdown(x, y);
                    return;
                }

                this.nuclearPowerPop++;

                if ((this.cityTime & 7) === 0) {
                    this.repairZone(x, y, NUCLEAR, 4);
                }

                this.pushPowerStack([x, y]);

                return;

            case FIRESTATION: {

                this.fireStationPop++;

                if (!(this.cityTime & 7)) {
                    this.repairZone(x, y, FIRESTATION, 3);
                }

                if (powered) {
                    z = this.fireEffect; // If powered get effect.
                } else {
                    z = this.fireEffect / 2; // From the funding ratio.
                }

                foundPos = this.findPerimeterRoad(x, y);
                foundX = foundPos ? foundPos[0] : x;
                foundY = foundPos ? foundPos[1] : y;

                if (!foundPos) {
                    z = z / 2; // FD's need roads.
                }

                value =
                    this.fireStationMap.worldGet(foundX, foundY);

                value += z;

                this.fireStationMap.worldSet(foundX, foundY, value);

                return;
            }

            case POLICESTATION: {

                this.policeStationPop++;

                if (!(this.cityTime & 7)) {
                    this.repairZone(x, y, POLICESTATION, 3);
                }

                if (powered) {
                    z = this.policeEffect;
                } else {
                    z = this.policeEffect / 2;
                }

                foundPos = this.findPerimeterRoad(x, y);
                foundX = foundPos ? foundPos[0] : x;
                foundY = foundPos ? foundPos[1] : y;

                if (!foundPos) {
                    z = z / 2; // PD's need roads.
                }

                value =
                    this.policeStationMap.worldGet(foundX, foundY);

                value += z;

                this.policeStationMap.worldSet(foundX, foundY, value);

                return;
            }

            case STADIUM: // Empty stadium

                this.stadiumPop++;

                if (!(this.cityTime & 15)) {
                    this.repairZone(x, y, STADIUM, 4);
                }

                if (powered) {

                    // Every now and then, display a match.
                    if (((this.cityTime + x + y) & 31) === 0) {

                        this.drawStadium(x, y, FULLSTADIUM);

                        if (this.testBounds(x + 1, y)) {
                            this.setTile(x + 1, y, FOOTBALLGAME1 + TILEMASK_ANIMATED);
                        }

                        if (this.testBounds(x + 1, y + 1)) {
                            this.setTile(x + 1, y + 1, FOOTBALLGAME2 + TILEMASK_ANIMATED);
                        }

                    }

                }

                return;

           case FULLSTADIUM: // Full stadium

                this.stadiumPop++;

                if (((this.cityTime + x + y) & 7) === 0) {
                    // Stop the match
                    this.drawStadium(x, y, STADIUM);
                }

                return;

           case AIRPORT:

                this.airportPop++;

                if ((this.cityTime & 7) === 0) {
                    this.repairZone(x, y, AIRPORT, 6);
                }

                // If powered, display a rotating radar
                if (this.testBounds(x, + 1, y - 1)) {

                    if (powered) {

                        if ((this.getTile(x + 1, y - 1) & TILEMASK_TILE) === RADAR) {

                            this.setTile(
                                x + 1,
                                y - 1,
                                RADAR0 | TILEMASK_ANIMATED | TILEMASK_CONDUCTIVE | TILEMASK_BURNABLE);

                        }

                    } else {


                        this.setTile(
                            x + 1,
                            y - 1,
                            RADAR | TILEMASK_CONDUCTIVE | TILEMASK_BURNABLE);

                    }

                }

                if (powered) { // Handle the airport only if there is power
                    this.doAirport(x, y);
                }

                return;

           case PORT:

                this.seaportPop++;

                if ((this.cityTime & 15) === 0) {
                    this.repairZone(x, y, PORT, 4);
                }

                // If port has power and there is no ship, generate one.
                if (powered &&
                    this.getSprite(SPRITE_SHIP) === null) {

                    this.generateShip();

                }

                return;

            default:
                break;

        }

    };


    p.drawStadium = function drawStadium(x, y, z) {

        z = z - 5;

        for (var yy = y - 1;
             yy < y + 3;
             yy++) {

            for (var xx = x - 1;
                 xx < x + 3;
                 xx++, z++) {

                this.setTile(xx, yy, z | TILEMASK_BURNABLE | TILEMASK_CONDUCTIVE);

            }

        }

        this.setTile(
            x,
            y,
            this.getTile(x, y) | TILEMASK_ZONE | TILEMASK_POWER);

    };


    p.doAirport = function doAirport(x, y) {

        if (this.getRandom(5) === 0) {
            this.generatePlane(x, y);
            return;
        }

        if (this.getRandom(12) === 0) {
            this.generateCopter(x, y);
        }

    };


    p.coalSmoke = function coalSmoke(x, y) {

        for (var z = 0;
             z < 4;
             z++) {

            this.setMap(
                x + coalSmokeDX[z],
                y + this.coalSmoleDY[z],
                coalSmokeTable[z] | TILEMASK_ANIMATED | TILEMASK_CONDUCTIVE | TILEMASK_POWER | TILEMASK_BURNABLE);

        }

    };


    p.doMeltdown = function doMeltdown(x, y) {

        var xx;
        var yy;
        var z;
        var tile;

        if (this.testBounds(x - 1, y - 1)) {
            this.makeExplosion(x - 1, y - 1);
        }

        if (this.testBounds(x - 1, y + 2)) {
            this.makeExplosion(x - 1, y + 2);
        }

        if (this.testBounds(x + 2, y - 1)) {
            this.makeExplosion(x + 2, y - 1);
        }

        if (this.testBounds(x + 2, y + 2)) {
            this.makeExplosion(x + 2, y + 2);
        }

        // Whole power plant is at fire.
        for (xx = x - 1;
             xx < x + 3;
             xx++) {

            for (yy = y - 1;
                 yy < y + 3;
                 y++) {

                if (this.testBounds(xx, yy)) {
                    this.setTile(xx, yy, this.randomFire());
                }

            }

        }

        // Add lots of radiation tiles around the plant.
        for (z = 0;
             z < 200;
             z++)  {

            xx = x - 20 + this.getRandom(40);
            yy = y - 15 + this.getRandom(30);

            if (!this.testBounds(xx, yy)) { // Ignore off-map positions.
                continue;
            }

            tile = this.getTile(xx, yy);

            if (tile & TILEMASK_ZONE) {
                continue; // Ignore zones.
            }

            if ((tile & TILEMASK_BURNABLE) ||
                (tile === DIRT)) {

                this.setTile(
                    xx,
                    yy,
                    RADTILE); // Make tile radioactive.

            }

        }

        // Report disaster to the user.
        this.sendMessage('NUCLEAR_MELTDOWN', x, y, true, true);

    };


    p.newSprite = function newSprite(name, type, x, y) {

        var sprite = {
            name: name,
            type: type
        };

        this.initSprite(sprite, x, y);

        // TODO: clear spriteList when reset.

        this.spriteList.push(sprite);

        return sprite;

    };


    p.initSprite = function initSprite(sprite, x, y) {

        sprite.x = x;
        sprite.y = y;
        sprite.frame = 0;
        sprite.origX = 0;
        sprite.origY = 0;
        sprite.destX = 0;
        sprite.destY = 0;
        sprite.count = 0;
        sprite.soundCount = 0;
        sprite.dir = 0;
        sprite.newDir = 0;
        sprite.step = 0;
        sprite.flag = 0;
        sprite.control = -1;
        sprite.turn = 0;
        sprite.accel = 0;
        sprite.speed = 100;

        if (!this.globalSprites[sprite.type]) {
            this.globalSprites[sprite.type] = sprite;
        }

        switch (sprite.type) {

            case SPRITE_TRAIN:

                sprite.width = 32;
                sprite.height = 32;
                sprite.xOffset = 32;
                sprite.yOffset = -16;
                sprite.xHot = 40;
                sprite.yHot = -8;
                sprite.frame = 1;
                sprite.dir = 4;

                break;

            case SPRITE_SHIP:

                sprite.width = 48;
                sprite.height = 48;
                sprite.xOffset = 32;
                sprite.yOffset = -16;
                sprite.xHot = 48;
                sprite.yHot = 0;

                if (x < (4 << 4)) {
                    sprite.frame = 3;
                } else if (x >= ((WORLD_W - 4) << 4)) {
                    sprite.frame = 7;
                } else if (y < (4 << 4)) {
                    sprite.frame = 5;
                } else if (y >= ((WORLD_H - 4) << 4)) {
                    sprite.frame = 1;
                } else {
                    sprite.frame = 3;
                }

                sprite.newDir = sprite.frame;
                sprite.dir = 10;
                sprite.count = 1;

                break;

            case SPRITE_MONSTER:

                sprite.width = 48;
                sprite.height = 48;
                sprite.xOffset = 24;
                sprite.yOffset = 0;
                sprite.xHot = 40;
                sprite.yHot = 16;

                if (x > ((WORLD_W << 4) / 2)) {
                    if (y > ((WORLD_H << 4) / 2)) {
                        sprite.frame = 10;
                    } else {
                        sprite.frame = 7;
                    }
                } else if (y > ((WORLD_H << 4) / 2)) {
                    sprite.frame = 1;
                } else {
                    sprite.frame = 4;
                }

                sprite.count = 1000;
                sprite.destX = this.pollutionMaxX << 4;
                sprite.destY = this.pollutionMaxY << 4;
                sprite.origX = sprite.x;
                sprite.origY = sprite.y;

                break;

            case SPRITE_HELICOPTER:

                sprite.width = 32;
                sprite.height = 32;
                sprite.xOffset = 32;
                sprite.yOffset = -16;
                sprite.xHot = 40;
                sprite.yHot = -8;
                sprite.frame = 5;
                sprite.count = 1500;
                sprite.destX = this.getRandom((WORLD_W << 4) - 1);
                sprite.destY = this.getRandom((WORLD_H << 4) - 1);
                sprite.origX = x - 30;
                sprite.origY = y;

                break;

            case SPRITE_AIRPLANE:

                sprite.width = 48;
                sprite.height = 48;
                sprite.xOffset = 24;
                sprite.yOffset = 0;
                sprite.xHot = 48;
                sprite.yHot = 16;

                if (x > ((WORLD_W - 20) << 4)) {
                    sprite.x -= 100 + 48;
                    sprite.destX = sprite.x - 200;
                    sprite.frame = 7;
                } else {
                    sprite.destX = sprite.x + 200;
                    sprite.frame = 11;
                }

                sprite.destY = sprite.y;

            break;

            case SPRITE_TORNADO:

                sprite.width = 48;
                sprite.height = 48;
                sprite.xOffset = 24;
                sprite.yOffset = 0;
                sprite.xHot = 40;
                sprite.yHot = 36;
                sprite.frame = 1;
                sprite.count = 200;

                break;

            case SPRITE_EXPLOSION:

                sprite.width = 48;
                sprite.height = 48;
                sprite.xOffset = 24;
                sprite.yOffset = 0;
                sprite.xHot = 40;
                sprite.yHot = 16;
                sprite.frame = 1;

                break;

            case SPRITE_BUS:

                sprite.width = 32;
                sprite.height = 32;
                sprite.xOffset = 30;
                sprite.yOffset = -18;
                sprite.xHot = 40;
                sprite.yHot = -8;
                sprite.frame = 1;
                sprite.dir = 1;

                break;

            default:
                break;

        }

    };


    p.destroyAllSprites = function destroyAllSprites() {

        for (var i = 0, n = this.spriteList.length;
             i < n;
             i++) {

            var sprite = this.spriteList[i];

            sprite.frame = 0;

        }

    };


    p.destroySprite = function destroySprite(sprite) {

        if (this.globalSprites[sprite.type] === sprite) {
            this.globalSprites[sprite.type] = null;
        }

        var index = this.spriteList.indexOf(sprite);

        if (index >= 0) {
            this.spriteList.splice(index);
        }

    };


    p.getSprite = function getSprite(type) {

        var sprite = this.globalSprites[type];

        if (sprite.frame === 0) {
            sprite = null;
        }

        return sprite || null;

    };


    p.makeSprite = function makeSprite(type, x, y) {

        var sprite = this.globalSprites[type];

        if (!sprite) {

            sprite =
                this.newSprite('', type, x, y);

        } else {

            this.initSprite(sprite, x, y);

        }

        return sprite;

    };


    p.getChar = function getChar(x, y) {

        // Convert sprite coordinates to tile coordinates.

        x >>= 4;
        y >>= 4;

        if (!this.testBounds(x, y)) {
            return -1;
        }

        return this.getTile(x, y) & TILEMASK_TILE;

    };


    p.turnTo = function turnTo(p, d) {

        if (p === d) {
            return p;
        }

        if (p < d) {

            if ((d - p) < 4) {
                p++;
            } else {
                p--;
            }

        } else {

            if ((p - d) < 4) {
                p--;
            } else {
                p++;
            }

        }

        if (p > 8) {
            p = 1;
        }

        if (p < 1) {
            p = 8;
        }

        return p;

    };


    p.tryOther = function tryOther(tpoo, told, tnew) {

        var z = told + 4;

        if (z > 8) {
            z -= 8;
        }

        if (tnew !== z) {
            return false;
        }

        if ((tpoo === POWERBASE) ||
            (tpoo === POWERBASE + 1) ||
            (tpoo === RAILBASE) ||
            (tpoo === RAILBASE + 1)) {
            return true;
        }

        return false;

    };


    p.spriteNotInBounds = function spriteNotInBounds(sprite) {

        var x = sprite.x + sprite.xHot;
        var y = sprite.y + sprite.yHot;

        return (
            (x < 0) ||
            (x >= (WORLD_W << 4)) ||
            (y < 0) ||
            (y >= (WORLD_H << 4)));

    };


    p.getDir = function getDir(orgX, orgY, desX, desY) {

        var dispX = desX - orgX;
        var dispY = desY - orgY;

        var z;
        if (dispX < 0) {
            if (dispY < 0) {
                z = 11;
            } else {
                z = 8;
            }
        } else {
            if (dispY < 0) {
                z = 2;
            } else {
                z = 5;
            }
        }

        dispX = Math.abs(dispX);
        dispY = Math.abs(dispY);

        this.absDist = dispX + dispY;

        if ((dispX * 2) < dispY) {
            z++;
        } else if ((dispY * 2) < dispY) {  // XXX This never holds!!
            z--;
        }

        if ((z < 0) ||
            (z > 12)) {
            z = 0;
        }

        return this.getDirectionTable[z];

    };


    p.getDistance = function getDistance(x1, y1, x2, y2) {

        return (Math.abs(x1 - x2) +
                Math.abs(y1 - y2));

    };


    p.checkSpriteCollision = function checkSpriteCollision(s1, s2) {

        return ((s1.frame !== 0) &&
                (s2.frame !== 0) &&
                (this.getDistance(
                    s1.x + s1.xHot,
                    s1.y + s1.yHot,
                    s2.x + s2.xHot,
                    s2.y + s2.yHot) < 30));

    };


    p.moveObjects = function moveObjects() {

        var sprite;
        var i;

        if (!this.simSpeed) {
            return;
        }

        this.spriteCycle++;

        for (i = this.spriteList.length - 1;
             i >= 0;
             i--) {

            sprite = this.spriteList[i];

            if (sprite.frame > 0) {

                switch (sprite.type) {

                    case SPRITE_TRAIN:
                        this.doTrainSprite(sprite);
                        break;

                    case SPRITE_HELICOPTER:
                        this.doCopterSprite(sprite);
                        break;

                    case SPRITE_AIRPLANE:
                        this.doAirplaneSprite(sprite);
                        break;

                    case SPRITE_SHIP:
                        this.doShipSprite(sprite);
                        break;

                    case SPRITE_MONSTER:
                        this.doMonsterSprite(sprite);
                        break;

                    case SPRITE_TORNADO:
                        this.doTornadoSprite(sprite);
                        break;

                    case SPRITE_EXPLOSION:
                        this.doExplosionSprite(sprite);
                        break;

                    case SPRITE_BUS:
                        this.doBusSprite(sprite);
                        break;

                    default:
                        break;

                }

            } else {

                if (sprite.name === null) {
                    this.destroySprite(sprite);
                }

            }

        }

    };


    p.doTrainSprite = function doTrainSprite(sprite) {

        // Offset in pixels of sprite x and y to map tile.
        var cx = [   0,  16,   0, -16 ];
        var cy = [ -16,   0,  16,   0 ];

        // X and Y movement of the sprite in pixels.
        var dx = [  0,   4,   0,  -4,   0 ];
        var dy = [ -4,   0,   4,   0,   0 ];

        var trainPic2 = [ 1, 2, 1, 2, 5 ];

        if ((sprite.frame === 3) ||
            (sprite.frame === 4)) {

            sprite.frame = trainPic2[sprite.dir];

        }

        sprite.x += dx[sprite.dir];
        sprite.y += dy[sprite.dir];

        if ((this.spriteCycle & 3) !== 0) {
            return;
        }

        var dir = this.getRandom16() & 3;
        for (var z = dir;
             z < dir + 4;
             z++) {

            var dir2 = z & 3;

            if ((sprite.dir !== 4) &&
                (dir2 === ((sprite.dir + 2) & 3))) {
                continue;
            }

            var c = this.getChar(sprite.x + cx[dir2] + 48, sprite.y + cy[dir2]);

            if (((c >= RAILBASE) &&
                 (c <= LASTRAIL)) ||
                (c === RAILVPOWERH) ||
                (c === RAILHPOWERV)) {

                if ((sprite.dir !== dir2) &&
                    (sprite.dir !== 4)) {

                    if ((sprite.dir + dir2) === 3) {
                        sprite.frame = 3;
                    } else {
                        sprite.frame = 4;
                    }

                } else {
                    sprite.frame = trainPic2[dir2];
                }

                if ((c === HRAIL) ||
                    (c === VRAIL)) {
                    sprite.frame = 5;
                }

                sprite.dir = dir2;

                return;

            }

        }

        if (sprite.dir === 4) {

            sprite.frame = 0;

            return;

        }

        sprite.dir = 4;

    };


    p.doCopterSprite = function doCopterSprite(sprite) {

        var cdx = [ 0,  0,  3,  5,  3,  0, -3, -5, -3 ];
        var cdy = [ 0, -5, -3,  0,  3,  5,  3,  0, -3 ];

        if (sprite.soundCount > 0) {
            sprite.soundCount--;
        }

        if (sprite.control < 0) {

            if (sprite.count > 0) {
                sprite.count--;
            }

            if (sprite.count === 0) {

                // Attract copter to monster so it blows up more often.
                var monsterSprite = this.getSprite(SPRITE_MONSTER);

                if (monsterSprite !== null) {
                    sprite.destX = monsterSprite.x;
                    sprite.destY = monsterSprite.y;
                } else {

                    // Attract copter to tornado so it blows up more often.
                    var tornadoSprite = getSprite(SPRITE_TORNADO);

                    if (tornadoSprite !== null) {
                        sprite.destX = tornadoSprite.x;
                        sprite.destY = tornadoSprite.y;
                    } else {
                        sprite.destX = sprite.origX;
                        sprite.destY = sprite.origY;
                    }

                }

            }

            if (sprite.count === 0) { // Land.

                this.getDir(sprite.x, sprite.y, sprite.origX, sprite.origY);

                if (this.absDist < 30) {
                    sprite.frame = 0;
                    return;
                }

            }

        } else {

            this.getDir(sprite.x, sprite.y, sprite.destX, sprite.destY);

            if (this.absDist < 16) {
                sprite.destX = sprite.origX;
                sprite.destY = sprite.origY;
                sprite.control = -1;
            }

        }

        if (sprite.soundCount === 0) { // Send report.

            // Convert sprite coordinates to world coordinates.
            var x = (sprite.x + 48) / 16;
            var y = sprite.y / 16;

            if ((x >= 0) &&
                (x < WORLD_W) &&
                (y >= 0) &&
                (y < WORLD_H)) {

                // Don changed from 160 to 170 to shut the #$%#$% thing up!

                var chopperX = x + 1;
                var chopperY = y + 1;

                if ((this.trafficDensityMap.worldGet(x, y) > 170) &&
                    (this.getRandom16() & 7) === 0) {

                    this.sendMessage('HEAVY_TRAFFIC', chopperX, chopperY, true, true);

                    this.makeSound('city', 'HeavyTraffic', chopperX, chopperY);

                    sprite.soundCount = 200;

                }

            }

        }

        var z = sprite.frame;

        if ((this.spriteCycle & 3) === 0) {
            var d = this.getDir(sprite.x, sprite.y, sprite.destX, sprite, destY);
            z = this.turnTo(z, d);
            sprite.frame = z;
        }

        sprite.x += cdx[z];
        sprite.y += cdy[z];

    };


    p.doAirplaneSprite = function doAirplaneSprite(sprite) {

        var cdx = [ 0,  0,  6, 8, 6, 0, -6, -8, -6, 8, 8, 8 ];
        var cdy = [ 0, -8, -6, 0, 6, 8,  6,  0, -6, 0, 0, 0 ];

        var z = sprite.frame;

        if ((this.spriteCycle % 5) === 0) {

            if (z > 8) { // Take off.

                z--;

                if (z < 9) {
                    z = 3;
                }

                sprite.frame = z;

            } else { // Go to destination.

                var d = 
                    this.getDir(sprite.x, sprite.y, sprite.destX, sprite.destY);

                z = this.turnTo(z, d);

                sprite.frame = z;

            }

        }

        if (this.absDist < 50) { // At destination.
            sprite.destX = this.getRandom((WORLD_W * 16) + 100) - 50;
            sprite.destY = this.getRandom((WORLD_H * 16) + 100) - 50;
        }

        var explode = false;

        // Don added test for enableDisasters.
        if (this.enableDisasters) {

            // Check whether another sprite is near enough to collide with.

            for (var i = this.spriteList.length - 1;
                 i >= 0;
                 i--) {

                var otherSprite = this.spriteList[i];

                if ((otherSprite === sprite) ||
                    (otherSprite.frame === 0)) {
                    // Non-active sprite, or self: skip.
                    continue;
                }

                if (((otherSprite.type === SPRITE_HELICOPTER) ||
                     (otherSprite.type === SPRITE_AIRPLANE)) &&
                    this.checkSpriteCollision(sprite, otherSprite)) {

                    this.explodeSprite(otherSprite);

                    explode = true;

                }

            }

            if (explode) {
                this.explodeSprite(sprite);
            }

        }

        if (!explode) {

            sprite.x += cdx[z];
            sprite.y += cdy[z];

            if (this.spriteNotInBounds(sprite)) {

                sprite.frame = 0;

            }

        }

    };


    p.doShipSprite = function doShipSprite(sprite) {

        var z;
        var bdx = [ 0,  0,  1,  1,  1,  0, -1, -1, -1 ];
        var bdy = [ 0, -1, -1,  0,  1,  1,  1,  0, -1 ];
        var bpx = [ 0,  0,  2,  2,  2,  0, -2, -2, -2 ];
        var bpy = [ 0, -2, -2,  0,  2,  2,  2,  0, -2 ];
        var btclrtab = [
            RIVER,
            CHANNEL,
            POWERBASE,
            POWERBASE + 1,
            RAILBASE,
            RAILBASE + 1,
            BRWH,
            BRWV
        ];

        var tile = RIVER;

        if (sprite.soundCount > 0) {
            sprite.soundCount--;
        }

        if (!sprite.soundCount) {

            if ((this.getRandom16() & 3) === 1) {

                // Convert sprite coordinates to tile coordinates.
                var shipX = sprite.x >> 4;
                var shipY = sprite.y >> 4;

                if ((this.scenario === SCENARIO_SAN_FRANCISCO) &&
                    (this.getRandom(10) < 5)) {

                    this.makeSound('city', 'FogHornLow', shipX, shipY);

                } else {

                    this.makeSound('city', 'HonkHonkLow', shipX, shipY);

                }

            }

            sprite.soundCount = 200;

        }

        if (sprite.count > 0) {
            sprite.count--;
        }

        if (sprite.count === 0) {

            sprite.count = 9;

            if (sprite.frame !== sprite.newDir) {

                sprite.frame =
                    this.turnTo(sprite.frame, sprite.newDir);

                return;

            }

            var tem = this.getRandom16() & 7;
            var pem;

            for (pem = tem;
                 pem < (tem + 8);
                 pem++) {

                z = (pem & 7) + 1;

                if (z === sprite.dir) {
                    continue;
                }

                x = ((sprite.x + (48 - 1)) >> 4) + bdx[z];
                y = (sprite.y >> 4) + bdy[z];

                if (this.testBounds(x, y)) {

                    tile = this.getTile(x, y) & TILEMASK_TILE;

                    if ((tile === CHANNEL) ||
                        (tile === BRWH) ||
                        (tile === BRWV) ||
                        this.tryOther(tile, sprite.dir, z)) {

                        sprite.dir = z + 4;
                        sprite.newDir = z;
                        sprite.frame =
                            this.turnTo(sprite.frame, sprite.newDir);

                        if (sprite.dir > 8) {
                            sprite.dir -= 8;
                        }

                        break;

                    }

                }

            }

            if (pem === (tem + 8)) {

                sprite.dir = 10;
                sprite.newDir = (this.getRandom16() & 7) + 1;

            }

        } else {

            z = sprite.frame;

            if (z === sprite.newDir)  {
                sprite.x += bpx[z];
                sprite.y += bpy[z];
            }
        }

        if (this.spriteNotInBounds(sprite)) {

            sprite.frame = 0;

            return;

        }

        for (z = 0;
             z < 8;
             z++) {

            if (tile === btclrtab[z]) {
                break;
            }

            if (z === 7) {
                this.explodeSprite(sprite);
                this.destroyMapTile(sprite.x + 48, sprite.y);
                break;
            }

        }

    };


    // Move monster sprite.
    //
    // There are 16 monster sprite frames:
    //
    // Frame 0: NorthEast Left Foot
    // Frame 1: NorthEast Both Feet
    // Frame 2: NorthEast Right Foot
    // Frame 3: SouthEast Right Foot
    // Frame 4: SouthEast Both Feet
    // Frame 5: SouthEast Left Foot
    // Frame 6: SouthWest Right Foot
    // Frame 7: SouthWest Both Feet
    // Frame 8: SouthWest Left Foot
    // Frame 9: NorthWest Left Foot
    // Frame 10: NorthWest Both Feet
    // Frame 11: NorthWest Right Foot
    // Frame 12: North Left Foot
    // Frame 13: East Left Foot
    // Frame 14: South Right Foot
    // Frame 15: West Right Foot
    //
    p.doMonsterSprite = function doMonsterSprite(sprite) {

        var gx = [  2,  2, -2, -2,  0 ];
        var gy = [ -2,  2,  2, -2,  0 ];
        var nd1 = [  0,  1,  2,  3 ];
        var nd2 = [  1,  2,  3,  0 ];
        var nn1 = [  2,  5,  8, 11 ];
        var nn2 = [ 11,  2,  5,  8 ];
        var c;
        var d;
        var z;

        if (sprite.soundCount > 0) {
            sprite.soundCount--;
        }

        if (sprite.control < 0) {

            // Business as usual.

            if (sprite.control === -2) {

                d = Math.floor((sprite.frame - 1) / 3);
                z = (sprite.frame - 1) % 3;

                if (z === 2) {
                    sprite.step = 0;
                }

                if (z === 0) {
                    sprite.step = 1;
                }

                if (sprite.step) {
                    z++;
                } else {
                    z--;
                }

                c = this.getDir(sprite.x, sprite.y, sprite.destX, sprite.destY);

                if (this.absDist < 18) {

                    sprite.control = -1;
                    sprite.count = 1000;
                    sprite.flag = 1;
                    sprite.destX = sprite.origX;
                    sprite.destY = sprite.origY;

                } else {

                    c = (c - 1) / 2;

                    if (((c !== d) &&
                         (this.getRandom(5) === 0)) ||
                        (this.getRandom(20) === 0)) {

                        var diff = (c - d) & 3;

                        if (diff === 1 || diff === 3) {

                            d = c;

                        } else {

                            if (this.getRandom16() & 1) {
                                d++;
                            } else {
                                d--;
                            }

                            d &= 3;
                        }

                    } else {

                        if (this.getRandom(20) === 0) {

                            if (this.getRandom16() & 1) {
                                d++;
                            } else {
                                d--;
                            }

                            d &= 3;

                        }

                    }

                }

            } else {

                d = (sprite.frame - 1) / 3;

                if (d < 4) { // Turn n s e w.

                    z = (sprite.frame - 1) % 3;

                    if (z === 2) {
                        sprite.step = 0;
                    }

                    if (z === 0) {
                        sprite.step = 1;
                    }

                    if (sprite.step) {
                        z++;
                    } else {
                        z--;
                    }

                    this.getDir(sprite.x, sprite.y, sprite.destX, sprite.destY);

                    if (this.absDist < 60) {

                        if (sprite.flag === 0) {

                            sprite.flag = 1;
                            sprite.destX = sprite.origX;
                            sprite.destY = sprite.origY;

                        } else {

                            sprite.frame = 0;

                            return;

                        }

                    }

                    c = this.getDir(sprite.x, sprite.y, sprite.destX, sprite.destY);
                    c = (c - 1) / 2;

                    if ((c !== d) &&
                        (!this.getRandom(10))) {

                        if (this.getRandom16() & 1) {
                            z = nd1[d];
                        } else {
                            z = nd2[d];
                        }

                        d = 4;

                        if (!sprite.soundCount) {

                            // Convert sprite coordinates to tile coordinates.
                            var monsterX = sprite.x >> 4;
                            var monsterY = sprite.y >> 4;

                            this.makeSound('city', 'Monster', monsterX, monsterY);

                            sprite.soundCount = 50 + this.getRandom(100);

                        }

                    }

                } else {

                    d = 4;
                    c = sprite.frame;
                    z = (c - 13) & 3;

                    if (!(this.getRandom16() & 3)) {

                        if (this.getRandom16() & 1) {
                            z = nn1[z];
                        } else {
                            z = nn2[z];
                        }

                        d = Math.floor((z - 1) / 3);
                        z = (z - 1) % 3;

                    }

                }

            }

        } else {

              // Somebody's taken control of the monster.

              d = sprite.control;
              z = (sprite.frame - 1) % 3;

              if (z === 2) {
                  sprite.step = 0;
              }

              if (z === 0) {
                  sprite.step = 1;
              }

              if (sprite.step) {
                  z++;
              } else {
                  z--;
              }

        }

        z = d * 3 + z + 1;

        if (z > 16) {
            z = 16;
        }

        sprite.frame = z;

        sprite.x += gx[d];
        sprite.y += gy[d];

        if (sprite.count > 0) {
            sprite.count--;
        }

        c = this.getChar(sprite.x + sprite.xHot, sprite.y + sprite.yHot);

        if ((c === -1) ||
            ((c === RIVER) &&
             (sprite.count !== 0) &&
             (sprite.control === -1))) {
            sprite.frame = 0; // Kill scary monster.
        }


        for (var i = this.spriteList.length - 1;
             i >= 0;
             i--) {

            var otherSprite = this.spriteList[i];

            if ((otherSprite.frame !== 0) &&
                ((otherSprite.type === SPRITE_AIRPLANE) ||
                 (otherSprite.type === SPRITE_HELICOPTER) ||
                 (otherSprite.type === SPRITE_SHIP) ||
                 (otherSprite.type === SPRITE_TRAIN)) &&
                this.checkSpriteCollision(sprite, otherSprite)) {

                this.explodeSprite(otherSprite);

            }
        }

        this.destroyMapTile(sprite.x + 48, sprite.y + 16);

    };


    p.doTornadoSprite = function doTornadoSprite(sprite) {

        var cdx = [  2,  3,  2,  0, -2, -3 ];
        var cdy = [ -2,  0,  2,  3,  2,  0 ];
        var z = sprite.frame;

        if (z === 2) {

            // Cycle animation.

            if (sprite.flag) {
                z = 3;
            } else {
                z = 1;
            }

        } else {

            if (z === 1) {
                sprite.flag = 1;
            } else {
                sprite.flag = 0;
            }

            z = 2;
        }

        if (sprite.count > 0) {
            sprite.count--;
        }

        sprite.frame = z;

        for (var i = this.spriteList.length - 1;
             i >= 0;
             i--) {

            var otherSprite = this.spriteList[i];

            if ((otherSprite.frame !== 0) &&
                ((otherSprite.type === SPRITE_AIRPLANE) ||
                 (otherSprite.type === SPRITE_HELICOPTER) ||
                 (otherSprite.type === SPRITE_SHIP) ||
                 (otherSprite.type === SPRITE_TRAIN)) &&
                this.checkSpriteCollision(sprite, otherSprite)) {

                this.explodeSprite(otherSprite);

            }
        }

        z = this.getRandom(5);

        sprite.x += cdx[z];
        sprite.y += cdy[z];

        if (this.spriteNotInBounds(sprite)) {
            sprite.frame = 0;
        }

        if ((sprite.count !== 0) &&
            (this.getRandom(500) === 0)) {
            sprite.frame = 0;
        }

        this.destroyMapTile(sprite.x + 48, sprite.y + 40);

    };


    p.doExplosionSprite = function doExplosionSprite(sprite) {

        var x;
        var y;

        if ((this.spriteCycle & 1) === 0) {

            if (sprite.frame === 1) {

            // Convert sprite coordinates to tile coordinates.
                var explosionX = sprite.x >> 4;
                var explosionY = sprite.y >> 4;

                this.makeSound('city', 'ExplosionHigh', explosionX, explosionY);

                x = (sprite.x >> 4) + 3;
                y = (sprite.y >> 4);

                this.sendMessage('EXPLOSION_REPORTED', x, y, false, true);
            }

            sprite.frame++;
        }

        if (sprite.frame > 6) {

            sprite.frame = 0;

            this.startFire(sprite.x + 48 - 8, sprite.y + 16);
            this.startFire(sprite.x + 48 - 24, sprite.y);
            this.startFire(sprite.x + 48 + 8, sprite.y);
            this.startFire(sprite.x + 48 - 24, sprite.y + 32);
            this.startFire(sprite.x + 48 + 8, sprite.y + 32);
        }

    };


    p.doBusSprite = function doBusSprite(sprite) {

        var dxTable = [  0,   1,   0,  -1,   0 ];
        var dyTable = [ -1,   0,   1,   0,   0 ];
        var dir2frame = [ 1, 2, 1, 2 ];
        var dx, dy, tx, ty, otx, oty;
        var turned = 0;
        var speed = 0;
        var z;

        if (sprite.turn) {

            if (sprite.turn < 0) { // ccw

                if (sprite.dir & 1) { // up or down
                    sprite.frame = 4;
                } else { // left or right
                    sprite.frame = 3;
                }

                sprite.turn++;
                sprite.dir = (sprite.dir - 1) & 3;

            } else { // cw

                if (sprite.dir & 1) { // up or down
                    sprite.frame = 3;
                } else { // left or right
                    sprite.frame = 4;
                }

                sprite.turn--;
                sprite.dir = (sprite.dir + 1) & 3;

            }

            turned = 1;

        } else {

            // finish turn
            if ((sprite.frame === 3) ||
                (sprite.frame === 4)) {
                turned = 1;
                sprite.frame = dir2frame[sprite.dir];
            }
        }

        if (sprite.speed === 0) {

            // brake
            dx = 0;
            dy = 0;

        } else { // cruise at traffic speed

            tx = (sprite.x + sprite.xHot) >> 5;
            ty = (sprite.y + sprite.yHot) >> 5;

            if ((tx >= 0) &&
                (tx < WORLD_W_2) &&
                (ty >= 0) &&
                (ty < WORLD_H_2)) {

                z = this.trafficDensityMap.worldGet(tx << 1, ty << 1) >> 6;

                if (z > 1) {
                    z--;
                }

            } else {

                z = 0;

            }

            switch (z) {

                case 0:
                    speed = 8;
                    break;

                case 1:
                    speed = 4;
                    break;

                case 2:
                    speed = 1;
                    break;

                default:
                    break;

            }

            // govern speed
            if (speed > sprite.speed) {
                speed = sprite.speed;
            }

            if (turned) {

                if (speed > 1) {
                    speed = 1;
                }

                dx = dxTable[sprite.dir] * speed;
                dy = dyTable[sprite.dir] * speed;

            } else {

                dx = dxTable[sprite.dir] * speed;
                dy = dyTable[sprite.dir] * speed;

                tx = (sprite.x + sprite.xHot) >> 4;
                ty = (sprite.y + sprite.yHot) >> 4;

                // drift into the right lane
                switch (sprite.dir) {

                      case 0: // up

                        z = ((tx << 4) + 4) - (sprite.x + sprite.xHot);

                        if (z < 0) {
                            dx = -1;
                        } else if (z > 0) {
                            dx = 1;
                        }

                        break;

                    case 1: // right

                        z = ((ty << 4) + 4) - (sprite.y + sprite.yHot);

                        if (z < 0) {
                            dy = -1;
                        } else if (z > 0) {
                            dy = 1;
                        }

                        break;

                    case 2: // down

                        z = (tx << 4) - (sprite.x + sprite.xHot);

                        if (z < 0) {
                            dx = -1;
                        } else if (z > 0) {
                            dx = 1;
                        }

                        break;

                    case 3: // left

                        z = (ty << 4) - (sprite.y + sprite.yHot);

                        if (z < 0) {
                            dy = -1;
                        } else if (z > 0) {
                            dy = 1;
                        }

                        break;

                    default:
                        break;

                }

            }

        }

        var ahead = 8;

        otx = (sprite.x + sprite.xHot + (dxTable[sprite.dir] * ahead)) >> 4;
        oty = (sprite.y + sprite.yHot + (dyTable[sprite.dir] * ahead)) >> 4;

        otx = this.clamp(otx, 0, WORLD_W - 1);
        oty = this.clamp(oty, 0, WORLD_H - 1);

        tx = (sprite.x + sprite.xHot + dx + (dxTable[sprite.dir] * ahead)) >> 4;
        ty = (sprite.y + sprite.yHot + dy + (dyTable[sprite.dir] * ahead)) >> 4;

        tx = this.clamp(tx, 0, WORLD_W - 1);
        ty = this.clamp(ty, 0, WORLD_H - 1);

        if ((tx !== otx) ||
            (ty !== oty)) {

            z = this.canDriveOn(tx, ty);

            if (z === 0) {

                // Can't drive forward into a new tile.
                if (speed === 8) {
                    this.bulldozerTool(tx, ty);
                }

            } else {

                // Drive forward into a new tile.
                if (z <= 0) {
                    // Bumpy offroad.
                    dx /= 2;
                    dy /= 2;
                }

            }

        }

        tx = (sprite.x + sprite.xHot + dx) >> 4;
        ty = (sprite.y + sprite.yHot + dy) >> 4;

        z = this.canDriveOn(tx, ty);

        // z > =: Cool, cruise along.
        // z < 0: Bumpy offroad.
        // z == 0: Something in the way.

        sprite.x += dx;
        sprite.y += dy;

        var explode = false;

        if (this.enableDisasters) {

            // Check whether another sprite is near enough to collide with.

            for (var i = this.spriteList.length - 1;
                 i >= 0;
                 i--) {

                var otherSprite = this.spriteList[i];

                if ((otherSprite.frame === 0) ||
                    (otherSprite === sprite)) {
                    // Non-active sprite, or self: skip.
                    continue;
                }

                if (((otherSprite.type === SPRITE_BUS) ||
                     ((otherSprite.type === SPRITE_TRAIN) &&
                      (otherSprite.frame !== 5))) &&
                    this.checkSpriteCollision(sprite, otherSprite)) {

                    this.explodeSprite(otherSprite);

                    explode = true;

                }

            }

            if (explode) {
                this.explodeSprite(sprite);
            }

        }

    };


    p.canDriveOn = function canDriveOn(x, y) {

        if (!this.testBounds(x, y)) {
            return 0;
        }

        var tile = this.getTile(x, y) & TILEMASK_TILE;

        if (((tile >= ROADBASE) &&
             (tile <= LASTROAD) &&
             (tile !== BRWH) &&
             (tile !== BRWV)) ||
            (tile === HRAILROAD) ||
            (tile === VRAILROAD)) {
            return 1;
        }

        if ((tile === DIRT) ||
            this.tally(tile)) {
            return -1;
        }

        return 0;

    };


    p.explodeSprite = function explodeSprite(sprite) {

        var x, y;

        sprite.frame = 0;

        x = sprite.x + sprite.xHot;
        y = sprite.y + sprite.yHot;

        this.makeExplosionAt(x, y);

        x = (x >> 4);
        y = (y >> 4);

        switch (sprite.type) {

            case SPRITE_AIRPLANE:
                this.sendMessage('PLANE_CRASHED', x, y, true, true);
                break;

            case SPRITE_SHIP:
                this.sendMessage('SHIP_CRASHED', x, y, true, true);
                break;

            case SPRITE_TRAIN:
                this.sendMessage('TRAIN_CRASHED', x, y, true, true);
                break;

            case SPRITE_HELICOPTER:
                this.sendMessage('HELICOPTER_CRASHED', x, y, true, true);
                break;

            case SPRITE_BUS:
                this.sendMessage('TRAIN_CRASHED', x, y, true, true);
                break;

                default:
                    break;

        }

        // Convert sprite coordinates to tile coordinates.
        this.makeSound('city', 'ExplosionHigh', x, y);

    };


    p.checkWet = function checkWet(x) {

        return (
            (x === HPOWER) ||
            (x === VPOWER) ||
            (x === HRAIL) ||
            (x === VRAIL) ||
            (x === BRWH) ||
            (x === BRWV));

    };


    p.destroyMapTile = function destroyMapTile(ox, oy) {

        var x = ox >> 4;
        var y = oy >> 4;

        if (!this.testBounds(x, y)) {
            return;
        }

        var z = this.getTile(x, y);
        var t = z & TILEMASK_TILE;

        if (t >= TREEBASE) {
            if (!(z & TILEMASK_BURNABLE)) {

              if ((t >= ROADBASE) &&
                  (t <= LASTROAD)) {
                  this.setTile(x, y, RIVER);
              }

              return;
            }

            if (z & TILEMASK_ZONE) {

                this.startFireInZone(x, y, z);

                if (t > RZB) {
                    this.makeExplosionAt(ox, oy);
                }

            }

            if (this.checkWet(t)) {

                this.setTile(
                    x,
                    y,
                    RIVER);

            } else {

                this.setTile(
                    x,
                    y,
                    (this.doAnimation ? TINYEXP : (LASTTINYEXP - 3)) |
                     this.fileMapBits.TILEMASK_BULLDOZABLE |
                     this.fileMapBits.TILEMASK_ANIMATED);

            }

        }

    };


    p.startFireInZone = function startFireInZone(Xloc, Yloc, ch) {

        var value =
            this.rateOfGrowthMap.worldGet(Xloc, Yloc);

        value =
            this.clamp(value - 20, -200, 200);

        this.rateOfGrowthMap.worldSet(Xloc, Yloc, value);

        ch &= TILEMASK_TILE;

        var XYmax;
        if (ch < PORTBASE) {
            XYmax = 2;
        } else {
            if (ch === AIRPORT) {
                XYmax = 5;
            } else {
                XYmax = 4;
            }
        }

        for (var x = -1;
             x < XYmax;
             x++) {

            for (var y = -1;
                 y < XYmax;
                 y++) {

                var Xtemp = Xloc + x;
                var Ytemp = Yloc + y;

                if (this.testBounds(Xtemp, Ytemp)) {

                    var newTile =
                        this.getTile(Xtemp, Ytemp);

                    if ((newTile & TILEMASK_TILE) >= ROADBASE) {

                        newTile |= TILEMASK_BULLDOZABLE;

                        this.setTile(Xtemp, Ytemp, newTile);

                    }

                }

            }

        }

    };


    p.startFire = function startFire(x, y) {

        x >>= 4;
        y >>= 4;

        if (!this.testBounds(x, y)) {
            return;
        }

        var z = this.getTile(x, y);
        var t = z & this.tileMaBits.TILEMASK_TILE;

        if (!(z & TILEMASK_BURNABLE) &&
            (t !== DIRT)) {
            return;
        }

        if (z & TILEMASK_ZONE) {
            return;
        }

        this.setTile(x, y, this.randomFire());

    };


    p.generateTrain = function generateTrain(x, y) {

        if ((this.totalPop > 20) &&
            (this.getSprite(SPRITE_TRAIN) === null) &&
            (this.getRandom(25) === 0)) {

            this.makeSprite(
                SPRITE_TRAIN,
                (x << 4) + TRAIN_GROOVE_X,
                (y << 4) + TRAIN_GROOVE_Y);

        }

    };


    p.generateBus = function generateBus(x, y) {

        if ((this.getSprite(SPRITE_BUS) === null) &&
            (this.getRandom(25) === 0)) {

            makeSprite(
                SPRITE_BUS,
                (x << 4) + BUS_GROOVE_X,
                (y << 4) + BUS_GROOVE_Y);

        }

    };


    p.generateShip = function generateShip() {

        var x;
        var y;

        if (!(this.getRandom16() & 3)) {

            for (x = 4;
                 x < WORLD_W - 2;
                 x++) {

                if (this.getTile(x, 0) === CHANNEL) {
                    this.makeShipHere(x, 0);
                    return;
                }

            }

        }

        if (!(this.getRandom16() & 3)) {

            for (y = 1;
                 y < WORLD_H - 2;
                 y++) {

                if (this.getTile(0, y) === CHANNEL)  {
                    this.makeShipHere(0, y);
                    return;
                }

            }

        }

        if (!(this.getRandom16() & 3)) {

            for (x = 4;
                 x < WORLD_W - 2;
                 x++) {

                if (this.getTile(x, WORLD_H - 1) === CHANNEL)  {
                    this.makeShipHere(x, WORLD_H - 1);
                    return;
                }
            }
        }

        if (!(this.getRandom16() & 3)) {

            for (y = 1;
                 y < WORLD_H - 2;
                 y++) {

                if (this.getTile(WORLD_W - 1, y) === CHANNEL) {
                    this.makeShipHere(WORLD_W - 1, y);
                    return;
                }

            }

        }

    };


    p.makeShipHere = function makeShipHere(x, y) {

        this.makeSprite(
            SPRITE_SHIP,
            (x << 4) - (48 - 1),
            (y << 4));

    };


    p.makeMonster = function makeMonster() {

        var done = false;

        var sprite =
            this.getSprite(SPRITE_MONSTER);

        if (sprite !== null) {
            sprite.soundCount = 1;
            sprite.count = 1000;
            sprite.destX = this.pollutionMaxX << 4;
            sprite.destY = this.pollutionMaxY << 4;
            return;
        }

        for (var z = 0;
             z < 300;
             z++)  {

            var x = this.getRandom(WORLD_W - 20) + 10;
            var y = this.getRandom(WORLD_H - 10) + 5;

            var tile = this.getTile(x, y) & TILEMASK_TILE;
            if (tile === RIVER) {
                this.makeMonsterAt(x, y);
                done = true;
                break;
            }

        }

        if (!done) {
            this.makeMonsterAt(60, 50);
        }

    };


    p.makeMonsterAt = function makeMonsterAt(x, y) {

        this.makeSprite(
            SPRITE_MONSTER,
            (x << 4) + 48,
            (y << 4));

        this.sendMessage('MONSTER_SIGHTED', x + 5, y, true, true);

    };


    p.generateCopter = function generateCopter(x, y) {

        if (this.getSprite(SPRITE_HELICOPTER) !== null) {
            return;
        }

        this.makeSprite(
            SPRITE_HELICOPTER,
            x << 4,
            (y << 4) + 30);

    };


    p.generatePlane = function generatePlane(pos) {

        if (this.getSprite(SPRITE_AIRPLANE) !== null) {
            return;
        }

        this.makeSprite(
            SPRITE_AIRPLANE,
            (pos.posX << 4) + 48,
            (pos.posY << 4) + 12);

    };


    p.makeTornado = function makeTornado() {

        var sprite =
            this.getSprite(SPRITE_TORNADO);

        if (sprite !== null) {
            sprite.count = 200;
            return;
        }

        var x = this.getRandom((WORLD_W << 4) - 800) + 400;
        var y = this.getRandom((WORLD_H << 4) - 200) + 100;

        this.makeSprite(SPRITE_TORNADO, x, y);
        this.sendMessage('TORNADO_SIGHTED', (x >> 4) + 3, (y >> 4) + 2, true, true);

    };


    p.makeExplosion = function makeExplosion(x, y) {

        if (this.testBounds(x, y)) {
            this.makeExplosionAt((x << 4) + 8, (y << 4) + 8);
        }

    };


    p.makeExplosionAt = function makeExplosionAt(x, y) {

        this.newSprite('', SPRITE_EXPLOSION, x - 40, y - 16);

    };


    p.spend = function spend(dollars) {

        this.setFunds(
            this.totalFunds - dollars);

    };


    p.setFunds = function setFunds(dollars) {

        this.totalFunds = dollars;
        this.updateFunds();

    };


    p.invalidateMaps = function invalidateMaps() {

        this.mapSerial++;
        this.doUpdateMap();

    };


    p.testBounds = function testBounds(wx, wy) {

        return (
            (wx >= 0) &&
            (wx < WORLD_W) &&
            (wy >= 0) &&
            (wy < WORLD_H));

    };


    p.getTile = function getTile(x, y) {

        if (!this.testBounds(x, y)) {
            return DIRT;
        }

        return this.map.get(x, y);
    };


    p.setTile = function setTile(x, y, tile) {

        if (!this.testBounds(x, y)) {
            return;
        }

        this.map.set(x, y, tile);
    };


    p.getMapBuffer = function getMapBuffer() {
        return this.map.getData();
    };


    p.getPowerGrid = function getPowerGrid(x, y) {

        return this.powerGridMap.worldGet(x, y);

    };


    p.setPowerGrid = function setPowerGrid(x, y, power) {

        this.powerGridMap.worldSet(x, y, power);

    };


    p.getPowerGridMapBuffer = function getPowerGridMapBuffer() {

        return this.powerGridMap.getData();
    };


    p.getPopulationDensity = function getPopulationDensity(x, y) {

        return this.populationDensityMap.get(x, y);

    };


    p.setPopulationDensity = function setPopulationDensity(x, y, density) {

        this.populationDensityMap.set(x, y, density);

    };


    p.getPopulationDensityMapBuffer = function getPopulationDensityMapBuffer() {

        return this.populationDensityMap.getData();

    };


    p.getRateOfGrowth = function getRateOfGrowth(x, y) {

        return this.rateOfGrowthMap.get(x, y);

    };


    p.setRateOfGrowth = function setRateOfGrowth(x, y, rate) {

        this.rateOfGrowthMap.set(x, y, rate);

    };


    p.getRateOfGrowthMapBuffer = function getRateOfGrowthMapBuffer() {

        return this.rateOfGrowthMap.getData();

    };


    p.getTrafficDensity = function getTrafficDensity(x, y) {

        return this.trafficDensityMap.get(x, y);

    };


    p.setTrafficDensity = function setTrafficDensity(x, y, density) {

        this.trafficDensityMap.set(x, y, density);

    };


    p.getTrafficDensityMapBuffer = function getTrafficDensityMapBuffer() {

        return this.trafficDensityMap.getData();

    };


    p.getPollutionDensity = function getPollutionDensity(x, y) {

        return this.pollutionDensityMap.get(x, y);

    };


    p.setPollutionDensity = function setPollutionDensity(x, y, density) {

        this.pollutionDensityMap.set(x, y, density);

    };


    p.getPollutionDensityMapBuffer = function getPollutionDensityMapBuffer() {

        return this.pollutionDensityMap.getData();

    };


    p.getCrimeRate = function getCrimeRate(x, y) {

        return this.crimeRateMap.get(x, y);

    };


    p.setCrimeRate = function setCrimeRate(x, y, rate) {

        this.crimeRateMap.set(x, y, rate);

    };


    p.getCrimeRateMapBuffer = function getCrimeRateMapBuffer() {

        return this.crimeRateMap.getData();

    };


    p.getLandValue = function getLandValue(x, y) {

        return this.landValueMap.get(x, y);

    };


    p.setLandValue = function setLandValue(x, y, value) {

        this.landValueMap.set(x, y, value);

    };


    p.getLandValueMapBuffer = function getLandValueMapBuffer() {

        return this.landValueMap.getData();

    };


    p.getFireCoverage = function getFireCoverage(x, y) {

        return this.fireStationEffectMap.get(x, y);

    };


    p.setFireCoverage = function setFireCoverage(x, y, coverage) {

        this.fireStationEffectMap.set(x, y, coverage);

    };


    p.getFireCoverageMapBuffer = function getFireCoverageMapBuffer() {

        return this.fireStationEffectMap.getData();

    };


    p.getPoliceCoverage = function getPoliceCoverage(x, y) {

        return this.policeStationEffectMap.get(x, y);

    };


    p.setPoliceCoverage = function setPoliceCoverage(x, y, coverage) {

        this.policeStationEffectMap.set(x, y, coverage);

    };


    p.getPoliceCoverageMapBuffer = function getPoliceCoverageMapBuffer() {

        return this.policeStationEffectMap.getData();

    };


    p.putDownPark = function putDownPark(x, y, effects) {

        var value = this.getRandom(4);
        var tile = TILEMASK_BURNABLE | TILEMASK_BULLDOZABLE;

        if (value === 4) {
            tile |= FOUNTAIN | TILEMASK_ANIMATED;
        } else {
            tile |= value + WOODS2;
        }

        if (effects.getTile(x, y) !== DIRT) {
            return TOOLRESULT_NEED_BULLDOZE;
        }

        effects.addCost(
            editingToolCost[TOOL_PARK]);

        effects.setTile(x, y, tile);

        return TOOLRESULT_OK;

    };


    p.putDownNetwork = function putDownNetwork(x, y, effects) {

        var tile = effects.getTile(x, y);

        if ((tile !== DIRT) &&
            this.tally(tile)) {

            effects.addCost(
                editingToolCost[TOOL_BULLDOZER]);

            effects.setTile(x, y, DIRT);

            tile = DIRT;
        }

        if (tile !== DIRT) {
            return TOOLRESULT_NEED_BULLDOZE;
        }

        effects.setTile(x, y, TELEBASE | TILEMASK_CONDUCTIVE | TILEMASK_BURNABLE | TILEMASK_BULLDOZABLE | TILEMASK_ANIMATED);

        effects.addCost(
            editingToolCost[TOOL_NETWORK]);

        return TOOLRESULT_OK;

    };


    p.putDownWater = function putDownWater(x, y, effects) {

        var tile = effects.getTile(x, y);

        if (tile === RIVER) {
            return TOOLRESULT_FAILED;
        }

        effects.setTile(x, y, RIVER);

        effects.addCost(
            editingToolCost[TOOL_WATER]);

        return TOOLRESULT_OK;
    };


    p.putDownLand = function putDownLand(x, y, effects) {

        var tile = effects.getTile(x, y);
        var dx = [ -1,  0,  1, -1, 1, -1,  0,  1 ];
        var dy = [ -1, -1, -1,  0, 0,  1,  1,  1 ];

        if (tile === DIRT) {
            return TOOLRESULT_FAILED;
        }

        effects.setTile(x, y, DIRT);

    /*
        for (var i = 0;
             i < 8;
             i++) {

            var xx = x + dx[i];
            var yy = y + dy[i];

            if (testBounds(xx, yy)) {
                smoothTreesAt(xx, yy, true);
            }

        }
    */

        effects.addCost(
            editingToolCost[TOOL_LAND]);

        return TOOLRESULT_OK;
    };


    p.putDownForest = function putDownForest(x, y, effects) {

        var dx = [ -1,  0,  1, -1, 1, -1,  0,  1 ];
        var dy = [ -1, -1, -1,  0, 0,  1,  1,  1 ];

        effects.setTile(x, y, WOODS | TILEMASK_BULLDOZABLE | TILEMASK_BURNABLE);

        for (var i = 0;
             i < 8;
             i++) {

            var xx = x + dx[i];
            var yy = y + dy[i];

            if (this.testBounds(xx, yy)) {
                this.smoothTreesAt(xx, yy, true, effects);
            }

        }

        effects.addCost(
            editingToolCost[TOOL_FOREST]);

        return TOOLRESULT_OK;
    };


    p.checkBigZone = function checkBigZone(tile) {

        switch (tile) {

            case POWERPLANT:
            case PORT:
            case NUCLEAR:
            case STADIUM:
                return [0, 0, 4];

            case POWERPLANT + 1:
            case COALSMOKE3:
            case COALSMOKE3 + 1:
            case COALSMOKE3 + 2:
            case PORT + 1:
            case NUCLEAR + 1:
            case STADIUM + 1:
                return [-1, 0, 4];

            case POWERPLANT + 4:
            case PORT + 4:
            case NUCLEAR + 4:
            case STADIUM + 4:
                return [0, -1, 4];

            case POWERPLANT + 5:
            case PORT + 5:
            case NUCLEAR + 5:
            case STADIUM + 5:
                return [-1, -1, 4];

            case AIRPORT:
                return [0, 0, 6];

            case AIRPORT + 1:
                return [-1, 0, 6];

            case AIRPORT + 2:
                return [-2, 0, 6];

            case AIRPORT + 3:
                return [-3, 0, 6];

            case AIRPORT + 6:
                return [0, -1, 6];

            case AIRPORT + 7:
                return [-1, -1, 6];

            case AIRPORT + 8:
                return [-2, -1, 6];

            case AIRPORT + 9:
                return [-3, -1, 6];

            case AIRPORT + 12:
                return [0, -2, 6];

            case AIRPORT + 13:
                return [-1, -2, 6];

            case AIRPORT + 14:
                return [-2, -2, 6];

            case AIRPORT + 15:
                return [-3, -2, 6];

            case AIRPORT + 18:
                return [0, -3, 6];

            case AIRPORT + 19:
                return [-1, -3, 6];

            case AIRPORT + 20:
                return [-2, -3, 6];

            case AIRPORT + 21:
                return [-3, -3, 6];

            default:
                break;

        }

        return [0, 0, 0];
    };


    p.tally = function tally(tile) {

        return ((tile >= FIRSTRIVEDGE)  && (tile <= LASTRUBBLE)) ||
               ((tile >= POWERBASE + 2) && (tile <= POWERBASE + 12)) ||
               ((tile >= TINYEXP)       && (tile <= LASTTINYEXP + 2));

    };


    p.checkSize = function checkSize(tile) {

        // check for the normal com, resl, ind 3x3 zones & the fireDept & PoliceDept
        if (((tile >= RESBASE - 1)        && (tile <= PORTBASE - 1)) ||
            ((tile >= LASTPOWERPLANT + 1) && (tile <= POLICESTATION + 4)) ||
            ((tile >= CHURCH1BASE)        && (tile <= CHURCH7LAST))) {
            return 3;
        }

        if (((tile >= PORTBASE)    && (tile <= LASTPORT)) ||
            ((tile >= COALBASE)    && (tile <= LASTPOWERPLANT)) ||
            ((tile >= STADIUMBASE) && (tile <= LASTZONE))) {
            return 4;
        }

        return 0;
    };


    p.checkBorder = function checkBorder(x, y, sizeX, sizeY, effects) {

        var i;

        // This will do the upper bordering row.
        for (i = 0; i < sizeX; i++) {
            this.connectTile(
                x + i,
                y - 1,
                CONNECTTILE_FIX,
                effects);
        }

        // This will do the left bordering row.
        for (i = 0; i < sizeY; i++) {
            this.connectTile(
                x - 1,
                y + i,
                CONNECTTILE_FIX,
                effects);
        }

        // This will do the bottom bordering row.
        for (i = 0; i < sizeX; i++) {
            this.connectTile(
                x + i,
                y + sizeY,
                CONNECTTILE_FIX,
                effects);
        }

        // This will do the right bordering row.
        for (i = 0; i < sizeY; i++) {
            this.connectTile(
                x + sizeX,
                y + i,
                CONNECTTILE_FIX, effects);
        }

    };


    p.putBuilding = function putBuilding(leftX, topY, sizeX, sizeY, baseTile, animated, effects) {

        for (var dy = 0;
             dy < sizeY;
             dy++) {

            var posY = topY + dy;

            for (var dx = 0;
                 dx < sizeX;
                 dx++) {

                var posX = leftX + dx;

                var tileValue = baseTile | TILE_BNCNBIT;

                if (dx === 1) {
                    if (dy === 1) {
                        tileValue |= TILEMASK_ZONE;
                    } else if (dy === 2 && animated) {
                        tileValue |= TILEMASK_ANIMATED;
                    }
                }

                effects.setTile(posX, posY, tileValue);

                baseTile++;

            }

        }

    };


    p.prepareBuildingSite = function prepareBuildingSite(leftX, topY, sizeX, sizeY, effects) {

        // Check that the entire site is on the map.
        if ((leftX < 0) ||
            ((leftX + sizeX) > WORLD_W) ||
            (topY < 0) ||
            ((topY + sizeY) > WORLD_H)) {
            return TOOLRESULT_FAILED;
        }

        // Check whether the tiles are clear.
        for (var dy = 0; dy < sizeY; dy++) {

            var posY = topY + dy;

            for (var dx = 0; dx < sizeX; dx++) {

                var posX = leftX + dx;

                var tileValue = effects.getTile(posX, posY);

                if (tileValue === DIRT) { // DIRT tile is buildable.
                    continue;
                }

                // No DIRT and no bull-dozer => not buildable.
                // Or if tally returns false => not buildable.
                if (!this.autoBulldoze ||
                    !this.tally(tileValue)) {

                    return TOOLRESULT_NEED_BULLDOZE;

                }

                effects.setTile(posX, posY, DIRT);

                effects.addCost(
                    editingToolCost[TOOL_BULLDOZER]);

            }

        }

        return TOOLRESULT_OK;
    };


    p.buildBuilding = function buildBuilding(x, y, buildingProps, effects) {

        // Move position to top-left.
        x--;
        y--;

        var prepareResult =
            this.prepareBuildingSite(
                x,
                y,
                buildingProps.sizeX,
                buildingProps.sizeY,
                effects);

        if (prepareResult !== TOOLRESULT_OK) {
            return prepareResult;
        }

        // Preparation was ok, put down the building.
        effects.addCost(
            editingToolCost[buildingProps.tool]);

        this.putBuilding(
            x,
            y,
            buildingProps.sizeX,
            buildingProps.sizeY,
            buildingProps.baseTile,
            buildingProps.buildingIsAnimated,
            effects);

        this.checkBorder(
            x,
            y,
            buildingProps.sizeX,
            buildingProps.sizeY,
            effects);

        return TOOLRESULT_OK;
    };


    p.getDensity = function getDensity(densityType, x, y) {

        var z;

        switch (densityType) {

            case 0:
                z = this.populationDensityMap.worldGet(x, y);
                z = z >> 6;
                z = z & 3;
                return 'POPULATIONDENSITY_' + z;

            case 1:
                z = this.landValueMap.worldGet(x, y);
                if (z < 30) {
                    return 'LANDVALUE_SLUM';
                }
                if (z < 80) {
                    return 'LANDVALUE_LOWERCLASS';
                }
                if (z < 150) {
                    return 'LANDVALUE_MIDDLECLASS';
                }
                return 'LANDVALUE_HIGHCLASS';

            case 2:
                z = this.crimeRateMap.worldGet(x, y);
                z = z >> 6;
                z = z & 3;
                return 'CRIMERATE_' + z;

            case 3:
                z = this.pollutionDensityMap.worldGet(x, y);
                if ((z < 64) && (z > 0)) {
                    z = 0;
                } else {
                    z = z >> 6;
                    z = z & 3;
                    z++;
                }
                return z + 'POLLUTIONDENSITY_' + z;

            case 4:
                z = this.rateOfGrowthMap.worldGet(x, y);
                if (z < 0) {
                    return 'RATEOFGROWTH_DECLINING';
                }
                if (z === 0) {
                    return 'RATEOFGROWTH_STABLE';
                }
                if (z > 100) {
                    return 'RATEOFGROWTH_FAST';
                }
                return 'RATEOFGROWTH_SLOW';

            default:
                return null;

        }

    };


    p.doZoneStatus = function doZoneStatus(x, y) {

        var tileNum = this.getTile(x, y) & TILEMASK_TILE;

        if ((tileNum >= COALSMOKE1) &&
            (tileNum < FOOTBALLGAME1)) {
          tileNum = COALBASE;
        }

        // Find the category where the tile belongs to.
        // Note: If 'tileNum < statusZones[i]',
        // then it belongs to category i - 1.
        var i;
        for (i = 1; i < 29; i++) {
            if (tileNum < statusZones[i]) {
                break;
            }
        }

        i--;

        // Now i contains the category that the tile belongs to (in theory 0..27).
        // However, it is 0..26, since 956 is the first unused tile

        // FIXME: This needs to be fixed to support plug-in churches.

        // TODO: This should also return the bounding box and hot spot of
        // the zone to the user interface, as well as other interesting
        // information.

        // Code below looks buggy, 0 is a valid value (namely 'dirt'), and upper
        // limit is not correctly checked either ('stri.219' has only 27 lines).

        // FIXME: This is strange... Normalize to zero based index.
        if ((i < 1) || (i > 28)) {
          i = 28;  // This breaks the program (when you click 'dirt')
        }

        // Obtain the string index of the tile category.
        // 'stri.219' has only 27 lines, so 0 <= i <= 26 is acceptable.
        var tileCategory = i + 1;

        var status = [];
        for (i = 0; i < 5; i++) {

            var id =
                this.clamp(
                    this.getDensity(i, x, y) + 1,
                    1,
                    20);

            status.push(id);

        }

        this.doShowZoneStatus(
            tileCategory,
            status[0],
            status[1],
            status[2],
            status[3],
            status[4], 
            x, y);

    };


    p.putRubble = function putRubble(x, y, size, effects) {

        for (var xx = x; xx < x + size; xx++) {

            for (var yy = y; yy < y + size; yy++)  {

                if (this.testBounds(this.xx, yy)) {

                    var tile = effects.getTile(xx, yy);

                    if ((tile !== RADTILE) &&
                        (tile !== DIRT)) {

                        tile =
                            (this.doAnimation ? (TINYEXP + this.getRandom(2)) : SOMETINYEXP);

                        effects.setTile(
                            xx,
                            yy,
                            tile | TILEMASK_ANIMATED | TILEMASK_BULLDOZABLE);

                    }

                }

            }

        }

    };


    p.queryTool = function queryTool(x, y) {

        if (!this.testBounds(x, y)) {
            return TOOLRESULT_FAILED;
        }

        this.doZoneStatus(x, y);

        this.doTool('Qry', x, y);

        return TOOLRESULT_OK;
    };


    p.bulldozerTool = function bulldozerTool(x, y) {

        var effects = new ToolEffects(this);

        var result = this.doBulldozerTool(x, y, effects);

        if (result === TOOLRESULT_OK) {
            effects.modifyWorld();
        }

        return result;

    };


    p.bulldozerTool = function bulldozerTool(x, y, effects) {

        var result = TOOLRESULT_OK;

        if (!this.testBounds(x, y)) {
            return TOOLRESULT_FAILED;
        }

        var mapValue = effects.getTile(x, y);
        var tile = mapValue & TILEMASK_TILE;
        var deltaH = 0; // Amount of horizontal shift to the center tile of the zone.
        var deltaV = 0; // Amount of vertical shift to the center tile of the zone.
        var zoneSize = 0; // size of the zone, 0 means invalid.

        if (mapValue & TILEMASK_ZONE) { // Zone center bit is set.

            deltaH = 0;
            deltaV = 0;
            zoneSize = this.checkSize(tile);

        } else {

            var hvSize = this.checkBigZone(tile);
            deltaH = hvSize[0];
            deltaV = hvSize[1];
            zoneSize = hvSize[2];

        }

        if (zoneSize > 0) {

            effects.addCost(
                editingToolCost[TOOL_BULLDOZER]);

            var centerX = x + deltaH;
            var centerY = y + deltaV;

            switch (zoneSize) {

                case 3:
                    effects.addMessage(
                        'makeSound',
                        ['city', 'Explosion-High', x, y]);
                    this.putRubble(centerX - 1, centerY - 1, 3, effects);
                    break;

                case 4:
                    effects.addMessage(
                        'makeSound',
                        ['city', 'Explosion-Low', x, y]);
                    this.putRubble(centerX - 1, centerY - 1, 4, effects);
                    break;

                case 6:
                    effects.addMessage(
                        'makeSound',
                        ['city', 'Explosion-High', x, y]);
                    effects.addMessage(
                        'makeSound',
                        ['city', 'Explosion-Low', x, y]);
                    this.putRubble(centerX - 1, centerY - 1, 6, effects);
                    break;

                default:
                    break;

            }


            if (result === TOOLRESULT_OK) {

                effects.addMessage(
                    'doTool',
                    ['Dozr', x, y]);

            }

            return result;

        }

        if ((tile === RIVER) ||
            (tile === REDGE) ||
            (tile === CHANNEL)) {

            result =
                this.connectTile(
                    x,
                    y,
                    CONNECTTILE_BULLDOZE,
                    effects);

            if (tile !== effects.getTile(x, y)) {
                effects.addCost(5);
            }

        } else {

            result =
                this.connectTile(
                    x,
                    y,
                    CONNECTTILE_BULLDOZE,
                    effects);

        }

        if (result === TOOLRESULT_OK) {

            effects.addMessage(
                'doTool',
                ['Dozr', x, y]);

        }

        return result;

    };


    p.roadTool = function roadTool(x, y, effects) {

        if (!this.testBounds(x, y)) {
            return TOOLRESULT_FAILED;
        }

        var result =
            this.connectTile(
                x,
                y,
                CONNECTTILE_ROAD,
                effects);

        if (result === TOOLRESULT_OK) {

            effects.addMessage(
                'doTool',
                ['Road', x, y]);

        }

        return result;

    };


    p.railroadTool = function railroadTool(x, y, effects) {

        if (!this.testBounds(x, y)) {
            return TOOLRESULT_FAILED;
        }

        var result =
            this.connectTile(
                x,
                y,
                CONNECTTILE_RAILROAD,
                effects);

        if (result === TOOLRESULT_OK) {

            effects.addMessage(
                'doTool',
                ['Rail', x, y]);

        }

        return result;

    };


    p.wireTool = function wireTool(x, y, effects) {

        if (!this.testBounds(x, y)) {
            return TOOLRESULT_FAILED;
        }

        var result =
            this.connectTile(
                x,
                y,
                CONNECTTILE_WIRE,
                effects);

        if (result === TOOLRESULT_OK) {

            effects.addMessage(
                'doTool',
                ['Wire', x, y]);

        }

        return result;

    };


    p.parkTool = function parkTool(x, y, effects) {

        if (!this.testBounds(x, y)) {
            return TOOLRESULT_FAILED;
        }

        var result =
            this.putDownPark(x, y, effects);

        if (result === TOOLRESULT_OK) {

            effects.addMessage(
                'doTool',
                ['Park', x, y]);

        }

        return result;

    };


    p.buildBuildingTool = function buildBuildingTool(x, y, buildingProps, effects) {

        var result =
            this.buildBuilding(
                x,
                y,
                buildingProps,
                effects);

        if (result === TOOLRESULT_OK) {

            effects.addMessage(
                'doTool',
                [buildingProps.toolName, x, y]);

        }

        return result;

    };


    p.networkTool = function networkTool(x, y, effects) {

        if (!this.testBounds(x, y)) {
            return TOOLRESULT_FAILED;
        }

        var result =
            this.putDownNetwork(
                x,
                y,
                effects);

        if (result === TOOLRESULT_OK) {

            effects.addMessage(
                'doTool',
                ['Net', x, y]);

        }

        return result;

    };


    p.waterTool = function waterTool(x, y, effects) {

        if (!this.testBounds(x, y)) {
            return TOOLRESULT_FAILED;
        }

        var result =
            this.bulldozerTool(
                x,
                y,
                effects);

        if (result === TOOLRESULT_OK) {

            result =
                this.putDownWater(
                    x,
                    y,
                    effects);

        }

        if (result === TOOLRESULT_OK) {

            effects.addMessage(
                'doTool',
                ['Water', x, y]);

        }

        return result;

    };


    p.landTool = function landTool(x, y, effects) {

        if (!this.testBounds(x, y)) {
            return TOOLRESULT_FAILED;
        }

        /// @todo: Is this good? It is not auto-bulldoze!!
        /// @todo: Handle result value (probably)
        var result =
            this.bulldozerTool(
                x,
                y,
                effects);

        result =
            this.putDownLand(
                x,
                y,
                effects);

        if (result === TOOLRESULT_OK) {

            effects.addMessage(
                'doTool',
                ['Land', x, y]);

        }

        return result;

    };


    p.forestTool = function forestTool(x, y, effects) {

        if (!this.testBounds(x, y)) {
            return TOOLRESULT_FAILED;
        }

        var result = TOOLRESULT_OK;
        var tile = effects.getTile(x, y);

        if (this.isTree(tile)) {
            return TOOLRESULT_OK;
        }

        if ((tile & TILEMASK_TILE) !== DIRT) {

            /// @todo bulldozer should be free in terrain mode or from a free tool.
            result =
                this.bulldozerTool(
                    x,
                    y,
                    effects);

        }

        tile = effects.getTile(x, y);

        if (tile === DIRT) {

            result =
                this.putDownForest(
                    x,
                    y,
                    effects);

            if (result === TOOLRESULT_OK) {

                effects.addMessage(
                    'doTool',
                    ['Forest', x, y]);

            }

        } else {

            result = TOOLRESULT_FAILED;

        }

        return result;

    };


    p.doTool = function doTool(tool, tileX, tileY) {

        var effects = new ToolEffects(this);
        var result;

        switch (tool) {

            case TOOL_RESIDENTIAL:
                result =
                    this.buildBuildingTool(
                        tileX,
                        tileY,
                        residentialZoneBuilding,
                        effects);
                break;

            case TOOL_COMMERCIAL:
                result =
                    this.buildBuildingTool(
                        tileX,
                        tileY,
                        commercialZoneBuilding,
                        effects);
                break;

            case TOOL_INDUSTRIAL:
                result =
                    this.buildBuildingTool(
                        tileX,
                        tileY,
                        industrialZoneBuilding,
                        effects);
                break;

            case TOOL_FIRESTATION:
                result =
                    this.buildBuildingTool(
                        tileX,
                        tileY,
                        fireStationBuilding,
                        effects);
                break;

            case TOOL_POLICESTATION:
                result =
                    this.buildBuildingTool(
                        tileX,
                        tileY,
                        policeStationBuilding,
                        effects);
                break;

            case TOOL_QUERY:
                return this.queryTool(
                    tileX,
                    tileY);

            case TOOL_WIRE:
                result =
                    this.wireTool(
                        tileX,
                        tileY,
                        effects);
                break;

            case TOOL_BULLDOZER:
                result =
                    this.bulldozerTool(
                        tileX,
                        tileY,
                        effects);
                break;

            case TOOL_RAILROAD:
                result =
                    this.railroadTool(
                        tileX,
                        tileY,
                        effects);
                break;

            case TOOL_ROAD:
                result =
                    this.roadTool(
                        tileX,
                        tileY,
                        effects);
                break;

            case TOOL_STADIUM:
                result =
                    this.buildBuildingTool(
                        tileX,
                        tileY,
                        stadiumBuilding,
                        effects);
                break;

            case TOOL_PARK:
                result =
                    this.parkTool(
                        tileX, tileY, effects);
                break;

            case TOOL_SEAPORT:
                result =
                    this.buildBuildingTool(
                        tileX,
                        tileY,
                        seaportBuilding,
                        effects);
                break;

            case TOOL_COALPOWER:
                result =
                    this.buildBuildingTool(
                        tileX,
                        tileY,
                        coalPowerBuilding,
                        effects);
                break;

            case TOOL_NUCLEARPOWER:
                result =
                    this.buildBuildingTool(
                        tileX,
                        tileY,
                        nuclearPowerBuilding,
                        effects);
                break;

            case TOOL_AIRPORT:
                result =
                    this.buildBuildingTool(
                        tileX,
                        tileY,
                        airportBuilding,
                        effects);
                break;

            case TOOL_NETWORK:
                result =
                    this.networkTool(
                        tileX,
                        tileY,
                        effects);
                break;

            case TOOL_WATER:
                result =
                    this.waterTool(
                        tileX,
                        tileY,
                        effects);
                break;

            case TOOL_LAND:
                result =
                    this.landTool(
                        tileX,
                        tileY,
                        effects);
                break;

            case TOOL_FOREST:
                result =
                    this.forestTool(
                        tileX,
                        tileY,
                        effects);
                break;

            default:
                return TOOLRESULT_FAILED;

        }

        // Perform the effects of applying the tool if enough funds.
        if ((result === TOOLRESULT_OK) &&
            !effects.modifyIfEnoughFunding()) {
            return TOOLRESULT_NO_MONEY;
        }

        return result;

    };


    p.toolDown = function toolDown(tool, tileX, tileY) {

        var result =
            this.doTool(
                tool,
                tileX,
                tileY);

        if (result === TOOLRESULT_NEED_BULLDOZE) {

            this.sendMessage('BULLDOZE_AREA_FIRST', null, null, false, true);

            /// @todo: Multi player: This sound should only be heard by the user
            ///        who called this function.
            this.makeSound('interface', 'UhUh', tileX << 4, tileY << 4);

        } else if (result === TOOLRESULT_NO_MONEY) {

            this.sendMessage('NOT_ENOUGH_FUNDS', null, null, false, true);

            /// @todo: Multi player: This sound should only be heard by the user
            ///        who called this function.
            this.makeSound('interface', 'Sorry', tileX << 4, tileY << 4);

        }

        this.simPass = 0;
        this.invalidateMaps();
    };


    p.toolDrag = function toolDrag(tool, fromX, fromY, toX, toY) {

        // Do not drag big tools.
        var toolSize =
            editingToolSize[tool];

        if (toolSize > 1) {

            this.doTool(tool, toX, toY);

            this.simPass = 0; // Update editors overlapping this one.
            this.invalidateMaps();

            return;

        }

        if ((fromX === toX) &&
            (fromY === toY)) {
            return;
        }

        var dirX = (toX > fromX) ? 1 : -1; // Horizontal step direction.
        var dirY = (toY > fromY) ? 1 : -1; // Vertical step direction.

        this.doTool(
            tool,
            fromX,
            fromY); // Ensure the start position is done.

        // Vertical line up or down
        if ((fromX === toX) &&
            (fromY !== toY)) {

            while (fromY !== toY) {
                fromY += dirY;
                this.doTool(tool, fromX, fromY);
            }

            this.simPass = 0; // Update editors overlapping this one.
            this.invalidateMaps();

            return;

        }

        // Horizontal line left/right
        if ((fromX !== toX) &&
            (fromY === toY)) {

            while (fromX !== toX) {
                fromX += dirX;
                this.doTool(tool, fromX, fromY);
            }

            this.simPass = 0; // Update editors overlapping this one.
            this.invalidateMaps();

            return;
        }

        // General case: both X and Y change.

        var dx = Math.abs(fromX - toX); // Number of horizontal steps.
        var dy = Math.abs(fromY - toY); // Number of vertical steps.

        var subX = 0; // Each X step is dy sub-steps.
        var subY = 0; // Each Y step is dx sub-steps.
        var numSubSteps = Math.min(dx, dy); // Number of sub-steps we can do.

        while ((fromX !== toX) ||
               (fromY !== toY)) {

            subX += numSubSteps;

            if (subX >= dy) {
                subX -= dy;
                fromX += dirX;
                this.doTool(tool, fromX, fromY);
            }

            subY += numSubSteps;

            if (subY >= dx) {
                subY -= dx;
                fromY += dirY;
                this.doTool(tool, fromX, fromY);
            }

        }

        this.simPass = 0; // Update editors overlapping this one.
        this.invalidateMaps();
    };


    p.makeTrafficAt = function makeTrafficAt(x, y, dest) {

        if (this.tryDrive(x, y, dest)) { // Attempt to drive somewhere.

            this.addToTrafficDensityMap(); // If sucessful, increment traffic density.

            return 1; // Traffic passed.

        }

        return 0; // Traffic failed.

    };


    p.makeTraffic = function makeTraffic(x, y, dest) {

        this.curMapStack = []; // Clear position stack

    /*
          if (!this.getRandom(2)) {
              var foundPos = this.findPerimeterTelecom(x, y);
              if (foundPos) {
                  return 1;
              }
          }
    */

        var foundPos = this.findPerimeterRoad(x, y);
        if (!foundPos) {
            return -1; // No road found.
        }

        // Look for road on zone perimeter.

        if (this.tryDrive(x, y, dest)) { // Attempt to drive somewhere.
            this.addToTrafficDensityMap(); // If sucessful, increment traffic density.
            return 1; // Traffic passed.
        }

        return 0; // Traffic failed.

    };


    p.addToTrafficDensityMap = function addToTrafficDensityMap() {

        // For each saved position of the drive.
        while (this.curMapStack.length > 0) {

            var xy = this.pullPos();
            var x = xy[0];
            var y = xy[1];
            if (this.testBounds(x, y)) {

                var tile = this.getTile(x, y) & TILEMASK_TILE;

                if ((tile >= ROADBASE) &&
                    (tile < POWERBASE)) {

                    // Update traffic density.
                    var traffic =
                        this.trafficDensityMap.worldGet(x, y);

                    traffic += 50;
                    traffic = Math.min(traffic, 240);

                    this.trafficDensityMap.worldSet(x, y, traffic);

                    // Check for heavy traffic.
                    if ((traffic >= 240) &&
                        (this.getRandom(5) === 0)) {

                        this.trafficMaxX = x;
                        this.trafficMaxY = y;

                        // Direct helicopter towards heavy traffic.
                        var sprite =
                            this.getSprite(SPRITE_HELICOPTER);

                        if ((sprite !== null) &&
                            (sprite.control === -1)) {

                            sprite.destX = this.trafficMaxX * 16;
                            sprite.destY = this.trafficMaxY * 16;

                        }

                    }

                }

            }

        }

    };


    p.pushPos = function pushPos(xy) {

        this.curMapStack.push(xy);

    };


    p.pullPos = function pullPos() {

        var xy = this.curMapStack.pop();
        return xy;

    };


    p.findPerimeterRoad = function findPerimeterRoad(x, y) {

        // Look for road on edges of zone.
        var perimX = [ -1, 0, 1, 2, 2, 2, 1, 0,-1,-2,-2,-2 ];
        var perimY = [ -2,-2,-2,-1, 0, 1, 2, 2, 2, 1, 0,-1 ];

        for (var z = 0; z < 12; z++) {

            var tx = x + perimX[z];
            var ty = y + perimY[z];

            if (this.testBounds(tx, ty)) {

                if (this.roadTest(this.getTile(tx, ty))) {

                    return [tx, ty];
                }
            }
        }

        return null;

    };


    p.findPerimeterTelecom = function findPerimeterTelecom(x, y) {

        // Look for telecom on edges of zone.
        var perimX = [ -1, 0, 1, 2, 2, 2, 1, 0,-1,-2,-2,-2 ];
        var perimY = [ -2,-2,-2,-1, 0, 1, 2, 2, 2, 1, 0,-1 ];

        for (var z = 0; z < 12; z++) {

            var tx = x + perimX[z];
            var ty = y + perimY[z];

            if (this.testBounds(tx, ty)) {

                var tile = this.getTile(tx, ty) & TILEMASK_TILE;

                if ((tile >= TELEBASE) &&
                    (tile <= this.mapTileTELELAST)) {
                    return [tx, ty];
                }

            }

        }

        return null;

    };


    p.tryDrive = function tryDrive(x, y, destZone) {

        var dirLast = DIR_INVALID;

        // Maximum distance to try.
        for (var dist = 0;
             dist < MAX_TRAFFIC_DISTANCE;
             dist++) {

            var dir = 
                this.tryGo(x, y, dirLast);

            if (dir !== DIR_INVALID) { // We found a road.

                x += this.directionDX[dir];
                y += directionDY[dir];

                dirLast = this.rotate180(dir);

                // Save pos every other move.
                // This also relates to trafficDensityMap.blockSize.
                if (dist & 1) {
                    this.pushPos([x, y]);
                }

                if (this.driveDone(x, y, destZone)) { // If destination is reached.
                    return true; // Pass.
                }

            } else {

                if (this.curMapStack.length > 0) { // dead end, backup.
                    this.curMapStack.pop();
                    dist += 3;
                } else {
                    return false; // Give up at start.
                }

            }
        }

        return false; // Gone MAX_TRAFFIC_DISTANCE.
    };


    p.tryGo = function tryGo(x, y, dirLast) {

        // Find connections from current position.
        var directions = [];
        var dir = DIR_NORTH;
        var count = 0;
        var i;

        for (i = 0; i < 4; i++) {

            if ((dir !== dirLast) &&
                this.roadTest(
                    this.getTileFromMap(
                        x,
                        y,
                        dir,
                        DIRT))) {

                // Found a road in an allowed direction.
                directions.push(
                    dir);
                count++;

            } else {

                directions.push(
                    DIR_INVALID);

            }

            dir = this.rotate90(dir);

        }

        if (count === 0) { // Dead end.
            return DIR_INVALID;
        }

        // We have at least one way to go.

        if (count === 1) { // Only one solution.

            for (i = 0; i < 4; i++) {

                if (directions[i] !== DIR_INVALID) {
                    return directions[i];
                }

            }

        }

        // more than one choice, draw a random number.
        i = this.getRandom16() & 3;

        while (directions[i] === DIR_INVALID) {
            i = (i + 1) & 3;
        }

        return directions[i];

    };


    p.getTileFromMap = function getTileFromMap(x, y, dir, defaultTile) {

        switch (dir) {

            case DIR_WEST:
                if (x > 0) {
                  return this.getTile(x - 1, y) & TILEMASK_TILE;
                }
                return defaultTile;

            case DIR_NORTH:
                if (y > 0) {
                  return this.getTile(x, y - 1) & TILEMASK_TILE;
                }
                return defaultTile;

            case DIR_EAST:
                if (x < (WORLD_W - 1)) {
                  return this.getTile(x + 1, y) & TILEMASK_TILE;
                }
                return defaultTile;

            case DIR_SOUTH:
                if (y < (WORLD_H - 1)) {
                  return this.getTile(x, y + 1) & TILEMASK_TILE;
                }
                return defaultTile;

            default:
                return defaultTile;

        }

    };


    p.driveDone = function driveDone(x, y, destZone) {

        // Commercial, industrial, residential destinations.
        var targetLow = [COMBASE, LHTHR, LHTHR];
        var targetHigh = [NUCLEAR, PORT, COMBASE];
        var low = targetLow[destZone]; // Lowest acceptable tile value
        var high = targetHigh[destZone]; // Highest acceptable tile value
        var z;

        if (y > 0) {
            z = this.getTile(x, y - 1) & TILEMASK_TILE;
            if ((z >= low) && (z <= high)) {
                return true;
            }
        }

        if (x < (WORLD_W - 1)) {
            z = this.getTile(x + 1, y) & TILEMASK_TILE;
            if ((z >= low) && (z <= high)) {
                return true;
            }
        }

        if (y < (WORLD_H - 1)) {
            z = this.getTile(x, y + 1) & TILEMASK_TILE;
            if ((z >= low) && (z <= high)) {
                return true;
            }
        }

        if (x > 0) {
            z = this.getTile(x - 1, y) & TILEMASK_TILE;
            if ((z >= low) && (z <= high)) {
                return true;
            }
        }

        return false;

    };


    p.roadTest = function roadTest(tile) {

        tile &= TILEMASK_TILE;

        if ((tile < ROADBASE) ||
            (tile > LASTRAIL)) {
            return false;
        }

        if ((tile >= POWERBASE) &&
            (tile < LASTPOWER)) {
            return false;
        }

        return true;

    };


    p.doUpdateHeads = function doUpdateHeads() {

        this.showValves();
        this.updateDate();
        this.reallyUpdateFunds();
        this.updateOptions();

    };


    p.updateMaps = function updateMaps() {

        this.invalidateMaps();
        this.doUpdateHeads();

    };


    p.updateGraphs = function updateGraphs() {

        this.changeCensus();

    };


    p.updateEvaluation = function updateEvaluation() {

        this.changeEval();

    };


    p.updateHeads = function updateHeads() {

        this.mustUpdateFunds = true;
        this.valveFlag = true;
        this.cityTimeLast = -999999;
        this.cityYearLast = -999999;
        this.cityMonthLast = -999999;
        this.totalFundsLast = -999999;
        this.resLast = -999999;
        this.comLast = -999999;
        this.indLast = -999999;

        this.doUpdateHeads();

    };


    p.updateFunds = function updateFunds() {

        this.mustUpdateFunds = true;

    };


    p.reallyUpdateFunds = function reallyUpdateFunds() {

        if (!this.mustUpdateFunds) {
            return;
        }

        this.mustUpdateFunds = false;

        if (this.totalFunds !== this.totalFundsLast) {
            this.totalFundsLast = this.totalFunds;

            this.doUpdateFunds();
        }

    };


    p.updateDate = function updateDate() {

        var megalinium = 1000000;

        this.cityTimeLast = this.cityTime >> 2;

        this.cityYear = Math.floor(this.cityTime / 48) + this.startingYear;
        this.cityMonth = (this.cityTime % 48) >> 2;

        if (this.cityYear >= megalinium) {
            this.setYear(this.startingYear);
            this.cityYear = this.startingYear;
            // FIXME: Why this message?
            this.sendMessage('NOT_ENOUGH_POWER', null, null, true, true);
        }

        if ((this.cityYearLast !== this.cityYear) ||
            (this.cityMonthLast !== this.cityMonth)) {

            this.cityYearLast = this.cityYear;
            this.cityMonthLast = this.cityMonth;

            this.doUpdateDate();

        }

    };


    p.showValves = function showValves() {

        if (this.valveFlag) {
            this.drawValve();
            this.valveFlag = false;
        }

    };


    p.drawValve = function drawValve() {

        var r =
            this.clamp(
                Math.floor(this.resValve),
                -1500,
                1500);

        var c =
            this.clamp(
                Math.floor(this.comValve),
                -1500,
                1500);

        var i =
            this.clamp(
                Math.floor(this.indValve),
                -1500,
                1500);

        if ((r !== this.resLast) ||
            (c !== this.comLast) ||
            (i !== this.indLast)) {

            this.resLast = r;
            this.comLast = c;
            this.indLast = i;

            this.doUpdateDemand();

        }

    };


    p.updateOptions = function updateOptions() {

        if (this.mustUpdateOptions) {
            this.mustUpdateOptions = false;
            this.doUpdateOptions();
        }

    };


    p.updateUserInterface = function updateUserInterface() {

        /// @todo Send all pending update messages to the user interface.

        // city: after load file, load scenario, or generate city
        // map: when small overall map changes
        // editor: when large close-up map changes
        // graph: when graph changes
        // evaluation: when evaluation changes
        // budget: when budget changes
        // date: when date changes
        // funds: when funds change
        // demand: when demand changes
        // level: when level changes
        // speed: when speed changes
        // delay: when delay changes
        // option: when options change

    };


    p.makeDollarDecimalStr = function makeDollarDecimalStr(number) {

        var numberString = '' + number;

        var digitCount = numberString.length;
        if (digitCount <= 3) {
            return '$' + numberString;
        }

        var leftMostSet = (digitCount % 3) || 3;
        var commaCount = Math.floor((digitCount - 1) / 3);
        var dollarString = '$';
        var numberIndex = 0;
        var x;

        for (x = 1;
             x <= leftMostSet;
             x++, numberIndex++) {

            dollarString += numberString.substr(numberIndex, 1);

        }

        for (x = 1;
             x <= commaCount;
             x++, numberIndex += 3) {

            dollarString += ',';
            dollarString += numberString.substr(numberIndex, 3);

        }

        return dollarString;

    };


    p.pause = function pause() {

        if (!this.simPaused) {
            this.simPausedSpeed = this.simSpeedMeta;
            this.setSpeed(0);
            this.simPaused = true;
        }

        this.stopTicking();

        // Call back even if the state did not change.
        this.doUpdatePaused();

    };


    p.resume = function resume() {

        if (this.simPaused) {
            this.simPaused = false;
            this.setSpeed(this.simPausedSpeed);
        }

        this.startTicking();

        // Call back even if the state did not change.
        this.doUpdatePaused();

    };


    p.setSpeed = function setSpeed(speed) {

        if (speed < 0) {
            speed = 0;
        } else if (speed > 3) {
            speed = 3;
        }

        this.simSpeedMeta = speed;

        if (this.simPaused) {
            this.simPausedSpeed = this.simSpeedMeta;
            speed = 0;
        }

        this.simSpeed = speed;

        this.doUpdateSpeed();

    };


    p.setPasses = function setPasses(passes) {

        this.simPasses = passes;
        this.simPass = 0;

        this.doUpdatePasses();

    };


    p.setGameLevelFunds = function setGameLevelFunds(level) {

        switch (level) {

            case LEVEL_EASY:
                this.setFunds(LEVEL_FUNDS_EASY);
                this.setGameLevel(level);
                break;

            case LEVEL_MEDIUM:
                this.setFunds(LEVEL_FUNDS_MEDIUM);
                this.setGameLevel(level);
                break;

            case LEVEL_HARD:
                this.setFunds(LEVEL_FUNDS_HARD);
                this.setGameLevel(level);
                break;

            default:
                break;

        }

    };


    p.setGameLevel = function setGameLevel(level) {

        this.gameLevel = level;
        this.doUpdateGameLevel();

    };


    p.setCityName = function setCityName(name) {

        var cleanName = '';

        for (var i = 0, n = name.length;
             i < n;
             i++) {

            var ch = name.substr(i, 1);

            if (VALID_CITY_NAME_CHARACTERS.substr(ch) === -1) {
                ch = '_';
            }

            cleanName += ch;

        }

        this.setCleanCityName(cleanName);

    };


    p.setCleanCityName = function setCleanCityName(name) {

        this.cityName = name;
        this.doUpdateCityName();

    };


    p.setYear = function setYear(year) {

        // Must prevent year from going negative, since it screws up the non-floored modulo arithmetic.
        if (year < this.startingYear) {
            year = this.startingYear;
        }

        year = (year - this.startingYear) - Math.floor(this.cityTime / 48);
        this.cityTime += year * 48;

        this.updateDate();

    };


    p.currentYear = function currentYear() {

        return Math.floor(this.cityTime / 48) + this.startingYear;

    };


    p.setEnableDisasters = function setEnableDisasters(value) {

        this.enableDisasters = value;
        this.mustUpdateOptions = true;

    };


    p.setAutoBudget = function setAutoBudget(value) {

        this.autoBudget = value;
        this.mustUpdateOptions = true;

    };


    p.setAutoBulldoze = function setAutoBulldoze(value) {

        this.autoBulldoze = value;
        this.mustUpdateOptions = true;

    };


    p.setAutoGoto = function setAutoGoto(value) {

        this.autoGoto = value;
        this.mustUpdateOptions = true;

    };


    p.setEnableSound = function setEnableSound(value) {

        this.enableSound = value;
        this.mustUpdateOptions = true;

    };


    p.setDoAnimation = function setDoAnimation(value) {

        this.doAnimation = value;
        this.mustUpdateOptions = true;

    };


    p.setDoMessages = function setDoMessages(value) {

        this.doMessages = value;
        this.mustUpdateOptions = true;

    };


    p.setDoNotices = function setDoNotices(value) {

        this.doNotices = value;
        this.mustUpdateOptions = true;

    };


    p.getDemands = function getDemands() {

        var resDemand = this.resValve / RES_VALVE_RANGE;
        var comDemand = this.comValve / COM_VALVE_RANGE;
        var indDemand = this.indValve / IND_VALVE_RANGE;

        return [resDemand, comDemand, indDemand];

    };


    p.doZone = function doZone(x, y) {

        // Set power bit in map from powerGridMap.
        var powered = this.setZonePower(x, y);

        if (powered) {
            this.poweredZoneCount++;
        } else {
            this.unpoweredZoneCount++;
        }

        var tile = this.getTile(x, y) & TILEMASK_TILE;

        // Do special zones.
        if ((tile > PORTBASE) &&
            (tile < CHURCH1BASE)) {
            this.doSpecialZone(x, y, powered);
            return;
        }

        // Do residential zones.
        if (tile < HOSPITAL) {
            this.doResidential(x, y, powered);
            return;
        }

        // Do hospitals and churches.
        if ((tile < COMBASE) ||
            ((tile >= CHURCH1BASE) &&
             (tile <= CHURCH7LAST))) {
            this.doHospitalChurch(x, y);
            return;
        }

        // Do commercial zones.
        if (tile < INDBASE)  {
            this.doCommercial(x, y, powered);
            return;
        }

        // Do industrial zones.
        if (tile < CHURCH1BASE) {
            this.doIndustrial(x, y, powered);
            return;
        }

    };


    p.doHospitalChurch = function doHospitalChurch(x, y) {

        var tile = this.getTile(x, y) & TILEMASK_TILE;

        if (tile === HOSPITAL) {

            this.hospitalPop++;

            if (!(this.cityTime & 15)) {
                this.repairZone(x, y, HOSPITAL, 3);
            }

            if (this.needHospital === -1) { // Too many hospitals!
                if (!this.getRandom(20)) {
                    this.zonePlop(x, y, RESBASE); // Remove hospital.
                }
            }

        } else if ((tile === CHURCH0) ||
                   (tile === CHURCH1) ||
                   (tile === CHURCH2) ||
                   (tile === CHURCH3) ||
                   (tile === CHURCH4) ||
                   (tile === CHURCH5) ||
                   (tile === CHURCH6) ||
                   (tile === CHURCH7)) {

            this.churchPop++;

            var simulate = true;

            if (!(this.cityTime & 15)) {
                this.repairZone(x, y, tile, 3);
            }

            if (this.needChurch === -1) { // Too many churches!
                if (!this.getRandom(20)) {
                    this.zonePlop(x, y, RESBASE); // Remove church.
                    simulate = false;
                }
            }

            if (simulate) {

                var churchNumber = 0;

                switch (tile) {

                    case CHURCH0:
                        churchNumber = 0;
                        break;

                    case CHURCH1:
                        churchNumber = 1;
                        break;

                    case CHURCH2:
                        churchNumber = 2;
                        break;

                    case CHURCH3:
                        churchNumber = 3;
                        break;

                    case CHURCH4:
                        churchNumber = 4;
                        break;

                    case CHURCH5:
                        churchNumber = 5;
                        break;

                    case CHURCH6:
                        churchNumber = 6;
                        break;

                    case CHURCH7:
                        churchNumber = 7;
                        break;

                    default:
                        break;

                }

                this.doSimChurch(x, y, churchNumber);

            }

        }

    };


    p.setSmoke = function setSmoke(x, y, zonePower) {

        var aniThis = [  true, false,  true,  true, false, false,  true,  true ];
        var dx1     = [    -1,     0,     1,     0,     0,     0,     0,     1 ];
        var dy1     = [    -1,     0,    -1,    -1,     0,     0,    -1,    -1 ];
        var aniTabA = [     0,     0,    32,    40,     0,     0,    48,    56 ]; // TODO: name tile numbers.
        var aniTabB = [     0,     0,    36,    44,     0,     0,    52,    60 ]; // TODO: name tile numbers.
        var aniTabC = [ IND1, 0, IND2, IND4, 0, 0, IND6, IND8 ];
        var aniTabD = [ IND1, 0, IND3, IND5, 0, 0, IND7, IND9 ];

        var tile = this.getTile(x, y) & TILEMASK_TILE;

        if (tile < IZB) {
            return;
        }

        var z = (tile - IZB) >> 3; /// @todo Why div 8? Industry is 9 tiles long!!
        z = z & 7;

        if (aniThis[z]) {

            var xx = x + dx1[z];
            var yy = y + dy1[z];

            if (this.testBounds(xx, yy)) {

                var t = this.getTile(xx, yy) & TILEMASK_TILE;

                if (zonePower) {

                    /// @todo Why do we assign the same map position twice?
                    /// @todo Add #SMOKEBASE into aniTabA and aniTabB tables?
                    if (t === aniTabC[z]) {

    /*
                        this.setTile(
                            xx,
                            yy,
                            (SMOKEBASE + aniTabA[z]) | TILEMASK_ANIMATED | TILEMASK_CONDUCTIVE | TILEMASK_BURNABLE);
    */

                        this.setTile(
                            xx,
                            yy,
                            (SMOKEBASE + aniTabB[z]) | TILEMASK_ANIMATED | TILEMASK_CONDUCTIVE | TILEMASK_BURNABLE);

                    }

                } else {

                    /// @todo Why do we assign the same map position twice?
                    if (t > aniTabC[z]) {

    /*
                        this.setTile(
                            xx,
                            yy,
                            aniTabC[z] | TILEMASK_CONDUCTIVE | TILEMASK_BURNABLE);
    */

                        this.setTile(
                            xx,
                            yy,
                            aniTabD[z] | TILEMASK_CONDUCTIVE | TILEMASK_BURNABLE);

                    }

                }

            }

        }

    };


    p.makeHospital = function makeHospital(x, y) {

        if (this.needHospital > 0) {
            this.zonePlop(x, y, HOSPITAL - 4);
            this.needHospital = 0;
            return;
        }

        if (this.needChurch <= 0) {
            return;
        }

        var churchType = this.getRandom(7); // 0 to 7 inclusive
        var tile;
        if (churchType === 0) {
            tile = CHURCH0;
        } else {
            tile = CHURCH1 + ((churchType - 1) * 9);
        }

        this.zonePlop(x, y, tile - 4);
        this.needChurch = 0;

    };


    p.getLandPollutionValue = function getLandPollutionValue(x, y) {

        var landValue =
            landValueMap.worldGet(x, y) -
            pollutionDensityMap.worldGet(x, y);

        if (landValue < 30) {
            return 0;
        }

        if (landValue < 80) {
            return 1;
        }

        if (landValue < 150) {
            return 2;
        }

        return 3;

    };


    p.incRateOfGrowth = function incRateOfGrowth(x, y, amount) {

        var value =
            this.rateOfGrowthMap.worldGet(x, y);

        value = 
            this.clamp(value + amount * 4, -200, 200);

        this.rateOfGrowthMap.worldSet(x, y, value);

    };


    p.zonePlop = function zonePlop(x, y, base) {

        var zx = [ -1, 0, 1,-1, 0, 1,-1, 0, 1 ];
        var zy = [ -1,-1,-1, 0, 0, 0, 1, 1, 1 ];
        var xx;
        var yy;
        var z;

        for (z = 0;
             z < 9;
             z++) { // Check for fire.

            xx = x + zx[z];
            yy = y + zy[z];

            if (this.testBounds(xx, yy)) {

                var tile = this.getTile(xx, yy) & TILEMASK_TILE;

                if ((tile >= FLOOD) &&
                    (tile < ROADBASE)) {
                    return false;
                }

            }

        }

        for (z = 0; z < 9; z++) {

            xx = x + zx[z];
            yy = y + zy[z];

            if (this.testBounds(xx, yy)) {

                this.setTile(
                    xx,
                    yy,
                    base + TILE_BNCNBIT);

            }

            base++;

        }

        this.setZonePower(x, y);

        this.setTile(
            x,
            y,
            this.getTile(x, y) | TILEMASK_ZONE | TILEMASK_BULLDOZABLE);

        return true;
    };


    p.doFreePop = function doFreePop(x, y) {

        var count = 0;

        for (var xx = x - 1;
             xx <= x + 1;
             xx++) {

            for (var yy = y - 1;
                 yy <= y + 1;
                 yy++) {

                if ((xx >= 0) &&
                    (xx < WORLD_W) &&
                    (yy >= 0) &&
                    (yy < WORLD_H)) {

                    var tile = this.getTile(xx, yy) & TILEMASK_TILE;

                    if ((tile >= LHTHR) &&
                        (tile <= HHTHR)) {
                        count++;
                    }

                }

            }

        }

        return count;
    };


    p.setZonePower = function setZonePower(x, y) {

        var mapValue = this.getTile(x, y);
        var tile = mapValue & TILEMASK_TILE;

        if ((tile === NUCLEAR) ||
            (tile === POWERPLANT)) {

            this.setTile(
                x,
                y,
                mapValue | TILEMASK_POWER);

            return true;

        }

        if (this.powerGridMap.worldGet(x, y)) {

            this.setTile(
                x,
                y,
                mapValue | TILEMASK_POWER);

            return true;

        } else {

            this.setTile(
                x,
                y,
                mapValue & (~TILEMASK_POWER));

            return false;

        }

    };


    p.buildHouse = function buildHouse(x, y, value) {

        var zex = [ 0,-1, 0, 1,-1, 1,-1, 0, 1 ];
        var zey = [ 0,-1,-1,-1, 0, 0, 1, 1, 1 ];
        var bestLocation = 0;
        var highScore = 0;
        var xx;
        var yy;
        var z;

        for (z = 1;
             z < 9;
             z++) {

            xx = x + zex[z];
            yy = y + zey[z];

            if (this.testBounds(xx, yy)) {

                var score = this.evalLot(xx, yy);

                /// @bug score is never 0 !!
                if (score !== 0) {

                    if (score > highScore) {
                        highScore = score;
                        bestLocation = z;
                    }

                    /// @todo Move the code below to a better place.
                    ///       If we just updated hscore above, we could
                    //        trigger this code too.
                    if (score === highScore &&
                        !(this.getRandom16() & 7)) {
                        bestLocation = z;
                    }

                }

            }

        }

        if (bestLocation) {

            xx = x + zex[bestLocation];
            yy = y + zey[bestLocation];

            if (this.testBounds(xx, yy)) {

                /// @todo Is HOUSE the proper constant here?
                this.setTile(
                    xx,
                    yy,
                    (HOUSE + this.getRandom(2) + (value * 3)) | TILEMASK_BULLDOZABLE | TILEMASK_BURNABLE | TILEMASK_CONDUCTIVE);

            }

        }
    };


    p.evalLot = function evalLot(x, y) {

        var dx = [  0, 1, 0,-1 ];
        var dy = [ -1, 0, 1, 0 ];

        // Test for clear lot.
        z = this.getTile(x, y) & TILEMASK_TILE;

        if (z &&
            ((z < RESBASE) ||
             (z > RESBASE + 8))) {
            return -1;
        }

        var score = 1;

        for (z = 0; z < 4; z++) {

            var xx = x + dx[z];
            var yy = y + dy[z];

            if (this.testBounds(xx, yy)) {

                var tile = this.getTile(xx, yy) & TILEMASK_TILE;

                if ((tile !== DIRT) &&
                    (tile <= LASTROAD)) {
                    score++; // Look for road.
                }

            }

        }

        return score;

    };


    p.doResidential = function doResidential(x, y, powered) {

        var tile = this.getTile(x, y) & TILEMASK_TILE;
        var val;

        this.resZonePop++;

        var population;
        if (tile === FREEZ) {
            population = this.doFreePop(x, y);
        } else {
            population = this.getResZonePop(tile);
        }

        this.resPop += population;

        var trafficGood;
        if (population > this.getRandom(35)) {

            // Try driving from residential to commercial.
            trafficGood =
                this.makeTraffic(x, y, ZONE_COMMERCIAL);

        } else {

            trafficGood = 1;

        }

        if (trafficGood === -1) {

            var value =
                this.getLandPollutionValue(x, y);

            this.doResOut(x, y, population, value);

            return;

        }

        if ((tile === FREEZ) ||
            !(this.getRandom16() & 7)) {

            var locationValve =
                this.evalRes(x, y, trafficGood);

            var score =
                this.resValve + locationValve;

            if (!powered) {
                score = -500;
            }

            // TODO: Make sure getRandom16Signed is returning the right range.
            if ((score > -350) &&
                ((score - 26380) > (this.getRandom16Signed()))) {

                if (!population &&
                    !(this.getRandom16() & 3)) {

                    this.makeHospital(x, y);

                    return;

                }

                value =
                    this.getLandPollutionValue(x, y);

                this.doResIn(x, y, population, value);

                return;

            }

            if (score < 350 &&
                (((score + 26380)) < (this.getRandom16Signed()))) {

                value =
                    this.getLandPollutionValue(x, y);

                this.doResOut(x, y, population, value);

            }

        }

    };


    p.doResIn = function doResIn(x, y, pop, value) {

        var pollution =
            this.pollutionDensityMap.worldGet(x, y);

        if (pollution > 128) {
            return;
        }

        var tile = this.getTile(x, y) & TILEMASK_TILE;

        if (tile === FREEZ) {

            if (pop < 8) {

                this.buildHouse(x, y, value);
                this.incRateOfGrowth(x. y, 1);

                return;

            }

            if (this.populationDensityMap.worldGet(x, y) > 64) {

                this.resPlop(x, y, 0, value);
                this.incRateOfGrowth(x, y, 8);

                return;

            }

            return;

        }

        if (pop < 40) {

            this.resPlop(x, y, (pop / 8) - 1, value);
            this.incRateOfGrowth(x, y, 8);

        }

    };


    p.doResOut = function doResOut(x, y, pop, value) {

        if (!pop) {
            return;
        }

        if (pop > 16) {

            this.resPlop(x, y, (pop - 24) / 8, value);
            this.incRateOfGrowth(x, y, -8);

            return;

        }

        var xx;
        var yy;

        if (pop === 16) {

            this.incRateOfGrowth(x, y, -8);

            this.setTile(
                x,
                y,
                FREEZ | TILEMASK_BULLDOZABLE | TILEMASK_BURNABLE | TILEMASK_CONDUCTIVE | TILEMASK_ZONE);

            for (xx = x - 1;
                 xx <= x + 1;
                 xx++) {

                for (yy = y - 1;
                     yy <= y + 1;
                     yy++) {

                    if (this.testBounds(xx, yy)) {

                        if ((this.getTile(xx, yy) & TILEMASK_TILE) !== FREEZ) {

                            this.setTile(
                                xx,
                                yy,
                                (LHTHR + value + this.getRandom(2)) | TILEMASK_BULLDOZABLE | TILEMASK_BURNABLE | TILEMASK_CONDUCTIVE);

                        }

                    }

                }

            }

        }

        if (pop < 16) {

            var brdr = [ 0, 3, 6, 1, 4, 7, 2, 5, 8 ];

            this.incRateOfGrowth(x, y, -1);

            for (xx = x - 1, z = 0;
                 xx <= x + 1;
                 xx++) {

                for (yy = y - 1;
                     yy <= y + 1;
                     yy++, z++) {

                    if (this.testBounds(xx, yy)) {

                        var tile = this.getTile(xx, yy) & TILEMASK_TILE;

                        if ((tile >= LHTHR) &&
                            (tile <= HHTHR)) {

                            this.setTile(
                                xx,
                                yy,
                                (brdr[z] + FREEZ - 4) | TILEMASK_BULLDOZABLE | TILEMASK_BURNABLE | TILEMASK_CONDUCTIVE);

                            return;

                        }
                    }

                }

            }

        }

    };


    p.getResZonePop = function getResZonePop(tile) {

        var czden = Math.floor((tile - RZB) / 9) % 4;

        return czden * 8 + 16;

    };


    p.resPlop = function resPlop(x, y, den, value) {

        var base =
            (((value * 4) + den) * 9) + RZB - 4;

        this.zonePlop(x, y, base);

    };


    p.evalRes = function evalRes(x, y, traffic) {

        if (traffic < 0) {
            return -3000;
        }

        var value =
            this.landValueMap.worldGet(x, y) -
            this.pollutionDensityMap.worldGet(x, y);

        if (value < 0) {
            value = 0; // Cap at 0.
        } else {
            value = Math.min(value * 32, 6000); // Cap at 6000.
        }

        value -= 3000;

        return value;

    };


    p.doCommercial = function doCommercial(x, y, powered) {

        var tile = this.getTile(x, y) & TILEMASK_TILE;

        this.comZonePop++;

        var population = this.getComZonePop(tile);

        this.comPop += population;

        var trafficGood;
        if (population > this.getRandom(5)) {

            // Try driving from commercial to industrial.
            trafficGood =
                this.makeTraffic(x, y, ZONE_INDUSTRIAL);

        } else {

            trafficGood = 1;

        }

        if (trafficGood === -1) {

            var value = this.getLandPollutionValue(x, y);

            this.doComOut(x, y, population, value);

            return;

        }

        if (!(this.getRandom16() & 7)) {

            var locationValue = this.evalCom(x, y, trafficGood);
            var score = this.comValve + locationValue;

            if (!powered) {
                score = -500;
            }

            if (trafficGood &&
                (score > -350) &&
                (((score - 26380)) > (this.getRandom16Signed()))) {

                value = this.getLandPollutionValue(x, y);

                this.doComIn(x, y, population, value);

            } else if ((score < 350) &&
                       (((score + 26380)) < (this.getRandom16Signed()))) {

                value = this.getLandPollutionValue(x, y);

                this.doComOut(x, y, population, value);

            }

        }

    };


    p.doComIn = function doComIn(x, y, pop, value) {

        var z = this.landValueMap.worldGet(x, y) >> 5;

        if ((pop <= z) &&
            (pop < 5)) {

            this.comPlop(x, y, pop, value);
            this.incRateOfGrowth(x, y, 8);

        }

    };


    p.doComOut = function doComOut(x, y, pop, value) {

        if (pop > 1) {

            this.comPlop(x, y, pop - 2, value);
            this.incRateOfGrowth(x, y, -8);

        } else if (pop === 1) {

            this.zonePlop(x, y, COMBASE);
            this.incRateOfGrowth(x, y, -8);

        }

    };


    p.getComZonePop = function getComZonePop(tile) {

        if (tile === COMCLR) {
            return 0;
        }

        var czden =
            (Math.floor((tile - CZB) / 9) % 5) + 1;

        return czden;

    };


    p.comPlop = function comPlop(x, y, den, value) {

        var base =
            (((value * 5) + den) * 9) + CZB - 4;

        this.zonePlop(x, y, base);

    };


    p.evalCom = function evalCom(x, y, traffic) {

        if (traffic < 0) {
            return -3000;
        }

        var value =
            this.comRateMap.worldGet(x, y);

        return value;
    };


    p.doIndustrial = function doIndustrial(x, y, powered) {

        var tile = this.getTile(x, y) & TILEMASK_TILE;

        this.indZonePop++;
        this.setSmoke(x, y, powered);

        var population = this.getIndZonePop(tile);

        this.indPop += population;

        var trafficGood;
        if (population > this.getRandom(5)) {

            // Try driving from industrial to residential.
            trafficGood = this.makeTraffic(x, y, ZONE_RESIDENTIAL);

        } else {

            trafficGood = 1;

        }

        if (trafficGood === -1) {

            this.doIndOut(x, y, population, this.getRandom16() & 1);
            return;

        }

        if (this.getRandom16() & 7) {
            return;
        }

        var score = this.indValve + this.evalInd(trafficGood);

        if (!powered) {
            score = -500;
        }

        if (score > -350 &&
            (((score - 26380)) > (this.getRandom16Signed()))) {

            this.doIndIn(x, y, population, this.getRandom16() & 1);

        } else if (score < 350 &&
                   (((score + 26380)) < (this.getRandom16Signed()))) {

            this.doIndOut(x, y, population, this.getRandom16() & 1);

        }

    };


    p.doIndIn = function doIndIn(x, y, pop, value) {

        if (pop < 4) {

            this.indPlop(x, y, pop, value);
            this.incRateOfGrowth(x, y, 8);

        }

    };


    p.doIndOut = function doIndOut(x, y, pop, value) {

        if (pop > 1) {

            this.indPlop(x, y, pop - 2, value);
            this.incRateOfGrowth(x, y, -8);

        } else if (pop === 1) {

            this.zonePlop(x, y, INDBASE); // Empty industrial zone.
            this.incRateOfGrowth(x, y, -8);

        }

    };


    p.getIndZonePop = function getIndZonePop(tile) {

        if (tile === INDCLR) {
            return 0;
        }

        var czden = (Math.floor((tile - IZB) / 9) % 4) + 1;

        return czden;

    };


    p.indPlop = function indPlop(x, y, den, value) {

        var base = (((value * 4) + den) * 9) + IND1;

        this.zonePlop(x, y, base);

    };


    p.evalInd = function evalInd(traffic) {

        if (traffic < 0) {
            return -1000;
        }

        return 0;

    };


    ////////////////////////////////////////////////////////////////////////
    // Callbacks


    p.callback = function callback(name, params) {
        if (!this.callbackHook) {
            return;
        }

        this.callbackHook(this.callbackData, name, params);
    };


    p.doLoadCity = function doLoadCity() {
        this.callback('doLoadCity', []);
    };


    p.doNotLoadCity = function doNotLoadCity(msg) {
        this.callback('doNotLoadCity', []);
    };


    p.doSaveCity = function doSaveCity() {
        this.callback('doSaveCity', []);
    };


    p.doNotSaveCity = function doNotSaveCity(msg) {
        this.callback('doNotSaveCity', []);
    };


    p.doLoadScenario = function doLoadScenario() {
        this.callback('doLoadScenario', []);
    };


    p.doNotLoadScenario = function doNotLoadScenario(msg) {
        this.callback('doNotLoadScenario', []);
    };


    p.doGenerateMap = function doGenerateMap(seed) {
        this.callback('doGenerateMap', [seed]);
    };


    p.doTool = function doTool(name, x, y) {
        this.callback('doTool', [name, x, y]);
    };


    p.doPlayNewCity = function doPlayNewCity() {
        this.callback('doPlayNewCity', []);
    };


    p.doReallyStartGame = function doReallyStartGame() {
        this.callback('doReallyStartGame', []);
    };


    p.doStartLoad = function doStartLoad() {
        this.callback('doStartLoad', []);
    };


    p.doStartScenario = function doStartScenario(scenario) {
        this.callback( 'doStartScenario', [scenario]);
    };


    p.doShowBudgetWindowAndWait = function doShowBudgetWindowAndWait() {
        this.callback('doShowBudgetAndWait', []);
    };


    p.doResetMapState = function doResetMapState() {
        this.callback('doResetMapState', []);
    };


    p.doResetEditorState = function doResetEditorState() {
        this.callback('doResetEditorState', []);
    };


    p.doSimRobots = function doSimRobots() {
        this.callback('doSimRobots', []);
    };


    p.doSimChurch = function doSimChurch(x, y, churchNumber) {
        this.callback('doSimChurch', [x, y, churchNumber]);
    };


    p.doSendMessage = function doSendMessage(messageID, x, y, picture, important) {
        this.callback('doSendMessage', [messageID, x, y, !!picture, !!important]);
    };


    p.doMakeSound = function doMakeSound(channel, sound, x, y) {
        this.callback('doMakeSound', [channel, sound, x, y]);
    };


    p.doAutoGoto = function doAutoGoto(x, y, message) {
        this.callback('doAutoGoto', [x, y, message]);
    };


    p.doLoseScenario = function doLoseScenario() {
        this.callback('doLoseScenario', []);
    };


    p.doWinScenario = function doWinScenario() {
        this.callback('doWinScenario', []);
    };


    p.doNewGame = function doNewGame() {
        this.callback('doNewGame', []);
    };


    p.doEarthquake = function doEarthquake(strength) {
        this.callback('doEarthquake', [strength]);
    };


    p.doShowZoneStatus = function doShowZoneStatus(tileCategory, s0, s1, s2, s3, s4, x, y) {
        this.callback('doShowZoneStatus', [tileCategory, s0, s1, s2, s3, s4, x, y]);
    };


    p.doUpdateFunds = function doUpdateFunds() {
        this.callback('doUpdate', ['funds']);
    };


    p.doUpdateDate = function doUpdateDate() {
        this.callback('doUpdate', ['date']);
    };


    p.doUpdateDemand = function doUpdateDemand() {
        this.callback('doUpdate', ['demand']);
    };


    p.doUpdateOptions = function doUpdateOptions() {
        this.callback('doUpdate', ['options']);
    };


    p.doUpdateBudget = function doUpdateBudget() {
        this.callback('doUpdate', ['budget']);
    };


    p.doUpdatePaused = function doUpdatePaused() {
        this.callback('doUpdate', ['paused']);
    };


    p.doUpdateSpeed = function doUpdateSpeed() {
        this.callback('doUpdate', ['speed']);
    };


    p.doUpdatePasses = function doUpdatePasses() {
        this.callback('doUpdate', ['passes']);
    };


    p.doUpdateTaxRate = function doUpdateTaxRate() {
        this.callback('doUpdate', ['taxRate']);
    };


    p.doUpdateEvaluation = function doUpdateEvaluation() {
        this.callback('doUpdate', ['evaluation']);
    };


    p.doUpdateHistory = function doUpdateHistory() {
        this.callback('doUpdate', ['history']);
    };


    p.doUpdateMap = function doUpdateMap() {
        this.callback('doUpdate', ['map']);
    };


    p.doUpdateGameLevel = function doUpdateGameLevel() {
        this.callback('doUpdate', ['gameLevel']);
    };


    p.doUpdateCityName = function doUpdateCityName() {
        this.callback('doUpdate', ['cityName']);
    };


////////////////////////////////////////////////////////////////////////


})();


var gMicropolis;


function TestMicropolis()
{

    console.log("TestMicropolis");

    gMicropolis = 
        new Micropolis();
    console.log("gMicropolis", gMicropolis);

    var callbackData = {
        'micropolis': gMicropolis
    };

    gMicropolis.callbackData = callbackData;

    gMicropolis.callbackHook = function(data, name, params) {
        console.log("Micropolis callback:", name, params);
    };

    gMicropolis.loadCityFromURL(
        'cities/haight.cty',
        function() {
            console.log("Loaded city from", gMicropolis.cityFileName);
            gMicropolis.setSpeed(3);
            gMicropolis.resume();
        },
        function() {
            console.log("Error loading city.");
        });

}


////////////////////////////////////////////////////////////////////////
