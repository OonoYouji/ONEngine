
/// 描画時にピクセルシェーダーから出力する構造体
struct Pixel {
    float32_t4 color         : SV_TARGET0;
    float32_t4 worldPosition : SV_TARGET1;
    float32_t4 normal        : SV_TARGET2;
    float32_t4 flags         : SV_TARGET3;
};