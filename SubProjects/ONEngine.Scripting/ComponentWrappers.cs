using System;
using ONEngine.Scripting.Generated;

namespace ONEngine.Scripting
{
    public abstract class Component
    {
        public Entity Entity { get; internal set; }
    }

    public unsafe class Transform : Component
    {
        private readonly Generated.Transform* _ptr;

        internal Transform(Generated.Transform* ptr)
        {
            _ptr = ptr;
        }

        public Math.Vector3 Position
        {
            get => _ptr->position;
            set => _ptr->position = value;
        }

        public Math.Vector3 Rotation
        {
            get => _ptr->rotation;
            set => _ptr->rotation = value;
        }

        public Math.Vector3 Scale
        {
            get => _ptr->scale;
            set => _ptr->scale = value;
        }
    }

    public unsafe class MeshRenderer : Component
    {
        private readonly Generated.MeshRenderer* _ptr;

        internal MeshRenderer(Generated.MeshRenderer* ptr)
        {
            _ptr = ptr;
        }

        public uint ModelIndex
        {
            get => _ptr->modelIndex;
            set => _ptr->modelIndex = value;
        }

        public uint MaterialIndex
        {
            get => _ptr->materialIndex;
            set => _ptr->materialIndex = value;
        }
    }
}
