#include "../Schema/Schema.hlsli"

struct ParticleUpdateParams {
    float dt;
    float3 emitterPos;
    uint totalParticles;
    float seed;
    uint modelIndex;
    uint textureIndex; // 追加
};

ConstantBuffer<ParticleUpdateParams> gParams : register(b1);
RWStructuredBuffer<ParticleGPUData> gParticles : register(u0);

uint pcg_hash(uint n) {
    uint state = n * 747796405u + 2891336453u;
    uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}

float rand(uint seed) {
    return float(pcg_hash(seed)) / 4294967296.0;
}

[numthreads(256, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
    uint idx = DTid.x;
    if (idx >= gParams.totalParticles) return;

    ParticleGPUData p = gParticles[idx];
    p.age += gParams.dt;

    if (p.age >= p.maxLifetime) {
        uint s = idx + (uint)(gParams.seed * 1000.0f);
        float r1 = rand(s);
        float r2 = rand(s + 100);
        float r3 = rand(s + 200);
        
        p.position = gParams.emitterPos;
        float phi = r1 * 2.0 * 3.14159;
        float theta = acos(r2 * 2.0 - 1.0) * 0.5;
        float3 dir = float3(sin(theta) * cos(phi), cos(theta), sin(theta) * sin(phi));

        p.velocity = dir * (5.0f + r3 * 10.0f); 
        p.age = 0;
        p.modelIndex = gParams.modelIndex;
        p.textureIndex = gParams.textureIndex; // セット
    } else {
        p.velocity.y -= 9.8f * gParams.dt;
        p.position += p.velocity * gParams.dt;
        p.rotation.y += 2.0f * gParams.dt;
        p.rotation.x += 1.0f * gParams.dt;
    }

    gParticles[idx] = p;
}
