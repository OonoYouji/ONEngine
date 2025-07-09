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
	}

	private bool initialized = false;

	public void InternalInitialize(int _entityId) {
		if (!initialized) {
			initialized = true;
			entity = EntityCollection.GetEntity(_entityId);
			Log.WriteLine("MonoBehavior initialized for Entity ID: " + _entityId);


			Initialize();
		}
	}

	public virtual void Initialize() { }
	public virtual void Update() { }

	public virtual void OnCollisionEnter(Entity collision) { }
	public virtual void OnCollisionExit(Entity collision) { }
	public virtual void OnCollisionStay(Entity collision) { }



}
