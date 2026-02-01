
struct InputInfo {
    float2 mouseUV;
};

struct MousePos {
    float4 worldPos;
};

ConstantBuffer<InputInfo> inputInfo : register(b0);
RWStructuredBuffer<MousePos> mousePosBuffer : register(u0);
Texture2D<float4> worldPositionTexture : register(t1);
SamplerState textureSampler : register(s0);


[numthreads(1,1,1)]
void main() {
    mousePosBuffer[0].worldPos = worldPositionTexture.SampleLevel(textureSampler, inputInfo.mouseUV, 0);   
}