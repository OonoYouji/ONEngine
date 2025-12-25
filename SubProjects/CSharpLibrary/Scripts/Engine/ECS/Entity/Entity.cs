using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Security.Principal;

public class Entity {

	/// =========================================
	/// objects
	/// =========================================

	// transform
	public Transform transform;

	// components, scripts
	private Dictionary<string, Component> components_ = new Dictionary<string, Component>();
	private Dictionary<string, MonoScript> scripts_ = new Dictionary<string, MonoScript>();

	// id
	private int entityId_;
	private int parentId_ = 0; // 親のID

	// ecs group name
	private string ecsGroupName_;
	private ECSGroup ecsGroup_;

	/// =========================================
	/// methods
	/// =========================================

	/// <summary>
	/// コンストラクタ
	/// </summary>
	public Entity(int _id, ECSGroup _ecsGroup) {
		entityId_ = _id;
		ecsGroup_ = _ecsGroup;
		ecsGroupName_ = ecsGroup_.groupName;
		transform = AddComponent<Transform>();
	}

	public int Id {
		get {
			return entityId_;
		}
	}

	public string name {
		get {
			IntPtr namePtr = InternalGetName(entityId_, ecsGroupName_);
			if (namePtr == IntPtr.Zero) {
				// Debug.Log("[error] Entity name is null for ID: " + entityId_);
				return "UnnamedEntity";
			}
			string name = Marshal.PtrToStringAnsi(namePtr);
			return name;
		}
		set {
			InternalSetName(entityId_, value, ecsGroupName_);
		}
	}

	public Entity parent {
		get {
			int parentId = InternalGetParentId(entityId_, ecsGroupName_);
			Entity parentEntity = ecsGroup_.GetEntity(parentId);
			if (parentEntity) {
				return parentEntity;
			}

			return null;
		}
		set {
			if (value == null) {
				return;
			}
			InternalSetParent(Id, value.Id, ecsGroupName_);
		}
	}

	public bool enable {
		get {
			return InternalGetEnable(entityId_, ecsGroupName_);
		}
		set {
			InternalSetEnable(entityId_, value, ecsGroupName_);
		}
	}


	/// =========================================
	/// methods
	/// =========================================

	public Entity GetChild(uint _index) {
		int childId = InternalGetChildId(entityId_, _index, ecsGroupName_);
		ECSGroup ecsGroup = EntityComponentSystem.GetECSGroup(ecsGroupName_);
		if (ecsGroup == null) {
			return null;
		}

		return ecsGroup.GetEntity(childId);
	}

	public uint GetChildCount() {
		return (uint)InternalGetChildrenCount(entityId_, ecsGroupName_);
	}


	public void Destroy() {
		/// 子の情報もクリア
		for (uint i = 0; i < GetChildCount(); i++) {
			Entity child = GetChild(i);
			if (child) {
				Debug.LogInfo("Entity.Destroy - Destroying child entity ID: " + child.Id + " of parent entity ID: " + entityId_);
				child.Destroy();
			}
		}

		/// Entityを削除
		ecsGroup_.DestroyEntity(entityId_);
		entityId_ = 0; // IDを無効化
		transform = null;

		foreach (var comp in components_) {
			ecsGroup_.componentCollection.RemoveComponent(comp.Value);
		}

		components_.Clear();
		scripts_.Clear();
	}


	/// ------------------------------------------
	/// components
	/// ------------------------------------------

	public T AddComponent<T>() where T : Component {
		/// コンポーネントを作る
		string typeName = typeof(T).Name;
		uint compId;
		ulong nativeHandle = InternalAddComponent<T>(entityId_, typeName, ecsGroupName_, out compId);

		T comp = Activator.CreateInstance<T>();
		comp.nativeHandle = nativeHandle;
		comp.entity = this;
		comp.compId = compId;
		components_[typeName] = comp;

		if (comp == null) {
			Debug.LogError("Failed to create component: " + typeName + " (Entity ID: " + entityId_ + ")");
		}

		ecsGroup_.componentCollection.AddComponent(comp);
		return comp;
	}

