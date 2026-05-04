using System;
using System.Runtime.InteropServices;

namespace ONEngine.Scripting
{
    public static class EngineHost
    {
        [UnmanagedCallersOnly]
        public static void Initialize()
        {
            Console.WriteLine("[C#] EngineHost initialized.");
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
