using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace printMonitor
{
    internal class Program
    {
        [DllImport("C:\\Users\\akshi\\Downloads\\project_interceptor_lib\\Debug\\project_interceptor_lib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void MonitorPrintQueue();
        static void Main(string[] args)
        {
            Console.WriteLine("Enumerating print jobs inside the system .....\n");
            MonitorPrintQueue();
            
        }
    }
}
