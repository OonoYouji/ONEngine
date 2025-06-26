using System.Runtime.CompilerServices;
using System;
using System.Runtime.InteropServices;
using System.IO;

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
	

		if (!initialized) {
			this.entityId = (int)_entityId;
			Initialize();
			initialized = true;

			entity = new Entity(_entityId);
			Log.WriteLine("MonoBehavior initialized for Entity ID: " + _entityId);
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


	public virtual void OnCollisionEnter(Entity collision) {
		// Override this method to handle collision events
	}

	public virtual void OnCollisionExit(Entity collision) {
		// Override this method to handle collision exit events
	}

	public virtual void OnCollisionStay(Entity collision) {
		// Override this method to handle collision stay events
	}


	[MethodImpl(MethodImplOptions.InternalCall)]
	private static extern int InternalGetEntityId();



}
