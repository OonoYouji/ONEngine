
struct VSOutput {
	float4 position : SV_POSITION;
	float4 color : COLOR;
};
struct VSInput {
	float4 position : POSITION0;
	float4 color : COLOR0;
};


static const float2 kScreenSize = float2(1280, 720);

VSOutput main(VSInput input) {
	VSOutput output;
	
	output.position.x = (input.position.x / kScreenSize.x) * 2.0f - 1.0f;
	output.position.y = 1.0f - (input.position.y / kScreenSize.y) * 2.0f;
	output.position.z = 0.0f;
	output.position.w = 1.0f;

	output.color = input.color;
	
	return output;
}