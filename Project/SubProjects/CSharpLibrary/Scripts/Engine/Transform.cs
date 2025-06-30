using System;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;

public class Transform : Component {

	public Vector3 position {
		get {
			float _x, _y, _z;
			InternalGetPosition(nativeHandle, out _x, out _y, out _z);
			return new Vector3(_x, _y, _z);
		}
		set {
			InternalSetPosition(nativeHandle, value.x, value.y, value.z);
		}
	}

	public Vector3 localPosition {
		get {
			float _x, _y, _z;
			InternalGetLocalPosition(nativeHandle, out _x, out _y, out _z);
			return new Vector3(_x, _y, _z);
		}
		set {
			InternalSetLocalPosition(nativeHandle, value.x, value.y, value.z);
		}
	}

	public Vector3 rotate {
		get {
			float _x, _y, _z;
			InternalGetRotate(nativeHandle, out _x, out _y, out _z);
			return new Vector3(_x, _y, _z);
		}
		set {
			InternalSetRotate(nativeHandle, value.x, value.y, value.z);
		}
	}

	public Vector3 scale {
		get {
			float _x, _y, _z;
			InternalGetScale(nativeHandle, out _x, out _y, out _z);
			return new Vector3(_x, _y, _z);
		}
		set {
			InternalSetScale(nativeHandle, value.x, value.y, value.z);
		}
	}

	//public Matrix4x4 matrix {
	//	get {
	//		Matrix4x4 _matrix = new Matrix4x4();
	//		_matrix.SetTRS(position, rotate, scale);
	//		return _matrix;
	//	}
	//}

	/// ========================================
	/// static methods
	/// ========================================

	static public Transform GetTransform(uint _entityId) {
		return InternalGetTransform(_entityId);
	}


	/// ========================================
	/// c++ internal methods
	/// ========================================

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern Transform InternalGetTransform(uint _entityId);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalGetPosition(ulong _nativeHandle, out float _x, out float _y, out float _z);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalGetLocalPosition(ulong _nativeHandle, out float _x, out float _y, out float _z);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalGetRotate(ulong _nativeHandle, out float _x, out float _y, out float _z);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalGetScale(ulong _nativeHandle, out float _x, out float _y, out float _z);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalSetPosition(ulong _nativeHandle, float _x, float _y, float _z);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalSetLocalPosition(ulong _nativeHandle, float _x, float _y, float _z);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalSetRotate(ulong _nativeHandle, float _x, float _y, float _z);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalSetScale(ulong _nativeHandle, float _x, float _y, float _z);

}
