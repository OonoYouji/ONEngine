#include "Sprite.hlsli"

VSOutput main(VSInput input) {
	VSOutput output;

	output.position = input.position;
	output.uv       = input.uv;

	return output;
}