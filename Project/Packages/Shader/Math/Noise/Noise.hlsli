/// 2D hash function (pseudo random number generator)
uint Hash(uint _x) {
	uint x = _x;
	x ^= x >> 16;
	x *= 0x7feb352d;
	x ^= x >> 15;
	x *= 0x846ca68b;
	x ^= x >> 16;
	return x;
}

float Rand(float2 _p) {
	uint x = asuint(_p.x) ^ (asuint(_p.y) << 16);
	return frac(float(Hash(x)) / 4294967296.0);
}

/// Linear interpolation
float Lerp(float _a, float _b, float _t) {
	return _a + _t * (_b - _a);
}

/// Fade function (6t^5 - 15t^4 + 10t^3)
float Fade(float _t) {
	return _t * _t * _t * (_t * (_t * 6 - 15) + 10);
}

/// Gradient vector
float2 Grad(int _hash) {
	int h = _hash & 3;
	float2 grad = float2(0, 0);
	switch (h) {
		case 0:
			grad = float2(1, 1);
			break;
		case 1:
			grad = float2(-1, 1);
			break;
		case 2:
			grad = float2(-1, -1);
			break;
		default:
			grad = float2(1, -1);
			break;
	}
	return grad;
}


/// Perlin noise main function
float PerlinNoise(float2 _p) {
	int x0 = (int) floor(_p.x);
	int y0 = (int) floor(_p.y);
	int x1 = x0 + 1;
	int y1 = y0 + 1;

	float2 f = frac(_p);

    /// Get hash values for 4 corners
	int h00 = (int) (Hash(asuint(float2(x0, y0).x) ^ asuint(float2(x0, y0).y)));
	int h10 = (int) (Hash(asuint(float2(x1, y0).x) ^ asuint(float2(x1, y0).y)));
	int h01 = (int) (Hash(asuint(float2(x0, y1).x) ^ asuint(float2(x0, y1).y)));
	int h11 = (int) (Hash(asuint(float2(x1, y1).x) ^ asuint(float2(x1, y1).y)));

    /// Gradient vectors
	float2 g00 = Grad(h00);
	float2 g10 = Grad(h10);
	float2 g01 = Grad(h01);
	float2 g11 = Grad(h11);

    /// Vectors from corners to point
	float2 d00 = f - float2(0.0, 0.0);
	float2 d10 = f - float2(1.0, 0.0);
	float2 d01 = f - float2(0.0, 1.0);
	float2 d11 = f - float2(1.0, 1.0);

    /// Dot products
	float v00 = dot(g00, d00);
	float v10 = dot(g10, d10);
	float v01 = dot(g01, d01);
	float v11 = dot(g11, d11);

    /// Fade curves
	float u = Fade(f.x);
	float v = Fade(f.y);

    /// Interpolation
	float x1Interp = Lerp(v00, v10, u);
	float x2Interp = Lerp(v01, v11, u);
	float result = Lerp(x1Interp, x2Interp, v);

    /// Convert result from [-1,1] to [0,1]
	return result * 0.5 + 0.5;
}
