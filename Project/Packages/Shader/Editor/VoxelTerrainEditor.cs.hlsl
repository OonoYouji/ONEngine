#include "../Render/VoxelTerrain/VoxelTerrainCommon.hlsli"
#include "../ConstantBufferData/ViewProjection.hlsli"
#include "../Math/Math.hlsli"

struct InputInfo {
	float2 screenMousePos;
	uint mouseLeftButton;
	uint keyboardLShift;
};

struct EditorInfo {
	uint32_t brushRadius;
    float32_t brushStrength;
};

struct ChunkID {
    uint value;
};

struct MousePos {
    float4 worldPos;
};


/// ///////////////////////////////////////////////////
/// buffers
/// ///////////////////////////////////////////////////

ConstantBuffer<VoxelTerrainInfo> voxelTerrainInfo : register(b0);
ConstantBuffer<ViewProjection> viewProjection : register(b1);
ConstantBuffer<Camera> camera : register(b2);
ConstantBuffer<InputInfo> inputInfo : register(b3);
ConstantBuffer<EditorInfo> editorInfo : register(b4);
ConstantBuffer<ChunkID> chunkID : register(b5);

RWStructuredBuffer<MousePos> mousePosBuffer : register(u0);
StructuredBuffer<Chunk> chunks : register(t0);
Texture2D<float4> worldPositionTexture : register(t1);
RWTexture3D<float4> voxelTextures[] : register(u1);
SamplerState textureSampler : register(s0);

/// ///////////////////////////////////////////////////
/// objects
/// ///////////////////////////////////////////////////

static const float2 kScreenSize = float2(1280.0f, 720.0f);


/// ///////////////////////////////////////////////////
/// fuinctions
/// ///////////////////////////////////////////////////

/// 指定した範囲内にポイントがあるかチェック
bool CheckInside(float3 _point, float3 _min, float3 _max) {
	return (_point.x >= _min.x && _point.y >= _min.y && _point.z >= _min.z &&
			_point.x <= _max.x && _point.y <= _max.y && _point.z <= _max.z);
}

/// ただのAABBと球の当たり判定
bool CheckSphereAABB(float3 _sphereCenter, float _sphereRadius, float3 _aabbMin, float3 _aabbMax) {
	float3 closestPoint = clamp(_sphereCenter, _aabbMin, _aabbMax);
	float distanceSq = dot(closestPoint - _sphereCenter, closestPoint - _sphereCenter);
	return distanceSq <= (_sphereRadius * _sphereRadius);
}


float3 ScreenToWorldRay(float2 _screenPos) {
	float4 clipPos = float4(_screenPos * 2.0f - 1.0f, 0, 1);
	float4 viewPos = mul(clipPos, InverseMatrix(viewProjection.matProjection));
	viewPos /= viewPos.w;
	float4 worldPos = mul(viewPos, InverseMatrix(viewProjection.matView));
	worldPos /= worldPos.w;

	float3 rayDir = normalize(worldPos.xyz - camera.position.xyz);
	return rayDir;
}

uint3 CaclVoxelPos(uint3 _center, int _value, uint _radius) {
	int x = _value % _radius;
	int y = (_value / _radius) % _radius;
	int z = _value / (_radius * _radius);
	return _center + int3(x, y, z);
}

bool OutsideChunk(int32_t3 brushCoord, uint32_t3 chunkSize) {
    return (brushCoord.x < 0 || brushCoord.y < 0 || brushCoord.z < 0 ||
            brushCoord.x >= chunkSize.x ||
            brushCoord.y >= chunkSize.y ||
            brushCoord.z >= chunkSize.z);
}

uint32_t3 GetNewChunkCoord(int32_t3 brushCoord, uint32_t3 chunkCoord) {
    uint32_t3 newChunkCoord = chunkCoord;

    if(brushCoord.x < 0) {
        newChunkCoord.x -= 1;
    } else if(brushCoord.x >= voxelTerrainInfo.chunkSize.x) {
        newChunkCoord.x += 1;
    }

    if(brushCoord.z < 0) {
        newChunkCoord.z -= 1;
    } else if(brushCoord.z >= voxelTerrainInfo.chunkSize.z) {
        newChunkCoord.z += 1;
    }

    return newChunkCoord;
}



