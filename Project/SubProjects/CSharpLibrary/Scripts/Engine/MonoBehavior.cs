using System.Runtime.CompilerServices;
using System;
using System.Runtime.InteropServices;
using System.IO;

public class MonoBehavior {
	///////////////////////////////////////////////////////////////////////////////////////////
	/// objects
	///////////////////////////////////////////////////////////////////////////////////////////

	/// Behaviorの生成
	public void CreateBehavior(int _entityId, string _name, ECSGroup _ecsGroup) {
		if(!_ecsGroup) {
			Debug.LogError("MonoBehavior.CreateBehavior - ECSGroup is null. Cannot create MonoBehavior for Entity ID: " + _entityId);
			return;
		}

		name_    = _name;
		ecsGroup = _ecsGroup;
		entity   = ecsGroup.GetEntity(_entityId);

		Debug.Log("MonoBehavior created for Entity ID: " + _entityId + ", Script Name: " + _name + ", Group Name: " + _ecsGroup.groupName);
	}


	/// この behavior が所属するECSGroup
	public ECSGroup ecsGroup {
		get; internal set;
	}

	private string name_;
	public bool enable {
		get {
			return InternalGetEnable(entity.Id, name_, ecsGroup.groupName);
		}
		set {
			InternalSetEnable(entity.Id, name_, value, ecsGroup.groupName);
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

	

	///////////////////////////////////////////////////////////////////////////////////////////
	/// methods
	///////////////////////////////////////////////////////////////////////////////////////////

	public virtual void Awake() { Debug.Log("called awake method. owner:" + entity.name + ", script name:" + name_); }
	public virtual void Initialize() { Debug.Log("called initialize method. owner:" + entity.name + ", script name:" + name_); }
	public virtual void Update() { Debug.Log("called update method. owner:" + entity.name + ", script name:" + name_); }

	public virtual void OnCollisionEnter(Entity collision) { }
	public virtual void OnCollisionExit(Entity collision) { }
	public virtual void OnCollisionStay(Entity collision) { }



	///////////////////////////////////////////////////////////////////////////////////////////
	/// internal methods
	///////////////////////////////////////////////////////////////////////////////////////////

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern bool InternalGetEnable(int _entityId, string _scriptName, string _ecsGroupName);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalSetEnable(int _entityId, string _scriptName, bool _enable, string _ecsGroupName);

	///////////////////////////////////////////////////////////////////////////////////////////
	/// operators
	///////////////////////////////////////////////////////////////////////////////////////////


	public static implicit operator bool(MonoBehavior _monoBehavior) {
		return _monoBehavior != null;
	}

}
