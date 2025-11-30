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
	Frustum frustum;

    // Left plane
	frustum.planes[0].plane = float4(
        matVP[0][3] + matVP[0][0],
        matVP[1][3] + matVP[1][0],
        matVP[2][3] + matVP[2][0],
        matVP[3][3] + matVP[3][0]
    );

    // Right plane
	frustum.planes[1].plane = float4(
        matVP[0][3] - matVP[0][0],
        matVP[1][3] - matVP[1][0],
        matVP[2][3] - matVP[2][0],
        matVP[3][3] - matVP[3][0]
    );

    // Bottom plane
	frustum.planes[2].plane = float4(
        matVP[0][3] + matVP[0][1],
        matVP[1][3] + matVP[1][1],
        matVP[2][3] + matVP[2][1],
        matVP[3][3] + matVP[3][1]
    );

    // Top plane
	frustum.planes[3].plane = float4(
        matVP[0][3] - matVP[0][1],
        matVP[1][3] - matVP[1][1],
        matVP[2][3] - matVP[2][1],
        matVP[3][3] - matVP[3][1]
    );

    // Near plane
	frustum.planes[4].plane = float4(
        matVP[0][2],
        matVP[1][2],
        matVP[2][2],
        matVP[3][2]
    );

    // Far plane
	frustum.planes[5].plane = float4(
        matVP[0][3] - matVP[0][2],
        matVP[1][3] - matVP[1][2],
        matVP[2][3] - matVP[2][2],
        matVP[3][3] - matVP[3][2]
    );

    // Normalize the planes
    [unroll]
	for (int i = 0; i < 6; ++i) {
		float3 normal = frustum.planes[i].plane.xyz;
		float len = length(normal);
		frustum.planes[i].plane /= len; // Normalize xyz and w
	}

	return frustum;
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
		if (allOutside) {
			return false;
		}
	}
	return true;
}
