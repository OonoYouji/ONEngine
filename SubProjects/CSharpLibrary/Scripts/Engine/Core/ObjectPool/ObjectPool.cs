using System;
using System.Collections.Generic;

public class ObjectPool<T> {
	private readonly Stack<T> pool = new Stack<T>();
	private readonly Func<T> factory;
	private readonly int maxSize;

	public ObjectPool(Func<T> factory, int initialCapacity = 10, int maxSize = 100) {
		this.factory = factory;
		this.maxSize = maxSize;
		for (int i = 0; i < initialCapacity; i++)
			pool.Push(factory());
	}

	public T Get() => pool.Count > 0 ? pool.Pop() : factory();

	public void Release(T obj) {
		if (pool.Count < maxSize) pool.Push(obj);
	}

	public int Count => pool.Count;
}