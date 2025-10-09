#include "../Render/Grass/Grass.hlsli"

struct UsedTexId {
    uint grassArrangementTexId;
};

ConstantBuffer<UsedTexId> usedTexId : register(b0);
RWStructuredBuffer<BladeInstance> bladeInstances : register(u0);

/// テクスチャ配列
Texture2D<float4> textures[] : register(t0);
SamplerState textureSampler : register(s0);

/// 草の配置を行うためのshader
[numthreads(64, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
    uint index = DTid.x;

    /// 草のBufferサイズを取得して、範囲外アクセスを防止
    uint numStructs;
    uint structSize;
    bladeInstances.GetDimensions(numStructs, structSize);
    if (index >= numStructs) {
        return;
    }

    BladeInstance newInstance;

    /// 草の配置する場所のテクスチャを参照
    float2 uv = float2((index + 0.5) / numStructs, 0.5);
    float4 samplingPos = textures[usedTexId.grassArrangementTexId].Sample(textureSampler, uv);
    newInstance.position = samplingPos.xyz;

    /// 草の向きをランダムに決定
    float angle = samplingPos.w * 6.28; // 0~1の値を0~2πに変換
    newInstance.tangent = float3(cos(angle), 0, sin(angle));
    newInstance.scale = 0.5 + (samplingPos.w * 0.5); // 0.5~1.0の範囲でスケールを決定
    newInstance.random01 = samplingPos.w; // 0~1のランダム値を保存

    bladeInstances[index] = newInstance;
}