using System;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using Engine.Generated;

namespace Engine.Core
{
    public unsafe struct SparseSetLookup
    {
        private uint** _pages;
        private uint _pageCount;
        private const uint PAGE_SIZE = 4096;
        private const uint PAGE_MASK = 4095;
        private const int PAGE_SHIFT = 12;

        public SparseSetLookup(void** pages, uint pageCount)
        {
            _pages = (uint**)pages;
            _pageCount = pageCount;
        }

        public uint GetDenseIndex(ulong entityId)
        {
            uint index = (uint)(entityId & 0xFFFFFFFF);
            uint pageIdx = index >> PAGE_SHIFT;
            if (pageIdx >= _pageCount) return 0xFFFFFFFF;
            uint* page = _pages[pageIdx];
            if (page == null) return 0xFFFFFFFF;
            return page[index & PAGE_MASK];
        }
    }

    public unsafe class EcsWorld : IDisposable
    {
        private IntPtr _nativeWorld;
        private const string DLL_NAME = "Temp.exe";
        private Dictionary<ulong, GCHandle> _scriptHandles = new();

        #region P/Invoke
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr ecs_world_create();
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)]
        private static extern void ecs_world_destroy(IntPtr world);
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)]
        private static extern ulong ecs_create_entity(IntPtr world);
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)]
        private static extern void ecs_destroy_entity(IntPtr world, ulong entityId);
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)]
        private static extern void ecs_submit_commands(IntPtr world);
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)]
        private static extern void ecs_add_component(IntPtr world, ulong entityId, uint typeId, void* data);
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)]
        private static extern void* ecs_get_sparse_pages(IntPtr world, uint typeId, out uint pageCount);
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)]
        private static extern void* ecs_get_chunk_ptr(IntPtr world, uint typeId, uint chunkIndex);
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)]
        private static extern uint ecs_get_chunk_count(IntPtr world, uint typeId);
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)]
        private static extern uint ecs_get_storage_size(IntPtr world, uint typeId);
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)]
        private static extern ulong* ecs_get_entities_ptr(IntPtr world, uint typeId, out uint count);
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)]
        private static extern void ecs_register_destruct_hook(IntPtr world, uint typeId);
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)]
        private static extern ulong* ecs_get_destroyed_entities(IntPtr world, out uint count);
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)]
        private static extern void ecs_clear_destroyed_entities(IntPtr world);
        #endregion

        public EcsWorld()
        {
            _nativeWorld = ecs_world_create();
            ecs_register_destruct_hook(_nativeWorld, ComponentID.Get<ScriptComponent>());
        }

        public void Dispose()
        {
            // 残っているすべてのハンドルを解放
            foreach (var handle in _scriptHandles.Values)
            {
                if (handle.IsAllocated) handle.Free();
            }
            _scriptHandles.Clear();

            if (_nativeWorld != IntPtr.Zero)
            {
                ecs_world_destroy(_nativeWorld);
                _nativeWorld = IntPtr.Zero;
            }
        }

        public ulong CreateEntity() => ecs_create_entity(_nativeWorld);
        public void DestroyEntity(ulong entityId) => ecs_destroy_entity(_nativeWorld, entityId);
        public void SubmitCommands() => ecs_submit_commands(_nativeWorld);

        public void AddComponent<T>(ulong entityId, T component) where T : unmanaged
        {
            ecs_add_component(_nativeWorld, entityId, GetTypeId<T>(), &component);
        }

        // スクリプト専用の追加メソッド
        public void AddScript<T>(ulong entityId) where T : GameScript, new()
        {
            // 既存のスクリプトがあれば解放 (上書き対応)
            if (_scriptHandles.TryGetValue(entityId, out var oldHandle))
            {
                if (oldHandle.IsAllocated) oldHandle.Free();
            }

            var script = new T { EntityId = entityId, World = this };
            script.Initialize();
            var handle = GCHandle.Alloc(script);
            _scriptHandles[entityId] = handle;

            var comp = new ScriptComponent {
                gcHandle = (ulong)GCHandle.ToIntPtr(handle),
                typeId = 0 // 今後はリフレクション等で型IDを振ることも可能
            };
            AddComponent(entityId, comp);
        }

        public uint GetTypeId<T>() => ComponentID.Get<T>();

        public SparseSetLookup GetLookup<T>() where T : unmanaged
        {
            void** pages = (void**)ecs_get_sparse_pages(_nativeWorld, GetTypeId<T>(), out uint count);
            return new SparseSetLookup(pages, count);
        }

        public uint GetChunkCount<T>() where T : unmanaged => ecs_get_chunk_count(_nativeWorld, GetTypeId<T>());
        public Span<T> GetChunkSpan<T>(uint index) where T : unmanaged
        {
            void* ptr = ecs_get_chunk_ptr(_nativeWorld, GetTypeId<T>(), index);
            uint total = ecs_get_storage_size(_nativeWorld, GetTypeId<T>());
            uint size = 1024;
            if ((index + 1) * 1024 > total) size = total % 1024;
            return new Span<T>(ptr, (int)size);
        }

        public T GetComponent<T>(ulong entityId) where T : unmanaged
        {
            var lookup = GetLookup<T>();
            uint denseIdx = lookup.GetDenseIndex(entityId);
            if (denseIdx == 0xFFFFFFFF) throw new Exception($"Component {typeof(T).Name} not found for Entity {entityId}");

            uint chunkIdx = denseIdx / 1024;
            uint offset = denseIdx % 1024;
            return GetChunkSpan<T>(chunkIdx)[(int)offset];
        }

        public bool HasComponent<T>(ulong entityId) where T : unmanaged
        {
            var lookup = GetLookup<T>();
            return lookup.GetDenseIndex(entityId) != 0xFFFFFFFF;
        }

        #region Queries
        // 1つのコンポーネントを持つエンティティをすべて列挙
        public IEnumerable<ulong> Query<T>() where T : unmanaged
        {
            var results = new List<ulong>();
            uint typeId = GetTypeId<T>();
            uint count;
            ulong* entities = ecs_get_entities_ptr(_nativeWorld, typeId, out count);
            
            for (uint i = 0; i < count; i++)
            {
                results.Add(entities[i]);
            }
            return results;
        }

        // 2つのコンポーネントを両方持つエンティティを列挙 (高速なフィルタリング)
        public IEnumerable<ulong> Query<T1, T2>() 
            where T1 : unmanaged 
            where T2 : unmanaged
        {
            var results = new List<ulong>();
            uint count1;
            ulong* entities1 = ecs_get_entities_ptr(_nativeWorld, GetTypeId<T1>(), out count1);
            var lookup2 = GetLookup<T2>();

            for (uint i = 0; i < count1; i++)
            {
                ulong entityId = entities1[i];
                if (lookup2.GetDenseIndex(entityId) != 0xFFFFFFFF)
                {
                    results.Add(entityId);
                }
            }
            return results;
        }

        // 3つのコンポーネントをすべて持つエンティティを列挙
        public IEnumerable<ulong> Query<T1, T2, T3>() 
            where T1 : unmanaged 
            where T2 : unmanaged
            where T3 : unmanaged
        {
            var results = new List<ulong>();
            uint count1;
            ulong* entities1 = ecs_get_entities_ptr(_nativeWorld, GetTypeId<T1>(), out count1);
            var lookup2 = GetLookup<T2>();
            var lookup3 = GetLookup<T3>();

            for (uint i = 0; i < count1; i++)
            {
                ulong entityId = entities1[i];
                if (lookup2.GetDenseIndex(entityId) != 0xFFFFFFFF &&
                    lookup3.GetDenseIndex(entityId) != 0xFFFFFFFF)
                {
                    results.Add(entityId);
                }
            }
            return results;
        }
        #endregion

        public void Cleanup()
        {
            uint count;
            ulong* ptr = ecs_get_destroyed_entities(_nativeWorld, out count);
            if (count > 0)
            {
                for (uint i = 0; i < count; i++)
                {
                    ulong entityId = ptr[i];
                    if (_scriptHandles.Remove(entityId, out var handle))
                    {
                        if (handle.IsAllocated)
                        {
                            Console.WriteLine($"[Cleanup] Freeing GCHandle for Entity {entityId}");
                            handle.Free();
                        }
                    }
                }
                ecs_clear_destroyed_entities(_nativeWorld);
            }
        }
    }
}
