using System.Runtime.InteropServices;

[StructLayout(LayoutKind.Sequential)]
public struct Transform {
	public Vector3 position;
	public Vector3 rotate;
	public Vector3 scale;
	public Matrix4x4 matrix;
	int matrixCalcFlags;
	//public Transform() {
	//	position = new Vector3();
	//	rotate = new Vector3();
	//	scale = new Vector3(1.0f, 1.0f, 1.0f);
	//}
}
