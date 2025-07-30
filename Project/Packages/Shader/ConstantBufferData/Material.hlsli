
struct Material {
	float4 color;
	int postEffectFlags;
	int entityId;
};

static const int PostEffectFlags_None      = 0;
static const int PostEffectFlags_Lighting  = 1 << 0;
static const int PostEffectFlags_Grayscale = 1 << 1;
static const int PostEffectFlags_Bloom     = 1 << 2;
static const int PostEffectFlags_Outline   = 1 << 3;


bool IsPostEffectEnabled(int flags, int effect) {
	return (flags & effect) != 0;
}
