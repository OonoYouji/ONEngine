using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.CompilerServices;

class MeshRenderer : Component {

	public string meshName {
		get {
			return InternalGetMeshName(entity.Id);
		}
		set {
			InternalSetMeshName(entity.Id, value);
		}
	}


	/// -------------------------------------------
	/// internal methods
	/// -------------------------------------------

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern string InternalGetMeshName(uint _entityId);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalSetMeshName(uint _entityId, string _meshName);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern Vector4 InternalGetColor(uint _entityId);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalSetColor(uint _entityId, Vector4 _color);


}
