using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

public class Entity {

	/// =========================================
	/// objects
	/// =========================================

	private Transform _transform;

	int entityId;
	string name;

	/// =========================================
	/// methods
	/// =========================================

	public Entity(uint _id) {
		entityId = (int)_id;
		name = "Entity_" + entityId.ToString();
		_transform = Transform.GetTransform(_id);
	}

	public Transform transform => _transform;



	//private Dictionary<Type, MonoBehavior> components = new Dictionary<Type, MonoBehavior>();
	//public int EntityId { get; private set; }

	//public Entity(Entity _gameObject) {
	//	//EntityId = _gameObject.EntityId;
	//	foreach (var component in _gameObject.components) {
	//		MonoBehavior newComponent = (MonoBehavior)Activator.CreateInstance(component.Key);
	//		newComponent.InternalInitialize(EntityId);
	//		components[component.Key] = newComponent;
	//	}
	//}

	//public Entity(int entityId) {
	//	EntityId = entityId;
	//}

	//public T AddComponent<T>() where T : MonoBehavior, new() {
	//	T component = new T();
	//	component.InternalInitialize(EntityId);
	//	components[typeof(T)] = component;
	//	return component;
	//}


}
