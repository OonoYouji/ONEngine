
float4x4 InverseMatrix(float4x4 _m) {

		float a = _m[0][0]; float b = _m[0][1]; float c = _m[0][2]; float d = _m[0][3];
		float e = _m[1][0]; float f = _m[1][1]; float g = _m[1][2]; float h = _m[1][3];
		float i = _m[2][0]; float j = _m[2][1]; float k = _m[2][2]; float l = _m[2][3];
		float m4 = _m[3][0]; float n = _m[3][1]; float o = _m[3][2]; float p = _m[3][3];

		float kp_lo = k * p - l * o;
		float jp_ln = j * p - l * n;
		float jo_kn = j * o - k * n;
		float ip_lm4 = i * p - l * m4;
		float io_km4 = i * o - k * m4;
		float in_jm4 = i * n - j * m4;

		float det =
			a * (f * kp_lo - g * jp_ln + h * jo_kn)
		  - b * (e * kp_lo - g * ip_lm4 + h * io_km4)
		  + c * (e * jp_ln - f * ip_lm4 + h * in_jm4)
		  - d * (e * jo_kn - f * io_km4 + g * in_jm4);

		if (abs(det) < 1e-8) {
			// 逆行列が存在しない -> 単位行列を返す
			return float4x4(
				1.0, 0.0, 0.0, 0.0,
				0.0, 1.0, 0.0, 0.0,
				0.0, 0.0, 1.0, 0.0,
				0.0, 0.0, 0.0, 1.0
			);
		}

		float invDet = 1.0 / det;

		float4x4 r;

		// 1 行目
		r[0][0] = (f * kp_lo - g * jp_ln + h * jo_kn) * invDet;
		r[0][1] = -(b * kp_lo - c * jp_ln + d * jo_kn) * invDet;
		r[0][2] = (b * (g * p - h * o) - c * (f * p - h * n) + d * (f * o - g * n)) * invDet;
		r[0][3] = -(b * (g * m4 - h * k) - c * (f * m4 - h * i) + d * (f * k - g * i)) * invDet;

		// 2 行目
		r[1][0] = -(e * kp_lo - g * ip_lm4 + h * io_km4) * invDet;
		r[1][1] = (a * kp_lo - c * ip_lm4 + d * io_km4) * invDet;
		r[1][2] = -(a * (g * p - h * o) - c * (e * p - h * m4) + d * (e * o - g * m4)) * invDet;
		r[1][3] = (a * (g * m4 - h * k) - c * (e * m4 - h * i) + d * (e * k - g * i)) * invDet;

		// 3 行目
		r[2][0] = (e * jp_ln - f * ip_lm4 + h * in_jm4) * invDet;
		r[2][1] = -(a * jp_ln - b * ip_lm4 + d * in_jm4) * invDet;
		r[2][2] = (a * (f * p - h * n) - b * (e * p - h * m4) + d * (e * n - f * m4)) * invDet;
		r[2][3] = -(a * (f * m4 - h * j) - b * (e * m4 - h * i) + d * (e * j - f * i)) * invDet;

		// 4 行目
		r[3][0] = -(e * jo_kn - f * io_km4 + g * in_jm4) * invDet;
		r[3][1] = (a * jo_kn - b * io_km4 + c * in_jm4) * invDet;
		r[3][2] = -(a * (f * o - g * n) - b * (e * o - g * m4) + c * (e * n - f * m4)) * invDet;
		r[3][3] = (a * (f * k - g * j) - b * (e * k - g * i) + c * (e * j - f * i)) * invDet;

		return r;
}