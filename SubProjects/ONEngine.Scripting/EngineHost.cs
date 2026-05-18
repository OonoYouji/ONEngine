using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Reflection;
using System.Text.Json;
using ONEngine.Scripting.Generated;

namespace ONEngine.Scripting
{
    public unsafe static class EngineHost
    {
        private static EcsWorld? _world;
        private static Dictionary<uint, GCHandle> _scriptHandles = new();

        static EngineHost()
        {
            // 静的コンストラクタでリゾルバーを登録
            // ONEngine.Native という抽象名に対し、現在のプロセス（EXE）のハンドルを割り当てる
            NativeLibrary.SetDllImportResolver(typeof(EngineHost).Assembly, (libraryName, assembly, searchPath) => {
                if (libraryName == "ONEngine.Native" || libraryName == "ONEngine.exe") {
                    return NativeLibrary.GetMainProgramHandle();
                }
                return IntPtr.Zero;
            });
        }

        [UnmanagedCallersOnly]
        public static void Initialize(IntPtr logHandler, IntPtr registryPtr)
        {
            try
            {
                Debug.SetLogHandler(logHandler);
                _world = new EcsWorld(registryPtr);
                Debug.Log("[C#] EngineHost: Initialized (Dynamic Native Resolver).");
            }
            catch (Exception e)
            {
                Console.WriteLine($"[C#] FATAL Error in Initialize: {e}");
            }
        }

        private static uint _frameCount = 0;

        [UnmanagedCallersOnly]
        public static void Update()
        {
            float deltaTime = 1.0f / 60.0f; // Temporary fixed DT for debugging
            if (_world == null) return;

            _frameCount++;
            if (_frameCount % 60 == 0)
            {
                Debug.Log($"[C#] EngineHost.Update: Ticking {_scriptHandles.Count} scripts.");
            }

            try
            {
                // スクリプトの実行: dictionary を回す方が安全 (gcHandle の整合性が保証される)
                // 実行中に _scriptHandles が変更される可能性に備え、一旦リスト化する
                var activeEntities = new List<uint>(_scriptHandles.Keys);
                
                foreach (var entityId in activeEntities)
                {
                    if (_scriptHandles.TryGetValue(entityId, out var handle))
                    {
                        try
                        {
                            if (handle.IsAllocated && handle.Target is GameScript script)
                            {
                                // コンポーネントが有効な場合のみ実行
                                if (_world.HasComponent<ScriptComponent>(entityId))
                                {
                                    ref var comp = ref _world.GetComponent<ScriptComponent>(entityId);
                                    if (comp.isEnabled != 0)
                                    {
                                        script.Update(deltaTime);
                                    }
                                }
                            }
                        }
                        catch (Exception e)
                        {
                            Debug.Log($"[C#] Error in Script Instance Update (Entity:{entityId}): {e.Message}");
                        }
                    }
                }

                CleanupDestroyedEntities();
            }
            catch (Exception e)
            {
                // UnmanagedCallersOnly メソッドから例外を漏らしてはならない
                Console.WriteLine($"[C#] FATAL Error in EngineHost.Update: {e}");
            }
        }

        private static void CleanupDestroyedEntities()
        {
            if (_world == null) return;
            
            // 破棄されたエンティティを検出してハンドルを解放
            var toRemove = new List<uint>();
            foreach (var id in _scriptHandles.Keys)
            {
                if (!_world.HasComponent<ScriptComponent>(id)) toRemove.Add(id);
            }
            foreach (var id in toRemove)
            {
                if (_scriptHandles[id].IsAllocated) _scriptHandles[id].Free();
                _scriptHandles.Remove(id);
            }
        }

        [UnmanagedCallersOnly]
        public static void GetScriptNameByEntity(uint entityId, IntPtr buffer, uint bufferSize)
        {
            if (_scriptHandles.TryGetValue(entityId, out var handle))
            {
                if (handle.IsAllocated && handle.Target != null)
                {
                    string name = handle.Target.GetType().Name;
                    byte[] bytes = System.Text.Encoding.UTF8.GetBytes(name + "\0");
                    int length = System.Math.Min((int)bufferSize, bytes.Length);
                    Marshal.Copy(bytes, 0, buffer, length);
                    return;
                }
            }
            Marshal.WriteByte(buffer, 0);
        }

