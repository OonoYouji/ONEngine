using System;
using Engine.Generated;

namespace Engine.Core
{
    /// <summary>
    /// ユーザーが定義するスクリプトの基底クラス。
    /// </summary>
    public abstract class GameScript
    {
        public ulong EntityId { get; internal set; }
        public EcsWorld World { get; internal set; } = null!;

        // Unity互換プロパティ
        public LocalTransform transform {
            get => World.GetComponent<LocalTransform>(EntityId);
            set => World.AddComponent(EntityId, value);
        }

        public T GetComponent<T>() where T : unmanaged => World.GetComponent<T>(EntityId);

        public virtual void Initialize() { }
        public virtual void Update() { }
    }
}
