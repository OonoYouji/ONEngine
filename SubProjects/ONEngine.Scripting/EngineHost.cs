using System;
using System.Runtime.InteropServices;
using ONEngine.Scripting.Generated;

namespace ONEngine.Scripting
{
    public unsafe static class EngineHost
    {
        private static IntPtr _registryPtr;
        private static Dictionary<uint, Entity> _entityCache = new Dictionary<uint, Entity>();
        private static List<GameScript> _scripts = new List<GameScript>();

        [UnmanagedCallersOnly]
        public static void Initialize(IntPtr logHandler, IntPtr registryPtr)
        {
            _registryPtr = registryPtr;
            Debug.SetLogHandler(logHandler);
            
            Entity.RegistryPtr = registryPtr;
            Entity.TransformView = new ComponentView<Generated.Transform>(registryPtr, ECS.GetTransformChunk);
            Entity.MeshRendererView = new ComponentView<Generated.MeshRenderer>(registryPtr, ECS.GetMeshRendererChunk);

            Debug.Log("[C#] EngineHost initialized with Entity Lifecycle support.");
        }

        private static bool _firstUpdate = true;

        [UnmanagedCallersOnly]
        public static void Update(float deltaTime)
        {
            uint entityCount = ECS.GetEntityCount(_registryPtr);

            // Sync: map current storage indices to Entity objects
            var currentEntities = new List<Entity>((int)entityCount);
            for (uint i = 0; i < entityCount; i++)
            {
                uint id = ECS.GetEntityId(_registryPtr, i);
                if (!_entityCache.TryGetValue(id, out var entity))
                {
                    entity = new Entity(i, id, _registryPtr);
                    _entityCache[id] = entity;

                    // Temporary logic: Add Rotator to initial cubes
                    if (id <= 20) {
                        _scripts.Add(new CubeRotator { Entity = entity });
                    }
                }
                // Update index in case storage shifted (though chunks are stable, sparse set might swap on delete)
                // Actually our Storage::Remove swaps with last, so indices DO change.
                // We need to re-sync every frame if we want safe pointers.
                currentEntities.Add(entity);
            }

            // Detect one spawner to manage lifecycle
            if (_firstUpdate) {
                var spawner = new Spawner { Entity = currentEntities[0] };
                _scripts.Add(spawner);
                _firstUpdate = false;
            }

            // Update all active scripts
            for (int i = _scripts.Count - 1; i >= 0; i--)
            {
                _scripts[i].Update(deltaTime);
            }
        }

        [UnmanagedCallersOnly]
        public static void Shutdown()
        {
            Debug.Log("[C#] EngineHost shutdown.");
        }
    }

    public class CubeRotator : GameScript
    {
        public float Speed = 2.0f;
        public override void Update(float deltaTime)
        {
            var rot = transform.Rotation;
            rot.y += Speed * deltaTime;
            transform.Rotation = rot;
        }
    }

    public class Spawner : GameScript
    {
        private float _timer = 0.0f;
        private List<Entity> _spawned = new List<Entity>();

        public override void Update(float deltaTime)
        {
            _timer += deltaTime;
            if (_timer > 2.0f)
            {
                var e = Entity.Create();
                e.AddComponent<Transform>();
                var mr = e.AddComponent<MeshRenderer>();
                mr.ModelIndex = 0;
                mr.MaterialIndex = 1; // White

                e.transform.Position = new Math.Vector3(0, 10, 0);
                e.transform.Scale = new Math.Vector3(1, 1, 1);

                _spawned.Add(e);
                Debug.Log($"[C#] Spawned Entity ID: {e.Id}. Total spawned: {_spawned.Count}");
                _timer = 0.0f;
            }

            // Move spawned objects up and destroy them if too high
            for (int i = _spawned.Count - 1; i >= 0; i--)
            {
                var e = _spawned[i];
                var pos = e.transform.Position;
                pos.y += 2.0f * deltaTime;
                e.transform.Position = pos;

                if (pos.y > 20.0f)
                {
                    Debug.Log($"[C#] Destroying Entity ID: {e.Id}");
                    e.Destroy();
                    _spawned.RemoveAt(i);
                }
            }
        }
    }
}
