
struct Test {
	float tmp;
};


ConstantBuffer<Test> gTest : register(b0);
SamplerState gSampler : register(s0);
Texture2D<float4> gTexture : register(t0);

[numthreads(1, 1, 1)]
void main() {
	float a = gTest.tmp;

	float4 color = gTexture.Sample(gSampler, float2(0.5f, a));

}