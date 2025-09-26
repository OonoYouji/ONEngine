using System;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;

public class TransformData {
	public Vector3 position;
	public Quaternion rotate;
	public Vector3 scale;
}

public class Transform : Component {
	
	public Vector3 position = new Vector3(0f, 0f, 0f);
	public Quaternion rotate = Quaternion.identity;
	public Vector3 scale = new Vector3(1f, 1f, 1f);

	public override void Begin() {
		InternalGetPosition(nativeHandle, out position.x, out position.y, out position.z);
		InternalGetRotate(nativeHandle, out  rotate.x, out rotate.y, out rotate.z, out rotate.w);
		InternalGetScale(nativeHandle, out scale.x, out scale.y, out scale.z);
	}

	public override void End() {
		InternalSetPosition(nativeHandle, position.x, position.y, position.z);
		InternalSetRotate(nativeHandle, rotate.x, rotate.y, rotate.z, rotate.w);
		InternalSetScale(nativeHandle, scale.x, scale.y, scale.z);
	}
	
	// public Vector3 position {
	// 	get {
	// 		float _x, _y, _z;
	// 		InternalGetPosition(nativeHandle, out _x, out _y, out _z);
	// 		return new Vector3(_x, _y, _z);
	// 	}
	// 	set {
	// 		InternalSetPosition(nativeHandle, value.x, value.y, value.z);
	// 	}
	// }
	//
	// public Quaternion rotate {
	// 	get {
	// 		float _x, _y, _z, _w;
	// 		InternalGetRotate(nativeHandle, out _x, out _y, out _z, out _w);
	// 		return new Quaternion(_x, _y, _z, _w);
	// 	}
	// 	set {
	// 		InternalSetRotate(nativeHandle, value.x, value.y, value.z, value.w);
	// 	}
	// }
	//
	// public Vector3 scale {
	// 	get {
	// 		float _x, _y, _z;
	// 		InternalGetScale(nativeHandle, out _x, out _y, out _z);
	// 		return new Vector3(_x, _y, _z);
	// 	}
	// 	set {
	// 		InternalSetScale(nativeHandle, value.x, value.y, value.z);
	// 	}
	// }


	/// ========================================
	/// c++ internal methods
	/// ========================================

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalGetPosition(ulong _nativeHandle, out float _x, out float _y, out float _z);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalGetLocalPosition(ulong _nativeHandle, out float _x, out float _y, out float _z);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalGetRotate(ulong _nativeHandle, out float _x, out float _y, out float _z, out float _w);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalGetScale(ulong _nativeHandle, out float _x, out float _y, out float _z);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalSetPosition(ulong _nativeHandle, float _x, float _y, float _z);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalSetLocalPosition(ulong _nativeHandle, float _x, float _y, float _z);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalSetRotate(ulong _nativeHandle, float _x, float _y, float _z, float _w);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalSetScale(ulong _nativeHandle, float _x, float _y, float _z);

}
