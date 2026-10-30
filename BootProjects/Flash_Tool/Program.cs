using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;

namespace FlashTool
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            FlashSettings.InitLogging();            // FlashTool.ini -> log.txt and the trace switches
            Application.Run( new FTMain());
        }
    }
}
