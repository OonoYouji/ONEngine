using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.CompilerServices;

public class SkinMeshRenderer : Component {

	public string meshPath {
		get {
			return InternalGetMeshName(nativeHandle);
		}
		set {
			InternalSetMeshName(nativeHandle, value);
		}
	}

	public string texturePath {
		get {
			return InternalGetTexturePath(nativeHandle);
		}
		set {
			InternalSetTexturePath(nativeHandle, value);
		}
	}

	public bool isPlaying {
		get {
			return InternalGetIsPlaying(nativeHandle);
		}
		set {
			InternalSetIsPlaying(nativeHandle, value);
		}
	}


	public float animationTime {
		get {
			return InternalGetAnimationTime(nativeHandle);
		}
		set {
			InternalSetAnimationTime(nativeHandle, value);
		}
	}

	public float animationScale {
		get {
			return InternalGetAnimationScale(nativeHandle);
		}
		set {
			InternalSetAnimationScale(nativeHandle, value);
		}
	}

	/// MeshPathのAccessor
	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern string InternalGetMeshName(ulong _nativeHandle);
	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalSetMeshName(ulong _nativeHandle, string _meshName);

	/// TexturePathのAccessor
	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern string InternalGetTexturePath(ulong _nativeHandle);
	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalSetTexturePath(ulong _nativeHandle, string _texturePath);

	/// IsPlayingのAccessor
	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern bool InternalGetIsPlaying(ulong _nativeHandle);
	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalSetIsPlaying(ulong _nativeHandle, bool _isPlaying);

	/// AnimationTimeのAccessor
	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern float InternalGetAnimationTime(ulong _nativeHandle);
	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalSetAnimationTime(ulong _nativeHandle, float _animationTime);

	/// AnimationScaleのAccessor
	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern float InternalGetAnimationScale(ulong _nativeHandle);
	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalSetAnimationScale(ulong _nativeHandle, float _animationScale);



}