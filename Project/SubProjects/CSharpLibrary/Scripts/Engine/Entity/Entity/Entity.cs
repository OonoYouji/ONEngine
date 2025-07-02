using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

public class Entity {

	/// =========================================
	/// objects
	/// =========================================

	private Transform _transform;
	Dictionary<string, Component> _components = new Dictionary<string, Component>();

	int entityId;
	int parentId = -1; // 親のID

	public Transform transform => _transform;
	public int Id {
		get {
			return entityId;
		}
	}

	public string name {
		get {
			return InternalGetName(entityId);
		}
		set {
			InternalSetName(entityId, value);
		}
	}


	public Entity parent {
		get {
			int parentId = InternalGetParentId(entityId);
			return EntityCollection.GetEntity(parentId);
		}
		set {
			if(value == null) {
				Log.WriteLine("Cannot set parent to null. Entity ID: " + Id);
				return;
			}
			InternalSetParent(Id, value.Id);
		}
	}


	/// =========================================
	/// methods
	/// =========================================

	public Entity(int _id) {
		entityId = _id;
		_transform = AddComponent<Transform>();
		Log.WriteLine("Entity created: [" + name + "] (ID: " + entityId + ")");
	}


	public Entity GetChild(uint _index) {
		int childId = InternalGetChildId(entityId, _index);
		return EntityCollection.GetEntity(childId);
	}



	/// ------------------------------------------
	/// components
	/// ------------------------------------------

	public T AddComponent<T>() where T : Component {
		/// コンポーネントを作る
		string typeName = typeof(T).Name;
		ulong nativeHandle = InternalAddComponent<T>(entityId, typeName);


		T comp = Activator.CreateInstance<T>();
		comp.nativeHandle = nativeHandle;
		comp.entity = this;
		_components[typeName] = comp;
		return comp;
	}

	public T GetComponent<T>() where T : Component {
		/// コンポーネントを得る
		string typeName = typeof(T).Name;
		ulong nativeHandle = InternalGetComponent<T>(entityId, typeName);

		T comp = Activator.CreateInstance<T>();
		comp.nativeHandle = nativeHandle;
		comp.entity = this;
		_components[typeName] = comp;
		return comp;
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

}
