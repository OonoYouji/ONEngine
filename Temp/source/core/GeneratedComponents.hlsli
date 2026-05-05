// Generated HLSL Components

struct WorldMatrix {
    float4x4 matrix;
};

struct CanvasWorldMatrix {
    float4x4 matrix;
};

struct LightData {
    int type;
    float3 color;
    float intensity;
    float range;
    float spotAngle;
    float _pad_final;
};

struct RenderMesh {
    uint4 meshId;
    uint4 materialId;
    uint renderLayer;
    bool isVisible;
    float2 _pad_final;
};

struct SpriteRenderData {
    uint4 spriteId;
    uint4 materialId;
    float4 color;
    bool flipX;
    bool flipY;
    float2 _pad_final;
};

struct UIImageData {
    uint4 spriteId;
    float4 color;
    uint4 materialId;
};

struct UITextData {
    uint4 fontId;
    uint textHashId;
    float fontSize;
    float2 _pad_align_24;
    float4 color;
};
