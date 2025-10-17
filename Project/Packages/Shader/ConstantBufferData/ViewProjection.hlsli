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

//Frustum CreateFrustumFromMatrix(float4x4 matVP) {
//	Frustum f;

//	f.planes[0].plane = matVP[3] + matVP[0]; // Left
//	f.planes[1].plane = matVP[3] - matVP[0]; // Right
//	f.planes[2].plane = matVP[3] + matVP[1]; // Bottom
//	f.planes[3].plane = matVP[3] - matVP[1]; // Top
//	f.planes[4].plane = matVP[2];            // Near
//	f.planes[5].plane = matVP[3] - matVP[2]; // Far

//	for (int i = 0; i < 6; ++i) {
//		float3 n = f.planes[i].plane.xyz;
//		float len = length(n);
//		f.planes[i].plane /= len;
//	}

//	return f;
//}

Frustum CreateFrustumFromMatrix(float4x4 matVP) {
	Frustum f;

	f.planes[0].plane = matVP[3] + matVP[0]; // Left
	f.planes[1].plane = matVP[3] - matVP[0]; // Right
	f.planes[2].plane = matVP[3] + matVP[1]; // Bottom
	f.planes[3].plane = matVP[3] - matVP[1]; // Top
	f.planes[4].plane = matVP[3] + matVP[2]; // Near  ✅ 修正
	f.planes[5].plane = matVP[3] - matVP[2]; // Far

    // Normalize planes
    [unroll]
	for (int i = 0; i < 6; ++i) {
		float3 n = f.planes[i].plane.xyz;
		float len = length(n);
		f.planes[i].plane /= len;
	}

	return f;
}


bool IsVisible(AABB box, Frustum frustum) {
	for (int i = 0; i < 6; ++i) {
		float3 normal = frustum.planes[i].plane.xyz;
		float dist = frustum.planes[i].plane.w;

		float3 vPos = box.min;
		if (normal.x >= 0) {
			vPos.x = box.max.x;
		}

		if (normal.y >= 0) {
			vPos.y = box.max.y;
		}

		if (normal.z >= 0) {
			vPos.z = box.max.z;
		}

		float d = dot(normal, vPos) + dist;

		if (d < 0) {
			return false;
		}
	}

	return true;
}
