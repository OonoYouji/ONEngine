using System;
using System.Collections.Generic;

namespace Engine.Core
{
    public static class ScriptRegistry
    {
        private static readonly Dictionary<string, Action<EcsWorld, ulong>> _creators = new();

        static ScriptRegistry()
        {
            // AOT互換のため、手動またはソース生成での登録を推奨
            // ここでは主要なスクリプトを手動登録
            Register<PlayerMoveScript>("PlayerMoveScript");
            Register<EnemyMoveScript>("EnemyMoveScript");
        }

        public static void Register<T>(string name) where T : GameScript, new()
        {
            _creators[name] = (world, entityId) => world.AddScript<T>(entityId);
        }

        public static void AddScriptByName(EcsWorld world, ulong entityId, string scriptName)
        {
            if (_creators.TryGetValue(scriptName, out var creator))
            {
                creator(world, entityId);
                Console.WriteLine($"[ScriptRegistry] Attached script '{scriptName}' to Entity {entityId}");
            }
            else
            {
                Console.WriteLine($"[ScriptRegistry] Error: Script type '{scriptName}' not found or not registered for AOT.");
            }
        }
    }
}
