using System.Runtime.CompilerServices;
using System;
using System.Runtime.InteropServices;

public class MonoBehavior {

	public Entity entity {
		get; internal set;
	}

	public Transform transform {
		get {
			return entity.transform;
		}
		set {
			entity.transform = value;
		} 
	}


	private bool initialized = false;
	internal int entityId = -1;


	public void InternalInitialize(int _entityId) {
		if(!initialized) {
			this.entityId = _entityId;
			Initialize();
			initialized = true;
		}
	}

	public virtual void Initialize() {}
	public virtual void Update() {}

	public int EntityId {
		get {
			if (entityId == -1) {
				entityId = InternalGetEntityId();
			}
			return entityId;
		}
		set {
			entityId = value;
		}
	}


	[MethodImpl(MethodImplOptions.InternalCall)]
	private static extern int InternalGetEntityId();



}