        [UnmanagedCallersOnly]
        public static void GetScriptFields(uint entityId, IntPtr buffer, uint bufferSize)
        {
            try
            {
                if (!_scriptHandles.TryGetValue(entityId, out var handle) || !handle.IsAllocated || handle.Target == null)
                {
                    Marshal.WriteByte(buffer, 0);
                    return;
                }

                var script = handle.Target;
                var type = script.GetType();
                var fields = type.GetFields(BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance);
                
                var resultList = new List<Dictionary<string, object>>();
                foreach (var f in fields)
                {
                    if (!f.IsPublic && f.GetCustomAttribute<SerializeField>() == null) continue;

                    var fieldInfo = new Dictionary<string, object>
                    {
                        ["name"] = f.Name,
                        ["type"] = f.FieldType.Name,
                        ["value"] = f.GetValue(script) ?? "null"
                    };
                    resultList.Add(fieldInfo);
                }

                string json = JsonSerializer.Serialize(resultList);
                byte[] bytes = System.Text.Encoding.UTF8.GetBytes(json + "\0");
                int writeLength = System.Math.Min((int)bufferSize, bytes.Length);
                Marshal.Copy(bytes, 0, buffer, writeLength);
            }
            catch (Exception e)
            {
                Debug.Log($"[C#] Error in GetScriptFields: {e}");
                Marshal.WriteByte(buffer, 0);
            }
        }

        public static string SerializeScript(uint entityId)
        {
            if (!_scriptHandles.TryGetValue(entityId, out var handle) || !handle.IsAllocated || handle.Target == null) return "{}";

            var script = handle.Target;
            var type = script.GetType();
            var fields = type.GetFields(BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance);
            
            var fieldData = new Dictionary<string, object>();
            foreach (var f in fields)
            {
                if (!f.IsPublic && f.GetCustomAttribute<SerializeField>() == null) continue;
                fieldData[f.Name] = f.GetValue(script) ?? "null";
            }

            var result = new Dictionary<string, object>
            {
                ["name"] = type.Name,
                ["variables"] = fieldData
            };

            return JsonSerializer.Serialize(result);
        }

        [UnmanagedCallersOnly]
        public static IntPtr SerializeScriptToNative(uint entityId)
        {
            string json = SerializeScript(entityId);
            if (string.IsNullOrEmpty(json) || json == "{}") return IntPtr.Zero;
            
            byte[] bytes = System.Text.Encoding.UTF8.GetBytes(json + "\0");
            IntPtr ptr = Marshal.AllocHGlobal(bytes.Length);
            Marshal.Copy(bytes, 0, ptr, bytes.Length);
            return ptr;
        }

        [UnmanagedCallersOnly]
        public static void SetScriptField(uint entityId, IntPtr namePtr, IntPtr valueJsonPtr)
        {
            if (!_scriptHandles.TryGetValue(entityId, out var handle) || !handle.IsAllocated || handle.Target == null) return;

            string fieldName = Marshal.PtrToStringAnsi(namePtr) ?? "";
            string valueJson = Marshal.PtrToStringAnsi(valueJsonPtr) ?? "";
            
            var script = handle.Target;
            var type = script.GetType();
            var field = type.GetField(fieldName, BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance);
            
            if (field != null)
            {
                try {
                    object? val = JsonSerializer.Deserialize(valueJson, field.FieldType);
                    field.SetValue(script, val);
                } catch (Exception e) {
                    Debug.Log($"[C#] Error setting field {fieldName}: {e.Message}");
                }
            }
        }

        [UnmanagedCallersOnly]
        public static void AddScriptByName(uint entityId, IntPtr namePtr, IntPtr varsJsonPtr)
        {
            if (_world == null) {
                Debug.Log("[C#] AddScriptByName: FAILED. World is null.");
                return;
            }

            try
            {
                string name = Marshal.PtrToStringAnsi(namePtr) ?? "";
                string varsJson = Marshal.PtrToStringAnsi(varsJsonPtr) ?? "{}";
                Debug.Log($"[C#] AddScriptByName called for Entity:{entityId}, Script:{name}");

                GameScript? script = null;
                
                var assembly = typeof(EngineHost).Assembly;
                foreach (var t in assembly.GetTypes())
                {
                    if (t.IsSubclassOf(typeof(GameScript)) && t.Name == name)
                    {
                        script = (GameScript?)Activator.CreateInstance(t);
                        break;
                    }
                }

                if (script == null)
                {
                    // Fallback to internal test scripts if not found dynamically
                    if (name == "InternalCubeRotator") script = new InternalCubeRotator();
                    else if (name == "InternalSpawner") script = new InternalSpawner();
                }
                
                if (script != null)
                {
                    Debug.Log($"[C#] Created instance of {script.GetType().FullName}.");
                    script.EntityId = entityId;
                    script.World = _world;
                    ApplyVariables(script, varsJson);
                    
                    if (_scriptHandles.TryGetValue(entityId, out var oldHandle))
                    {
                        Debug.Log($"[C#] Freeing old handle for Entity:{entityId}.");
                        if (oldHandle.IsAllocated) oldHandle.Free();
                    }

                    var handle = GCHandle.Alloc(script);
                    _scriptHandles[entityId] = handle;
                    
                    ulong gcHandlePtr = (ulong)GCHandle.ToIntPtr(handle);
                    Debug.Log($"[C#] Allocated new handle: {gcHandlePtr}. Adding ScriptComponent to ECS.");
                    _world.AddScriptComponent(entityId, gcHandlePtr, 0);
                    
                    script.Start();
                    Debug.Log($"[C#] Script {name} attached and Start() called.");
                }
                else
                {
                    Debug.Log($"[C#] Script type '{name}' not found in registry.");
                }
            }
            catch (Exception e)
            {
                Debug.Log($"[C#] Error in AddScriptByName: {e}");
            }
        }