[shader("compute")]
[numthreads(10, 10, 10)]
void main(
    uint3 DTid : SV_DispatchThreadID,
	uint3 Gid : SV_GroupThreadID,
    uint groupIndex : SV_GroupIndex) {

    /// このThreadが担当するボクセルがブラシ範囲内かチェック
    /// 角丸にするための処理
    int32_t3 brushCoord = (int3(DTid) - int3(editorInfo.brushRadius, editorInfo.brushRadius, editorInfo.brushRadius));;
    int lengthSq = brushCoord.x * brushCoord.x + brushCoord.y * brushCoord.y + brushCoord.z * brushCoord.z;
    if (lengthSq > editorInfo.brushRadius * editorInfo.brushRadius) {
        return;
    }



	/// 地形のローカル座標に変換
    float3 mouseWorldPos = mousePosBuffer[0].worldPos.xyz;
	float3 terrainLocalMousePos = mouseWorldPos - voxelTerrainInfo.terrainOrigin;

    /// マウスが指しているチャンクのID計算
    uint32_t3 chunkCoord = uint32_t3(terrainLocalMousePos / voxelTerrainInfo.chunkSize);
    uint chunkID = chunkCoord.x + (chunkCoord.z * voxelTerrainInfo.chunkCountXZ.y);
    float3 chunkOrigin = float3(
        chunkCoord.x * voxelTerrainInfo.chunkSize.x,
        0,
        chunkCoord.z * voxelTerrainInfo.chunkSize.z
    );

    float3 chunkLocalMousePos = terrainLocalMousePos - chunkOrigin;
    int3 voxelPos = int3(chunkLocalMousePos) + brushCoord;

    /// brushCoordがチャンク外ならIDを更新する
    if(OutsideChunk(voxelPos, uint32_t3(voxelTerrainInfo.chunkSize))) {
        chunkCoord = GetNewChunkCoord(brushCoord, chunkCoord);
        chunkOrigin = float3(
            (chunkCoord.x) * voxelTerrainInfo.chunkSize.x,
            0,
            (chunkCoord.z) * voxelTerrainInfo.chunkSize.z
        );
        chunkID = chunkCoord.x + (chunkCoord.z * voxelTerrainInfo.chunkCountXZ.y);
        chunkLocalMousePos = terrainLocalMousePos - chunkOrigin;
        voxelPos = int3(chunkLocalMousePos) + brushCoord;
    }

	/// ---------------------------------------------------
	/// ここから実際に編集する処理
	/// ---------------------------------------------------

	/// 対応するチャンクの情報
	/// マウスのチャンク内でのローカル位置
	
    /// Y軸を反転させ左手座標系からテクスチャ座標系に
	float posY = chunkLocalMousePos.y / voxelTerrainInfo.textureSize.y;
	posY -= 1.0f;
	posY = abs(posY);
	posY *= voxelTerrainInfo.textureSize.y;
	chunkLocalMousePos.y = posY;
	
	/// 範囲外チェック
	if (!CheckInside(voxelPos, int3(0, 1, 0), int3(voxelTerrainInfo.textureSize) - int3(0, 1, 0))) {
        return;
	}

	
	Chunk chunk = chunks[chunkID];
	float4 voxelColor = voxelTextures[chunk.textureId][voxelPos];
	if (voxelColor.a != 0.0f) {
		voxelTextures[chunk.textureId][voxelPos] = float4(0, 1, 0, voxelColor.a);
	}
	
	
	/// 操作次第で色を変更
	if (inputInfo.mouseLeftButton == 1) {
        float val = editorInfo.brushStrength;
		if (inputInfo.keyboardLShift == 1) {
			// ----- 押し下げ ----- //
			voxelColor.a -= val;
            if(voxelColor.a < 0.0f) {
                voxelColor.a = 0.0f;
            }
		} else {
			// ----- 押し上げ ----- //
			voxelColor.a += val;
            if(voxelColor.a > 1.0f) {
                voxelColor.a = 1.0f;
            }
		}
    
		voxelTextures[chunk.textureId][voxelPos] = voxelColor;
	}

}