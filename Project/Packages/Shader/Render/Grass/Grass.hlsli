// Grass.hlsli
struct VertexOut {
	float4 position : SV_Position; // 頂点位置（Pixel Shader へ渡す）
	float3 normal : NORMAL; // 法線
	float2 uv : TEXCOORD; // UV
};


static const uint kMaxBladeVertexNum = 5;
// 草の三角形1本分の頂点
static const float3 bladeVertices[kMaxBladeVertexNum] = {
	float3(0, 0, 0),       /// 根元
	float3(-0.05, 0.5, 0), /// 左上
	float3(0.05, 0.5, 0),  /// 右上
	float3(0, 0.5, 0.05),  /// 奥上
	float3(0, 0.5, -0.05)  /// 手前上
};

static const float2 bladeUVs[kMaxBladeVertexNum] = {
	float2(0.5f, 1.0f), /// 根本
	float2(0.0f, 0.0f), /// 左上
	float2(1.0f, 0.0f), /// 右上
	float2(1.0f, 0.0f), /// 奥上
	float2(0.0f, 0.0f)  /// 手前上
};


struct BladeInstance {
	float3 position;
	float3 tangent;
	float scale;
	float random01;
};


struct GrassData {
	uint index;
	bool isCulled;
};

static const uint kMaxGrassDataSize = 32;
static const uint kMaxRenderingGrassSize = 51;
struct Payload {
	//GrassData grassData[kMaxRenderingGrassSize];
	//uint startIndex;
	uint startIndices[64]; /// MSのnumthreadsに合わせる
};