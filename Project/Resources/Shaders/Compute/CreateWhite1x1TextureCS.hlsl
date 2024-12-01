
RWTexture2D<float4> gUAVTexture : register(u0);

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	gUAVTexture[DTid.xy] = float4(1, 1, 1, 1);
}
