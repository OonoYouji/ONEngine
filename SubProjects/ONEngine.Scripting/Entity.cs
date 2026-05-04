using System;
using System.Collections.Generic;

namespace ONEngine.Scripting
{
    public class Entity
    {
        public uint Index; // 0-based index in the ECS storage
        public readonly uint Id;    // The actual Entity ID in the Registry
        private readonly IntPtr _registry;

        // Static registry pointer managed by EngineHost
        internal static IntPtr RegistryPtr;

        // Static views managed by EngineHost
        internal static ComponentView<Generated.Transform> TransformView;
        internal static ComponentView<Generated.MeshRenderer> MeshRendererView;

        public Entity(uint index, uint id, IntPtr registry)
        {
            Index = index;
            Id = id;
            _registry = registry;
        }

        public static Entity Create()
        {
            uint id = ECS.CreateEntity(RegistryPtr);
            return new Entity(0, id, RegistryPtr);
        }

        public void Destroy()
        {
            ECS.DestroyEntity(_registry, Id);
        }

        public T AddComponent<T>() where T : Component
        {
            if (typeof(T) == typeof(Transform)) { ECS.AddTransform(_registry, Id); return transform as T; }
            if (typeof(T) == typeof(MeshRenderer)) { ECS.AddMeshRenderer(_registry, Id); return MeshRenderer as T; }
            return null;
        }

        private Transform _transform;
        public unsafe Transform transform
        {
            get
            {
                if (_transform == null)
                {
                    var ptr = TransformView.GetPointer(Index);
                    if (ptr != null) _transform = new Transform(ptr) { Entity = this };
                }
                return _transform;
            }
        }

        private MeshRenderer _meshRenderer;
        public unsafe MeshRenderer MeshRenderer
        {
            get
            {
                if (_meshRenderer == null)
                {
                    var ptr = MeshRendererView.GetPointer(Index);
                    if (ptr != null) _meshRenderer = new MeshRenderer(ptr) { Entity = this };
                }
                return _meshRenderer;
            }
        }

        public T GetComponent<T>() where T : Component
        {
            if (typeof(T) == typeof(Transform)) return transform as T;
            if (typeof(T) == typeof(MeshRenderer)) return MeshRenderer as T;
            return null;
        }
    }
}
