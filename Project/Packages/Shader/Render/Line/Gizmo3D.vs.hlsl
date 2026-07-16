#include "Gizmo3D.hlsli"
#include "../../ConstantBufferData/ViewProjection.hlsli"

ConstantBuffer<ViewProjection> viewProjection : register(b0);

VSOutput main(VSInput input) {
	VSOutput output;

	// 始点と終点を投影
	float4 pStart = mul(input.position, viewProjection.matVP);
	float4 pEnd = mul(input.otherPosition, viewProjection.matVP);

	// w除算してNDC空間へ (-1 ~ 1)
	float2 ndc0 = pStart.xy / pStart.w;
	float2 ndc1 = pEnd.xy / pEnd.w;

	// スクリーン解像度
	float2 screenRes = float2(1920.0, 1080.0);
	float aspect = screenRes.x / screenRes.y;
	
	// スクリーンスペースでの方向 (アスペクト比補正)
	float2 dir = ndc1 - ndc0;
	dir.x *= aspect;
	
	// 線が非常に短い場合のガード
	float len = length(dir);
	float2 normal;
	if (len < 0.0001) {
		normal = float2(0, 1);
	} else {
		dir /= len;
		normal = float2(-dir.y, dir.x);
	}

	// アスペクト比補正を戻す
	normal.x /= aspect;

	// 現在の頂点が始点側か終点側か
	float4 basePos = (input.expansionDir.y > 0.5) ? pEnd : pStart;

	// 拡大オフセットの計算
	// NDC空間の全幅は 2.0 なので、1ピクセルは 2.0 / screenRes
	// 太さの半分(0.5)だけずらす
	float2 pixelToNDC = 2.0 / screenRes;
	float2 offset = normal * input.expansionDir.x * (input.thickness * 0.5) * pixelToNDC;
	
	output.position = basePos;
	output.position.xy += offset * basePos.w;

	output.worldPosition = input.position;
	output.color = input.color;
	
	return output;
}