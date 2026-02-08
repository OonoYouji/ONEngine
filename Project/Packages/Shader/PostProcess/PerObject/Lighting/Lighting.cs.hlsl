#include "../../../ConstantBufferData/ViewProjection.hlsli"
#include "../../../ConstantBufferData/Material.hlsli"

struct DirectionalLight {
    float4 position;
    float4 color;
    float3 direction;
    float intensity;
};

ConstantBuffer<DirectionalLight> light : register(b0);
ConstantBuffer<Camera> camera : register(b1);

Texture2D<float4> colorTex : register(t0);
Texture2D<float4> positionTex : register(t1);
Texture2D<float4> normalTex : register(t2);
Texture2D<float4> flagsTex : register(t3);
TextureCube<float4> environmentTexture : register(t4);

RWTexture2D<float4> outputTex : register(u0);
SamplerState textureSampler : register(s0);

static const float2 kTextureSize = float2(1920.0f, 1080.0f);

//////////////////////////////////////////////////////////////
// Lambert Diffuse
//////////////////////////////////////////////////////////////
float3 LambertDiffuse(float3 albedo, float3 N, float3 L)
{
    float NdotL = saturate(dot(N, L));
    return albedo * NdotL;
}

//////////////////////////////////////////////////////////////
// Blinn-Phong Specular
//////////////////////////////////////////////////////////////
float3 BlinnPhongSpecular(float3 N, float3 L, float3 V)
{
    float3 H = normalize(L + V);

    float specPower = 32.0f; // 適宜調整
    float spec = pow(saturate(dot(N, H)), specPower);

    return spec.xxx;
}

//////////////////////////////////////////////////////////////
// Fresnel (Schlick approximation)
//////////////////////////////////////////////////////////////
float FresnelSchlick(float3 N, float3 V)
{
    float f = 1.0f - saturate(dot(N, V));
    return pow(f, 5.0f);
}

//////////////////////////////////////////////////////////////
// Environment Reflection
//////////////////////////////////////////////////////////////
float3 EnvironmentReflection(float3 position, float3 normal)
{
    float3 V = normalize(camera.position.xyz - position);

    float3 reflectVec = reflect(-V, normal);

    float3 env = environmentTexture.Sample(textureSampler, reflectVec).rgb;

    float fresnel = FresnelSchlick(normal, V);

    return env * fresnel;
}

//////////////////////////////////////////////////////////////
// Main
//////////////////////////////////////////////////////////////
[numthreads(16, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    float2 texCoord = float2(DTid.xy + 0.5f) / kTextureSize;

    float4 color = colorTex.Sample(textureSampler, texCoord);
    float4 position = positionTex.Sample(textureSampler, texCoord);
    float4 normal = normalTex.Sample(textureSampler, texCoord);
    float4 flags = flagsTex.Sample(textureSampler, texCoord);

    float3 albedo = color.rgb;
    float3 worldPos = position.xyz;
    float3 N = normalize(normal.xyz);

    //////////////////////////////////////////////////////////
    // Lighting無効
    //////////////////////////////////////////////////////////
    if (!IsPostEffectEnabled((int)flags.x, PostEffectFlags_Lighting))
    {
        float3 outputColor = albedo;

        if (IsPostEffectEnabled((int)flags.x, PostEffectFlags_EnvironmentReflection))
        {
            outputColor += EnvironmentReflection(worldPos, N);
        }

        outputTex[DTid.xy] = float4(outputColor, 1.0f);
        return;
    }

    //////////////////////////////////////////////////////////
    // Lighting
    //////////////////////////////////////////////////////////
    float3 L = normalize(-light.direction);
    float3 V = normalize(camera.position.xyz - worldPos);

    // Diffuse
    float3 diffuse = LambertDiffuse(albedo, N, L);

    // Specular
    float3 specular = BlinnPhongSpecular(N, L, V);

    float3 lighting = (diffuse + specular) * light.color.rgb * light.intensity;

    //////////////////////////////////////////////////////////
    // Environment Reflection
    //////////////////////////////////////////////////////////
    if (IsPostEffectEnabled((int)flags.x, PostEffectFlags_EnvironmentReflection))
    {
        lighting += EnvironmentReflection(worldPos, N);
    }

    outputTex[DTid.xy] = float4(lighting, 1.0f);
}
