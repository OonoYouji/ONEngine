#include "Line2D.hlsli"

VSOutput main(VSInput input) {
    VSOutput output;
	
    output.position.x = (input.position.x / input.screenSize.x) * 2.0f - 1.0f;
    output.position.y = 1.0f-(input.position.y / input.screenSize.y) * 2.0f;
    output.position.z = 0.0f;
    output.position.w = 1.0f;

    output.color = input.color;
	
    return output;
}