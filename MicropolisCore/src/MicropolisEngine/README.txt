Micropolis Core Engine
By Will Wright and Don Hopkins

[TODO: Write documentation here.]

Here are some notes about hooking up a user interface. 

In general, the philosophy is to tell the scriptinging engine that something has changed by sending a callback message out, without any parameters. 
(The code may not currently be doing that, but that's how it should work.)
Then the script handler can pull the parameters out of the simulator itself, instead of being passed the parameters in the callback. 
This is so that the script can delay updating until the next time it paints the screen, consolidating a bunch of changes into one refresh. 
That avoides the overhead of converting and formatting values that don't end up getting used. 
Also, we want to defer all formatting to the scripting language, instead of formatting in the core simulator, so the script can internationalize the numbers and text. 
Likewise, the simulator should not have any strings hard wired into it -- all those should be looked up by the scripting language, in translation files. 
To conserve power on the OLPC, the script might want to schedule an update at the next round timer tick, so all the updates happen at once. 

Parts of the simulator that update the user interface:

Budget
  SetBudget
    flowStr
    previousStr
    currentStr
    collectedStr
    tax
  SetBudgetValues
	roadGot
	roadWant
	policeGot
	policeWant
	fireGot
	fireWant
  ShowBudgetWindowAndStartWaiting

Evaluation
  SetEvaluation
    changed: deltaCityScore
    score: CityScore
    ps0: ProblemOrder[0] ? probStr[ProblemOrder[0]] : ""
    ps1: ProblemOrder[1] ? probStr[ProblemOrder[1]] : ""
    ps2: ProblemOrder[2] ? probStr[ProblemOrder[2]] : ""
    ps3: ProblemOrder[3] ? probStr[ProblemOrder[3]] : ""
    pv0: ProblemOrder[0] ? ProblemVotes[ProblemOrder[0]] : ""
    pv1: ProblemOrder[1] ? ProblemVotes[ProblemOrder[1]] : ""
    pv2: ProblemOrder[2] ? ProblemVotes[ProblemOrder[2]] : ""
    pv3: ProblemOrder[3] ? ProblemVotes[ProblemOrder[3]] : ""
    pop: CityPop
    delta: deltaCityPop
    assessed_dollars: CityAssValue
    cityclass: cityClassStr[CityClass]
    citylevel: cityLevelStr[GameLevel]
    goodyes: XXX%
    goodno: XXX%
    title: City Evaluation [YEAR]
  probStr
	Crime
	Pollution
	Housing Costs
	Taxes
	Traffic
	Unemployment
	Fires
  cityClassStr
    Village
    Town
    City
    Capital
    Metropolis
    Megalopolis
  cityLevelStr
    Easy
    Medium
    Hard


====

User interface surfaces to implement:

New city dialog (full screen or shared with main city view)
  View of selected city (or use main city view if still on screen)
  City Name Editor
  Difficulty Level Selector
    Easy
    Medium
    Hard
  Generate New City Button
  Load City Button
  8 Scenarios Buttons
    Dullsville 1900
    San Francisco 1906
    Hamburg 1944
    Bern 1965
    Tokyo 1957
    Rio de Janeiro 2047
    Boston 2010
    Detroit 1972
  About button
  Quit button

Control Menus (visible while game playing, or disabled as appropriate)
  Micropolis
    About
    Save City
    Save city as...
    New city...
    Quit
  Options
    Auto Budget
    Auto Bulldoze
    Disasters
    Sound
    Animation
    Messages
    Notices
  Disasters
    Monster
    Fire
    Flood
    Meltdown
    Air Crash
    Tornado
    Earthquake
  Speed
    Extra Slow
    Slow
    Medium
    Fast
    Extra Fast
  Window
    Budget
    Evaluation
    Graph
	New Editor
	New Map

Date Display
  Click to pause
  Drag to change speed

Funds Display
  Click to open budget window and pause

Tax Rate Display
  Click to open budget window and pause

Tax Rate Slider
  Drag to set tax rate without pausing

Demand Gauge (called "valves" in code)
  Shows Residential, Commercial and Industrial demand
  Click to toggle evaluation window or graph window

Message Field
  Displays the current message.
  Click to go to the location of that message. 

Scrolling Text Journal
  Scrolling text window of messages. 
  Click messages to do something useful like scroll to location of event. 
    Messages can reconfigure the user interface to help deal with the problem described. 
  Click dates to rewind to save checkpoints

Budget Window
  Road Fund Requested Label
  Road Fund Percentage Slider
  Fire Fund Requested Label
  Fire Fund Percentage Slider
  Police Fund Requested Label
  Police Fund Percentage Slider
  Tax Rate Label
  Tax Rate Slider
  Taxes Collected Label
  Cash Flow Label
  Previous Funds Label
  Current Funds Label
  Continue Button
  Reset Button
  Cancel Button
  Toggle Auto Budget Button
  
Evaluation Window
  Is The Mayor Doing a Good Job?
    Yes: XX%
    No: XX%
  What are the Worst Problems?
    Problem 1: XXX%
    Problem 2: XXX%
    Problem 3: XXX%
    Problem 4: XXX%
  Statistics
    Population: XXX
	Net Migration: XXX (last year)
	Assessed Value: XXX
	Category: XXX
	Game Level: XXX
  Overall City Score (0 - 1000)
    Current Score: XXX
    Annual Change: XXX
  Dismiss

Graph window
  Long term graph
  Short term graph
  Each graph has 8 values it can overlay
  Legend showing 8 labels for 8 colors
    Labels
      Residential: Light Green = (0.50, 0.50, 1.00)
      Commercial: Dark Blue = (0.00, 0.00, 0.50)
      Industrial: Yellow = (1.00, 1.00, 0.00)
      Cash Flow: Dark Green = (0.00, 0.50, 0.00)
      Crime: Red = (1.00, 0.00, 0.00)
      Pollution: Olive = (0.33, 0.42, 0.18)
    Click to toggle each graph
    Point to highlight individual graphs, and show scale and value
    Advanced scaling, scrolling and data mining features
  Dismiss

Edit Window
  Selected Tool
  Cost of Selected Tool
  Tool Palette
  Options
    Auto Goto
    Hide Panel
    Show Overlay (for layers and chalk)
	Dynamic Filter (for dynamic zone finder: "frob-o-matic")

Map Window
  Shows draggable location(s) of edit window(s)
  Overlays
    Residential
    Commercial
    Industrial
    Transportation
    Population Density
    Rate of Growth
    Land Value
    Crime Rate
    Pollution Density
    Traffic Density
    Power Grid
    Fire Coverage
    Police Coverage
    Dynamic ("frob-o-matic")
  Dismiss

Notice Window
  Shows messages and pictures.
  Can display a live animated view of a location on the map, zoomed into wherever the message is about. 
  Can also be used to ask questions, like "Are you sure you want to XXX?" with yes/no/etc buttons.
  Dismiss

Help Window
  A xulrunner web browser to read HTML docs.
