using System;
using System.Runtime.InteropServices;
using ONEngine.Scripting.Math;
using ONEngine.Scripting.Generated;

namespace ONEngine.Scripting
{
    public unsafe static class ECS
    {
        [DllImport("ONEngine.exe")]
        public static extern void* GetTransformChunk(IntPtr registry, uint chunkIndex);

        [DllImport("ONEngine.exe")]
        public static extern uint GetEntityCount(IntPtr registry);
    }

    public unsafe delegate void* GetChunkDelegate(IntPtr registry, uint chunkIndex);

    public unsafe class ComponentView<T> where T : unmanaged
    {
        private IntPtr _registry;
        private GetChunkDelegate _getChunkFunc;

        public ComponentView(IntPtr registry, GetChunkDelegate getChunkFunc)
        {
            _registry = registry;
            _getChunkFunc = getChunkFunc;
        }

        public T* GetPointer(uint index)
        {
            uint chunkIdx = index / 1024;
            uint localIdx = index % 1024;
            void* chunkPtr = _getChunkFunc(_registry, chunkIdx);
            if (chunkPtr == null) return null;
            return &((T*)chunkPtr)[localIdx];
        }
    }
}
