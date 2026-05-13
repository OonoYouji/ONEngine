using System;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using ONEngine.Scripting.Math;
using ONEngine.Scripting.Generated;

namespace ONEngine.Scripting
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

        public uint GetDenseIndex(uint entityId)
        {
            uint pageIdx = entityId >> PAGE_SHIFT;
            if (pageIdx >= _pageCount) return 0xFFFFFFFF;
            uint* page = _pages[pageIdx];
            if (page == null) return 0xFFFFFFFF;
            return page[entityId & PAGE_MASK];
        }
    }

    public static class ComponentID<T> {
        public static readonly uint ID = ComponentID.GetInternal<T>();
    }

    public static class ComponentID {
        // FIXED: Use a generic name for the Native library
        private const string NATIVE_LIB = "ONEngine.Native";

        [DllImport(NATIVE_LIB)] private static extern uint Ecs_GetTypeId_Transform();
        [DllImport(NATIVE_LIB)] private static extern uint Ecs_GetTypeId_MeshRenderer();
        [DllImport(NATIVE_LIB)] private static extern uint Ecs_GetTypeId_ScriptComponent();

        public static uint Get<T>() => ComponentID<T>.ID;

        internal static uint GetInternal<T>() {
            if (typeof(T) == typeof(Transform)) return Ecs_GetTypeId_Transform();
            if (typeof(T) == typeof(MeshRenderer)) return Ecs_GetTypeId_MeshRenderer();
            if (typeof(T) == typeof(ScriptComponent)) return Ecs_GetTypeId_ScriptComponent();
            return 0;
        }
    }

    public unsafe class EcsWorld
    {
        private IntPtr _registry;
        private const string NATIVE_LIB = "ONEngine.Native";

        [DllImport(NATIVE_LIB)] private static extern void* ecs_get_sparse_pages(IntPtr registry, uint typeId, out uint pageCount);
        [DllImport(NATIVE_LIB)] private static extern void* ecs_get_chunk_ptr(IntPtr registry, uint typeId, uint chunkIndex);
        [DllImport(NATIVE_LIB)] private static extern uint ecs_get_chunk_count(IntPtr registry, uint typeId);
        [DllImport(NATIVE_LIB)] private static extern uint ecs_get_storage_size(IntPtr registry, uint typeId);
        [DllImport(NATIVE_LIB)] private static extern uint CreateEntity(IntPtr registry);
        [DllImport(NATIVE_LIB)] private static extern void DestroyEntity(IntPtr registry, uint entity);
        [DllImport(NATIVE_LIB)] private static extern void AddTransform(IntPtr registry, uint entity);
        [DllImport(NATIVE_LIB)] private static extern void AddMeshRenderer(IntPtr registry, uint entity);
        [DllImport(NATIVE_LIB)] private static extern void AddScriptComponent(IntPtr registry, uint entity, ulong gcHandle, uint typeId);

        public EcsWorld(IntPtr registry)
        {
            _registry = registry;
        }

        public uint CreateEntity() => CreateEntity(_registry);
        public void DestroyEntity(uint entityId) => DestroyEntity(_registry, entityId);
        public void AddTransform(uint entityId) => AddTransform(_registry, entityId);
        public void AddMeshRenderer(uint entityId) => AddMeshRenderer(_registry, entityId);
        public void AddScriptComponent(uint entityId, ulong gcHandle, uint typeId) => AddScriptComponent(_registry, entityId, gcHandle, typeId);

        public uint GetTypeId<T>() => ComponentID.Get<T>();

        public SparseSetLookup GetLookup<T>() where T : unmanaged
        {
            uint count;
            void** pages = (void**)ecs_get_sparse_pages(_registry, GetTypeId<T>(), out count);
            return new SparseSetLookup(pages, count);
        }

        public uint GetChunkCount<T>() where T : unmanaged => ecs_get_chunk_count(_registry, GetTypeId<T>());
        
        public Span<T> GetChunkSpan<T>(uint index) where T : unmanaged
        {
            void* ptr = ecs_get_chunk_ptr(_registry, GetTypeId<T>(), index);
            if (ptr == null) return Span<T>.Empty;
            uint total = ecs_get_storage_size(_registry, GetTypeId<T>());
            uint size = 1024;
            if ((index + 1) * 1024 > total) size = total % 1024;
            return new Span<T>(ptr, (int)size);
        }

        public ref T GetComponent<T>(uint entityId) where T : unmanaged
        {
            var lookup = GetLookup<T>();
            uint denseIdx = lookup.GetDenseIndex(entityId);
            if (denseIdx == 0xFFFFFFFF) throw new Exception($"Component {typeof(T).Name} not found for Entity {entityId}");

            uint chunkIdx = denseIdx / 1024;
            uint offset = denseIdx % 1024;
            void* ptr = ecs_get_chunk_ptr(_registry, GetTypeId<T>(), chunkIdx);
            return ref ((T*)ptr)[offset];
        }

        public bool HasComponent<T>(uint entityId) where T : unmanaged
        {
            var lookup = GetLookup<T>();
            return lookup.GetDenseIndex(entityId) != 0xFFFFFFFF;
        }
    }
}
