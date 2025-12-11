#include "VoxelTerrain.hlsli"

struct VoxelColorCluter {
	float4 colors[3][3][3];
};

/// １つのボクセルを描画するための最大のデータ
struct RenderingData {
	/// 重複込みでの20頂点分の情報
	VertexOut verts[32];
	/// 最大10三角形分のインデックス情報
	uint3 indis[12];
};

/// 描画対象が持つべき情報
struct DrawInstanceInfo {
	uint3 voxelPos;
	uint3 rotation;

	uint normalizedPattern;
	uint patternIndex;
	
	uint vertexStartIndex;
	uint indexStartIndex;
	
	float4 color;
};

/// ---------------------------------------------------
/// Buffers
/// ---------------------------------------------------

Texture3D<float4> voxelChunkTextures[] : register(t1);
SamplerState texSampler : register(s0);


static const float3 kPixelOffset = float3(0.5f, 0.5f, 0.5f);
static const float kVertexOffset = 0.5f;

/*
	 y
	↑
	│
	│       6───────────7
	│      /│          /│
	│     / │         / │
	│    2───────────3  │
	│    │  │        │  │
	│    │  4────────│──5
	│    │ /         │ /
	│    │/          │/
	│    0───────────1
	│
	└────────────────────────→ x
	 (z方向は奥行き)
*/

/// ボクセルのデフォルト頂点位置 (中心を原点とした場合)
static const float4 kDefaultVertices[8] = {
	float4(-kVertexOffset, -kVertexOffset, -kVertexOffset, 1.0f), /// 手前左下
	float4(+kVertexOffset, -kVertexOffset, -kVertexOffset, 1.0f), /// 手前右下
	float4(-kVertexOffset, +kVertexOffset, -kVertexOffset, 1.0f), /// 手前左上
	float4(+kVertexOffset, +kVertexOffset, -kVertexOffset, 1.0f), /// 手前右上
	float4(-kVertexOffset, -kVertexOffset, +kVertexOffset, 1.0f), /// 奥左下
	float4(+kVertexOffset, -kVertexOffset, +kVertexOffset, 1.0f), /// 奥右下
	float4(-kVertexOffset, +kVertexOffset, +kVertexOffset, 1.0f), /// 奥左上
	float4(+kVertexOffset, +kVertexOffset, +kVertexOffset, 1.0f), /// 奥右上
};

/*
	Bit 0 : Up    -> Y+
	Bit 1 : Down  -> Y-
	Bit 2 : Right -> X+
	Bit 3 : Left  -> X-
	Bit 4 : Front -> Z-
	Bit 5 : Back  -> Z+
*/

/// 6bitでボクセルの周囲の有無を表現したデータ
static const uint kUniquePatterns[10] = {
	0x00, /// パターン0: 000000 - 空（回転しても同じ）
	0x01, /// パターン1: 000001 - 1方向のみ（6通りの回転）
	0x03, /// パターン2: 000011 - 対向する2方向（3通りの回転: 上下/左右/前後）
	0x05, /// パターン3: 000101 - 垂直な2方向（12通りの回転）
	0x07, /// パターン4: 000111 - T字型3方向（24通りの回転）
	0x0F, /// パターン5: 001111 - 平面4方向（6通りの回転）
	0x15, /// パターン6: 010101 - 3軸各1方向（8通りの回転）
	0x17, /// パターン7: 010111 - L字+対向（24通りの回転）
	0x1F, /// パターン8: 011111 - 5方向（6通りの回転）
	0x3F /// パターン9: 111111 - 全方向（回転しても同じ）
};

/// 各代表パターンが表現する配置の数
/// 合計: 1+6+3+12+24+6+8+24+6+1 = 91通り（実際は64通りなので一部重複あり）
static const uint kPatternMultiplicity[10] = {
	1, // 0x00: 1通り
	6, // 0x01: 6通り（6方向）
	3, // 0x03: 3通り（3軸）
	12, // 0x05: 12通り
	24, // 0x07: 24通り
	6, // 0x0F: 6通り
	8, // 0x15: 8通り
	24, // 0x17: 24通り
	6, // 0x1F: 6通り
	1 // 0x3F: 1通り
};


/// 各代表パターンが三角形を何個持つか
static const uint kPatternPrimitiveCount[10] = {
	0, // 0x00 パターン0
	2, // 0x01 パターン1
	4, // 0x03 パターン2
	4, // 0x05 パターン3
	6, // 0x07 パターン4
	8, // 0x0F パターン5
	4, // 0x15 パターン6
	8, // 0x17 パターン7
	10, // 0x1F パターン8
	0 // 0x3F (全方向は描画しない)
};

