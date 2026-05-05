using System;
using Engine.Generated;

namespace Engine.Core
{
    public static class PrefabLoader
    {
        public static ulong Load(EcsWorld world, string path)
        {
            if (!System.IO.File.Exists(path))
            {
                Console.WriteLine($"[Loader] Error: Prefab file not found: {path}");
                return 0;
            }

            Console.WriteLine($"[Loader] Loading prefab from {path}...");
            string jsonString = System.IO.File.ReadAllText(path);
            using var doc = System.Text.Json.JsonDocument.Parse(jsonString);
            return LoadFromElement(world, doc.RootElement);
        }

        public static ulong LoadFromElement(EcsWorld world, System.Text.Json.JsonElement root)
        {
            ulong entityId = world.CreateEntity();

            if (root.TryGetProperty("components", out var components))
            {
                foreach (var component in components.EnumerateObject())
                {
                    string typeName = component.Name;
                    ComponentRegistry.AddComponentFromJson(world, entityId, typeName, component.Value);
                }
            }

            // スクリプトの自動アタッチ
            if (root.TryGetProperty("scripts", out var scripts))
            {
                foreach (var scriptEntry in scripts.EnumerateArray())
                {
                    string scriptName = scriptEntry.GetString() ?? "";
                    ScriptRegistry.AddScriptByName(world, entityId, scriptName);
                }
            }

            return entityId;
        }
    }
}
