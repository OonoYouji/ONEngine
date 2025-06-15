using System.Runtime.CompilerServices;
using System;
using System.Runtime.InteropServices;

public class MonoBehavior {

	public int entityId;

	public virtual void Initialize() {}
	public virtual void Update() {}

	public Transform transform {
		get {
			IntPtr ptr = InternalGetTransform(entityId);
			return Marshal.PtrToStructure<Transform>(ptr);
		}
		set {
			InternalSetTransform(entityId, ref value);
		}
	}


	[MethodImpl(MethodImplOptions.InternalCall)]
	private static extern int InternalGetEntityId();

	[MethodImpl(MethodImplOptions.InternalCall)]
	private static extern IntPtr InternalGetTransform(int _entityId);

	[MethodImpl(MethodImplOptions.InternalCall)]
	private static extern void InternalSetTransform(int _entityId, ref Transform _transform);


}