/// 各代表パターンが頂点を何個持つか (法線を分けるため重複込み)
static const uint kPatternVertexCount[10] = {
	0, // 0x00 パターン0
	4, // 0x01 パターン1 [000001]
	8, // 0x03 パターン2 [000011]
	10, // 0x05 パターン3 [000101]
	8, // 0x07 パターン4 [000111]
	16, // 0x0F パターン5 [001111]
	6, // 0x15 パターン6 [010101]
	16, // 0x17 パターン7 [010111]
	20, // 0x1F パターン8 [011111]
	0 // 0x3F (全方向は描画しないので0頂点)
};



/// ビットマスク定義
static const uint VOXEL_UP = 1 << 0;
static const uint VOXEL_DOWN = 1 << 1;
static const uint VOXEL_RIGHT = 1 << 2;
static const uint VOXEL_LEFT = 1 << 3;
static const uint VOXEL_FRONT = 1 << 4;
static const uint VOXEL_BACK = 1 << 5;


/// デバッグ用にPatternごとの色を定義
static const float4 kPatternColor[10] = {
	float4(0, 0, 0, 1), // パターン0: 黒
	float4(1, 0, 0, 1), // パターン1: 赤
	float4(0, 1, 0, 1), // パターン2: 緑
	float4(0, 0, 1, 1), // パターン3: 青
	float4(1, 1, 0, 1), // パターン4: 黄
	float4(1, 0, 1, 1), // パターン5: マゼンタ
	float4(0, 1, 1, 1), // パターン6: シアン
	float4(1, 0.5f, 0, 1), // パターン7: オレンジ
	float4(0.5f, 0, 1, 1), // パターン8: 紫
	float4(1, 1, 1, 1) // パターン9: 白
};


/// ---------------------------------------------------
/// function
/// ---------------------------------------------------

/// 指定したボクセル位置の周囲3x3x3の色を取得
VoxelColorCluter GetVoxelColorCluster(uint3 _voxelPos, uint _chunkId, uint3 _subChunkFactor) {
	VoxelColorCluter vcc;
	
	for (int z = -1; z <= 1; z++) {
		for (int y = -1; y <= 1; y++) {
			for (int x = -1; x <= 1; x++) {

				int3 samplePos = _voxelPos + uint3(x, y, z) * _subChunkFactor;
				float3 uvw = (float3(samplePos.xyz) + kPixelOffset) / float3(voxelTerrainInfo.textureSize);
				uvw.y = 1.0f - uvw.y; // Y軸の反転
				
				/// 範囲外であれば隣のチャンクからサンプリングする
				int chunkId = _chunkId;

				float4 noDrawColor = float4(1, 1, 1, 0);
				
				/// X方向
				if (uvw.x < 0.0f) {
					/// 左隣のチャンクからサンプリング
					int leftChunkId = chunkId - 1;
					if (leftChunkId % voxelTerrainInfo.chunkCountXZ.x != voxelTerrainInfo.chunkCountXZ.x - 1) {
						uvw.x += 1.0f;
						chunkId = leftChunkId;
					} else {
						vcc.colors[x + 1][y + 1][z + 1] = noDrawColor;
						continue;
					}
					
				} else if (uvw.x > 1.0f) {
					/// 右隣のチャンクからサンプリング
					int rightChunkId = chunkId + 1;
					if (rightChunkId % voxelTerrainInfo.chunkCountXZ.x != 0) {
						uvw.x -= 1.0f;
						chunkId = rightChunkId;
					} else {
						vcc.colors[x + 1][y + 1][z + 1] = noDrawColor;
						continue;
					}
				}
				
				
				/// Z方向
				if (uvw.z < 0.0f) {
					/// 手前のチャンクからサンプリング
					int frontChunkId = chunkId - int(voxelTerrainInfo.chunkCountXZ.x);
					if (frontChunkId >= 0) {
						uvw.z += 1.0f;
						chunkId = frontChunkId;
					} else {
						vcc.colors[x + 1][y + 1][z + 1] = noDrawColor;
						continue;
					}

				} else if (uvw.z > 1.0f) {
					/// 奥のチャンクからサンプリング
					int backChunkId = int(chunkId) + int(voxelTerrainInfo.chunkCountXZ.x);
					if (backChunkId < voxelTerrainInfo.maxChunkCount) {
						uvw.z -= 1.0f;
						chunkId = backChunkId;
					} else {
						vcc.colors[x + 1][y + 1][z + 1] = noDrawColor;
						continue;
					}
				}
				
				
				vcc.colors[x + 1][y + 1][z + 1] = voxelChunkTextures[chunks[chunkId].textureId].SampleLevel(texSampler, uvw, 0);
			}
		}
	}
	
	
	return vcc;
}


