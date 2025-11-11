
#include "TerrainProcedural.hlsli"

#include "../../ConstantBufferData/Transform.hlsli"
#include "../../ConstantBufferData/ViewProjection.hlsli"

ConstantBuffer<ViewProjection> viewProjection : register(b0);
StructuredBuffer<InstanceData> instanceData : register(t0);


VSOutput main(VSInput input, uint instanceId : SV_InstanceID) {
	VSOutput output;
	
	float4x4 matWVP = mul(instanceData[instanceId].matWorld, viewProjection.matVP);
	
	output.position  = mul(input.position, matWVP);
	output.wPosition = mul(input.position, instanceData[instanceId].matWorld);
	output.normal    = normalize(mul(input.normal, (float3x3) instanceData[instanceId].matWorld));
	output.uv        = input.uv;

	return output;
}