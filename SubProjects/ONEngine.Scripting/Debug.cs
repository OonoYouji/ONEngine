using System;
using System.Runtime.InteropServices;
using ONEngine.Scripting.Math;

namespace ONEngine.Scripting
{
    public static class Debug
    {
        private static IntPtr _logHandler;

        public static void SetLogHandler(IntPtr handler)
        {
            _logHandler = handler;
        }

        [DllImport("ONEngine.exe")]
        private static extern void Debug_DrawLine(float sx, float sy, float sz, float ex, float ey, float ez, float r, float g, float b, float a);

        public static void DrawLine(Vector3 start, Vector3 end, Vector4 color)
        {
            Debug_DrawLine(start.x, start.y, start.z, end.x, end.y, end.z, color.x, color.y, color.z, color.w);
        }

        public static void DrawLine(Vector3 start, Vector3 end)
        {
            DrawLine(start, end, new Vector4(1, 1, 1, 1));
        }

        public static void Log(string message)
        {
            if (_logHandler != IntPtr.Zero)
            {
                // TODO: Call native log delegate if necessary, or just use Console for now
                Console.WriteLine(message);
            }
            else
            {
                Console.WriteLine(message);
            }
        }
    }
}
