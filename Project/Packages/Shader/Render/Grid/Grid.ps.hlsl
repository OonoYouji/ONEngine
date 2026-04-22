#include "Grid.hlsli"
#include "../../Pixel.hlsli"

// 指定したスケール(間隔)のグリッド線のアルファ値を計算する関数
float CalculateGrid(float2 worldPosXZ, float scale)
{
    float2 coord = worldPosXZ / scale;
    
    // ピクセル間の座標の変化量を取得（これでどんなに離れても線の太さが一定になる）
    float2 derivative = fwidth(coord);

    // フラクショナル(小数)部分を使って線を計算
    float2 grid = abs(frac(coord - 0.5f) - 0.5f) / derivative;
    float line_ = min(grid.x, grid.y);

    // 線の中心が最も濃く(1.0)、縁に行くほど薄くなるように反転
    // 係数(1.0f)を変えると線の太さが変わります
    return 1.0f - min(line_, 1.0f);
}

// ==========================================
// ピクセルシェーダー
// ==========================================

Pixel main(PSInput input)
{
    float2 posXZ = input.worldPosition.xz;

    // 1m間隔の細いグリッドと、10m間隔の太いグリッドを計算
    float grid1m = CalculateGrid(posXZ, 1.0f);
    float grid10m = CalculateGrid(posXZ, 10.0f);

    // 2つのグリッドを合成（1mは薄く、10mは濃くする）
    float finalGridAlpha = max(grid1m * 0.3f, grid10m);

    // 基本のグリッド色（少し暗めのグレー）
    float4 gridColor = float4(0.5f, 0.5f, 0.5f, finalGridAlpha);

    // --- 原点の軸（X軸とZ軸）の色分け ---
    // ワールド座標が0に近い場所を検知して色を上書きする
    float2 derivPos = fwidth(posXZ);
    float2 axisWidth = derivPos * 1.5f; // 軸の線の太さ

    if (abs(input.worldPosition.z) < axisWidth.y) // X軸 (Z=0の線)
    {
        gridColor = float4(1.0f, 0.2f, 0.2f, finalGridAlpha * 1.5f); // 赤
    }
    if (abs(input.worldPosition.x) < axisWidth.x) // Z軸 (X=0の線)
    {
        gridColor = float4(0.2f, 0.2f, 1.0f, finalGridAlpha * 1.5f); // 青
    }

    // --- カメラ距離によるフェードアウト ---
    float distanceToCamera = length(input.worldPosition - cCamera.position.xyz);
    float maxFadeDistance = 500.0f; // 100m先で完全に透明になる
    float fade = max(0.0f, 1.0f - (distanceToCamera / maxFadeDistance));

    gridColor.a *= fade;

    // 完全に透明なピクセルは描画を破棄して負荷を下げる
	if (gridColor.a <= 0.01f) {
		discard;
	}

    Pixel output;
	output.color = gridColor;
    output.worldPosition = float4(input.worldPosition, 1.0f);
    output.normal = float4(0.0f, 1.0f, 0.0f, 0.0f);
    output.flags = float4(0.0f, 0.0f, 0.0f, 0.0f);
    return output;
}