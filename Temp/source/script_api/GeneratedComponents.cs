using System;
using System.Runtime.InteropServices;
using Engine.Core;

namespace Engine.Generated {
    // ユーザー（C#）が操作するローカル座標。
    [StructLayout(LayoutKind.Explicit, Size = 48)]
    public struct LocalTransform {
        [FieldOffset(0)]
        public Vector3 position;
        [FieldOffset(16)]
        public Vector4 rotation;
        [FieldOffset(32)]
        public Vector3 scale;
    }

    // C++システムが計算し、GPUや物理エンジンに渡す絶対座標行列。
    [StructLayout(LayoutKind.Explicit, Size = 64)]
    public struct WorldMatrix {
        [FieldOffset(0)]
        public Matrix4x4 matrix;
    }

    // 階層構造の構築。
    [StructLayout(LayoutKind.Explicit, Size = 8)]
    public struct Parent {
        [FieldOffset(0)]
        public uint parentId;
    }

    // UIのアンカー、ピボット、ローカル配置データ。
    [StructLayout(LayoutKind.Explicit, Size = 40)]
    public struct LocalRect {
        [FieldOffset(0)]
        public Vector2 anchorMin;
        [FieldOffset(8)]
        public Vector2 anchorMax;
        [FieldOffset(16)]
        public Vector2 pivot;
        [FieldOffset(24)]
        public Vector2 anchoredPosition;
        [FieldOffset(32)]
        public Vector2 sizeDelta;
    }

    // Canvasの解像度計算を加味した最終的なUI描画用行列。
    [StructLayout(LayoutKind.Explicit, Size = 64)]
    public struct CanvasWorldMatrix {
        [FieldOffset(0)]
        public Matrix4x4 matrix;
    }

    // 毎フレームの物理更新・移動処理に使われる動的データ。
    [StructLayout(LayoutKind.Explicit, Size = 24)]
    public struct Velocity {
        [FieldOffset(0)]
        public Vector3 linear;
        [FieldOffset(12)]
        public Vector3 angular;
    }

    // 質量や重力設定などの静的パラメータ。
    [StructLayout(LayoutKind.Explicit, Size = 24)]
    public struct PhysicsProperties {
        [FieldOffset(0)]
        public float inverseMass;
        [FieldOffset(4)]
        public Vector3 inverseInertia;
        [FieldOffset(16)]
        public uint useGravity;
        [FieldOffset(20)]
        public uint isKinematic;
    }

    // 立方体形状データ。
    [StructLayout(LayoutKind.Explicit, Size = 24)]
    public struct BoxGeometry {
        [FieldOffset(0)]
        public Vector3 center;
        [FieldOffset(12)]
        public Vector3 extents;
    }

    // 球形状データ。
    [StructLayout(LayoutKind.Explicit, Size = 16)]
    public struct SphereGeometry {
        [FieldOffset(0)]
        public Vector3 center;
        [FieldOffset(12)]
        public float radius;
    }

    // カプセル形状データ。
    [StructLayout(LayoutKind.Explicit, Size = 24)]
    public struct CapsuleGeometry {
        [FieldOffset(0)]
        public Vector3 center;
        [FieldOffset(12)]
        public float radius;
        [FieldOffset(16)]
        public float height;
        [FieldOffset(20)]
        public int direction;
    }

    // 衝突レイヤーとマスク設定。
    [StructLayout(LayoutKind.Explicit, Size = 16)]
    public struct ColliderFilter {
        [FieldOffset(0)]
        public uint layer;
        [FieldOffset(4)]
        public uint collisionMask;
        [FieldOffset(8)]
        public uint isTrigger;
    }

    // カメラの投影設定。
    [StructLayout(LayoutKind.Explicit, Size = 16)]
    public struct CameraData {
        [FieldOffset(0)]
        public float fov;
        [FieldOffset(4)]
        public float nearClip;
        [FieldOffset(8)]
        public float farClip;
        [FieldOffset(12)]
        public uint cullingMask;
    }

    // 光源設定。
    [StructLayout(LayoutKind.Explicit, Size = 32)]
    public struct LightData {
        [FieldOffset(0)]
        public int type;
        [FieldOffset(4)]
        public Vector3 color;
        [FieldOffset(16)]
        public float intensity;
        [FieldOffset(20)]
        public float range;
        [FieldOffset(24)]
        public float spotAngle;
    }

    // 描画するメッシュとマテリアルのハンドル。
    [StructLayout(LayoutKind.Explicit, Size = 48)]
    public struct RenderMesh {
        [FieldOffset(0)]
        public Engine.Core.Guid meshId;
        [FieldOffset(16)]
        public Engine.Core.Guid materialId;
        [FieldOffset(32)]
        public uint renderLayer;
        [FieldOffset(36)]
        public uint isVisible;
    }

    // 2Dスプライトの描画情報。
    [StructLayout(LayoutKind.Explicit, Size = 64)]
    public struct SpriteRenderData {
        [FieldOffset(0)]
        public Engine.Core.Guid spriteId;
        [FieldOffset(16)]
        public Engine.Core.Guid materialId;
        [FieldOffset(32)]
        public Vector4 color;
        [FieldOffset(48)]
        public uint flipX;
        [FieldOffset(52)]
        public uint flipY;
    }

    // 音源の設定パラメータ。
    [StructLayout(LayoutKind.Explicit, Size = 32)]
    public struct AudioSourceData {
        [FieldOffset(0)]
        public Engine.Core.Guid clipId;
        [FieldOffset(16)]
        public float volume;
        [FieldOffset(20)]
        public float pitch;
        [FieldOffset(24)]
        public float spatialBlend;
        [FieldOffset(28)]
        public uint loop;
    }

    // 現在の再生状況。
    [StructLayout(LayoutKind.Explicit, Size = 16)]
    public struct AudioPlayState {
        [FieldOffset(0)]
        public uint isPlaying;
        [FieldOffset(4)]
        public float time;
        [FieldOffset(8)]
        public uint triggerPlay;
    }

    // マイクとなるエンティティを示すタグ。
    [StructLayout(LayoutKind.Explicit, Size = 0)]
    public struct AudioListenerTag {
    }

    // UIのルート設定。
    [StructLayout(LayoutKind.Explicit, Size = 8)]
    public struct CanvasData {
        [FieldOffset(0)]
        public int renderMode;
        [FieldOffset(4)]
        public float scaleFactor;
    }

    // UI画像の描画情報。
    [StructLayout(LayoutKind.Explicit, Size = 48)]
    public struct UIImageData {
        [FieldOffset(0)]
        public Engine.Core.Guid spriteId;
        [FieldOffset(16)]
        public Vector4 color;
        [FieldOffset(32)]
        public Engine.Core.Guid materialId;
    }

