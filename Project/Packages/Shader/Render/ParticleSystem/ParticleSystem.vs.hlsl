#include "ParticleSystem.hlsli"

#include "../../ConstantBufferData/ViewProjection.hlsli"

struct Particle {
    float3 position;
    float3 velocity;
    float4 color;
    float startLifetime;
    float remainingLifetime;
    float size;
    float rotation;
    float4 startColor;
    float startSize;
    float3 baseVelocity;
    float randomValue;
    uint simulationSpace;
};

ConstantBuffer<ViewProjection> viewProjection : register(b0);
StructuredBuffer<Particle> particles : register(t0);

// Global camera data
cbuffer GlobalCameraData : register(b1) {
    float4x4 billboardMatrix;
    float3 cameraPosition;
    float padding;
}

// Per-system data (Root Constants)
cbuffer PerSystemData : register(b2) {
    float4x4 emitterWorldMatrix;
    uint renderMode;
    uint renderAlignment;
    float speedScale;
    float lengthScale;
    uint instanceOffset;
}

VSOutput main(VSInput input, uint instanceId : SV_InstanceID) {
    VSOutput output;

    uint instanceIndex = instanceId + instanceOffset;
    Particle p = particles[instanceIndex];

    // Get particle center and velocity in world space
    float3 worldCenter;
    float3 worldVelocity = p.velocity;
    if (p.simulationSpace == 1) { // Local
        worldCenter = mul(float4(p.position, 1.0f), emitterWorldMatrix).xyz;
        worldVelocity = mul(float4(p.velocity, 0.0f), emitterWorldMatrix).xyz;
    } else { // World
        worldCenter = p.position;
    }

    // Billboarding: rotate the quad to face the camera
    float4 localPos = input.position;
    
    // Determine basis vectors for the billboard
    float3 right, up, forward;
    float3 viewDir = normalize(cameraPosition - worldCenter);
    
    uint currentAlignment = renderAlignment;
    if (renderMode == 1) currentAlignment = 1; // Stretched Billboard implies Velocity Alignment

    if (currentAlignment == 1) { // Velocity Alignment
        up = normalize(worldVelocity);
        if (length(worldVelocity) < 0.001f) up = float3(0, 1, 0); // Fallback
        
        right = normalize(cross(up, viewDir));
        if (length(right) < 0.001f) {
            // viewDir is parallel to up. Pick any other vector.
            float3 other = abs(up.y) > 0.99f ? float3(1, 0, 0) : float3(0, 1, 0);
            right = normalize(cross(up, other));
        }
        forward = cross(right, up);
    } else {
        // Standard View Alignment
        right = billboardMatrix[0].xyz;
        up = billboardMatrix[1].xyz;
        forward = billboardMatrix[2].xyz;
    }

    // Apply rotation around the quad's local Z (now our forward direction)
    float s = sin(p.rotation);
    float c = cos(p.rotation);
    float3 rotRight = right * c + up * s;
    float3 rotUp = -right * s + up * c;

    // Apply scaling
    float currentSize = p.size;
    float3 localScale = float3(currentSize, currentSize, currentSize);
    
    if (renderMode == 1) { // Stretched Billboard
        float speed = length(worldVelocity);
        // Stretched billboards are scaled along the 'up' axis (velocity direction)
        // Unity style: length = size * (lengthScale + speed * speedScale)
        // Here we use a slightly simpler version or the same
        localScale.y = currentSize * (lengthScale + speed * speedScale);
    }

    // Calculate final world position
    float3 worldPosOffset = rotRight * (localPos.x * localScale.x) + rotUp * (localPos.y * localScale.y);
    float4 worldPos;
    worldPos.xyz = worldCenter + worldPosOffset;
    worldPos.w = 1.0f;

    output.position = mul(worldPos, viewProjection.matVP);
    output.worldPosition = worldPos;
    output.normal = normalize(cross(rotRight, rotUp));
    output.uv = input.uv;
    output.instanceId = instanceIndex;
    output.color = p.color;
    
    return output;
}
