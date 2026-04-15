#include "Grid.hlsli"

PSInput main(VSInput input) {
	PSInput output;
    
    // ローカル座標をワールド座標へ変換
	output.worldPosition = mul(input.position, gGridWorldMatrix).xyz;
    
    // ワールド座標からクリップ空間（画面上の位置）へ変換
	output.position = mul(float4(output.worldPosition, 1.0f), cViewProjection.matVP);
    
	return output;
}