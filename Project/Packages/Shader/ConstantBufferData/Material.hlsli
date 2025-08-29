struct UVTransform {
	float2 offset;
	float2 scale;
	float  rotate;
	float pad1;
};

struct Material {
	UVTransform uvTransform;
	float4      color;
	int         postEffectFlags;
	int         entityId;
	int         baseTextureId;
	int         normalTextureId;
};

static const int PostEffectFlags_None                  = 0;
static const int PostEffectFlags_Lighting              = 1 << 0;
static const int PostEffectFlags_Grayscale             = 1 << 1;
static const int PostEffectFlags_EnvironmentReflection = 1 << 2;


bool IsPostEffectEnabled(int flags, int effect) {
	return (flags & effect) != 0;
}

float3x3 MatUVTransformToMatrix(UVTransform uvTransform) {
	float cosTheta = cos(uvTransform.rotate);
	float sinTheta = sin(uvTransform.rotate);
	
	return float3x3(
		uvTransform.scale.x * cosTheta, uvTransform.scale.x * -sinTheta, 0,
		uvTransform.scale.y * sinTheta, uvTransform.scale.y * cosTheta, 0,
		uvTransform.offset.x, uvTransform.offset.y, 1
	);
}