uint EncodePattern6(VoxelColorCluter _vcc) {
	uint pattern = 0;
	
	/// 上
	if (_vcc.colors[1][2][1].a != 0.0f) {
		pattern |= VOXEL_UP;
	}
	/// 下
	if (_vcc.colors[1][0][1].a != 0.0f) {
		pattern |= VOXEL_DOWN;
	}
	/// 左
	if (_vcc.colors[0][1][1].a != 0.0f) {
		pattern |= VOXEL_LEFT;
	}
	/// 右
	if (_vcc.colors[2][1][1].a != 0.0f) {
		pattern |= VOXEL_RIGHT;
	}
	/// 前
	if (_vcc.colors[1][1][0].a != 0.0f) {
		pattern |= VOXEL_FRONT;
	}
	/// 奥
	if (_vcc.colors[1][1][2].a != 0.0f) {
		pattern |= VOXEL_BACK;
	}
	
	return pattern;
}

uint RotateX90(uint _pattern) {
	/// ----- X軸に90度回転 ----- ///
	bool up = (_pattern & VOXEL_UP) != 0;
	bool down = (_pattern & VOXEL_DOWN) != 0;
	bool left = (_pattern & VOXEL_LEFT) != 0;
	bool right = (_pattern & VOXEL_RIGHT) != 0;
	bool front = (_pattern & VOXEL_FRONT) != 0;
	bool back = (_pattern & VOXEL_BACK) != 0;
	
	uint result = 0;

	/// right, left はそのまま
	if (right) {
		result |= VOXEL_RIGHT;
	}
	if (left) {
		result |= VOXEL_LEFT;
	}

	/// up -> front
	if (up) {
		result |= VOXEL_FRONT;
	}
	/// down -> back
	if (down) {
		result |= VOXEL_BACK;
	}
	
	/// front -> down
	if (front) {
		result |= VOXEL_DOWN;
	}
	/// back -> up
	if (back) {
		result |= VOXEL_UP;
	}
	
	return result;
}

uint RotateY90(uint _pattern) {
	/// ----- Y軸に90度回転 ----- ///
	bool up = (_pattern & VOXEL_UP) != 0;
	bool down = (_pattern & VOXEL_DOWN) != 0;
	bool left = (_pattern & VOXEL_LEFT) != 0;
	bool right = (_pattern & VOXEL_RIGHT) != 0;
	bool front = (_pattern & VOXEL_FRONT) != 0;
	bool back = (_pattern & VOXEL_BACK) != 0;
	
	uint result = 0;
	/// up, down はそのまま
	if (up) {
		result |= VOXEL_UP;
	}
	if (down) {
		result |= VOXEL_DOWN;
	}
	/// left -> front
	if (left) {
		result |= VOXEL_FRONT;
	}
	/// right -> back
	if (right) {
		result |= VOXEL_BACK;
	}
	
	/// front -> right
	if (front) {
		result |= VOXEL_RIGHT;
	}
	/// back -> left
	if (back) {
		result |= VOXEL_LEFT;
	}
	
	return result;
}

uint RotateZ90(uint _pattern) {
	/// ----- Z軸に90度回転 ----- ///
	bool up = (_pattern & VOXEL_UP) != 0;
	bool down = (_pattern & VOXEL_DOWN) != 0;
	bool left = (_pattern & VOXEL_LEFT) != 0;
	bool right = (_pattern & VOXEL_RIGHT) != 0;
	bool front = (_pattern & VOXEL_FRONT) != 0;
	bool back = (_pattern & VOXEL_BACK) != 0;
	
	uint result = 0;
	/// front, back はそのまま
	if (front) {
		result |= VOXEL_FRONT;
	}
	if (back) {
		result |= VOXEL_BACK;
	}
	/// left -> up
	if (left) {
		result |= VOXEL_UP;
	}
	/// right -> down
	if (right) {
		result |= VOXEL_DOWN;
	}
	
	/// up -> right
	if (up) {
		result |= VOXEL_RIGHT;
	}
	/// down -> left
	if (down) {
		result |= VOXEL_LEFT;
	}
	
	return result;
}

