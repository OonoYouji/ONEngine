#include "../Render/Grass/Grass.hlsli"

struct UsedTexId {
	uint grassArrangementTexId;
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
[numthreads(64, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	uint index = DTid.x;

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
	float2 uv = float2(
		((index % 256) + 0.5) / 256.0f, // 横方向に256個配置
		((index / 256) + 0.5) / 256.0f  // 縦方向に256個配置
	);

	/// 草の配置テクスチャをサンプリング
	float4 texColor = textures[usedTexId.grassArrangementTexId].Sample(textureSampler, uv);
	/// 
	float luminance = CalculateLuminance(texColor);
	//if(luminance < 0.1f){
	//	// 輝度が低い場所には草を生やさない
	//	newInstance.position = float3(0, -1000, 0); // 地面の下に配置して見えなくする
	//	newInstance.tangent = float3(0, 1, 0);
	//	newInstance.scale = 0;
	//	newInstance.random01 = 0;
	//	bladeInstances[index] = newInstance;
	//	return;
	//}
	

	/// 地形のサイズに変換
	float3 pos = float3(uv.x, 0.0f, uv.y) * 1000.0f; // 例: 1000x1000の地形
	newInstance.position = pos;

    /// 草の向きをランダムに決定
	float angle = texColor.w * 6.28; // 0~1の値を0~2πに変換
	newInstance.tangent = float3(cos(angle), 0, sin(angle));
	newInstance.scale = 0.5 + (texColor.w * 0.5); // 0.5~1.0の範囲でスケールを決定
	newInstance.random01 = texColor.w; // 0~1のランダム値を保存

	bladeInstances[index] = newInstance;
}