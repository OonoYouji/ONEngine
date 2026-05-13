#include "../Schema/Buffers.hlsli"

struct ParticleUpdateParams {
    float dt;
    float3 emitterPos;
    uint totalParticles;

    float seed;
    float speed;
    float speedRandom;
    float lifetime;

    float lifetimeRandom;
    float spreadAngle;
    float gravity;
    float startScale;

    float endScale;
    uint modelIndex;
    uint textureIndex;
    float padding;

    float4 startColor;
    float4 endColor;
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

float3 random_unit_vector(uint seed) {
    float z = rand(seed) * 2.0f - 1.0f;
    float a = rand(seed + 1) * 2.0f * 3.14159f;
    float r = sqrt(1.0f - z * z);
    float x = r * cos(a);
    float y = r * sin(a);
    return float3(x, y, z);
}

[numthreads(256, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
    uint idx = DTid.x;
    if (idx >= gParams.totalParticles) return;

    ParticleGPUData p = gParticles[idx];
    p.age += gParams.dt;

    if (p.age >= p.maxLifetime) {
        uint s = idx + (uint)(gParams.seed * 1000.0f);
        
        p.position = gParams.emitterPos;
        p.age = 0;
        p.maxLifetime = gParams.lifetime * (1.0f + (rand(s) * 2.0f - 1.0f) * gParams.lifetimeRandom);
        
        // Conical emission
        float3 baseDir = float3(0, 1, 0); // Upward by default
        float3 randDir = random_unit_vector(s + 10);
        float angle = radians(gParams.spreadAngle) * rand(s + 20);
        float3 dir = normalize(lerp(baseDir, randDir, angle / 3.14159f));

        float v = gParams.speed * (1.0f + (rand(s + 30) * 2.0f - 1.0f) * gParams.speedRandom);
        p.velocity = dir * v;

        p.modelIndex = gParams.modelIndex;
        p.textureIndex = gParams.textureIndex;
    } else {
        p.velocity.y -= gParams.gravity * gParams.dt;
        p.position += p.velocity * gParams.dt;
        p.rotation.y += 2.0f * gParams.dt;
        p.rotation.x += 1.0f * gParams.dt;
    }

    // Animation
    float t = saturate(p.age / p.maxLifetime);
    p.color = lerp(gParams.startColor, gParams.endColor, t);
    p.scale = lerp(gParams.startScale, gParams.endScale, t);

    gParticles[idx] = p;
}
