
#include "Terrain.hlsli"

/// 地形の頂点データ

struct InputInfo {
	float2 position;
	float brushRadius;
	float brushStrength;
	int pressKey;
};

ConstantBuffer<TerrainInfo> terrainInfo : register(b0);
ConstantBuffer<InputInfo> inputInfo : register(b1);

RWStructuredBuffer<TerrainVertex> vertices : register(u0);

Texture2D<float4> positionTexture : register(t0);
Texture2D<float4> flagsTexture : register(t1);
SamplerState textureSampler : register(s0);

/// 入力のフラグ
static const int INPUT_POSITIVE = 1 << 0; // 押し上げに使用するキー入力フラグ
static const int INPUT_NEGATIVE = 1 << 1; // 押し下げに使用するキー入力フラグ

/// 引数のボタンが入力されているか
bool IsInput(int input) {
	return (inputInfo.pressKey & input) != 0;
}


[numthreads(16, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {

	float2 texUv = float2(DTid.x / 1920.0f, DTid.y / 1080.0f);
	float4 flags = flagsTexture.Sample(textureSampler, texUv);

	/// レンダリング位置が地形かどうか確認
	if ((int) flags.y != terrainInfo.entityId) {
		return; /// 違った
	}
	
	/// マウスとの位置を計算
	float4 position = positionTexture.Sample(textureSampler, texUv);
	float4 mousePosition = positionTexture.Sample(textureSampler, inputInfo.position / float2(1920.0f, 1080.0f));
	float length = distance(position.xyz, mousePosition.xyz);
	if (length > inputInfo.brushRadius) {
		return; /// マウス位置から離れすぎている
	}
	
	
	int index = (int) flags.z;
	if (index >= 1000) {
		return;
	}

	/// 範囲内なら
	Vertex vertex = vertices[index];
	if (IsInput(INPUT_POSITIVE)) {
		/// 押し上げ
		vertex.position.y += inputInfo.brushStrength;
	} else if (IsInput(INPUT_NEGATIVE)) {
		/// 押し下げ
		vertex.position.y -= inputInfo.brushStrength;
	}
	
	vertices[index] = vertex;
	
}