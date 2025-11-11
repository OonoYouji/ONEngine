/// ///////////////////////////////////////////////////
/// プロシージャル植生の描画前に行い、試錐台のカリングを行う
/// ///////////////////////////////////////////////////

#include "../../ConstantBufferData/ViewProjection.hlsli"
#include "TerrainProcedural.hlsli"


/// --------------- Buffer --------------- ///

ConstantBuffer<ViewProjection> viewProjection : register(b0);
StructuredBuffer<InstanceData> instanceData : register(t0);
RWStructuredBuffer<RenderingInstance> renderingInstances : register(u0);


/// --------------- Main --------------- ///

[numthreads(1024, 1, 1)]
void main(uint DTid : SV_DispatchThreadID) {

    /// インスタンス数を超えたら処理しない
    uint index = DTid.x;

    InstanceData instance = instanceData[index];

    float4x4 matWVP = mul(instance.matWorld, viewProjection.matVP);
    float4 ndcPos = mul(float4(0.0f, 0.0f, 0.0f, 1.0f), matWVP);
    ndcPos /= ndcPos.w;

    // AABB -> 8 corners (local space)
    float3 bmin = instance.minBounds.xyz;
    float3 bmax = instance.maxBounds.xyz;
    float3 corners[8] = {
        float3(bmin.x, bmin.y, bmin.z),
        float3(bmax.x, bmin.y, bmin.z),
        float3(bmin.x, bmax.y, bmin.z),
        float3(bmax.x, bmax.y, bmin.z),
        float3(bmin.x, bmin.y, bmax.z),
        float3(bmax.x, bmin.y, bmax.z),
        float3(bmin.x, bmax.y, bmax.z),
        float3(bmax.x, bmax.y, bmax.z)
    };

    // Track if all corners are outside any of the 6 clip planes
    bool outsideLeft  = true;
    bool outsideRight = true;
    bool outsideBottom= true;
    bool outsideTop   = true;
    bool outsideNear  = true;
    bool outsideFar   = true;

    for (int i = 0; i < 8; ++i) {
        float4 clip = mul(float4(corners[i], 1.0f), matWVP);

        // left  : x >= -w
        if (clip.x >= -clip.w) outsideLeft = false;
        // right : x <= w
        if (clip.x <=  clip.w) outsideRight = false;
        // bottom: y >= -w
        if (clip.y >= -clip.w) outsideBottom = false;
        // top   : y <= w
        if (clip.y <=  clip.w) outsideTop = false;
        // near  : z >= 0 (Direct3D clip-space near = 0)
        if (clip.z >= 0.0f) outsideNear = false;
        // far   : z <= w
        if (clip.z <= clip.w) outsideFar = false;

        // early-out: if none of the "all-outside" flags remain true, the box intersects frustum
        if (!outsideLeft && !outsideRight && !outsideBottom && !outsideTop && !outsideNear && !outsideFar) {
            break;
        }
    }

    bool culled = outsideLeft || outsideRight || outsideBottom || outsideTop || outsideNear || outsideFar;
    uint visibleFlag = culled ? 0u : 1u;

    // Store result (adjust field name as needed in RenderingInstance)
    renderingInstances[index].id = visibleFlag;
}