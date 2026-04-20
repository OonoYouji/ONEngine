#include "../../Render/VoxelTerrain/VoxelTerrainCommon.hlsli"
#include "../../ConstantBufferData/ViewProjection.hlsli"
#include "../../Math/Math.hlsli"


struct InputInfo {
	float2 screenMousePos;
	uint mouseLeftButton;
	uint keyboardLShift;
};

struct EditorInfo {
	uint32_t brushRadius;
    float32_t brushStrength;
    uint32_t materialId;
};

struct ChunkID {
    uint value;
};

struct MousePos {
    float4 worldPos;
};

struct BitMask {
    uint32_t value;
};

static const float2 kScreenSize = float2(1280.0f, 720.0f);

ConstantBuffer<VoxelTerrainInfo> voxelTerrainInfo : register(b0);
ConstantBuffer<ViewProjection> viewProjection : register(b1);
ConstantBuffer<Camera> camera : register(b2);
ConstantBuffer<InputInfo> inputInfo : register(b3);
ConstantBuffer<EditorInfo> editorInfo : register(b4);
ConstantBuffer<BitMask> bitMask : register(b5);
ConstantBuffer<ChunkID> chunkID : register(b6);

RWStructuredBuffer<MousePos> mousePosBuffer : register(u0);
StructuredBuffer<Chunk> chunks : register(t0);
Texture2D<float4> worldPositionTexture : register(t1);
RWTexture3D<float4> voxelTextures[] : register(u1);
SamplerState textureSampler : register(s0);



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


/// -------------------------------------------------------------------
/// ボクセルの色を取得する関数
/// サンプリング位置がチャンク外の場合にも隣接チャンクを参照する
/// -------------------------------------------------------------------
float4 SampleVoxelColor(int3 samplePos, int currentChunkX, int currentChunkZ) {
    if (samplePos.y < 1 || samplePos.y >= voxelTerrainInfo.textureSize.y - 1) {
        return float4(0.0f, 0.0f, 0.0f, -1.0f); // 範囲外
    }

    int targetChunkX = currentChunkX;
    int targetChunkZ = currentChunkZ;
    int wrapX = samplePos.x;
    int wrapZ = samplePos.z;

    /// X軸のチャンク跨ぎ判定
    if (wrapX < 0) {
        targetChunkX -= 1; 
        wrapX += voxelTerrainInfo.textureSize.x;
    } else if (wrapX >= voxelTerrainInfo.textureSize.x) {
        targetChunkX += 1; 
        wrapX -= voxelTerrainInfo.textureSize.x;
    }

    /// Z軸のチャンク跨ぎ判定
    if (wrapZ < 0) {
        targetChunkZ -= 1; 
        wrapZ += voxelTerrainInfo.textureSize.z;
    } else if (wrapZ >= voxelTerrainInfo.textureSize.z) {
        targetChunkZ += 1; 
        wrapZ -= voxelTerrainInfo.textureSize.z;
    }

    /// 対象チャンクがワールド範囲内かチェック
    if (targetChunkX >= 0 && targetChunkX < voxelTerrainInfo.chunkCountXZ.x &&
        targetChunkZ >= 0 && targetChunkZ < voxelTerrainInfo.chunkCountXZ.y) {
        
        int targetChunkID = targetChunkZ * voxelTerrainInfo.chunkCountXZ.x + targetChunkX;
        int3 finalSamplePos = int3(wrapX, samplePos.y, wrapZ);
        
        // テクスチャにアクセスし、RGBA(float4)をそのまま返す
        return voxelTextures[chunks[targetChunkID].textureId][finalSamplePos];
    }

    /// ワールド範囲外
    return float4(0.0f, 0.0f, 0.0f, -1.0f);
}