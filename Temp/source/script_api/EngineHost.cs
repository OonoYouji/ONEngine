using System;
using System.Runtime.InteropServices;
using System.Diagnostics;
using Engine.Core;
using Engine.Generated;

namespace Engine.Core
{
    public static class EngineHost
    {
        private static EcsWorld? _world;

        [UnmanagedCallersOnly(EntryPoint = "engine_init")]
        public static void Initialize()
        {
            // デバッガ待機 (手動アタッチ用)
            // デバッグしたい時は引数に --debug を入れるか、以下の if を true にしてください
            if (Environment.CommandLine.Contains("--debug")) 
            {
                WaitForDebugger();
            }

            _world = new EcsWorld();
            
            // Sceneをロード
            string scenePath = "data/main_scene.json";
            SceneLoader.Load(_world, scenePath);

            Console.WriteLine("[C#] Script Engine Initialized.");
        }

        private static void WaitForDebugger()
        {
            Console.WriteLine("[C#] Waiting for debugger to attach...");
            while (!Debugger.IsAttached)
            {
                System.Threading.Thread.Sleep(100);
            }
            Console.WriteLine("[C#] Debugger attached!");
        }

        private static int _frameCount = 0;

        [UnmanagedCallersOnly(EntryPoint = "engine_update")]
        public static void Update()
        {
            if (_world == null) return;
            
            _frameCount++;

            // スクリプトの実行
            uint chunkCount = _world.GetChunkCount<ScriptComponent>();
            for (uint i = 0; i < chunkCount; i++)
            {
                var chunk = _world.GetChunkSpan<ScriptComponent>(i);
                foreach (var comp in chunk)
                {
                    var handle = GCHandle.FromIntPtr((IntPtr)comp.gcHandle);
                    var script = (GameScript)handle.Target!;
                    
                    // ログが多すぎないように調整
                    if (_frameCount % 10 == 0) {
                        script.Update();
                    }
                }
            }
            
            _world.SubmitCommands();
            _world.Cleanup();
        }
        
        [UnmanagedCallersOnly(EntryPoint = "engine_shutdown")]
        public static void Shutdown()
        {
            _world?.Dispose();
            _world = null;
            Console.WriteLine("[C#] Script Engine Shutdown.");
        }
    }
}
