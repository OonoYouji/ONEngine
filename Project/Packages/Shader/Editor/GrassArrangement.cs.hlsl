#include "../Render/Grass/Grass.hlsli"

#include "Terrain.hlsli"

struct UsedTexId {
	uint grassArrangementTexId; /// 草の配置する場所のテクスチャID
	uint terrainVertexTexId; /// 地形の頂点情報を持つテクスチャID
};

ConstantBuffer<UsedTexId> usedTexId : register(b0);
RWStructuredBuffer<BladeInstance> bladeInstances : register(u0);

/// テクスチャ配列
Texture2D<float4> textures[] : register(t0);
SamplerState textureSampler : register(s0);

/// 輝度を計算する関数
float CalculateLuminance(float4 color) {
    // 輝度を計算するための標準的な係数を使用 (Rec. 709)
	return dot(color.rgb, float3(0.2126, 0.7152, 0.0722));
}

/// 草の配置を行うためのshader
[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {

	// スレッドIDを間引く（例: 2x2の間隔で草を配置）
	if ((DTid.x % 2 != 0) || (DTid.y % 2 != 0)) {
		return; // 草を配置しない
	}

	uint index = DTid.x + DTid.y * 32; // 32x32のスレッドグループを想定

    /// 草のBufferサイズを取得して、範囲外アクセスを防止
	uint numStructs;
	uint structSize;
	bladeInstances.GetDimensions(numStructs, structSize);
	if (index >= numStructs) {
		return;
	}

	BladeInstance newInstance;

    /// 草の配置する場所のテクスチャを参照
	/// 地形のサイズ 1000x1000 に対して、テクスチャのUVを計算
	float terrainSize = 1000.0f; // 地形のサイズ
	float2 uv = float2(
		(DTid.x + 0.5) / terrainSize, // 横方向のUV計算
		(DTid.y + 0.5) / terrainSize  // 縦方向のUV計算
	);


	
	/// 草の配置テクスチャをサンプリング
	float4 texColor = textures[usedTexId.grassArrangementTexId].Sample(textureSampler, uv);
	/// 
	float luminance = CalculateLuminance(texColor);
	//if (luminance < 0.1f) {
	//	// 輝度が低い場所には草を生やさない
	//	newInstance.position = float3(0, -1000, 0); // 地面の下に配置して見えなくする
	//	newInstance.tangent = float3(0, 1, 0);
	//	newInstance.scale = 0;
	//	newInstance.random01 = 0;
	//	bladeInstances[index] = newInstance;
	//	return;
	//}
	
	
	float4 terrainVertex = textures[usedTexId.terrainVertexTexId].Sample(textureSampler, uv);
	float height = DenormalizeHeight(terrainVertex.g); // 高さ情報をY成分に格納していると仮定

	/// 地形のサイズに変換
	float3 pos = float3(uv.x, 0.0f, uv.y) * 1000.0f; // 例: 1000x1000の地形
	pos.y = height; // 高さを設定
	newInstance.position = pos;

    /// 草の向きをランダムに決定
	float angle = texColor.w * 6.28; // 0~1の値を0~2πに変換
	newInstance.tangent = float3(cos(angle), 0, sin(angle));
	newInstance.scale = 0.5 + (texColor.w * 0.5); // 0.5~1.0の範囲でスケールを決定
	newInstance.random01 = texColor.w; // 0~1のランダム値を保存

	bladeInstances[index] = newInstance;


}