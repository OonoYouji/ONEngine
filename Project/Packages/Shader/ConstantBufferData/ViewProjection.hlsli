struct ViewProjection {
	float4x4 matVP;
	float4x4 matView;
	float4x4 matProjection;
};

struct Camera {
	float4 position;
};

struct Plane {
	float4 plane;
	// xyz = normal, w = -distance
};

struct Frustum {
	Plane planes[6];
	/// 0: Left
	/// 1: Right
	/// 2: Bottom
	/// 3: Top
	/// 4: Near
	/// 5: Far
};

struct AABB {
	float3 min;
	float3 max;
};

// LH座標系、World空間でのAABBと直接比較可能
Frustum CreateFrustumFromMatrix(float4x4 matVP) {
	Frustum f;

    // Left plane
	f.planes[0].plane = matVP[3] + matVP[0];
    // Right plane
	f.planes[1].plane = matVP[3] - matVP[0];
    // Bottom plane
	f.planes[2].plane = matVP[3] + matVP[1];
    // Top plane
	f.planes[3].plane = matVP[3] - matVP[1];
    // Near plane
	f.planes[4].plane = matVP[3] + matVP[2];
    // Far plane
	f.planes[5].plane = matVP[3] - matVP[2];

    // 法線と距離を正規化
    [unroll]
	for (int i = 0; i < 6; ++i) {
		float3 n = f.planes[i].plane.xyz;
		float len = length(n);
		f.planes[i].plane /= len; // xyz と w を同じ比率で割る
	}

	return f;
}

bool IsVisible(AABB box, Frustum frustum) {
	float3 vertices[8] = {
		box.min,
        float3(box.max.x, box.min.y, box.min.z),
        float3(box.min.x, box.max.y, box.min.z),
        float3(box.max.x, box.max.y, box.min.z),
        float3(box.min.x, box.min.y, box.max.z),
        float3(box.max.x, box.min.y, box.max.z),
        float3(box.min.x, box.max.y, box.max.z),
        box.max
	};

    [unroll]
	for (int i = 0; i < 6; ++i) {
		bool allOutside = true;
        [unroll]
		for (int v = 0; v < 8; ++v) {
			float d = dot(frustum.planes[i].plane.xyz, vertices[v]) + frustum.planes[i].plane.w;
			if (d >= 0) {
				allOutside = false;
				break;
			}
		}
		if (allOutside)
			return false;
	}
	return true;
}
