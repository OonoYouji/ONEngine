
using System.Runtime.InteropServices;

//namespace Mathf {

//	//struct Vec3 {
//	//	float x, y, z;
//	//};

//}



[StructLayout(LayoutKind.Sequential)]
public struct Vec3 {
	public float x, y, z;
}

[DllImport("MyCppLib.dll")]
public static extern void GetPosition(out Vec3 pos);


Vec3 position;
GetPosition(out position);
Console.WriteLine($"Position: x={position.x}, y={position.y}, z={position.z}");