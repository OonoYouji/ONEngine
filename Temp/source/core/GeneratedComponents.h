#pragma once
#include <cstdint>
#include "EngineMath.h"

// ユーザー（C#）が操作するローカル座標。
struct LocalTransform {
    Engine::Vector3 position;
    uint8_t _pad_align_12[4];
    Engine::Vector4 rotation;
    Engine::Vector3 scale;
    uint8_t _pad_final[4];
}; // size: 48

// C++システムが計算し、GPUや物理エンジンに渡す絶対座標行列。
struct alignas(16) WorldMatrix {
    Engine::Matrix4x4 matrix;
}; // size: 64

// 階層構造の構築。
struct Parent {
    uint32_t parentId;
    uint8_t _pad_final[4];
}; // size: 8

// UIのアンカー、ピボット、ローカル配置データ。
struct LocalRect {
    Engine::Vector2 anchorMin;
    Engine::Vector2 anchorMax;
    Engine::Vector2 pivot;
    Engine::Vector2 anchoredPosition;
    Engine::Vector2 sizeDelta;
}; // size: 40

// Canvasの解像度計算を加味した最終的なUI描画用行列。
struct alignas(16) CanvasWorldMatrix {
    Engine::Matrix4x4 matrix;
}; // size: 64

// 毎フレームの物理更新・移動処理に使われる動的データ。
struct Velocity {
    Engine::Vector3 linear;
    Engine::Vector3 angular;
}; // size: 24

// 質量や重力設定などの静的パラメータ。
struct PhysicsProperties {
    float inverseMass;
    Engine::Vector3 inverseInertia;
    bool useGravity;
    bool isKinematic;
}; // size: 24

// 立方体形状データ。
struct BoxGeometry {
    Engine::Vector3 center;
    Engine::Vector3 extents;
}; // size: 24

// 球形状データ。
struct SphereGeometry {
    Engine::Vector3 center;
    float radius;
}; // size: 16

// カプセル形状データ。
struct CapsuleGeometry {
    Engine::Vector3 center;
    float radius;
    float height;
    int32_t direction;
}; // size: 24

// 衝突レイヤーとマスク設定。
struct ColliderFilter {
    uint32_t layer;
    uint32_t collisionMask;
    bool isTrigger;
    uint8_t _pad_final[4];
}; // size: 16

// カメラの投影設定。
struct CameraData {
    float fov;
    float nearClip;
    float farClip;
    uint32_t cullingMask;
}; // size: 16

// 光源設定。
struct alignas(16) LightData {
    int32_t type;
    Engine::Vector3 color;
    float intensity;
    float range;
    float spotAngle;
    uint8_t _pad_final[4];
}; // size: 32

// 描画するメッシュとマテリアルのハンドル。
struct alignas(16) RenderMesh {
    Engine::Guid meshId;
    Engine::Guid materialId;
    uint32_t renderLayer;
    bool isVisible;
    uint8_t _pad_final[8];
}; // size: 48

// 2Dスプライトの描画情報。
struct alignas(16) SpriteRenderData {
    Engine::Guid spriteId;
    Engine::Guid materialId;
    Engine::Vector4 color;
    bool flipX;
    bool flipY;
    uint8_t _pad_final[8];
}; // size: 64

// 音源の設定パラメータ。
struct AudioSourceData {
    Engine::Guid clipId;
    float volume;
    float pitch;
    float spatialBlend;
    bool loop;
}; // size: 32

// 現在の再生状況。
struct AudioPlayState {
    bool isPlaying;
    float time;
    bool triggerPlay;
    uint8_t _pad_final[4];
}; // size: 16

// マイクとなるエンティティを示すタグ。
struct AudioListenerTag {
}; // size: 0

// UIのルート設定。
struct CanvasData {
    int32_t renderMode;
    float scaleFactor;
}; // size: 8

// UI画像の描画情報。
struct alignas(16) UIImageData {
    Engine::Guid spriteId;
    Engine::Vector4 color;
    Engine::Guid materialId;
}; // size: 48

// テキストの描画情報。
struct alignas(16) UITextData {
    Engine::Guid fontId;
    uint32_t textHashId;
    float fontSize;
    uint8_t _pad_align_24[8];
    Engine::Vector4 color;
}; // size: 48

// ボタンのインタラクション状態。
struct UIButtonState {
    bool isHovered;
    bool isPressed;
    bool interactable;
    uint8_t _pad_final[4];
}; // size: 16

// アニメーションコントローラーの現在状態。
struct AnimatorState {
    Engine::Guid controllerId;
    uint32_t currentClipHash;
    float normalizedTime;
    float speed;
    uint8_t _pad_final[4];
}; // size: 32

// エージェントの静的パラメータ。
struct NavAgentParams {
    float radius;
    float height;
    float maxSpeed;
    int32_t avoidancePriority;
}; // size: 16

// 移動の目標と現在の状況。
struct NavAgentState {
    Engine::Vector3 destination;
    Engine::Vector3 currentVelocity;
    bool hasPath;
    float remainingDistance;
}; // size: 32

// C#のGameScriptインスタンスへの参照を保持。
struct ScriptComponent {
    int32_t typeId;
    uint8_t _pad_align_4[4];
    uint64_t gcHandle;
}; // size: 16

// マルチシーン実行時のフィルタリング用ビットマスク。
struct SceneMask {
    uint32_t mask;
    uint8_t _pad_final[4];
}; // size: 8
