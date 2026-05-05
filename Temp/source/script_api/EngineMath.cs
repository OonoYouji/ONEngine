using System;
using System.Runtime.InteropServices;
using System.Numerics;

namespace Engine.Core
{
    [StructLayout(LayoutKind.Sequential)]
    public struct Vector2
    {
        public float x, y;
        public Vector2(float x, float y) { this.x = x; this.y = y; }
        
        public static implicit operator Vector2(Vector3 v) => new Vector2(v.x, v.y);
        public static implicit operator System.Numerics.Vector2(Vector2 v) => new System.Numerics.Vector2(v.x, v.y);
        public static implicit operator Vector2(System.Numerics.Vector2 v) => new Vector2(v.X, v.Y);

        public override string ToString() => $"({x}, {y})";
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct Vector3
    {
        public float x, y, z;
        public Vector3(float x, float y, float z) { this.x = x; this.y = y; this.z = z; }

        public static implicit operator System.Numerics.Vector3(Vector3 v) => new System.Numerics.Vector3(v.x, v.y, v.z);
        public static implicit operator Vector3(System.Numerics.Vector3 v) => new Vector3(v.X, v.Y, v.Z);

        public override string ToString() => $"({x}, {y}, {z})";
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct Vector4
    {
        public float x, y, z, w;
        public Vector4(float x, float y, float z, float w) { this.x = x; this.y = y; this.z = z; this.w = w; }

        public static implicit operator System.Numerics.Vector4(Vector4 v) => new System.Numerics.Vector4(v.x, v.y, v.z, v.w);
        public static implicit operator Vector4(System.Numerics.Vector4 v) => new Vector4(v.X, v.Y, v.Z, v.W);

        public override string ToString() => $"({x}, {y}, {z}, {w})";
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct Matrix4x4
    {
        public float m00, m01, m02, m03;
        public float m10, m11, m12, m13;
        public float m20, m21, m22, m23;
        public float m30, m31, m32, m33;

        public Matrix4x4(float v00, float v01, float v02, float v03, 
                         float v10, float v11, float v12, float v13,
                         float v20, float v21, float v22, float v23,
                         float v30, float v31, float v32, float v33)
        {
            m00 = v00; m01 = v01; m02 = v02; m03 = v03;
            m10 = v10; m11 = v11; m12 = v12; m13 = v13;
            m20 = v20; m21 = v21; m22 = v22; m23 = v23;
            m30 = v30; m31 = v31; m32 = v32; m33 = v33;
        }

        public static Matrix4x4 Identity => new Matrix4x4(
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        );
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct Guid
    {
        public ulong low;
        public ulong high;

        public Guid(ulong low, ulong high) { this.low = low; this.high = high; }
        public override string ToString() => $"{high:X16}{low:X16}";

        public static implicit operator Guid(System.Guid systemGuid)
        {
            byte[] bytes = systemGuid.ToByteArray();
            return new Guid(BitConverter.ToUInt64(bytes, 0), BitConverter.ToUInt64(bytes, 8));
        }
    }
}
