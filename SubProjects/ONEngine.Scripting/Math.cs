using System.Runtime.InteropServices;

namespace ONEngine.Scripting.Math
{
    [StructLayout(LayoutKind.Sequential)]
    public struct Vector2
    {
        public float x, y;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct Vector3
    {
        public float x, y, z;
        public Vector3(float x, float y, float z) { this.x = x; this.y = y; this.z = z; }
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct Vector4
    {
        public float x, y, z, w;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct Matrix4x4
    {
        public float m00, m01, m02, m03;
        public float m10, m11, m12, m13;
        public float m20, m21, m22, m23;
        public float m30, m31, m32, m33;
    }
}
