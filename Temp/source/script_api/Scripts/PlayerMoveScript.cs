using System;
using Engine.Core;
using Engine.Generated;

namespace Engine.Core
{
    public class PlayerMoveScript : GameScript
    {
        public override void Update()
        {
            // Unityスタイルのアクセス (LocalTransform)
            var tr = transform;
            tr.position.x += 0.01f; 
            transform = tr;

            // ImGuiによるデバッグ表示
            ImGui.Begin("Player Debug");
            ImGui.Text($"Entity ID: {EntityId}");
            ImGui.Text($"Position: {tr.position}");
            ImGui.SliderFloat("Move Speed", ref _speed, 0.1f, 2.0f);
            if (ImGui.Button("Reset Position"))
            {
                tr.position = new Vector3(0, 0, 0);
                transform = tr;
                Console.WriteLine("[Script:Player] Reset position via Unity-style API!");
            }
            ImGui.End();

            // プレイヤー固有のロジック
            Console.WriteLine($"[Unity API Test] Entity {EntityId} Pos: {tr.position}");
        }
        
        private float _speed = 0.5f;
    }
}