	public T GetComponent<T>() where T : Component {
		if (components_.ContainsKey(typeof(T).Name)) {
			return (T)components_[typeof(T).Name];
		}

		/// コンポーネントを得る
		string typeName = typeof(T).Name;
		uint compId;
		ulong nativeHandle = InternalGetComponent<T>(entityId_, typeName, ecsGroupName_, out compId);

		if (nativeHandle == 0) {
			return null;
		}

		T comp = Activator.CreateInstance<T>();
		comp.nativeHandle = nativeHandle;
		comp.compId = compId;
		comp.entity = this;
		components_[typeName] = comp;

		ecsGroup_.componentCollection.AddComponent(comp);

		return comp;
	}

	public List<Component> GetComponents() {
		List<Component> result = new List<Component>();
		foreach (var keyValuePair in components_) {
			result.Add(keyValuePair.Value);
		}
		return result;
	}

	public T GetScript<T>() where T : MonoScript {
		/// スクリプトを得る
		string typeName = typeof(T).Name;
		if (scripts_.ContainsKey(typeName)) {
			return (T)scripts_[typeName];
		}
		if (InternalGetScript(entityId_, typeName, ecsGroupName_)) {
			return AddScript<T>();
		}
		return null;
	}

	public MonoScript GetScript(string _scriptName) {
		/// スクリプトを得る
		if (scripts_.ContainsKey(_scriptName)) {
			return scripts_[_scriptName];
		}
		return null;
	}

	public List<MonoScript> GetScripts() {
		List<MonoScript> result = new List<MonoScript>();
		foreach (var keyValuePair in scripts_) {
			result.Add(keyValuePair.Value);
		}
		return result;
	}

	public T AddScript<T>() where T : MonoScript {
		/// スクリプトを得る
		string typeName = typeof(T).Name;
		if (scripts_.ContainsKey(typeName)) {
			/// あったので返す
			return (T)scripts_[typeName];
		}

		/// なかったので新しく作る
		T script = Activator.CreateInstance<T>();
		script.CreateBehavior(entityId_, typeName, ecsGroup_);
		scripts_[typeName] = script;

		/// c++側でもスクリプトを追加
		InternalAddScript(entityId_, typeName, ecsGroupName_);

		return (T)scripts_[typeName];
	}

	public MonoScript AddScript(MonoScript mb) {
		string scriptName = mb.GetType().Name;

		/// スクリプトを得る
		if (scripts_.ContainsKey(scriptName)) {
			return scripts_[scriptName];
		}

		/// なかったので新しく作る
		scripts_[scriptName] = mb;
		/// c++側でもスクリプトを追加
		InternalAddScript(entityId_, scriptName, ecsGroupName_);
		return mb;
	}


	public static implicit operator bool(Entity _entity) {
		return _entity != null;
	}

	/// ------------------------------------------
	/// internal methods
	/// ------------------------------------------

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern ulong InternalAddComponent<T>(int _entityId, string _compTypeName, string _groupName, out uint _compId);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern ulong InternalGetComponent<T>(int _entityId, string _compTypeName, string _groupName, out uint _compId);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern IntPtr InternalGetName(int _entityId, string _groupName);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalSetName(int _entityId, string _name, string _groupName);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern int InternalGetChildId(int _entityId, uint _childIndex, string _groupName);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern int InternalGetChildrenCount(int _entityId, string _groupName);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern int InternalGetParentId(int _entityId, string _groupName);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalSetParent(int _entityId, int _parentId, string _groupName);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalAddScript(int _entityId, string _scriptName, string _groupName);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern bool InternalGetScript(int _entityId, string _scriptName, string _groupName);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern bool InternalGetEnable(int _entityId, string _groupName);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalSetEnable(int _entityId, bool _enable, string _groupName);

}
