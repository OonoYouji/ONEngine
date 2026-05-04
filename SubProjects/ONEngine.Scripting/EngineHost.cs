using System;
using System.Runtime.InteropServices;

namespace ONEngine.Scripting
{
    public static class EngineHost
    {
        [UnmanagedCallersOnly]
        public static void Initialize(IntPtr logHandler)
        {
            Debug.SetLogHandler(logHandler);
            Debug.Log("[C#] EngineHost initialized with custom log handler.");
        }

        [UnmanagedCallersOnly]
        public static void Update()
        {
            // Update logic here
        }

        [UnmanagedCallersOnly]
        public static void Shutdown()
        {
            Console.WriteLine("[C#] EngineHost shutdown.");
        }
    }
}
