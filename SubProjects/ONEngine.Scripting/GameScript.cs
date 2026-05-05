using System;
using ONEngine.Scripting.Generated;

namespace ONEngine.Scripting
{
    public abstract class GameScript
    {
        public uint EntityId { get; internal set; }
        public EcsWorld World { get; internal set; } = null!;

        // ./Temp互換: コピーを返すプロパティ
        public Transform transform {
            get => World.GetComponent<Transform>(EntityId);
            set => World.GetComponent<Transform>(EntityId) = value;
        }

        // 高速なアクセス用 (ref)
        public ref Transform transformRef => ref World.GetComponent<Transform>(EntityId);

        public T GetComponent<T>() where T : unmanaged => World.GetComponent<T>(EntityId);
        public ref T GetComponentRef<T>() where T : unmanaged => ref World.GetComponent<T>(EntityId);

        public virtual void Start() { }
        public virtual void Update(float deltaTime) { }
        public virtual void OnEnable() { }
        public virtual void OnDisable() { }
    }
}
