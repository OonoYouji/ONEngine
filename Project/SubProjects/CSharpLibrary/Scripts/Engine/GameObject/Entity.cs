using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

public class Entity {

	/// =========================================
	/// objects
	/// =========================================

	private Transform _transform;
	private Dictionary<Type, Component> components;

	uint entityId;
	string name;

	/// =========================================
	/// methods
	/// =========================================

	public Entity(uint _id) {
		entityId = _id;
		name = "Entity_" + entityId.ToString();
		_transform = AddComponent<Transform>();
	}

	public Transform transform => _transform;


	/// ------------------------------------------
	/// components
	/// ------------------------------------------

	public T AddComponent<T>() where T : Component {

		/// すでにコンポーネントが存在する場合はそれを返す
		if (components.TryGetValue(typeof(T), out var c)) {
			return c as T;
		}

		/// コンポーネントを作る
		string typeName = typeof(T).Name;
		ulong nativeHandle = InternalAddComponent<T>(entityId, typeName);

		T comp = Activator.CreateInstance<T>();
		comp.nativeHandle = nativeHandle;
		components[typeof(T)] = comp;
		return comp;
	}

	public T GetComponent<T>() where T : Component {
		if(components.TryGetValue(typeof(T), out var comp)) {
			return comp as T;
		}

		return null;
	}


	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern ulong InternalAddComponent<T>(uint _entityId, string _compTypeName);


}
