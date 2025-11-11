struct UVTransform {
    float2 position;
    float2 scale;
    float  rotate;
    float  pad1[3];
};

struct Material {
    UVTransform uvTransform;  // 32 bytes
    float4     baseColor;     // 16 bytes
    uint       postEffectFlags;
    int        entityId;
    int        baseTextureId;
    int        normalTextureId;
};

struct ConstantUVTransform {
    float2 position;
    float2 scale;
    float  rotate;
};

struct ConstantBufferMaterial {
    ConstantUVTransform uvTransform;  // 32 bytes
    float4     baseColor;         // 16 bytes
    int4       intValues;     // 16 bytes
    /// intValues.x : postEffectFlags
    /// intValues.y : entityId
    /// intValues.z : baseTextureId
    /// intValues.w : normalTextureId
};


static const int PostEffectFlags_None                  = 0;
static const int PostEffectFlags_Lighting              = 1 << 0;
static const int PostEffectFlags_Grayscale             = 1 << 1;
static const int PostEffectFlags_EnvironmentReflection = 1 << 2;
static const int PostEffectFlags_Shadow                = 1 << 3;


bool IsPostEffectEnabled(int flags, int effect) {
	return (flags & effect) != 0;
}

float3x3 MatUVTransformToMatrix(UVTransform uvTransform) {
	float cosTheta = cos(uvTransform.rotate);
	float sinTheta = sin(uvTransform.rotate);
	
	return float3x3(
		uvTransform.scale.x * cosTheta, uvTransform.scale.x * -sinTheta, 0,
		uvTransform.scale.y * sinTheta, uvTransform.scale.y * cosTheta, 0,
		uvTransform.position.x, uvTransform.position.y, 1
	);
}

float3x3 MatUVTransformToMatrix(ConstantUVTransform uvTransform) {
    float cosTheta = cos(uvTransform.rotate);
    float sinTheta = sin(uvTransform.rotate);
    
    return float3x3(
        uvTransform.scale.x * cosTheta, uvTransform.scale.x * -sinTheta, 0,
        uvTransform.scale.y * sinTheta, uvTransform.scale.y * cosTheta, 0,
        uvTransform.position.x, uvTransform.position.y, 1
    );
}