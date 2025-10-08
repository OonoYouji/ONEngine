#include "Grass.hlsli"

[shader("amplification")]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint index = dispatchThreadID.x;

    // Payload を構築
    GrassPayload payload;
    payload.grassIndex = index;

    // DispatchMeshは全スレッドで呼ぶ
    DispatchMesh(1, 1, 1, payload);
}
