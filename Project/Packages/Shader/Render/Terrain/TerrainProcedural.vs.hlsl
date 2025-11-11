
#include "TerrainProcedural.hlsli"

#include "../../ConstantBufferData/Transform.hlsli"
#include "../../ConstantBufferData/ViewProjection.hlsli"

ConstantBuffer<ViewProjection> viewProjection : register(b0);
StructuredBuffer<InstanceData> instanceData : register(t0);
StructuredBuffer<RenderingInstance> renderingInstances : register(t1);


VSOutput main(VSInput input, uint instanceId : SV_InstanceID) {
	VSOutput output;
	
	RenderingInstance renderInst = renderingInstances[instanceId];
	uint index = renderInst.id;
	float4x4 matWVP = mul(instanceData[index].matWorld, viewProjection.matVP);
	
	output.position  = mul(input.position, matWVP);
	output.wPosition = mul(input.position, instanceData[index].matWorld);
	output.normal    = normalize(mul(input.normal, (float3x3) instanceData[index].matWorld));
	output.uv        = input.uv;

	return output;
}