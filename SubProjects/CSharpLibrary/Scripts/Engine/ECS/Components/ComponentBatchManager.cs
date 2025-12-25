using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;


delegate Array ComponentBatchConverter(IComponentArray array);
delegate Array ComponentBatchAllocator(int count);

static class ComponentBatchManager {
	private static Dictionary<Type, ComponentBatchConverter> converters = new Dictionary<Type, ComponentBatchConverter>();
	private static Dictionary<Type, ComponentBatchAllocator> allocators = new Dictionary<Type, ComponentBatchAllocator>();


	public static void Initialize() {

		/// Transform のバッチコンバータ登録
		RegisterConverter<Transform, Transform.BatchData>((ComponentArray<Transform> array) => {
			int count = array.Count;
			Transform.BatchData[] batch = new Transform.BatchData[count];
			for (int i = 0; i < count; i++) {
				var comp = array.Get(i);
				batch[i].compId = comp.compId;
				batch[i].position = comp.position;
				batch[i].rotate = comp.rotate;
				batch[i].scale = comp.scale;
			}
			return batch;
		});

		RegisterAllocator<Transform, Transform.BatchData>((int count) => {
			return new Transform.BatchData[count];
		});


		/// MeshRenderer のバッチコンバータ登録
		RegisterConverter<MeshRenderer, MeshRenderer.BatchData>((ComponentArray<MeshRenderer> array) => {
			int count = array.Count;
			MeshRenderer.BatchData[] batch = new MeshRenderer.BatchData[count];
			for (int i = 0; i < count; i++) {
				var comp = array.Get(i);
				var batchData = comp.GetBatchData();
				batch[i].nativeHandle = comp.nativeHandle;
				batch[i].meshPath = batchData.meshPath;
				batch[i].color = batchData.color;
				batch[i].postEffectFlags = batchData.postEffectFlags;
			}

			return batch;
		});

		RegisterAllocator<MeshRenderer, MeshRenderer.BatchData>((int count) => {
			return new MeshRenderer.BatchData[count];
		});

	}




	// Converter を登録
	public static void RegisterConverter<TComponent, TData>(Func<ComponentArray<TComponent>, TData[]> converter)
		where TComponent : Component {
		converters[typeof(TComponent)] = (IComponentArray array) => {
			return converter((ComponentArray<TComponent>)array);
		};
	}

	public static void RegisterAllocator<TComponent, TData>(Func<int, TData[]> allocator)
		where TComponent : Component {
		allocators[typeof(TComponent)] = (int count) => {
			return allocator(count);
		};
	}


	// 一括送信
	public static void SendAllBatches(ComponentCollection _collection, string _ecsGroupName) {
		Debug.LogInfo("ComponentBatchManager.SendAllBatches: Start sending all batches.");
		Debug.LogInfo($"ComponentBatchManager.SendAllBatches: Total converters registered: {converters.Count}.");

		foreach (var kv in converters) {
			if (!_collection.TryGetArray(kv.Key, out IComponentArray array)) {
				Debug.LogWarning($"ComponentBatchManager.SendAllBatches: ComponentArray for {kv.Key} not found.");
				continue;
			}

			Debug.LogInfo($"ComponentBatchManager.SendAllBatches: Sending batch for {kv.Key}.");
			Array batch = kv.Value(array);
			InternalSetBatch(kv.Key, batch, batch.Length, _ecsGroupName);
		}
	}

	// 一括受信
	public static void ReceiveAllBatches(ComponentCollection _collection, string _ecsGroupName) {
		foreach (var kv in allocators) {
			if (!_collection.TryGetArray(kv.Key, out IComponentArray array)) {
				Debug.LogWarning($"ComponentBatchManager.ReceiveAllBatches: ComponentArray for {kv.Key} not found.");
				continue;
			}

			int count = array.Count;
			if (count == 0) {
				Debug.LogWarning($"ComponentBatchManager.ReceiveAllBatches: No components to receive for {kv.Key}.");
				continue;
			}

			Array batch = kv.Value(count);

			Debug.LogInfo($"ComponentBatchManager.ReceiveAllBatches: Receiving batch for {kv.Key} with count {count}.");
			InternalGetBatch(kv.Key, batch, count, _ecsGroupName);

			ApplyBatch(kv.Key, batch, array);
		}
	}


	static public void ApplyBatch(Type _componentType, Array _batch, IComponentArray _array) {
		if (_componentType == typeof(Transform)) {
			var array = (ComponentArray<Transform>)_array;
			var batch = (Transform.BatchData[])_batch;
			for (int i = 0; i < batch.Length; i++) {
				var comp = array.Get(i);
				comp.compId = batch[i].compId;
				comp.position = batch[i].position;
				comp.rotate = batch[i].rotate;
				comp.scale = batch[i].scale;
			}
		}

		if (_componentType == typeof(MeshRenderer)) {
			var array = (ComponentArray<MeshRenderer>)_array;
			var batch = (MeshRenderer.BatchData[])_batch;
			for (int i = 0; i < batch.Length; i++) {
				var comp = array.Get(i);
				comp.nativeHandle = batch[i].nativeHandle;
				comp.meshPath = batch[i].meshPath;
				comp.color = batch[i].color;
				comp.postEffectFlags = batch[i].postEffectFlags;
			}
		}
	}


	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalSetBatch(Type _componentType, Array _batch, int _count, string _ecsGroupName);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalGetBatch(Type _componentType, Array batch_, int _count, string _ecsGroupName);
}