int GetPatternIndex(uint _normalizedPattern) {
	/// ----- 正規化済みのパターンからどのパターンに対応するのかチェックする ----- ///

	/// 1,9番目は描画しないものなのでスキップする
	for (int i = 1; i < 9; i++) {
		if (kUniquePatterns[i] == _normalizedPattern) {
			return i;
		}
	}
	return -1; // 見つからなかった場合
}

uint3 GetPatternRotate(uint _pattern, inout uint _normalizedPattern) {
	/// ----- パターンに対応する回転を取得する ----- ///
	/// 戻り値は (X回転回数, Y回転回数, Z回転回数) で表現する

	uint minPattern = _pattern;
	uint3 best = uint3(0, 0, 0);

	uint pX = _pattern;
	for (int x = 0; x < 4; x++) {
		uint pY = pX;
		for (int y = 0; y < 4; y++) {
			uint pZ = pY;
			for (int z = 0; z < 4; z++) {
				if (pZ < minPattern) {
					minPattern = pZ;
					best = uint3(x, y, z);
				}
				pZ = RotateZ90(pZ);
			}
			pY = RotateY90(pY);
		}
		pX = RotateX90(pX);
	}

	_normalizedPattern = minPattern;
	return best;
}


/// パターン1の頂点、インデックスを生成 [000001]
RenderingData GenerateRenderingDataPattern1() {
	/// パターン1(1方向のみ(6通りの回転)) デフォBit: 000001
	/// デフォBitより 上方向にボクセルが存在するパターン

	const uint used[4] = {
		2, 3, 7, 6
	};

	const uint3 indis[2] = {
		/// 上面
		uint3(0, 1, 2),
		uint3(0, 2, 3)
	};
	

	/// データを適用　(color, position, normalは呼び出し元で設定する)
	RenderingData rd;
	for (int i = 0; i < kPatternVertexCount[1]; i++) {
		rd.verts[i].worldPosition = kDefaultVertices[used[i]];
		rd.verts[i].normal = float3(0, -1, 0);
	}
	
	for (int i = 0; i < kPatternPrimitiveCount[1]; i++) {
		rd.indis[i] = indis[i];
	}
	
	return rd;
}

/// パターン2の頂点、インデックスを生成 [000011]
RenderingData GenerateRenderingDataPattern2() {
	/// パターン2(対向する2方向(3通りの回転)) デフォBit: 000011
	/// デフォBitより 上下方向にボクセルが存在するパターン

	const uint used[8] = {
		/// 上面
		2, 3, 7, 6,
		/// 下面
		1, 0, 4, 5
	};

	const uint3 indis[4] = {
		/// 上面
		uint3(2, 3, 7),
		uint3(2, 7, 6),
		/// 下面
		uint3(1, 0, 4),
		uint3(1, 4, 5)
	};
	
	/// 上面と下面で法線が異なるので別々に設定する
	float3 upNormal = float3(0, 1, 0);
	float3 downNormal = float3(0, -1, 0);
	
	RenderingData rd;
	for (int i = 0; i < kPatternVertexCount[2]; i++) {
		rd.verts[i].worldPosition = kDefaultVertices[i];
	}

	/// 上面の法線設定
	rd.verts[2].normal = upNormal;
	rd.verts[3].normal = upNormal;
	rd.verts[6].normal = upNormal;
	rd.verts[7].normal = upNormal;

	/// 下面の法線設定
	rd.verts[0].normal = downNormal;
	rd.verts[1].normal = downNormal;
	rd.verts[4].normal = downNormal;
	rd.verts[5].normal = downNormal;
	

	for (int i = 0; i < kPatternPrimitiveCount[2]; i++) {
		rd.indis[i] = indis[i];
	}
	
	return rd;
}

