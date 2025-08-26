using System;
using System.Collections.Generic;

static public class ObjectPools {
	// static public Pool<Vector3> vector3Pool = new Pool<Vector3>();


	public class Pool<T> {
		private readonly Stack<T> pool = new Stack<T>();
		private readonly Func<T> factory;
		private readonly int maxSize;

		public Pool(Func<T> factory, int initialCapacity = 10, int maxSize = 100) {
			this.factory = factory;
			this.maxSize = maxSize;
			for (int i = 0; i < initialCapacity; i++) {
				pool.Push(factory());
			}
		}

		public T Get() => pool.Count > 0 ? pool.Pop() : factory();

		public void Release(T obj) {
			if (pool.Count < maxSize) pool.Push(obj);
		}

		public int Count => pool.Count;
	}
}