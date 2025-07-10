using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

public class Entity {

	/// <summary>
	/// コンストラクタ
	/// </summary>
	public Entity(int _id) {
		entityId_ = _id;
		transform = AddComponent<Transform>();
		Log.WriteLine("Entity created: [" + name + "] (ID: " + entityId_ + ")");
	}



	/// =========================================
	/// objects
	/// =========================================

	Dictionary<string, Component> components_ = new Dictionary<string, Component>();
	Dictionary<string, MonoBehavior> scripts_ = new Dictionary<string, MonoBehavior>();

	int entityId_;
	int parentId_ = -1; // 親のID

	public Transform transform;
	public int Id {
		get {
			return entityId_;
		}
	}

	public string name {
		get {
			return InternalGetName(entityId_);
		}
		set {
			InternalSetName(entityId_, value);
		}
	}


	public Entity parent {
		get {
			int parentId = InternalGetParentId(entityId_);
			return EntityCollection.GetEntity(parentId);
		}
		set {
			if (value == null) {
				Log.WriteLine("Cannot set parent to null. Entity ID: " + Id);
				return;
			}
			InternalSetParent(Id, value.Id);
		}
	}


	/// =========================================
	/// methods
	/// =========================================


	public Entity GetChild(uint _index) {
		int childId = InternalGetChildId(entityId_, _index);
		return EntityCollection.GetEntity(childId);
	}



	/// ------------------------------------------
	/// components
	/// ------------------------------------------

	public T AddComponent<T>() where T : Component {
		/// コンポーネントを作る
		string typeName = typeof(T).Name;
		ulong nativeHandle = InternalAddComponent<T>(entityId_, typeName);


		T comp = Activator.CreateInstance<T>();
		comp.nativeHandle = nativeHandle;
		comp.entity = this;
		components_[typeName] = comp;


		if (comp == null) {
			Log.WriteLine("Failed to create component: " + typeName + " (Entity ID: " + entityId_ + ")");
		} else {
			Log.WriteLine("AddComponent<" + typeName + ">(): pointer:" + nativeHandle);
			Log.WriteLine("Component added: " + typeName + " (Entity ID: " + entityId_ + ")");
		}

		return comp;
	}

	public T GetComponent<T>() where T : Component {
		/// コンポーネントを得る
		string typeName = typeof(T).Name;
		ulong nativeHandle = InternalGetComponent<T>(entityId_, typeName);

		if (nativeHandle == 0) {
			Log.WriteLine("Component not found: " + typeName + " (Entity ID: " + entityId_ + ")");
			return null;
		}

		T comp = Activator.CreateInstance<T>();
		comp.nativeHandle = nativeHandle;
		comp.entity = this;
		components_[typeName] = comp;
		return comp;
	}


	public T GetScript<T>() where T : MonoBehavior {
		/// スクリプトを得る
		string typeName = typeof(T).Name;
		if (scripts_.ContainsKey(typeName)) {
			/// あったので返す
			return (T)scripts_[typeName];
		}

		/// なかったのでnullを返す
		return null;
	}

	public T AddScript<T>() where T : MonoBehavior {

		/// スクリプトを得る
		string typeName = typeof(T).Name;
		if (scripts_.ContainsKey(typeName)) {
			/// あったので返す
			return (T)scripts_[typeName];
		}

		/// なかったので新しく作る
		T script = Activator.CreateInstance<T>();
		script.InternalInitialize(entityId_);
		scripts_[typeName] = script;

		/// c++側でもスクリプトを追加
		InternalAddScript(entityId_, typeName);

		return (T)scripts_[typeName];
	}

	public MonoBehavior AddScript(MonoBehavior mb) {
		string scriptName = mb.GetType().Name;

		/// スクリプトを得る
		if (scripts_.ContainsKey(scriptName)) {
			Log.WriteLine("Script already exists: " + scriptName + " (Entity ID: " + entityId_ + ")");
			/// あったので返す
			return scripts_[scriptName];
		}

		/// なかったので新しく作る
		scripts_[scriptName] = mb;
		Log.WriteLine("Adding script: \"" + scriptName + "\" to Entity ID: " + entityId_);

		/// c++側でもスクリプトを追加
		InternalAddScript(entityId_, scriptName);

		return mb;
	}




	/// ------------------------------------------
	/// internal methods
	/// ------------------------------------------

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern ulong InternalAddComponent<T>(int _entityId, string _compTypeName);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern ulong InternalGetComponent<T>(int _entityId, string _compTypeName);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern string InternalGetName(int _entityId);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalSetName(int _entityId, string _name);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern int InternalGetChildId(int _entityId, uint _childIndex);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern int InternalGetParentId(int _entityId);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalSetParent(int _entityId, int _parentId);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalAddScript(int _entityId, string _scriptName);

}