/// パターン3の頂点、インデックスを生成 [000101]
RenderingData GenerateRenderingDataPattern3() {
	/// パターン3(垂直な2方向(12通りの回転)) デフォBit: 000101
	/// デフォBitより 上方向、右方向にボクセルが存在するパターン

	const uint used[10] = {
		/// 斜め面
		5, 1, 6, 2,
		/// 前面
		2, 3, 1,
		/// 奥面
		6, 5, 7
	};


	const uint3 indis[4] = {
		/// 斜め面
		uint3(2, 1, 0),
		uint3(2, 3, 1),
		/// 奥面
		uint3(4, 5, 6),
		/// 前面
		uint3(7, 8, 9)
	};
	
	float3 normals[] = {
		normalize(float3(-1, -1, 0)), // 斜め面
		float3(0, 0, 1), // 奥面
		float3(0, 0, -1) // 前面
	};

	
	RenderingData rd;
	for (int i = 0; i < kPatternVertexCount[3]; i++) {
		rd.verts[i].worldPosition = kDefaultVertices[used[i]];

		if (i < 4) {
			rd.verts[i].normal = normals[0]; // 斜め面
		} else if (i < 7) {
			rd.verts[i].normal = normals[1]; // 奥面
		} else {
			rd.verts[i].normal = normals[2]; // 前面
		}
	}
	
	for (int i = 0; i < kPatternPrimitiveCount[3]; i++) {
		rd.indis[i] = indis[i];
	}
	
	return rd;
}

/// パターン4の頂点、インデックスを生成 [000111]
RenderingData GenerateRenderingDataPattern4() {
	/// パターン4(T字型3方向(24通りの回転)) デフォBit: 000111
	/// デフォBitより 上下方向、右方向にボクセルが存在するパターン
	
	const uint used[8] = {
		6,7,2,3,
		4,5,0,1
	};

	const uint3 indis[6] = {
		/// 上面
		uint3(0,2,1),
		uint3(1,2,3),
		/// 下面
		uint3(4,5,6),
		uint3(5,7,6),
		/// 右面
		uint3(1,3,5),
		uint3(5,3,7)
	};

	
	RenderingData rd;
	for (int i = 0; i < kPatternVertexCount[4]; i++) {
		rd.verts[i].worldPosition = kDefaultVertices[used[i]];
		rd.verts[i].normal = float3(0, 1, 0); 
	}
	
	for (int i = 0; i < kPatternPrimitiveCount[4]; i++) {
		rd.indis[i] = indis[i];
	}
	
	return rd;
}

/// パターン5の頂点、インデックスを生成 [001111]
RenderingData GenerateRenderingDataPattern5() {
	/// パターン5(平面4方向(6通りの回転)) デフォBit: 001111
	/// デフォBitより 上下左右方向にボクセルが存在するパターン

	const uint used[16] = {
		/// 上面
		2, 3, 7, 6,
		/// 右面
		3, 5, 7, 1,
		/// 下面
		0, 4, 5, 1,
		/// 左面
		0, 2, 6, 4
	};

	const uint3 indis[8] = {
		/// 上面
		uint3(2, 3, 6),
		uint3(3, 7, 6),
		/// 右面
		uint3(3, 5, 7),
		uint3(3, 1, 5),
		/// 下面
		uint3(0, 4, 1),
		uint3(1, 4, 5),
		/// 左面
		uint3(0, 2, 4),
		uint3(2, 6, 4)
	};
	
	const float3 normals[4] = {
		float3(0, 1, 0), // 上面
		float3(1, 0, 0), // 右面
		float3(0, -1, 0), // 下面
		float3(-1, 0, 0) // 左面
	};

	
	
	RenderingData rd;
	for (int i = 0; i < kPatternVertexCount[5]; i++) {
		rd.verts[i].worldPosition = kDefaultVertices[used[i]];
		rd.verts[i].normal = normals[i / 4];
	}

	for (int i = 0; i < kPatternPrimitiveCount[5]; ++i) {
		rd.indis[i] = indis[i];
	}
	
	return rd;
}

/// パターン6の頂点、インデックスを生成 [010101]
RenderingData GenerateRenderingDataPattern6() {
	/// パターン6(3軸各1方向(8通りの回転)) デフォBit: 010101
	/// デフォBitより 上方向、右方向、奥方向にボクセルが存在するパターン

	/// 使用する頂点インデックス
	const uint used[6] = {
		0, 1, 2, 5, 6, 7
	};
	
	const uint3 indis[4] = {
		/// 奥(三角)
		uint3(5,4,3),
		/// 左(三角)
		uint3(4,2,0),
		/// 下(三角)
		uint3(3,1,1),
		/// 斜め(三角)
		uint3(4,0,3)
	};
	
	float3 normal = normalize(float3(-1, 1, 1));
	
	RenderingData rd;
	for (int i = 0; i < kPatternVertexCount[6]; i++) {
		rd.verts[i].worldPosition = kDefaultVertices[used[i]];
		rd.verts[i].normal = normal;
	}
	for (int i = 0; i < kPatternPrimitiveCount[6]; ++i) {
		rd.indis[i] = indis[i];
	}
	
	return rd;
}

