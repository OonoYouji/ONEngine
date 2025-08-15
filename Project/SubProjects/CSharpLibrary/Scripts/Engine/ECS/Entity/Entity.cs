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
	private Dictionary<string, MonoBehavior> scripts_ = new Dictionary<string, MonoBehavior>();

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
		Debug.Log("Entity created: [" + name + "] (ID: " + entityId_ + ")");


		/// transformがnullじゃないかチェック
		if (!transform) {
			Debug.LogError("Entity.Entity - Transform component is null for Entity ID: " + entityId_);
			return;
		}
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
				Debug.Log("[error] Entity name is null for ID: " + entityId_);
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
			ECSGroup ecsGroup = EntityComponentSystem.GetECSGroup(ecsGroupName_);
			if (ecsGroup) {
				return ecsGroup.GetEntity(parentId);
			}

			Debug.LogError("Entity.parent - ECSGroup not found for Entity ID: " + entityId_ + " Name: " + name);
			return null;
		}
		set {
			if (value == null) {
				Debug.Log("Entity.parent - Cannot set parent to null. Entity ID: " + Id);
				return;
			}
			InternalSetParent(Id, value.Id, ecsGroupName_);
		}
	}


	/// =========================================
	/// methods
	/// =========================================


	public Entity GetChild(uint _index) {
		int childId = InternalGetChildId(entityId_, _index, ecsGroupName_);
		ECSGroup ecsGroup = EntityComponentSystem.GetECSGroup(ecsGroupName_);
		if (ecsGroup == null) {
			Debug.LogError("Entity.GetChild - ECSGroup not found for Entity ID: " + entityId_);
			return null;
		}

		return ecsGroup.GetEntity(childId);
	}


	public void Destroy() {
		/// Entityを削除
		Debug.Log("Destroying Entity: " + name + " (ID: " + entityId_ + ")");
		ecsGroup_.DestroyEntity(entityId_);
		entityId_ = 0; // IDを無効化
		transform = null;
		components_.Clear();
		scripts_.Clear();
	}


	/// ------------------------------------------
	/// components
	/// ------------------------------------------

	public T AddComponent<T>() where T : Component {
		/// コンポーネントを作る
		string typeName = typeof(T).Name;
		ulong nativeHandle = InternalAddComponent<T>(entityId_, typeName, ecsGroupName_);

		T comp = Activator.CreateInstance<T>();
		comp.nativeHandle = nativeHandle;
		comp.entity = this;
		components_[typeName] = comp;

		if (comp == null) {
			Debug.LogError("Failed to create component: " + typeName + " (Entity ID: " + entityId_ + ")");
		} else {
			Debug.Log(name + "(" + Id + ")" + "->AddComponent<" + typeName + ">(): pointer:" + nativeHandle);
		}

		return comp;
	}

	public T GetComponent<T>() where T : Component {
		/// コンポーネントを得る
		string typeName = typeof(T).Name;
		ulong nativeHandle = InternalGetComponent<T>(entityId_, typeName, ecsGroupName_);

		if (nativeHandle == 0) {
			Debug.LogError("Component not found: " + typeName + " (Entity ID: " + entityId_ + ")");
			return null;
		}

		T comp = Activator.CreateInstance<T>();
		comp.nativeHandle = nativeHandle;
		comp.entity = this;
		components_[typeName] = comp;
		return comp;
	}


	public T GetScript<T>() where T : MonoBehavior {
		Debug.LogInfo("GetScript<" + typeof(T).Name + ">() called for Entity ID: " + entityId_);

		/// スクリプトを得る
		string typeName = typeof(T).Name;
		if (scripts_.ContainsKey(typeName)) {
			/// あったので返す
			return (T)scripts_[typeName];
		}

		if (InternalGetScript(entityId_, typeName, ecsGroupName_)) {
			Debug.LogInfo("Entity.GetScript<T> - [Entity: " + entityId_ + "] Script " + typeName + " found in C++ — adding to C# side.");
			return AddScript<T>();
		}

		Debug.LogWarning("GetScript<" + typeof(T).Name + ">(); did not exist.");
		/// なかったのでnullを返す
		return null;
	}

	public List<MonoBehavior> GetScripts() {
		List<MonoBehavior> result = new List<MonoBehavior>();
		foreach (var keyValuePair in scripts_) {
			result.Add(keyValuePair.Value);
		}

		return result;
	}

	public T AddScript<T>() where T : MonoBehavior {
		Debug.LogInfo("Entity.AddScript<T> - Adding script: " + typeof(T).Name + " to Entity ID: " + entityId_);

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

	public MonoBehavior AddScript(MonoBehavior mb) {
		string scriptName = mb.GetType().Name;

		/// スクリプトを得る
		if (scripts_.ContainsKey(scriptName)) {
			Debug.Log("MonoBehavior.AddScript - Script already exists: " + scriptName + " (Entity ID: " + entityId_ + ")");
			/// あったので返す
			return scripts_[scriptName];
		}

		/// なかったので新しく作る
		scripts_[scriptName] = mb;
		Debug.Log("MonoBehavior.AddScript - Adding script: \"" + scriptName + "\" to Entity ID: " + entityId_);

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
	static extern ulong InternalAddComponent<T>(int _entityId, string _compTypeName, string _groupName);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern ulong InternalGetComponent<T>(int _entityId, string _compTypeName, string _groupName);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern IntPtr InternalGetName(int _entityId, string _groupName);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalSetName(int _entityId, string _name, string _groupName);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern int InternalGetChildId(int _entityId, uint _childIndex, string _groupName);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern int InternalGetParentId(int _entityId, string _groupName);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalSetParent(int _entityId, int _parentId, string _groupName);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalAddScript(int _entityId, string _scriptName, string _groupName);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern bool InternalGetScript(int _entityId, string _scriptName, string _groupName);
}
