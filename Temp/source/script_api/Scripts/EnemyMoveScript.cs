using System;
using Engine.Core;
using Engine.Generated;

namespace Engine.Core
{
    public class EnemyMoveScript : GameScript
    {
        public override void Update()
        {
            // Queryシステムを使用して、LocalTransformを持つ全エンティティを走査
            int count = 0;
            foreach (var otherEntityId in World.Query<LocalTransform>())
            {
                if (otherEntityId == EntityId) continue; // 自分自身はスキップ

                var tr = World.GetComponent<LocalTransform>(otherEntityId);
                // Console.WriteLine($"[Script:Enemy] I see Entity {otherEntityId} at {tr.position}");
                count++;
            }

            Console.WriteLine($"[Script:Enemy] Entity {EntityId} is patrolling... (Detected {count} other entities)");
        }

    }
}