/// パターン7の頂点、インデックスを生成 [010111]
RenderingData GenerateRenderingDataPattern7() {
	/// パターン7(L字+対向(24通りの回転)) デフォBit: 010111
	/// デフォBitより 上下方向、右方向、前方向にボクセルが存在するパターン

	const uint used[16] = {
		/// 上面
		6, 7, 3, 2,
		/// 下面
		4, 5, 1, 0,
		/// 右面
		7, 5, 1, 3,
		/// 前面
		3, 2, 0, 1
	};
	
	float3 normals[4] = {
		float3(0, 1, 0), // 上面
		float3(0, -1, 0), // 下面
		float3(1, 0, 0), // 右面
		float3(0, 0, 1) // 前面
	};

	/// 8頂点なのでデフォルト頂点をそのまま使用
	const uint3 indis[8] = {
		/// 上面
		uint3(6, 3, 2),
		uint3(6, 7, 3),
		/// 下面
		uint3(4, 1, 0),
		uint3(4, 5, 1),
		/// 右面
		uint3(7, 3, 5),
		uint3(5, 3, 1),
		/// 前面
		uint3(3, 2, 1),
		uint3(1, 2, 0)
	};
	
	
	RenderingData rd;
	for (int i = 0; i < kPatternVertexCount[7]; i++) {
		rd.verts[i].worldPosition = kDefaultVertices[i];
		rd.verts[i].normal = normals[i / 4];
	}
	for (int i = 0; i < kPatternPrimitiveCount[7]; ++i) {
		rd.indis[i] = indis[i];
	}
	
	return rd;
}

/// パターン8の頂点、インデックスを生成 [011111]
RenderingData GenerateRenderingDataPattern8() {
	/// パターン8(斜め4方向(8通りの回転)) デフォBit: 011111
	/// デフォBitより 上下、左右、前方向にボクセルが存在するパターン
	
	const uint used[20] = {
		/// 上面
		2, 3, 7, 6,
		/// 下面
		0, 1, 5, 4,
		/// 右面
		3, 5, 7, 1,
		/// 左面
		0, 2, 6, 4,
		/// 前面
		3, 2, 0, 1
	};

	float3 normals[5] = {
		float3(0, 1, 0), // 上面
		float3(0, -1, 0), // 下面
		float3(1, 0, 0), // 右面
		float3(-1, 0, 0), // 左面
		float3(0, 0, 1) // 前面
	};

	/// 8頂点なのでデフォルト頂点をそのまま使用
	const uint3 indis[10] = {
		/// 上面
		uint3(2, 3, 6),
		uint3(3, 7, 6),
		/// 下面
		uint3(2, 3, 6),
		uint3(3, 7, 6),
		/// 右面
		uint3(7, 3, 5),
		uint3(3, 1, 5),
		/// 左面
		uint3(0, 2, 4),
		uint3(2, 6, 4),
		/// 前面
		uint3(3, 2, 1),
		uint3(2, 0, 1)
	};
	
	
	RenderingData rd;
	for (int i = 0; i < kPatternVertexCount[8]; i++) {
		rd.verts[i].worldPosition = kDefaultVertices[used[i]];
		rd.verts[i].normal = normals[i / 4];
	}
	for (int i = 0; i < kPatternPrimitiveCount[8]; ++i) {
		rd.indis[i] = indis[i];
	}
	
	return rd;
}


/// 指定したパターンインデックスに対応するレンダリングデータを取得
RenderingData GetPatternRenderingData(uint _patternIndex) {
	switch (_patternIndex) {
		case 1:
			return GenerateRenderingDataPattern1();
		case 2:
			return GenerateRenderingDataPattern2();
		case 3:
			return GenerateRenderingDataPattern3();
		case 4:
			return GenerateRenderingDataPattern4();
		case 5:
			return GenerateRenderingDataPattern5();
		case 6:
			return GenerateRenderingDataPattern6();
		case 7:
			return GenerateRenderingDataPattern7();
		case 8:
			return GenerateRenderingDataPattern8();
	}
	RenderingData empty;
	return empty;
}

/// ======================================================
/// 1回の回転処理（パターン回転に対応した位置変換）
/// ======================================================
float3 RotateZ90Pos(float3 p) {
	return float3(p.y, -p.x, p.z);
}
float3 RotateY90Pos(float3 p) {
	return float3(-p.z, p.y, p.x);
}
float3 RotateX90Pos(float3 p) {
	return float3(p.x, p.z, -p.y);
}


