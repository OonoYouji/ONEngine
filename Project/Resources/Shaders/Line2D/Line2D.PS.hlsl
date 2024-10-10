#include "Line2D.hlsli"

PSOutput main(GSOutput input) {
	PSOutput output;
	output.color = input.color;
	return output;
}