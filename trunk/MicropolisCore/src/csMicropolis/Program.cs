//
// Program.cs Main file for running Micropolis with the Winforms frontend
//

using System;
using System.Windows.Forms;

namespace MicropolisClient
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            try
            {
                var engine = new MicropolisWinFormsEngine();

                engine.cityTax = 10;
                engine.setPasses(200);
                // setTile = engine.setTile;

                // add robot

                var x = 0;
                var y = 0;

                var w = 800;
                var h = 600;
            
                Application.EnableVisualStyles();
                Application.SetCompatibleTextRenderingDefault(false);
                var win1 = new MicropolisPannedWindow(engine);
                win1.Width = w;
                win1.Height = h;
                win1.Top = y;
                win1.Left = x;
                Application.Run(win1);
            }
            catch (Exception e)
            {
                MessageBox.Show(e.ToString());
            }
        }
    }
}