RenderingData GenerateVoxelRenderingData(uint _patternIndex, uint3 _voxelPos, uint3 _rotation, uint3 _subChunkFactor) {
	RenderingData rd = GetPatternRenderingData(_patternIndex);

	for (int i = 0; i < kPatternVertexCount[_patternIndex]; i++) {
		rd.verts[i].worldPosition.xyz *= float3(_subChunkFactor);
	}


    // ======================================================
    // normalizedPattern → 元のパターンへ戻すので
    // 逆回転 = (4 - 各軸の回転) 回、かつ Z→Y→X の順で回す
    // ======================================================
	uint cntZ = (4 - (_rotation.z & 3)) & 3;
	uint cntY = (4 - (_rotation.y & 3)) & 3;
	uint cntX = (4 - (_rotation.x & 3)) & 3;

	/// Z 逆回転
	for (uint i = 0; i < cntZ; ++i) {
		for (int v = 0; v < 8; ++v) {
			float3 pos = rd.verts[v].worldPosition.xyz;
			rd.verts[v].worldPosition.xyz = RotateZ90Pos(pos);
			rd.verts[v].normal = RotateZ90Pos(rd.verts[v].normal);
		}
	}

	/// Y 逆回転
	for (uint i = 0; i < cntY; ++i) {
		for (int v = 0; v < 8; ++v) {
			float3 pos = rd.verts[v].worldPosition.xyz;
			rd.verts[v].worldPosition.xyz = RotateY90Pos(pos);
			rd.verts[v].normal = RotateY90Pos(rd.verts[v].normal);
		}
	}

	/// X 逆回転
	for (uint i = 0; i < cntX; ++i) {
		for (int v = 0; v < 8; ++v) {
			float3 pos = rd.verts[v].worldPosition.xyz;
			rd.verts[v].worldPosition.xyz = RotateX90Pos(pos);
			rd.verts[v].normal = RotateX90Pos(rd.verts[v].normal);
		}
	}

    // ======================================================
    // 展開して色、座標、法線を再計算
    // ======================================================
    [unroll]
	for (int i = 0; i < 8; i++) {
		float3 wp = rd.verts[i].worldPosition.xyz + float3(_voxelPos);

		/// heightで色を変化させる
		rd.verts[i].color.rgb = lerp(0.5f, 1, rd.verts[i].worldPosition.y / voxelTerrainInfo.chunkSize.y);
		rd.verts[i].color.a = 1.0f;
		rd.verts[i].worldPosition.xyz = wp;

		//rd.verts[i].color = kPatternColor[_patternIndex];
		rd.verts[i].position = mul(rd.verts[i].worldPosition, viewProjection.matVP);
	}

	return rd;
}


uint3 CalcSubChunkPos(uint3 _DTid, uint3 _dispatchSize, uint3 _chunkSize) {
	uint3 result;
	
	/// DTidが0~_dispatchSize*numthreads-1の範囲なので
	float3 normalizedDTid = float3(_DTid) / float3(_dispatchSize);
	float3 subChunkPosF = float3(_chunkSize) * normalizedDTid;
	result = uint3(subChunkPosF);
	
	return result;
}

uint3 CalcVoxelPos(uint3 _DTid, uint3 _dispatchSize, float3 _subChunkMin, float3 _subChunkMax) {
	uint3 result;
	
	/// DTidが0~_dispatchSize*numthreads-1の範囲なので
	float3 normalizedDTid = float3(_DTid) / float3(_dispatchSize * uint3(2, 2, 2));

	float3 voxelPosF = lerp(_subChunkMin, _subChunkMax, normalizedDTid);
	result = uint3(voxelPosF);
	
	return result;
}


