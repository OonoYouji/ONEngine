
#include "Terrain.hlsli"

/// 地形の頂点データ

struct InputInfo {
	float2 position;
	float brushRadius;
	float brushStrength;
	int pressKey;
	int editMode;
	int editTextureIndex;
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

static const int vertexCount = 1000 * 1000;

[numthreads(256, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	uint index = DTid.x;
	if (index >= vertexCount) {
		return;
	}

	TerrainVertex vertex = vertices[index];
	
	float2 uv = inputInfo.position / float2(1280.0f, 720.0f); // マウス位置をUVに変換
	float3 mousePosition = positionTexture.Sample(textureSampler, uv).xyz; // マウス位置


	
	if (inputInfo.editMode == 0) {
		// ----- 頂点の編集 ----- //

		float distanceToBrush = distance(vertex.position.xz, mousePosition.xz); // XZ平面で距離を測る
		if (distanceToBrush < inputInfo.brushRadius) {
			float strength = inputInfo.brushStrength * (1.0f - (distanceToBrush / inputInfo.brushRadius)); // フェード
			if (IsInput(INPUT_POSITIVE)) {
				vertex.position.y += strength;
			} else if (IsInput(INPUT_NEGATIVE)) {
				vertex.position.y -= strength;
			}

			vertices[index] = vertex;
		}

	} else if (inputInfo.editMode == 1) {
		// ----- テクスチャの編集 ----- //
		
		float distanceToBrush = distance(vertex.position.xz, mousePosition.xz); // XZ平面で距離を測る
		if (distanceToBrush < inputInfo.brushRadius) {
			float strength = inputInfo.brushStrength * (1.0f - (distanceToBrush / inputInfo.brushRadius)); // フェード
			float prevBlend = vertex.splatBlend[inputInfo.editTextureIndex];
			if (IsInput(INPUT_POSITIVE)) {
				prevBlend += strength;
			} else if (IsInput(INPUT_NEGATIVE)) {
				prevBlend -= strength;
			}

			vertex.splatBlend[inputInfo.editTextureIndex] = clamp(prevBlend, 0.0f, 1.0f);
			vertices[index] = vertex;
		}
		
	}
	

	
}