        private static void ApplyVariables(GameScript script, string json)
        {
            try
            {
                using var doc = JsonDocument.Parse(json);
                var root = doc.RootElement;
                if (root.ValueKind != JsonValueKind.Object) return;

                var type = script.GetType();
                foreach (var prop in root.EnumerateObject())
                {
                    var field = type.GetField(prop.Name, BindingFlags.Public | BindingFlags.Instance | BindingFlags.IgnoreCase);
                    if (field != null)
                    {
                        object? val = null;
                        var fieldType = field.FieldType;
                        if (fieldType == typeof(float)) val = (float)prop.Value.GetDouble();
                        else if (fieldType == typeof(int)) val = prop.Value.GetInt32();
                        else if (fieldType == typeof(bool)) val = prop.Value.GetBoolean();
                        else if (fieldType == typeof(string)) val = prop.Value.GetString();
                        else if (fieldType == typeof(Math.Vector3))
                        {
                            val = new Math.Vector3(
                                (float)prop.Value.GetProperty("x").GetDouble(),
                                (float)prop.Value.GetProperty("y").GetDouble(),
                                (float)prop.Value.GetProperty("z").GetDouble()
                            );
                        }
                        if (val != null) field.SetValue(script, val);
                    }
                }
            }
            catch (Exception e) { Debug.Log($"[C#] Error applying variables: {e.Message}"); }
        }

        [UnmanagedCallersOnly]
        public static void Shutdown()
        {
            try
            {
                foreach (var handle in _scriptHandles.Values)
                {
                    if (handle.IsAllocated) handle.Free();
                }
                _scriptHandles.Clear();
                _world = null;
                Debug.Log("[C#] EngineHost: Shutdown.");
            }
            catch (Exception e)
            {
                Console.WriteLine($"[C#] Error in Shutdown: {e}");
            }
        }

        // --- Test Scripts ---
        internal class InternalCubeRotator : GameScript
        {
            public float Speed = 2.0f;
            public override void Update(float deltaTime)
            {
                ref var t = ref transformRef;
                t.rotation.y += Speed * deltaTime;
            }
        }

        internal class InternalSpawner : GameScript
        {
            private float _timer = 0.0f;
            private List<uint> _spawned = new List<uint>();
            public override void Update(float deltaTime)
            {
                _timer += deltaTime;
                if (_timer > 2.0f)
                {
                    uint e = World.CreateEntity();
                    World.AddTransform(e);
                    World.AddMeshRenderer(e);
                    
                    ref var mr = ref World.GetComponent<MeshRenderer>(e);
                    mr.modelIndex = 0; 
                    mr.materialIndex = 1;

                    ref var t = ref World.GetComponent<Transform>(e);
                    t.position = new Math.Vector3(0, 10, 0);
                    t.rotation = new Math.Vector3(0, 0, 0);
                    t.scale = new Math.Vector3(1, 1, 1);
                    _spawned.Add(e);
                    _timer = 0.0f;
                }
                for (int i = _spawned.Count - 1; i >= 0; i--)
                {
                    uint e = _spawned[i];
                    if (World.HasComponent<Transform>(e)) {
                        ref var t = ref World.GetComponent<Transform>(e);
                        t.position.y += 2.0f * deltaTime;
                        if (t.position.y > 20.0f) { World.DestroyEntity(e); _spawned.RemoveAt(i); }
                    } else { _spawned.RemoveAt(i); }
                }
            }
        }
    }
}
