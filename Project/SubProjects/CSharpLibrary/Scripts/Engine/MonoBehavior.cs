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
			//entity.transform = value;
		} 
	}


	private bool initialized = false;
	internal int entityId = -1;


	public void InternalInitialize(uint _entityId) {
		if(!initialized) {
			this.entityId = (int)_entityId;
			Initialize();
			initialized = true;

			entity = new Entity(_entityId);
			transform = entity.transform;

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
