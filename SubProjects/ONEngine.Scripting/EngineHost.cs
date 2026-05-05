using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using ONEngine.Scripting.Generated;

namespace ONEngine.Scripting
{
    public unsafe static class EngineHost
    {
        private static IntPtr _registryPtr;
        private static Dictionary<uint, Entity> _entityCache = new Dictionary<uint, Entity>();
        private static List<GameScript> _activeScripts = new List<GameScript>();

        public static IntPtr RegistryPtr => _registryPtr;

        [UnmanagedCallersOnly]
        public static void Initialize(IntPtr logHandler, IntPtr registryPtr)
        {
            _registryPtr = registryPtr;
            Debug.SetLogHandler(logHandler);
            
            Entity.RegistryPtr = registryPtr;
            Entity.TransformView = new ComponentView<Generated.Transform>(registryPtr, ECS.GetTransformChunk);
            Entity.MeshRendererView = new ComponentView<Generated.MeshRenderer>(registryPtr, ECS.GetMeshRendererChunk);

            Debug.Log("[C#] EngineHost: Initialized.");
        }

        [UnmanagedCallersOnly]
        public static void Update(float deltaTime)
        {
            SyncEntities();

            // Update all active scripts
            for (int i = _activeScripts.Count - 1; i >= 0; i--)
            {
                try
                {
                    _activeScripts[i].Update(deltaTime);
                }
                catch (Exception e)
                {
                    Debug.Log($"[C#] Error updating script on Entity {_activeScripts[i].Entity.Id}: {e.Message}");
                }
            }
        }

        [UnmanagedCallersOnly]
        public static void AddScriptByName(uint entityId, IntPtr namePtr)
        {
            string name = Marshal.PtrToStringAnsi(namePtr);
            uint entityCount = ECS.GetEntityCount(_registryPtr);
            
            // Find the entity in the cache (it should be there if SyncEntities was called)
            if (_entityCache.TryGetValue(entityId, out var entity))
            {
                if (name == "InternalCubeRotator") AddScript(new InternalCubeRotator { Entity = entity });
                else if (name == "InternalSpawner") AddScript(new InternalSpawner { Entity = entity });
                // TODO: Dynamic instantiation using Reflection
                else Debug.Log($"[C#] Script {name} not found.");
            }
        }

        private static void SyncEntities()
        {
            uint entityCount = ECS.GetEntityCount(_registryPtr);
            
            _entityCache.Clear();

            for (uint i = 0; i < entityCount; i++)
            {
                uint id = ECS.GetEntityId(_registryPtr, i);
                var entity = new Entity(i, id, _registryPtr);
                _entityCache[id] = entity;
            }
        }

        public static void AddScript(GameScript script)
        {
            _activeScripts.Add(script);
            script.Start();
        }

        [UnmanagedCallersOnly]
        public static void Shutdown()
        {
            foreach (var script in _activeScripts)
            {
                script.OnDisable();
            }
            _activeScripts.Clear();
            _entityCache.Clear();
            Debug.Log("[C#] EngineHost: Shutdown.");
        }

        // --- Internal Classes for Testing ---
        // These should eventually move to a separate user assembly.

        internal class InternalCubeRotator : GameScript
        {
            public float Speed = 2.0f;
            public override void Update(float deltaTime)
            {
                var rot = transform.Rotation;
                rot.y += Speed * deltaTime;
                transform.Rotation = rot;
            }
        }

        internal class InternalSpawner : GameScript
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
                    Debug.Log($"[C#] Spawned Entity ID: {e.Id}");
                    _timer = 0.0f;
                }

                for (int i = _spawned.Count - 1; i >= 0; i--)
                {
                    var e = _spawned[i];
                    var pos = e.transform.Position;
                    pos.y += 2.0f * deltaTime;
                    e.transform.Position = pos;

                    if (pos.y > 20.0f)
                    {
                        e.Destroy();
                        _spawned.RemoveAt(i);
                    }
                }
            }
        }
    }
}
