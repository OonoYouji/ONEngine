using System;
using System.Runtime.InteropServices;

namespace ONEngine.Scripting
{
    public static class Debug
    {
        private delegate void LogDelegate(string msg);
        private static LogDelegate _logDelegate;

        public static void SetLogHandler(IntPtr ptr)
        {
            if (ptr != IntPtr.Zero)
            {
                _logDelegate = Marshal.GetDelegateForFunctionPointer<LogDelegate>(ptr);
            }
        }

        public static void Log(string msg)
        {
            if (_logDelegate != null)
            {
                _logDelegate(msg);
            }
            else
            {
                Console.WriteLine(msg);
            }
        }
    }
}