    // テキストの描画情報。
    [StructLayout(LayoutKind.Explicit, Size = 48)]
    public struct UITextData {
        [FieldOffset(0)]
        public Engine.Core.Guid fontId;
        [FieldOffset(16)]
        public uint textHashId;
        [FieldOffset(20)]
        public float fontSize;
        [FieldOffset(32)]
        public Vector4 color;
    }

    // ボタンのインタラクション状態。
    [StructLayout(LayoutKind.Explicit, Size = 16)]
    public struct UIButtonState {
        [FieldOffset(0)]
        public uint isHovered;
        [FieldOffset(4)]
        public uint isPressed;
        [FieldOffset(8)]
        public uint interactable;
    }

    // アニメーションコントローラーの現在状態。
    [StructLayout(LayoutKind.Explicit, Size = 32)]
    public struct AnimatorState {
        [FieldOffset(0)]
        public Engine.Core.Guid controllerId;
        [FieldOffset(16)]
        public uint currentClipHash;
        [FieldOffset(20)]
        public float normalizedTime;
        [FieldOffset(24)]
        public float speed;
    }

    // エージェントの静的パラメータ。
    [StructLayout(LayoutKind.Explicit, Size = 16)]
    public struct NavAgentParams {
        [FieldOffset(0)]
        public float radius;
        [FieldOffset(4)]
        public float height;
        [FieldOffset(8)]
        public float maxSpeed;
        [FieldOffset(12)]
        public int avoidancePriority;
    }

    // 移動の目標と現在の状況。
    [StructLayout(LayoutKind.Explicit, Size = 32)]
    public struct NavAgentState {
        [FieldOffset(0)]
        public Vector3 destination;
        [FieldOffset(12)]
        public Vector3 currentVelocity;
        [FieldOffset(24)]
        public uint hasPath;
        [FieldOffset(28)]
        public float remainingDistance;
    }

    // C#のGameScriptインスタンスへの参照を保持。
    [StructLayout(LayoutKind.Explicit, Size = 16)]
    public struct ScriptComponent {
        [FieldOffset(0)]
        public int typeId;
        [FieldOffset(8)]
        public ulong gcHandle;
    }

    // マルチシーン実行時のフィルタリング用ビットマスク。
    [StructLayout(LayoutKind.Explicit, Size = 8)]
    public struct SceneMask {
        [FieldOffset(0)]
        public uint mask;
    }

    public static class EcsExtensions {
        public static void AddLocalTransform(this EcsWorld world, ulong entityId, Vector3 position, Vector4 rotation, Vector3 scale) {
            world.AddComponent(entityId, new LocalTransform { position = position, rotation = rotation, scale = scale });
        }
        public static void AddWorldMatrix(this EcsWorld world, ulong entityId, Matrix4x4 matrix) {
            world.AddComponent(entityId, new WorldMatrix { matrix = matrix });
        }
        public static void AddParent(this EcsWorld world, ulong entityId, uint parentId) {
            world.AddComponent(entityId, new Parent { parentId = parentId });
        }
        public static void AddLocalRect(this EcsWorld world, ulong entityId, Vector2 anchorMin, Vector2 anchorMax, Vector2 pivot, Vector2 anchoredPosition, Vector2 sizeDelta) {
            world.AddComponent(entityId, new LocalRect { anchorMin = anchorMin, anchorMax = anchorMax, pivot = pivot, anchoredPosition = anchoredPosition, sizeDelta = sizeDelta });
        }
        public static void AddCanvasWorldMatrix(this EcsWorld world, ulong entityId, Matrix4x4 matrix) {
            world.AddComponent(entityId, new CanvasWorldMatrix { matrix = matrix });
        }
        public static void AddVelocity(this EcsWorld world, ulong entityId, Vector3 linear, Vector3 angular) {
            world.AddComponent(entityId, new Velocity { linear = linear, angular = angular });
        }
        public static void AddPhysicsProperties(this EcsWorld world, ulong entityId, float inverseMass, Vector3 inverseInertia, uint useGravity, uint isKinematic) {
            world.AddComponent(entityId, new PhysicsProperties { inverseMass = inverseMass, inverseInertia = inverseInertia, useGravity = useGravity, isKinematic = isKinematic });
        }
        public static void AddBoxGeometry(this EcsWorld world, ulong entityId, Vector3 center, Vector3 extents) {
            world.AddComponent(entityId, new BoxGeometry { center = center, extents = extents });
        }
        public static void AddSphereGeometry(this EcsWorld world, ulong entityId, Vector3 center, float radius) {
            world.AddComponent(entityId, new SphereGeometry { center = center, radius = radius });
        }
        public static void AddCapsuleGeometry(this EcsWorld world, ulong entityId, Vector3 center, float radius, float height, int direction) {
            world.AddComponent(entityId, new CapsuleGeometry { center = center, radius = radius, height = height, direction = direction });
        }
        public static void AddColliderFilter(this EcsWorld world, ulong entityId, uint layer, uint collisionMask, uint isTrigger) {
            world.AddComponent(entityId, new ColliderFilter { layer = layer, collisionMask = collisionMask, isTrigger = isTrigger });
        }
        public static void AddCameraData(this EcsWorld world, ulong entityId, float fov, float nearClip, float farClip, uint cullingMask) {
            world.AddComponent(entityId, new CameraData { fov = fov, nearClip = nearClip, farClip = farClip, cullingMask = cullingMask });
        }
        public static void AddLightData(this EcsWorld world, ulong entityId, int type, Vector3 color, float intensity, float range, float spotAngle) {
            world.AddComponent(entityId, new LightData { type = type, color = color, intensity = intensity, range = range, spotAngle = spotAngle });
        }
        public static void AddRenderMesh(this EcsWorld world, ulong entityId, Engine.Core.Guid meshId, Engine.Core.Guid materialId, uint renderLayer, uint isVisible) {
            world.AddComponent(entityId, new RenderMesh { meshId = meshId, materialId = materialId, renderLayer = renderLayer, isVisible = isVisible });
        }
        public static void AddSpriteRenderData(this EcsWorld world, ulong entityId, Engine.Core.Guid spriteId, Engine.Core.Guid materialId, Vector4 color, uint flipX, uint flipY) {
            world.AddComponent(entityId, new SpriteRenderData { spriteId = spriteId, materialId = materialId, color = color, flipX = flipX, flipY = flipY });
        }
        public static void AddAudioSourceData(this EcsWorld world, ulong entityId, Engine.Core.Guid clipId, float volume, float pitch, float spatialBlend, uint loop) {
            world.AddComponent(entityId, new AudioSourceData { clipId = clipId, volume = volume, pitch = pitch, spatialBlend = spatialBlend, loop = loop });
        }
        public static void AddAudioPlayState(this EcsWorld world, ulong entityId, uint isPlaying, float time, uint triggerPlay) {
            world.AddComponent(entityId, new AudioPlayState { isPlaying = isPlaying, time = time, triggerPlay = triggerPlay });
        }
        public static void AddAudioListenerTag(this EcsWorld world, ulong entityId) {
            world.AddComponent(entityId, new AudioListenerTag {  });
        }
        public static void AddCanvasData(this EcsWorld world, ulong entityId, int renderMode, float scaleFactor) {
            world.AddComponent(entityId, new CanvasData { renderMode = renderMode, scaleFactor = scaleFactor });
        }
        public static void AddUIImageData(this EcsWorld world, ulong entityId, Engine.Core.Guid spriteId, Vector4 color, Engine.Core.Guid materialId) {
            world.AddComponent(entityId, new UIImageData { spriteId = spriteId, color = color, materialId = materialId });
        }
        public static void AddUITextData(this EcsWorld world, ulong entityId, Engine.Core.Guid fontId, uint textHashId, float fontSize, Vector4 color) {
            world.AddComponent(entityId, new UITextData { fontId = fontId, textHashId = textHashId, fontSize = fontSize, color = color });
        }
        public static void AddUIButtonState(this EcsWorld world, ulong entityId, uint isHovered, uint isPressed, uint interactable) {
            world.AddComponent(entityId, new UIButtonState { isHovered = isHovered, isPressed = isPressed, interactable = interactable });
        }
        public static void AddAnimatorState(this EcsWorld world, ulong entityId, Engine.Core.Guid controllerId, uint currentClipHash, float normalizedTime, float speed) {
            world.AddComponent(entityId, new AnimatorState { controllerId = controllerId, currentClipHash = currentClipHash, normalizedTime = normalizedTime, speed = speed });
        }
        public static void AddNavAgentParams(this EcsWorld world, ulong entityId, float radius, float height, float maxSpeed, int avoidancePriority) {
            world.AddComponent(entityId, new NavAgentParams { radius = radius, height = height, maxSpeed = maxSpeed, avoidancePriority = avoidancePriority });
        }
        public static void AddNavAgentState(this EcsWorld world, ulong entityId, Vector3 destination, Vector3 currentVelocity, uint hasPath, float remainingDistance) {
            world.AddComponent(entityId, new NavAgentState { destination = destination, currentVelocity = currentVelocity, hasPath = hasPath, remainingDistance = remainingDistance });
        }
        public static void AddScriptComponent(this EcsWorld world, ulong entityId, int typeId, ulong gcHandle) {
            world.AddComponent(entityId, new ScriptComponent { typeId = typeId, gcHandle = gcHandle });
        }
        public static void AddSceneMask(this EcsWorld world, ulong entityId, uint mask) {
            world.AddComponent(entityId, new SceneMask { mask = mask });
        }
    }