/// ---------------------------------------------------
/// Main
/// ---------------------------------------------------
[shader("mesh")]
[outputtopology("triangle")]
[numthreads(2, 2, 2)]
void main(
    uint3 DTid : SV_DispatchThreadID,
	uint gi : SV_GroupIndex,
	uint3 groupId : SV_GroupID,
    in payload Payload asPayload,
    out vertices VertexOut verts[256],
    out indices uint3 indices[256]) {
	
	/// グループ内の最初のスレッドのみ処理
	if (gi != 0) {
		return;
	}
	
	
	/// forループですべてのボクセルを処理、最適な頂点数、プリミティブ数しか出力しない

	static const uint kMaxDrawVoxels = 2 * 2 * 2;
	uint drawVoxelCount = 0;
	DrawInstanceInfo diis[kMaxDrawVoxels];
	
	/// 描画する頂点数、プリミティブ数
	uint numVertices = 0;
	uint numPrimitives = 0;
	
	/// ---------------------------------------------------
	/// 事前にカリング、ボクセルごとに描画するか判定
	/// ---------------------------------------------------
	
	/// デフォのサイズとの比率を計算
	uint3 subChunkFactor = asPayload.subChunkSize / uint3(2, 2, 2);

	AABB aabb;
	aabb.min = asPayload.chunkOrigin + (DTid * subChunkFactor);
	//aabb.min = CalcSubChunkPos(DTid, asPayload.dispatchSize, voxelTerrainInfo.chunkSize) + asPayload.chunkOrigin;
	aabb.max = aabb.min + asPayload.subChunkSize;
	
	if (IsVisible(aabb, CreateFrustumFromMatrix(viewProjection.matVP))) {

		float3 aabbDiff = aabb.max - aabb.min;
		
		for (int z = 0; z < 2; z++) {
			for (int y = 0; y < 2; y++) {
				for (int x = 0; x < 2; x++) {
					uint3 voxelPos = uint3(x, y, z) * subChunkFactor + (DTid * subChunkFactor);
					//float3 voxelPos = CalcVoxelPos(DTid, asPayload.dispatchSize, aabb.min, aabb.max);
					//voxelPos += float3(x, y, z);

					/// ボクセルの色クラスタを取得
					VoxelColorCluter vcc = GetVoxelColorCluster(uint3(voxelPos), asPayload.chunkIndex, subChunkFactor);
					
					if (vcc.colors[1][1][1].a != 0.0f) {
						continue; // 自身が地ボクセルならスキップ
					}
					
					/// パターンをBit化
					uint pattern = EncodePattern6(vcc);
					uint normalizedPattern = pattern;
					uint3 rotation = GetPatternRotate(pattern, normalizedPattern);
					uint patternIndex = GetPatternIndex(normalizedPattern);

					/// 描画しないパターンならスキップ
					if (patternIndex == -1) {
						continue;
					}
					
					/// 描画するボクセルとして登録
					DrawInstanceInfo dii;
					dii.voxelPos = uint3(voxelPos);
					dii.rotation = rotation;
						
					dii.patternIndex = patternIndex;
					dii.normalizedPattern = normalizedPattern;

					dii.vertexStartIndex = numVertices;
					dii.indexStartIndex = numPrimitives;
					
					float3 rgb = float3(0, 0, 0);
					for (int i = 0; i < 27; i++) {
						int xi = i % 3;
						int yi = (i / 3) % 3;
						int zi = i / 9;
						rgb += vcc.colors[xi][yi][zi].rgb;
					}
					dii.color = float4(rgb / 27.0f, 1.0f);


					diis[drawVoxelCount] = dii;
					drawVoxelCount++;

					/// パターンごとに頂点数、プリミティブ数を加算
					numPrimitives += kPatternPrimitiveCount[patternIndex];
					numVertices += kPatternVertexCount[patternIndex];
				}
			}
		}
	}

	
	/// 描画するボクセル数に応じて頂点数、プリミティブ数を設定
	SetMeshOutputCounts(numVertices, numPrimitives);
	if (numVertices == 0 || numPrimitives == 0) {
		return;
	}


	/// ---------------------------------------------------
	/// ボクセルごとに頂点、インデックスを設定
	/// ---------------------------------------------------

	for (uint i = 0; i < drawVoxelCount; i++) {
		float3 worldPos = float3(diis[i].voxelPos) + asPayload.chunkOrigin;
		
		RenderingData rd = GenerateVoxelRenderingData(diis[i].patternIndex, worldPos, diis[i].rotation, subChunkFactor);

		uint vIndex = diis[i].vertexStartIndex;
		uint iIndex = diis[i].indexStartIndex;
		
		for (int j = 0; j < kPatternVertexCount[diis[i].patternIndex]; j++) {
			verts[vIndex + j] = rd.verts[j];
			verts[vIndex + j].color = diis[i].color;
		}
		
		for (int j = 0; j < kPatternPrimitiveCount[diis[i].patternIndex]; ++j) {
			indices[iIndex + j] = rd.indis[j] + uint3(vIndex, vIndex, vIndex);
		}
		
	}

}
