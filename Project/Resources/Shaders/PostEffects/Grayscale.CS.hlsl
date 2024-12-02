

Texture2D<float4>   gInputTexture  : register(t0);
RWTexture2D<float4> gOutputTexture : register(u0);

[numthreads(16, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {

	float4 color     = gInputTexture.Load(int3(DTid.xy, 0));
	float  grayscale = 0.2989 * color.r + 0.5870 * color.g + 0.1140 * color.b;

	gOutputTexture[DTid.xy] = float4(grayscale, grayscale, grayscale, 1.0f);

}