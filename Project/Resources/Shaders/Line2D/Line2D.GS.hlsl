#include "Line2D.hlsli"

[maxvertexcount(6)]
void main(line VSOutput input[2], inout TriangleStream<GSOutput> output) {
	float lineWidth = 0.005; // 線の幅（ビュー空間での幅）

    // 始点と終点の位置を取得
	float3 start = input[0].position.xyz;
	float3 end = input[1].position.xyz;

    // ラインの方向ベクトルを計算
	float3 direction = normalize(end - start);
	float3 perpendicular = float3(-direction.y, direction.x, 0.0);
	float3 offset = perpendicular * lineWidth;

    // 頂点の位置を設定
	GSOutput gsOutput;
	gsOutput.color = input[0].color; // 必要な色を設定

	gsOutput.position = float4(start + offset, 1.0f);
	output.Append(gsOutput);
	gsOutput.position = float4(end + offset, 1.0f);
	output.Append(gsOutput);
	gsOutput.position = float4(end - offset, 1.0f);
	output.Append(gsOutput);
	output.RestartStrip();


	gsOutput.position = float4(start + offset, 1.0f);
	output.Append(gsOutput);
	gsOutput.position = float4(end - offset, 1.0f);
	output.Append(gsOutput);
	gsOutput.position = float4(start - offset, 1.0f);
	output.Append(gsOutput);
	output.RestartStrip();
}