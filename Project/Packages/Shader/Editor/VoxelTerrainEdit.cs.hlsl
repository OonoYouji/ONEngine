#include "../Render/VoxelTerrain/VoxelTerrainCommon.hlsli"
#include "../ConstantBufferData/ViewProjection.hlsli"
#include "../Math/Math.hlsli"

struct InputInfo {
    uint mouseLeftButton;
    float2 screenMousePos;
};

ConstantBuffer<ViewProjection> viewProjection   : register(b1);
ConstantBuffer<Camera>         camera           : register(b2);
ConstantBuffer<InputInfo>      inputInfo        : register(b3);

StructuredBuffer<Chunk> chunks : register(t0);
RWTexture3D<float4> voxelTextures[] : register(u0);
SamplerState textureSampler : register(s0);


float3 ScreenToWorldRay(float2 _screenPos) {
    float4 clipPos = float4(_screenPos * 2.0f - 1.0f, 0, 1);
    float4 viewPos = mul(clipPos, InverseMatrix(viewProjection.matProjection));
    viewPos /= viewPos.w;
    float4 worldPos = mul(viewPos, InverseMatrix(viewProjection.matView));
    worldPos /= worldPos.w;

    float3 rayDir = normalize(worldPos.xyz - camera.position.xyz);
    return rayDir;
}


[numthreads(8, 8, 1)]
void main( 
    uint3 DTid : SV_DispatchThreadID,
    uint groupIndex : SV_GroupIndex) {

    uint chunkIndex = groupIndex;
    
    /// 対応するチャンクの情報
    Chunk chunk = chunks[chunkIndex];

    float3 rayDir = ScreenToWorldRay(inputInfo.screenMousePos);
    float3 rayOrigin = camera.position.xyz;
    
    
    
}