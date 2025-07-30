using System.Runtime.CompilerServices;
using System;
using System.Runtime.InteropServices;
using System.IO;

public class MonoBehavior {

	string name;
	public bool enable {
		get {
			return InternalGetEnable(entity.Id, name);
		}
		set {
			InternalSetEnable(entity.Id, name, value);
		}
	}

	public Entity entity {
		get; internal set;
	}

	public Transform transform {
		get {
			if (entity == null) {
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
	public void InternalInitialize(int _entityId, string _name) {
		if (initialized) {
			Debug.LogError("MonoBehavior is already initialized for Entity ID: " + _entityId);
			return;
		}

		if (!initialized) {
			initialized = true;

			name = _name;
			Debug.Log("Initializing MonoBehavior: " + name + " for Entity ID: " + _entityId);

			entity = EntityCollection.GetEntity(_entityId);

			entity.AddScript(this);

			Awake();
		}
	}

	public virtual void Awake() { Debug.Log("called awake method. owner:" + entity.name + ", script name:" + name); }
	public virtual void Initialize() { Debug.Log("called initialize method. owner:" + entity.name + ", script name:" + name); }
	public virtual void Update() { Debug.Log("called update method. owner:" + entity.name + ", script name:" + name); }

	public virtual void OnCollisionEnter(Entity collision) { }
	public virtual void OnCollisionExit(Entity collision) { }
	public virtual void OnCollisionStay(Entity collision) { }

	
	public static implicit operator bool(MonoBehavior _monoBehavior) {
		return _monoBehavior != null;
	}


	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern bool InternalGetEnable(int _entityId, string _scriptName);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalSetEnable(int _entityId, string _scriptName, bool _enable);

}
