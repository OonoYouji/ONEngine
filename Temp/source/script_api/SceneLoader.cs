using System;
using System.Collections.Generic;
using System.IO;
using System.Text.Json;
using Engine.Generated;

namespace Engine.Core
{
    public static class SceneLoader
    {
        public static void Load(EcsWorld world, string scenePath)
        {
            if (!File.Exists(scenePath))
            {
                Console.WriteLine($"[SceneLoader] Error: Scene file not found: {scenePath}");
                return;
            }

            Console.WriteLine($"[SceneLoader] Loading scene: {scenePath}");
            string jsonString = File.ReadAllText(scenePath);
            using var doc = JsonDocument.Parse(jsonString);
            var root = doc.RootElement;

            if (root.TryGetProperty("entities", out var entities))
            {
                foreach (var entityEntry in entities.EnumerateArray())
                {
                    ulong entityId;

                    // 1. 外部プリファブの参照がある場合
                    if (entityEntry.TryGetProperty("prefab", out var prefabProp))
                    {
                        string prefabPath = prefabProp.GetString() ?? "";
                        entityId = PrefabLoader.Load(world, prefabPath);

                        // 全コンポーネントの差分上書き (オーバーライド)
                        if (entityEntry.TryGetProperty("components", out var componentsProp))
                        {
                            foreach (var compProp in componentsProp.EnumerateObject())
                            {
                                Console.WriteLine($"[SceneLoader] Overriding component: {compProp.Name} for Entity {entityId}");
                                ComponentRegistry.UpdateComponentFromJson(world, entityId, compProp.Name, compProp.Value);
                            }
                        }
                    }
                    // 2. インラインでエンティティが定義されている場合
                    else
                    {
                        entityId = PrefabLoader.LoadFromElement(world, entityEntry);
                        Console.WriteLine($"[SceneLoader] Loaded inline entity.");
                    }
                }
            }
            
            world.SubmitCommands();
            Console.WriteLine($"[SceneLoader] Scene '{scenePath}' loaded successfully.");
        }
    }
}
