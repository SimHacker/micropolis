using System;
using System.Windows.Forms;

namespace MicropolisClient
{
    class MicropolisPanedWindow : Form
    {
        private MicropolisCSEngine _engine;
        private EditableMicropolisDrawingArea _editMapView;
        private NavigationMicropolisDrawingArea _navigationMapView;
        private MicropolisGaugeView _gaugeView;

        public MicropolisPanedWindow(MicropolisCSEngine engine)
        {
            Text = "Open Source Micropolis on C#";
            _engine = engine;

            _editMapView = new EditableMicropolisDrawingArea(engine);

            _navigationMapView = new NavigationMicropolisDrawingArea(engine);
//            _navigationMapView.set_size_request(micropolisengine.WORLD_W, micropolisengine.WORLD_H);

            _gaugeView = new MicropolisGaugeView(engine);

            // Make the vbox for the gauge and navigation map views

            // Make the notebooks

            startGame(); 
        }

        private void startGame()
        {
            generateCity();

            _engine.setSpeed(2);
            _engine.setCityTax(9);
            _engine.enableDisasters = false;
        }

        private void generateCity()
        {
            _engine.generateNewMetaCity();
        }
    }
}