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
			if(entity == null) {
				Debug.Log("[error] Entity is not initialized. Please call InternalInitialize first.");
				return null;
			}

			if (entity.transform == null) {
				Debug.Log("[error] Transform component is not initialized for Entity ID: " + entity.Id);
				return null;
			}

			return entity.transform;
		}
	}

	private bool initialized = false;

	public void InternalInitialize(int _entityId) {
		if(initialized) {
			Debug.Log("[error] MonoBehavior is already initialized for Entity ID: " + _entityId);
			return;
		}

		if (!initialized) {
			initialized = true;
			entity = EntityCollection.GetEntity(_entityId);
			Debug.Log("Initializing MonoBehavior for Entity ID: " + _entityId + "Entity Name " + entity.name);

			entity.AddScript(this);

			Awake();
		}
	}

	public virtual void Awake() { }
	public virtual void Initialize() { }
	public virtual void Update() { }

	public virtual void OnCollisionEnter(Entity collision) { }
	public virtual void OnCollisionExit(Entity collision) { }
	public virtual void OnCollisionStay(Entity collision) { }



}
