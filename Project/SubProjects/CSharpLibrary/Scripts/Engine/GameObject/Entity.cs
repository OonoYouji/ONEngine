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

	uint entityId;

	public Transform transform => _transform;
	public uint Id {
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



	/// =========================================
	/// methods
	/// =========================================

	public Entity(uint _id) {
		entityId = _id;
		_transform = AddComponent<Transform>();
		Log.WriteLine("Entity created: " + name + " (ID: " + entityId + ")");
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
	static extern ulong InternalAddComponent<T>(uint _entityId, string _compTypeName);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern ulong InternalGetComponent<T>(uint _entityId, string _compTypeName);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern string InternalGetName(uint _entityId);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalSetName(uint _entityId, string _name);


}
