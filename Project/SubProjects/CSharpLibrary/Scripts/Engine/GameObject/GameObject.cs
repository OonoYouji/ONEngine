using System;
using System.Collections.Generic;

public class GameObject {
	private Dictionary<Type, MonoBehavior> components = new Dictionary<Type, MonoBehavior>();
	public int EntityId { get; private set; }

	public GameObject(int entityId) {
		EntityId = entityId;
	}

	public T AddComponent<T>() where T : MonoBehavior, new() {
		T component = new T();
		component.InternalInitialize(EntityId);
		components[typeof(T)] = component;
		return component;
	}
}
