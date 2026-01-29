using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.CompilerServices;

class MeshRenderer : Component {
	public struct BatchData {
		public uint compId;
		public Vector4 color;
		public uint postEffectFlags;
	}

	BatchData batchData;
	public BatchData GetBatchData() {
		return batchData;
	}


	public string meshPath {
		get {
			return InternalGetMeshName(nativeHandle);
		}
		set {
			InternalSetMeshName(nativeHandle, value);
		}
	}

	public Vector4 color {
		get {
			return batchData.color;
		}
		set {
			batchData.color = value;
		}
	}

	public uint postEffectFlags {
		get {
			return batchData.postEffectFlags;
		}
		set {
			batchData.postEffectFlags = value;
		}
	}

	/// -------------------------------------------
	/// internal methods
	/// -------------------------------------------

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern string InternalGetMeshName(ulong _nativeHandle);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalSetMeshName(ulong _nativeHandle, string _meshName);

	//[MethodImpl(MethodImplOptions.InternalCall)]
	//static extern Vector4 InternalGetColor(ulong _nativeHandle);

	//[MethodImpl(MethodImplOptions.InternalCall)]
	//static extern void InternalSetColor(ulong _nativeHandle, Vector4 _color);

	//[MethodImpl(MethodImplOptions.InternalCall)]
	//static extern uint InternalGetPostEffectFlags(ulong _nativeHandle);

	//[MethodImpl(MethodImplOptions.InternalCall)]
	//static extern void InternalSetPostEffectFlags(ulong _nativeHandle, uint _flags);

}
