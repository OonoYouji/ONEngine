using System;
using System.Runtime.InteropServices;

[StructLayout(LayoutKind.Sequential)]
public struct Transform {
	UInt64 virtualMethod; ///< IComponentの仮想関数
	public int enable; ///< コンポーネントが有効かどうか
	UInt32 id;
	UInt64 gameObjectId; ///< GameObjectのID

	public Vector3 position;
	public Vector3 rotate;
	public Vector3 scale;
	public Matrix4x4 matrix;
	int matrixCalcFlags;
}