    public static class ComponentRegistry {
        public static void AddComponentFromJson(EcsWorld world, ulong entityId, string typeName, System.Text.Json.JsonElement data) {
            UpdateComponentFromJson(world, entityId, typeName, data);
        }

        public static void UpdateComponentFromJson(EcsWorld world, ulong entityId, string typeName, System.Text.Json.JsonElement data) {
            switch (typeName) {
                case "LocalTransform": {
                    var comp = world.HasComponent<LocalTransform>(entityId) ? world.GetComponent<LocalTransform>(entityId) : new LocalTransform();
                    if (data.TryGetProperty("position", out var prop_position)) {
                        comp.position = new Vector3(prop_position[0].GetSingle(), prop_position[1].GetSingle(), prop_position[2].GetSingle());
                    } else if (!world.HasComponent<LocalTransform>(entityId)) {
                        comp.position = new Vector3(0.0f, 0.0f, 0.0f);
                    }
                    if (data.TryGetProperty("rotation", out var prop_rotation)) {
                        comp.rotation = new Vector4(prop_rotation[0].GetSingle(), prop_rotation[1].GetSingle(), prop_rotation[2].GetSingle(), prop_rotation[3].GetSingle());
                    } else if (!world.HasComponent<LocalTransform>(entityId)) {
                        comp.rotation = new Vector4(0.0f, 0.0f, 0.0f, 1.0f);
                    }
                    if (data.TryGetProperty("scale", out var prop_scale)) {
                        comp.scale = new Vector3(prop_scale[0].GetSingle(), prop_scale[1].GetSingle(), prop_scale[2].GetSingle());
                    } else if (!world.HasComponent<LocalTransform>(entityId)) {
                        comp.scale = new Vector3(1.0f, 1.0f, 1.0f);
                    }
                    world.AddComponent(entityId, comp);
                    break;
                }
                case "WorldMatrix": {
                    var comp = world.HasComponent<WorldMatrix>(entityId) ? world.GetComponent<WorldMatrix>(entityId) : new WorldMatrix();
                    if (data.TryGetProperty("matrix", out var prop_matrix)) {
                        float[] m = new float[16]; for(int i=0; i<16; i++) m[i] = prop_matrix[i].GetSingle();
                        comp.matrix = new Matrix4x4(m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7], m[8], m[9], m[10], m[11], m[12], m[13], m[14], m[15]);
                    } else if (!world.HasComponent<WorldMatrix>(entityId)) {
                        comp.matrix = Matrix4x4.Identity;
                    }
                    world.AddComponent(entityId, comp);
                    break;
                }
                case "Parent": {
                    var comp = world.HasComponent<Parent>(entityId) ? world.GetComponent<Parent>(entityId) : new Parent();
                    if (data.TryGetProperty("parentId", out var prop_parentId)) {
                        comp.parentId = prop_parentId.GetUInt32();
                    } else if (!world.HasComponent<Parent>(entityId)) {
                        comp.parentId = 0;
                    }
                    world.AddComponent(entityId, comp);
                    break;
                }
                case "LocalRect": {
                    var comp = world.HasComponent<LocalRect>(entityId) ? world.GetComponent<LocalRect>(entityId) : new LocalRect();
                    if (data.TryGetProperty("anchorMin", out var prop_anchorMin)) {
                        comp.anchorMin = new Vector2(prop_anchorMin[0].GetSingle(), prop_anchorMin[1].GetSingle());
                    } else if (!world.HasComponent<LocalRect>(entityId)) {
                        comp.anchorMin = new Vector2(0.5f, 0.5f);
                    }
                    if (data.TryGetProperty("anchorMax", out var prop_anchorMax)) {
                        comp.anchorMax = new Vector2(prop_anchorMax[0].GetSingle(), prop_anchorMax[1].GetSingle());
                    } else if (!world.HasComponent<LocalRect>(entityId)) {
                        comp.anchorMax = new Vector2(0.5f, 0.5f);
                    }
                    if (data.TryGetProperty("pivot", out var prop_pivot)) {
                        comp.pivot = new Vector2(prop_pivot[0].GetSingle(), prop_pivot[1].GetSingle());
                    } else if (!world.HasComponent<LocalRect>(entityId)) {
                        comp.pivot = new Vector2(0.5f, 0.5f);
                    }
                    if (data.TryGetProperty("anchoredPosition", out var prop_anchoredPosition)) {
                        comp.anchoredPosition = new Vector2(prop_anchoredPosition[0].GetSingle(), prop_anchoredPosition[1].GetSingle());
                    } else if (!world.HasComponent<LocalRect>(entityId)) {
                        comp.anchoredPosition = new Vector2(0.0f, 0.0f);
                    }
                    if (data.TryGetProperty("sizeDelta", out var prop_sizeDelta)) {
                        comp.sizeDelta = new Vector2(prop_sizeDelta[0].GetSingle(), prop_sizeDelta[1].GetSingle());
                    } else if (!world.HasComponent<LocalRect>(entityId)) {
                        comp.sizeDelta = new Vector2(100.0f, 100.0f);
                    }
                    world.AddComponent(entityId, comp);
                    break;
                }
                case "CanvasWorldMatrix": {
                    var comp = world.HasComponent<CanvasWorldMatrix>(entityId) ? world.GetComponent<CanvasWorldMatrix>(entityId) : new CanvasWorldMatrix();
                    if (data.TryGetProperty("matrix", out var prop_matrix)) {
                        float[] m = new float[16]; for(int i=0; i<16; i++) m[i] = prop_matrix[i].GetSingle();
                        comp.matrix = new Matrix4x4(m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7], m[8], m[9], m[10], m[11], m[12], m[13], m[14], m[15]);
                    } else if (!world.HasComponent<CanvasWorldMatrix>(entityId)) {
                        comp.matrix = Matrix4x4.Identity;
                    }
                    world.AddComponent(entityId, comp);
                    break;
                }
                case "Velocity": {
                    var comp = world.HasComponent<Velocity>(entityId) ? world.GetComponent<Velocity>(entityId) : new Velocity();
                    if (data.TryGetProperty("linear", out var prop_linear)) {
                        comp.linear = new Vector3(prop_linear[0].GetSingle(), prop_linear[1].GetSingle(), prop_linear[2].GetSingle());
                    } else if (!world.HasComponent<Velocity>(entityId)) {
                        comp.linear = new Vector3(0.0f, 0.0f, 0.0f);
                    }
                    if (data.TryGetProperty("angular", out var prop_angular)) {
                        comp.angular = new Vector3(prop_angular[0].GetSingle(), prop_angular[1].GetSingle(), prop_angular[2].GetSingle());
                    } else if (!world.HasComponent<Velocity>(entityId)) {
                        comp.angular = new Vector3(0.0f, 0.0f, 0.0f);
                    }
                    world.AddComponent(entityId, comp);
                    break;
                }
                case "PhysicsProperties": {
                    var comp = world.HasComponent<PhysicsProperties>(entityId) ? world.GetComponent<PhysicsProperties>(entityId) : new PhysicsProperties();
                    if (data.TryGetProperty("inverseMass", out var prop_inverseMass)) {
                        comp.inverseMass = prop_inverseMass.GetSingle();
                    } else if (!world.HasComponent<PhysicsProperties>(entityId)) {
                        comp.inverseMass = 1.0f;
                    }
                    if (data.TryGetProperty("inverseInertia", out var prop_inverseInertia)) {
                        comp.inverseInertia = new Vector3(prop_inverseInertia[0].GetSingle(), prop_inverseInertia[1].GetSingle(), prop_inverseInertia[2].GetSingle());
                    } else if (!world.HasComponent<PhysicsProperties>(entityId)) {
                        comp.inverseInertia = new Vector3(1.0f, 1.0f, 1.0f);
                    }
                    if (data.TryGetProperty("useGravity", out var prop_useGravity)) {
                        comp.useGravity = prop_useGravity.GetBoolean() ? 1u : 0u;
                    } else if (!world.HasComponent<PhysicsProperties>(entityId)) {
                        comp.useGravity = 1u;
                    }
                    if (data.TryGetProperty("isKinematic", out var prop_isKinematic)) {
                        comp.isKinematic = prop_isKinematic.GetBoolean() ? 1u : 0u;
                    } else if (!world.HasComponent<PhysicsProperties>(entityId)) {
                        comp.isKinematic = 0u;
                    }
                    world.AddComponent(entityId, comp);
                    break;
                }
                case "BoxGeometry": {
                    var comp = world.HasComponent<BoxGeometry>(entityId) ? world.GetComponent<BoxGeometry>(entityId) : new BoxGeometry();
                    if (data.TryGetProperty("center", out var prop_center)) {
                        comp.center = new Vector3(prop_center[0].GetSingle(), prop_center[1].GetSingle(), prop_center[2].GetSingle());
                    } else if (!world.HasComponent<BoxGeometry>(entityId)) {
                        comp.center = new Vector3(0.0f, 0.0f, 0.0f);
                    }
                    if (data.TryGetProperty("extents", out var prop_extents)) {
                        comp.extents = new Vector3(prop_extents[0].GetSingle(), prop_extents[1].GetSingle(), prop_extents[2].GetSingle());
                    } else if (!world.HasComponent<BoxGeometry>(entityId)) {
                        comp.extents = new Vector3(0.5f, 0.5f, 0.5f);
                    }
                    world.AddComponent(entityId, comp);
                    break;
                }
                case "SphereGeometry": {
                    var comp = world.HasComponent<SphereGeometry>(entityId) ? world.GetComponent<SphereGeometry>(entityId) : new SphereGeometry();
                    if (data.TryGetProperty("center", out var prop_center)) {
                        comp.center = new Vector3(prop_center[0].GetSingle(), prop_center[1].GetSingle(), prop_center[2].GetSingle());
                    } else if (!world.HasComponent<SphereGeometry>(entityId)) {
                        comp.center = new Vector3(0.0f, 0.0f, 0.0f);
                    }
                    if (data.TryGetProperty("radius", out var prop_radius)) {
                        comp.radius = prop_radius.GetSingle();
                    } else if (!world.HasComponent<SphereGeometry>(entityId)) {
                        comp.radius = 0.5f;
                    }
                    world.AddComponent(entityId, comp);
                    break;
                }
                case "CapsuleGeometry": {
                    var comp = world.HasComponent<CapsuleGeometry>(entityId) ? world.GetComponent<CapsuleGeometry>(entityId) : new CapsuleGeometry();
                    if (data.TryGetProperty("center", out var prop_center)) {
                        comp.center = new Vector3(prop_center[0].GetSingle(), prop_center[1].GetSingle(), prop_center[2].GetSingle());
                    } else if (!world.HasComponent<CapsuleGeometry>(entityId)) {
                        comp.center = new Vector3(0.0f, 0.0f, 0.0f);
                    }
                    if (data.TryGetProperty("radius", out var prop_radius)) {
                        comp.radius = prop_radius.GetSingle();
                    } else if (!world.HasComponent<CapsuleGeometry>(entityId)) {
                        comp.radius = 0.5f;
                    }
                    if (data.TryGetProperty("height", out var prop_height)) {
                        comp.height = prop_height.GetSingle();
                    } else if (!world.HasComponent<CapsuleGeometry>(entityId)) {
                        comp.height = 2.0f;
                    }
                    if (data.TryGetProperty("direction", out var prop_direction)) {
                        comp.direction = prop_direction.GetInt32();
                    } else if (!world.HasComponent<CapsuleGeometry>(entityId)) {
                        comp.direction = 1;
                    }
                    world.AddComponent(entityId, comp);
                    break;
                }
                case "ColliderFilter": {
                    var comp = world.HasComponent<ColliderFilter>(entityId) ? world.GetComponent<ColliderFilter>(entityId) : new ColliderFilter();
                    if (data.TryGetProperty("layer", out var prop_layer)) {
                        comp.layer = prop_layer.GetUInt32();
                    } else if (!world.HasComponent<ColliderFilter>(entityId)) {
                        comp.layer = 1;
                    }
                    if (data.TryGetProperty("collisionMask", out var prop_collisionMask)) {
                        comp.collisionMask = prop_collisionMask.GetUInt32();
                    } else if (!world.HasComponent<ColliderFilter>(entityId)) {
                        comp.collisionMask = 4294967295;
                    }
                    if (data.TryGetProperty("isTrigger", out var prop_isTrigger)) {
                        comp.isTrigger = prop_isTrigger.GetBoolean() ? 1u : 0u;
                    } else if (!world.HasComponent<ColliderFilter>(entityId)) {
                        comp.isTrigger = 0u;
                    }
                    world.AddComponent(entityId, comp);
                    break;
                }
                case "CameraData": {
                    var comp = world.HasComponent<CameraData>(entityId) ? world.GetComponent<CameraData>(entityId) : new CameraData();
                    if (data.TryGetProperty("fov", out var prop_fov)) {
                        comp.fov = prop_fov.GetSingle();
                    } else if (!world.HasComponent<CameraData>(entityId)) {
                        comp.fov = 60.0f;
                    }
                    if (data.TryGetProperty("nearClip", out var prop_nearClip)) {
                        comp.nearClip = prop_nearClip.GetSingle();
                    } else if (!world.HasComponent<CameraData>(entityId)) {
                        comp.nearClip = 0.1f;
                    }
                    if (data.TryGetProperty("farClip", out var prop_farClip)) {
                        comp.farClip = prop_farClip.GetSingle();
                    } else if (!world.HasComponent<CameraData>(entityId)) {
                        comp.farClip = 1000.0f;
                    }
                    if (data.TryGetProperty("cullingMask", out var prop_cullingMask)) {
                        comp.cullingMask = prop_cullingMask.GetUInt32();
                    } else if (!world.HasComponent<CameraData>(entityId)) {
                        comp.cullingMask = 4294967295;
                    }
                    world.AddComponent(entityId, comp);
                    break;
                }
                case "LightData": {
                    var comp = world.HasComponent<LightData>(entityId) ? world.GetComponent<LightData>(entityId) : new LightData();
                    if (data.TryGetProperty("type", out var prop_type)) {
                        comp.type = prop_type.GetInt32();
                    } else if (!world.HasComponent<LightData>(entityId)) {
                        comp.type = 0;
                    }
                    if (data.TryGetProperty("color", out var prop_color)) {
                        comp.color = new Vector3(prop_color[0].GetSingle(), prop_color[1].GetSingle(), prop_color[2].GetSingle());
                    } else if (!world.HasComponent<LightData>(entityId)) {
                        comp.color = new Vector3(1.0f, 1.0f, 1.0f);
                    }
                    if (data.TryGetProperty("intensity", out var prop_intensity)) {
                        comp.intensity = prop_intensity.GetSingle();
                    } else if (!world.HasComponent<LightData>(entityId)) {
                        comp.intensity = 1.0f;
                    }
                    if (data.TryGetProperty("range", out var prop_range)) {
                        comp.range = prop_range.GetSingle();
                    } else if (!world.HasComponent<LightData>(entityId)) {
                        comp.range = 10.0f;
                    }
                    if (data.TryGetProperty("spotAngle", out var prop_spotAngle)) {
                        comp.spotAngle = prop_spotAngle.GetSingle();
                    } else if (!world.HasComponent<LightData>(entityId)) {
                        comp.spotAngle = 30.0f;
                    }
                    world.AddComponent(entityId, comp);
                    break;
                }
                case "RenderMesh": {
                    var comp = world.HasComponent<RenderMesh>(entityId) ? world.GetComponent<RenderMesh>(entityId) : new RenderMesh();
                    if (data.TryGetProperty("meshId", out var prop_meshId)) {
                        string s_meshId = prop_meshId.GetString() ?? "";
                        comp.meshId = System.Guid.TryParse(s_meshId, out var g_meshId) ? (Engine.Core.Guid)g_meshId : new Engine.Core.Guid();
                    } else if (!world.HasComponent<RenderMesh>(entityId)) {
                        comp.meshId = new Engine.Core.Guid();
                    }
                    if (data.TryGetProperty("materialId", out var prop_materialId)) {
                        string s_materialId = prop_materialId.GetString() ?? "";
                        comp.materialId = System.Guid.TryParse(s_materialId, out var g_materialId) ? (Engine.Core.Guid)g_materialId : new Engine.Core.Guid();
                    } else if (!world.HasComponent<RenderMesh>(entityId)) {
                        comp.materialId = new Engine.Core.Guid();
                    }
                    if (data.TryGetProperty("renderLayer", out var prop_renderLayer)) {
                        comp.renderLayer = prop_renderLayer.GetUInt32();
                    } else if (!world.HasComponent<RenderMesh>(entityId)) {
                        comp.renderLayer = 0;
                    }
                    if (data.TryGetProperty("isVisible", out var prop_isVisible)) {
                        comp.isVisible = prop_isVisible.GetBoolean() ? 1u : 0u;
                    } else if (!world.HasComponent<RenderMesh>(entityId)) {
                        comp.isVisible = 1u;
                    }
                    world.AddComponent(entityId, comp);
                    break;
                }
                case "SpriteRenderData": {
                    var comp = world.HasComponent<SpriteRenderData>(entityId) ? world.GetComponent<SpriteRenderData>(entityId) : new SpriteRenderData();
                    if (data.TryGetProperty("spriteId", out var prop_spriteId)) {
                        string s_spriteId = prop_spriteId.GetString() ?? "";
                        comp.spriteId = System.Guid.TryParse(s_spriteId, out var g_spriteId) ? (Engine.Core.Guid)g_spriteId : new Engine.Core.Guid();
                    } else if (!world.HasComponent<SpriteRenderData>(entityId)) {
                        comp.spriteId = new Engine.Core.Guid();
                    }
                    if (data.TryGetProperty("materialId", out var prop_materialId)) {
                        string s_materialId = prop_materialId.GetString() ?? "";
                        comp.materialId = System.Guid.TryParse(s_materialId, out var g_materialId) ? (Engine.Core.Guid)g_materialId : new Engine.Core.Guid();
                    } else if (!world.HasComponent<SpriteRenderData>(entityId)) {
                        comp.materialId = new Engine.Core.Guid();
                    }
                    if (data.TryGetProperty("color", out var prop_color)) {
                        comp.color = new Vector4(prop_color[0].GetSingle(), prop_color[1].GetSingle(), prop_color[2].GetSingle(), prop_color[3].GetSingle());
                    } else if (!world.HasComponent<SpriteRenderData>(entityId)) {
                        comp.color = new Vector4(1.0f, 1.0f, 1.0f, 1.0f);
                    }
                    if (data.TryGetProperty("flipX", out var prop_flipX)) {
                        comp.flipX = prop_flipX.GetBoolean() ? 1u : 0u;
                    } else if (!world.HasComponent<SpriteRenderData>(entityId)) {
                        comp.flipX = 0u;
                    }
                    if (data.TryGetProperty("flipY", out var prop_flipY)) {
                        comp.flipY = prop_flipY.GetBoolean() ? 1u : 0u;
                    } else if (!world.HasComponent<SpriteRenderData>(entityId)) {
                        comp.flipY = 0u;
                    }
                    world.AddComponent(entityId, comp);
                    break;
                }
                case "AudioSourceData": {
                    var comp = world.HasComponent<AudioSourceData>(entityId) ? world.GetComponent<AudioSourceData>(entityId) : new AudioSourceData();
                    if (data.TryGetProperty("clipId", out var prop_clipId)) {
                        string s_clipId = prop_clipId.GetString() ?? "";
                        comp.clipId = System.Guid.TryParse(s_clipId, out var g_clipId) ? (Engine.Core.Guid)g_clipId : new Engine.Core.Guid();
                    } else if (!world.HasComponent<AudioSourceData>(entityId)) {
                        comp.clipId = new Engine.Core.Guid();
                    }
                    if (data.TryGetProperty("volume", out var prop_volume)) {
                        comp.volume = prop_volume.GetSingle();
                    } else if (!world.HasComponent<AudioSourceData>(entityId)) {
                        comp.volume = 1.0f;
                    }
                    if (data.TryGetProperty("pitch", out var prop_pitch)) {
                        comp.pitch = prop_pitch.GetSingle();
                    } else if (!world.HasComponent<AudioSourceData>(entityId)) {
                        comp.pitch = 1.0f;
                    }
                    if (data.TryGetProperty("spatialBlend", out var prop_spatialBlend)) {
                        comp.spatialBlend = prop_spatialBlend.GetSingle();
                    } else if (!world.HasComponent<AudioSourceData>(entityId)) {
                        comp.spatialBlend = 0.0f;
                    }
                    if (data.TryGetProperty("loop", out var prop_loop)) {
                        comp.loop = prop_loop.GetBoolean() ? 1u : 0u;
                    } else if (!world.HasComponent<AudioSourceData>(entityId)) {
                        comp.loop = 0u;
                    }
                    world.AddComponent(entityId, comp);
                    break;
                }
                case "AudioPlayState": {
                    var comp = world.HasComponent<AudioPlayState>(entityId) ? world.GetComponent<AudioPlayState>(entityId) : new AudioPlayState();
                    if (data.TryGetProperty("isPlaying", out var prop_isPlaying)) {
                        comp.isPlaying = prop_isPlaying.GetBoolean() ? 1u : 0u;
                    } else if (!world.HasComponent<AudioPlayState>(entityId)) {
                        comp.isPlaying = 0u;
                    }
                    if (data.TryGetProperty("time", out var prop_time)) {
                        comp.time = prop_time.GetSingle();
                    } else if (!world.HasComponent<AudioPlayState>(entityId)) {
                        comp.time = 0.0f;
                    }
                    if (data.TryGetProperty("triggerPlay", out var prop_triggerPlay)) {
                        comp.triggerPlay = prop_triggerPlay.GetBoolean() ? 1u : 0u;
                    } else if (!world.HasComponent<AudioPlayState>(entityId)) {
                        comp.triggerPlay = 0u;
                    }
                    world.AddComponent(entityId, comp);
                    break;
                }
                case "AudioListenerTag": {
                    var comp = world.HasComponent<AudioListenerTag>(entityId) ? world.GetComponent<AudioListenerTag>(entityId) : new AudioListenerTag();
                    world.AddComponent(entityId, comp);
                    break;
                }
                case "CanvasData": {
                    var comp = world.HasComponent<CanvasData>(entityId) ? world.GetComponent<CanvasData>(entityId) : new CanvasData();
                    if (data.TryGetProperty("renderMode", out var prop_renderMode)) {
                        comp.renderMode = prop_renderMode.GetInt32();
                    } else if (!world.HasComponent<CanvasData>(entityId)) {
                        comp.renderMode = 0;
                    }
                    if (data.TryGetProperty("scaleFactor", out var prop_scaleFactor)) {
                        comp.scaleFactor = prop_scaleFactor.GetSingle();
                    } else if (!world.HasComponent<CanvasData>(entityId)) {
                        comp.scaleFactor = 1.0f;
                    }
                    world.AddComponent(entityId, comp);
                    break;
                }
                case "UIImageData": {
                    var comp = world.HasComponent<UIImageData>(entityId) ? world.GetComponent<UIImageData>(entityId) : new UIImageData();
                    if (data.TryGetProperty("spriteId", out var prop_spriteId)) {
                        string s_spriteId = prop_spriteId.GetString() ?? "";
                        comp.spriteId = System.Guid.TryParse(s_spriteId, out var g_spriteId) ? (Engine.Core.Guid)g_spriteId : new Engine.Core.Guid();
                    } else if (!world.HasComponent<UIImageData>(entityId)) {
                        comp.spriteId = new Engine.Core.Guid();
                    }
                    if (data.TryGetProperty("color", out var prop_color)) {
                        comp.color = new Vector4(prop_color[0].GetSingle(), prop_color[1].GetSingle(), prop_color[2].GetSingle(), prop_color[3].GetSingle());
                    } else if (!world.HasComponent<UIImageData>(entityId)) {
                        comp.color = new Vector4(1.0f, 1.0f, 1.0f, 1.0f);
                    }
                    if (data.TryGetProperty("materialId", out var prop_materialId)) {
                        string s_materialId = prop_materialId.GetString() ?? "";
                        comp.materialId = System.Guid.TryParse(s_materialId, out var g_materialId) ? (Engine.Core.Guid)g_materialId : new Engine.Core.Guid();
                    } else if (!world.HasComponent<UIImageData>(entityId)) {
                        comp.materialId = new Engine.Core.Guid();
                    }
                    world.AddComponent(entityId, comp);
                    break;
                }
                case "UITextData": {
                    var comp = world.HasComponent<UITextData>(entityId) ? world.GetComponent<UITextData>(entityId) : new UITextData();
                    if (data.TryGetProperty("fontId", out var prop_fontId)) {
                        string s_fontId = prop_fontId.GetString() ?? "";
                        comp.fontId = System.Guid.TryParse(s_fontId, out var g_fontId) ? (Engine.Core.Guid)g_fontId : new Engine.Core.Guid();
                    } else if (!world.HasComponent<UITextData>(entityId)) {
                        comp.fontId = new Engine.Core.Guid();
                    }
                    if (data.TryGetProperty("textHashId", out var prop_textHashId)) {
                        comp.textHashId = prop_textHashId.GetUInt32();
                    } else if (!world.HasComponent<UITextData>(entityId)) {
                        comp.textHashId = 0;
                    }
                    if (data.TryGetProperty("fontSize", out var prop_fontSize)) {
                        comp.fontSize = prop_fontSize.GetSingle();
                    } else if (!world.HasComponent<UITextData>(entityId)) {
                        comp.fontSize = 14.0f;
                    }
                    if (data.TryGetProperty("color", out var prop_color)) {
                        comp.color = new Vector4(prop_color[0].GetSingle(), prop_color[1].GetSingle(), prop_color[2].GetSingle(), prop_color[3].GetSingle());
                    } else if (!world.HasComponent<UITextData>(entityId)) {
                        comp.color = new Vector4(1.0f, 1.0f, 1.0f, 1.0f);
                    }
                    world.AddComponent(entityId, comp);
                    break;
                }
                case "UIButtonState": {
                    var comp = world.HasComponent<UIButtonState>(entityId) ? world.GetComponent<UIButtonState>(entityId) : new UIButtonState();
                    if (data.TryGetProperty("isHovered", out var prop_isHovered)) {
                        comp.isHovered = prop_isHovered.GetBoolean() ? 1u : 0u;
                    } else if (!world.HasComponent<UIButtonState>(entityId)) {
                        comp.isHovered = 0u;
                    }
                    if (data.TryGetProperty("isPressed", out var prop_isPressed)) {
                        comp.isPressed = prop_isPressed.GetBoolean() ? 1u : 0u;
                    } else if (!world.HasComponent<UIButtonState>(entityId)) {
                        comp.isPressed = 0u;
                    }
                    if (data.TryGetProperty("interactable", out var prop_interactable)) {
                        comp.interactable = prop_interactable.GetBoolean() ? 1u : 0u;
                    } else if (!world.HasComponent<UIButtonState>(entityId)) {
                        comp.interactable = 1u;
                    }
                    world.AddComponent(entityId, comp);
                    break;
                }
                case "AnimatorState": {
                    var comp = world.HasComponent<AnimatorState>(entityId) ? world.GetComponent<AnimatorState>(entityId) : new AnimatorState();
                    if (data.TryGetProperty("controllerId", out var prop_controllerId)) {
                        string s_controllerId = prop_controllerId.GetString() ?? "";
                        comp.controllerId = System.Guid.TryParse(s_controllerId, out var g_controllerId) ? (Engine.Core.Guid)g_controllerId : new Engine.Core.Guid();
                    } else if (!world.HasComponent<AnimatorState>(entityId)) {
                        comp.controllerId = new Engine.Core.Guid();
                    }
                    if (data.TryGetProperty("currentClipHash", out var prop_currentClipHash)) {
                        comp.currentClipHash = prop_currentClipHash.GetUInt32();
                    } else if (!world.HasComponent<AnimatorState>(entityId)) {
                        comp.currentClipHash = 0;
                    }
                    if (data.TryGetProperty("normalizedTime", out var prop_normalizedTime)) {
                        comp.normalizedTime = prop_normalizedTime.GetSingle();
                    } else if (!world.HasComponent<AnimatorState>(entityId)) {
                        comp.normalizedTime = 0.0f;
                    }
                    if (data.TryGetProperty("speed", out var prop_speed)) {
                        comp.speed = prop_speed.GetSingle();
                    } else if (!world.HasComponent<AnimatorState>(entityId)) {
                        comp.speed = 1.0f;
                    }
                    world.AddComponent(entityId, comp);
                    break;
                }
                case "NavAgentParams": {
                    var comp = world.HasComponent<NavAgentParams>(entityId) ? world.GetComponent<NavAgentParams>(entityId) : new NavAgentParams();
                    if (data.TryGetProperty("radius", out var prop_radius)) {
                        comp.radius = prop_radius.GetSingle();
                    } else if (!world.HasComponent<NavAgentParams>(entityId)) {
                        comp.radius = 0.5f;
                    }
                    if (data.TryGetProperty("height", out var prop_height)) {
                        comp.height = prop_height.GetSingle();
                    } else if (!world.HasComponent<NavAgentParams>(entityId)) {
                        comp.height = 2.0f;
                    }
                    if (data.TryGetProperty("maxSpeed", out var prop_maxSpeed)) {
                        comp.maxSpeed = prop_maxSpeed.GetSingle();
                    } else if (!world.HasComponent<NavAgentParams>(entityId)) {
                        comp.maxSpeed = 3.5f;
                    }
                    if (data.TryGetProperty("avoidancePriority", out var prop_avoidancePriority)) {
                        comp.avoidancePriority = prop_avoidancePriority.GetInt32();
                    } else if (!world.HasComponent<NavAgentParams>(entityId)) {
                        comp.avoidancePriority = 50;
                    }
                    world.AddComponent(entityId, comp);
                    break;
                }
                case "NavAgentState": {
                    var comp = world.HasComponent<NavAgentState>(entityId) ? world.GetComponent<NavAgentState>(entityId) : new NavAgentState();
                    if (data.TryGetProperty("destination", out var prop_destination)) {
                        comp.destination = new Vector3(prop_destination[0].GetSingle(), prop_destination[1].GetSingle(), prop_destination[2].GetSingle());
                    } else if (!world.HasComponent<NavAgentState>(entityId)) {
                        comp.destination = new Vector3(0.0f, 0.0f, 0.0f);
                    }
                    if (data.TryGetProperty("currentVelocity", out var prop_currentVelocity)) {
                        comp.currentVelocity = new Vector3(prop_currentVelocity[0].GetSingle(), prop_currentVelocity[1].GetSingle(), prop_currentVelocity[2].GetSingle());
                    } else if (!world.HasComponent<NavAgentState>(entityId)) {
                        comp.currentVelocity = new Vector3(0.0f, 0.0f, 0.0f);
                    }
                    if (data.TryGetProperty("hasPath", out var prop_hasPath)) {
                        comp.hasPath = prop_hasPath.GetBoolean() ? 1u : 0u;
                    } else if (!world.HasComponent<NavAgentState>(entityId)) {
                        comp.hasPath = 0u;
                    }
                    if (data.TryGetProperty("remainingDistance", out var prop_remainingDistance)) {
                        comp.remainingDistance = prop_remainingDistance.GetSingle();
                    } else if (!world.HasComponent<NavAgentState>(entityId)) {
                        comp.remainingDistance = 0.0f;
                    }
                    world.AddComponent(entityId, comp);
                    break;
                }
                case "ScriptComponent": {
                    var comp = world.HasComponent<ScriptComponent>(entityId) ? world.GetComponent<ScriptComponent>(entityId) : new ScriptComponent();
                    if (data.TryGetProperty("typeId", out var prop_typeId)) {
                        comp.typeId = prop_typeId.GetInt32();
                    } else if (!world.HasComponent<ScriptComponent>(entityId)) {
                        comp.typeId = -1;
                    }
                    if (data.TryGetProperty("gcHandle", out var prop_gcHandle)) {
                        comp.gcHandle = prop_gcHandle.GetUInt64();
                    } else if (!world.HasComponent<ScriptComponent>(entityId)) {
                        comp.gcHandle = 0;
                    }
                    world.AddComponent(entityId, comp);
                    break;
                }
                case "SceneMask": {
                    var comp = world.HasComponent<SceneMask>(entityId) ? world.GetComponent<SceneMask>(entityId) : new SceneMask();
                    if (data.TryGetProperty("mask", out var prop_mask)) {
                        comp.mask = prop_mask.GetUInt32();
                    } else if (!world.HasComponent<SceneMask>(entityId)) {
                        comp.mask = 1;
                    }
                    world.AddComponent(entityId, comp);
                    break;
                }
                default: throw new ArgumentException($"Unknown component type: {typeName}");
            }
        }
    }

    public static class ComponentID<T> {
        public static readonly uint ID = ComponentID.GetInternal<T>();
    }

    public static class ComponentID {
        private const string DLL_NAME = "Temp.exe";
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)] private static extern uint Ecs_GetTypeId_LocalTransform();
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)] private static extern uint Ecs_GetTypeId_WorldMatrix();
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)] private static extern uint Ecs_GetTypeId_Parent();
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)] private static extern uint Ecs_GetTypeId_LocalRect();
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)] private static extern uint Ecs_GetTypeId_CanvasWorldMatrix();
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)] private static extern uint Ecs_GetTypeId_Velocity();
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)] private static extern uint Ecs_GetTypeId_PhysicsProperties();
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)] private static extern uint Ecs_GetTypeId_BoxGeometry();
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)] private static extern uint Ecs_GetTypeId_SphereGeometry();
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)] private static extern uint Ecs_GetTypeId_CapsuleGeometry();
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)] private static extern uint Ecs_GetTypeId_ColliderFilter();
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)] private static extern uint Ecs_GetTypeId_CameraData();
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)] private static extern uint Ecs_GetTypeId_LightData();
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)] private static extern uint Ecs_GetTypeId_RenderMesh();
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)] private static extern uint Ecs_GetTypeId_SpriteRenderData();
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)] private static extern uint Ecs_GetTypeId_AudioSourceData();
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)] private static extern uint Ecs_GetTypeId_AudioPlayState();
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)] private static extern uint Ecs_GetTypeId_AudioListenerTag();
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)] private static extern uint Ecs_GetTypeId_CanvasData();
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)] private static extern uint Ecs_GetTypeId_UIImageData();
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)] private static extern uint Ecs_GetTypeId_UITextData();
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)] private static extern uint Ecs_GetTypeId_UIButtonState();
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)] private static extern uint Ecs_GetTypeId_AnimatorState();
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)] private static extern uint Ecs_GetTypeId_NavAgentParams();
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)] private static extern uint Ecs_GetTypeId_NavAgentState();
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)] private static extern uint Ecs_GetTypeId_ScriptComponent();
        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)] private static extern uint Ecs_GetTypeId_SceneMask();

        public static uint Get<T>() => ComponentID<T>.ID;

        internal static uint GetInternal<T>() {
            if (typeof(T) == typeof(LocalTransform)) return Ecs_GetTypeId_LocalTransform();
            if (typeof(T) == typeof(WorldMatrix)) return Ecs_GetTypeId_WorldMatrix();
            if (typeof(T) == typeof(Parent)) return Ecs_GetTypeId_Parent();
            if (typeof(T) == typeof(LocalRect)) return Ecs_GetTypeId_LocalRect();
            if (typeof(T) == typeof(CanvasWorldMatrix)) return Ecs_GetTypeId_CanvasWorldMatrix();
            if (typeof(T) == typeof(Velocity)) return Ecs_GetTypeId_Velocity();
            if (typeof(T) == typeof(PhysicsProperties)) return Ecs_GetTypeId_PhysicsProperties();
            if (typeof(T) == typeof(BoxGeometry)) return Ecs_GetTypeId_BoxGeometry();
            if (typeof(T) == typeof(SphereGeometry)) return Ecs_GetTypeId_SphereGeometry();
            if (typeof(T) == typeof(CapsuleGeometry)) return Ecs_GetTypeId_CapsuleGeometry();
            if (typeof(T) == typeof(ColliderFilter)) return Ecs_GetTypeId_ColliderFilter();
            if (typeof(T) == typeof(CameraData)) return Ecs_GetTypeId_CameraData();
            if (typeof(T) == typeof(LightData)) return Ecs_GetTypeId_LightData();
            if (typeof(T) == typeof(RenderMesh)) return Ecs_GetTypeId_RenderMesh();
            if (typeof(T) == typeof(SpriteRenderData)) return Ecs_GetTypeId_SpriteRenderData();
            if (typeof(T) == typeof(AudioSourceData)) return Ecs_GetTypeId_AudioSourceData();
            if (typeof(T) == typeof(AudioPlayState)) return Ecs_GetTypeId_AudioPlayState();
            if (typeof(T) == typeof(AudioListenerTag)) return Ecs_GetTypeId_AudioListenerTag();
            if (typeof(T) == typeof(CanvasData)) return Ecs_GetTypeId_CanvasData();
            if (typeof(T) == typeof(UIImageData)) return Ecs_GetTypeId_UIImageData();
            if (typeof(T) == typeof(UITextData)) return Ecs_GetTypeId_UITextData();
            if (typeof(T) == typeof(UIButtonState)) return Ecs_GetTypeId_UIButtonState();
            if (typeof(T) == typeof(AnimatorState)) return Ecs_GetTypeId_AnimatorState();
            if (typeof(T) == typeof(NavAgentParams)) return Ecs_GetTypeId_NavAgentParams();
            if (typeof(T) == typeof(NavAgentState)) return Ecs_GetTypeId_NavAgentState();
            if (typeof(T) == typeof(ScriptComponent)) return Ecs_GetTypeId_ScriptComponent();
            if (typeof(T) == typeof(SceneMask)) return Ecs_GetTypeId_SceneMask();
            return 0;
        }
    }
}