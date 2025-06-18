using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

public class Entity {

	//private UInt64 padding; // 64bit align, virtual destructor


	public Transform transform {
		get {
			IntPtr ptr = InternalGetTransform(entityId);
			return Marshal.PtrToStructure<Transform>(ptr);
		}
		set {
			InternalSetTransform(entityId, ref value);
		}
	}

	int entityId;
	string name;

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


	[MethodImpl(MethodImplOptions.InternalCall)]
	private static extern IntPtr InternalGetTransform(int _entityId);

	[MethodImpl(MethodImplOptions.InternalCall)]
	private static extern void InternalSetTransform(int _entityId, ref Transform _transform);

}
