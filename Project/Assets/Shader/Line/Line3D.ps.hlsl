#include "Line3D.hlsli"

PSOutput main(VSOutput input) {
	PSOutput output;
	output.color = input.color;
	return output;
}