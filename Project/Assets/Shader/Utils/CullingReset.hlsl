RWStructuredBuffer<uint> gCountBuffer : register(u0);

[numthreads(64, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
    if (DTid.x < 64) {
        gCountBuffer[DTid.x] = 0;
    }
}
