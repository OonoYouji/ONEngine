struct SpotLight {
	float4 color;
	float3 position;
	float  intensity;
	float3 direction;
	float  distance;
	float  decay;
	float  cosAngle;
	float  cosFalloffStart;
	int    active;
};