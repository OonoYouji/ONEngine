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

	public void InternalInitialize(uint _entityId) {
		if (!initialized) {
			Initialize();
			initialized = true;

			entity = new Entity(_entityId);
			Log.WriteLine("MonoBehavior initialized for Entity ID: " + _entityId);
		}
	}

	public virtual void Initialize() { }
	public virtual void Update() { }

	public virtual void OnCollisionEnter(Entity collision) { }
	public virtual void OnCollisionExit(Entity collision) { }
	public virtual void OnCollisionStay(Entity collision) { }


	[MethodImpl(MethodImplOptions.InternalCall)]
	private static extern int InternalGetEntityId();



}
