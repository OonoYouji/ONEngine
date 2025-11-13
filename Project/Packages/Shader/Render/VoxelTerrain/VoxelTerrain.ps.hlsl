#include "VoxelTerrain.hlsli"

struct PSOutput {
    float4 color : SV_Target0;
    float4 worldPos : SV_Target1;
    float4 normal : SV_Target2;
    float4 flags : SV_Target3;
};


PSOutput main(VertexOut _out) {

}