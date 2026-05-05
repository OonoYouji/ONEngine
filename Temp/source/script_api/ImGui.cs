using System;
using System.Runtime.InteropServices;

namespace Engine.Core
{
    public static class ImGui
    {
        private const string DLL_NAME = "Temp.exe";

        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool igBegin(string name, IntPtr p_open);

        public static bool Begin(string name) => igBegin(name, IntPtr.Zero);

        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)]
        public static extern void igEnd();

        public static void End() => igEnd();

        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)]
        public static extern void igText(string fmt);

        public static void Text(string text) => igText(text);

        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool igButton(string label);

        public static bool Button(string label) => igButton(label);

        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)]
        public static extern void igCheckbox(string label, ref bool v);

        public static void Checkbox(string label, ref bool v) => igCheckbox(label, ref v);

        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)]
        public static extern void igSliderFloat(string label, ref float v, float v_min, float v_max);

        public static void SliderFloat(string label, ref float v, float v_min, float v_max) 
            => igSliderFloat(label, ref v, v_min, v_max);
    }
}
