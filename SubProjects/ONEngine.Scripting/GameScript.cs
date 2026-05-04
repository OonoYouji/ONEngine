using System;

namespace ONEngine.Scripting
{
    public abstract class GameScript : Component
    {
        public virtual void OnEnable() { }
        public virtual void Start() { }
        public virtual void Update(float deltaTime) { }
        public virtual void OnDisable() { }

        // Short-hands
        public Transform transform => Entity.transform;
    }
}
