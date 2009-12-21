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
                var engine = new MicropolisCSEngine {cityTax = 10};

                engine.setPasses(200);
            
                Application.EnableVisualStyles();
                Application.SetCompatibleTextRenderingDefault(false);

                var win1 = new MicropolisPanedWindow(engine) {Top = 0, Left = 0, Width = 800, Height = 600};

                Application.Run(win1);
            }
            catch (Exception e)
            {
                MessageBox.Show(e.ToString());
            }
        }
    }
}
