using System;
using System.Runtime.InteropServices;
using ONEngine.Scripting.Generated;

namespace ONEngine.Scripting
{
    public unsafe static class EngineHost
    {
        private static IntPtr _registryPtr;
        private static ComponentView<Transform> _transformView;

        [UnmanagedCallersOnly]
        public static void Initialize(IntPtr logHandler, IntPtr registryPtr)
        {
            _registryPtr = registryPtr;
            Debug.SetLogHandler(logHandler);
            
            _transformView = new ComponentView<Transform>(registryPtr, ECS.GetTransformChunk);

            Debug.Log("[C#] EngineHost initialized with Zero-Copy ECS.");
        }

        private static float _logTimer = 0.0f;

        [UnmanagedCallersOnly]
        public static void Update(float deltaTime)
        {
            uint entityCount = ECS.GetEntityCount(_registryPtr);
            
            _logTimer += deltaTime;
            if (_logTimer > 2.0f)
            {
                Debug.Log($"[C#] Update called. DeltaTime: {deltaTime}, Entities: {entityCount}");
                _logTimer = 0.0f;
            }

            for (uint i = 0; i < entityCount; i++)
            {
                Transform* transform = _transformView.GetPointer(i);
                if (transform != null)
                {
                    // Rotate on Y axis from C#!
                    transform->rotation.y += 2.0f * deltaTime;
                }
            }
        }

        [UnmanagedCallersOnly]
        public static void Shutdown()
        {
            Debug.Log("[C#] EngineHost shutdown.");
        }
    }
}
