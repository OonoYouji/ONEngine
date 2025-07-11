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

	public Quaternion rotate {
		get {
			float _x, _y, _z, _w;
			InternalGetRotate(nativeHandle, out _x, out _y, out _z, out _w);
			return new Quaternion(_x, _y, _z, _w);
		}
		set {
			InternalSetRotate(nativeHandle, value.x, value.y, value.z, value.w);
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
