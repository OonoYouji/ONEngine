using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.CompilerServices;


static public class EntityUtility {

	static public void Create(Entity _src, Vector3 _position) {
		ulong nativeHandle;
		InternalCreateEntity(_src.Id, out nativeHandle);
		//Entity entity = 
	}



	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalCreateEntity(uint _id, out ulong _nativeHandle);

}