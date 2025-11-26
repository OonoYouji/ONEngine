#include "VoxelTerrain.hlsli"

static const uint3 kNumthreads = uint3(4, 4, 4);

struct VoxelVertexColor {
	float4 color[9];
};

struct Vertices {
	VertexOut verts[8];
};

struct Indices {
	uint3 indis[12];
};

struct VoxelColorCluter {
	float4 colors[3][3][3];
};

/// １つのボクセルを描画するための最大のデータ
struct RenderingData {
	/// 8頂点分の情報
	VertexOut verts[8];
	/// 最大10三角形分のインデックス情報
	uint3 indis[10];
};

/// 描画対象が持つべき情報
struct DrawInstanceInfo {
	uint3 voxelPos;
	uint3 rotation;

	uint normalizedPattern;
	uint patternIndex;
	
	uint vertexStartIndex;
	uint indexStartIndex;
};

/// ---------------------------------------------------
/// Buffers
/// ---------------------------------------------------

Texture3D<float4> voxelChunkTextures[] : register(t1);
SamplerState texSampler : register(s0);


static const float3 kUVWOffset = float3(0.5f, -0.0f, 0.5f);
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

/// ボクセルの頂点インデックス定義
static const uint FRONT_LEFT_BOTTOM = 0;
static const uint FRONT_RIGHT_BOTTOM = 1;
static const uint FRONT_LEFT_TOP = 2;
static const uint FRONT_RIGHT_TOP = 3;
static const uint BACK_LEFT_BOTTOM = 4;
static const uint BACK_RIGHT_BOTTOM = 5;
static const uint BACK_LEFT_TOP = 6;
static const uint BACK_RIGHT_TOP = 7;

/// 立方体の三角形インデックス (外側向き CCW)
static const uint3 kVoxelTriangles[12] = {
    /// 前面 (Zマイナス側)
	uint3(FRONT_LEFT_BOTTOM, FRONT_RIGHT_BOTTOM, FRONT_LEFT_TOP), // 0-1-2
    uint3(FRONT_LEFT_TOP, FRONT_RIGHT_BOTTOM, FRONT_RIGHT_TOP), // 2-1-3

    /// 背面 (Zプラス側)
    uint3(BACK_RIGHT_BOTTOM, BACK_LEFT_BOTTOM, BACK_LEFT_TOP), // 5-4-6
    uint3(BACK_RIGHT_TOP, BACK_RIGHT_BOTTOM, BACK_LEFT_TOP), // 7-5-6

    /// 左面 (Xマイナス側)
    uint3(BACK_LEFT_BOTTOM, FRONT_LEFT_BOTTOM, FRONT_LEFT_TOP), // 4-0-2
    uint3(BACK_LEFT_TOP, BACK_LEFT_BOTTOM, FRONT_LEFT_TOP), // 6-4-2

    /// 右面 (Xプラス側)
    uint3(FRONT_RIGHT_BOTTOM, BACK_RIGHT_BOTTOM, BACK_RIGHT_TOP), // 1-5-7
    uint3(FRONT_RIGHT_TOP, FRONT_RIGHT_BOTTOM, BACK_RIGHT_TOP), // 3-1-7

    /// 上面 (Yプラス側)
    uint3(FRONT_LEFT_TOP, FRONT_RIGHT_TOP, BACK_RIGHT_TOP), // 2-3-7
    uint3(BACK_LEFT_TOP, FRONT_LEFT_TOP, BACK_RIGHT_TOP), // 6-2-7

    /// 下面 (Yマイナス側)
    uint3(FRONT_RIGHT_BOTTOM, FRONT_LEFT_BOTTOM, BACK_LEFT_BOTTOM), // 1-0-4
    uint3(BACK_RIGHT_BOTTOM, FRONT_RIGHT_BOTTOM, BACK_LEFT_BOTTOM), // 5-1-4
};


/*
	Bit 0 : Up
	Bit 1 : Down
	Bit 2 : Left
	Bit 3 : Right
	Bit 4 : Front
	Bit 5 : Back
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
	0, // 0x00
	2, // 0x01
	4, // 0x03
	2, // 0x05
	6, // 0x07
	8, // 0x0F
	4, // 0x15
	8, // 0x17
	10, // 0x1F
	0 // 0x3F (全方向は描画しない)
};

/// 各代表パターンが頂点を何個持つか (重複頂点は排除済み)
static const uint kPatternVertexCount[10] = {
	0, // 0x00
	4, // 0x01
	8, // 0x03
	4, // 0x05
	8, // 0x07
	8, // 0x0F
	6, // 0x15
	8, // 0x17
	8, // 0x1F
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
VoxelColorCluter GetVoxelColorCluster(uint3 _voxelPos, uint _chunkTextureId) {
	VoxelColorCluter vcc;
	
	[unroll]
	for (int z = -1; z <= 1; z++) {
		[unroll]
		for (int y = -1; y <= 1; y++) {
			[unroll]
			for (int x = -1; x <= 1; x++) {
				uint3 samplePos = _voxelPos + uint3(x, y, z);
				float3 uvw = (float3(samplePos.xyz) + kUVWOffset) / float3(voxelTerrainInfo.chunkSize);
				uvw.y = 1.0f - uvw.y; // Y軸の反転
				
				/// 範囲外であれば隣のチャンクからサンプリングする
				uint textureId = _chunkTextureId;
				
				/// X方向
				if (uvw.x < 0.0f) {
					/// 右隣のチャンクからサンプリング
					uvw.x += 1.0f;
					textureId = max(0, int(_chunkTextureId) - 1);
				} else if (uvw.x > 1.0f) {
					/// 左隣のチャンクからサンプリング
					uvw.x -= 1.0f;
					textureId = min(int(_chunkTextureId) + 1, int(voxelTerrainInfo.maxChunkCount) - 1);
				}
				
				
				/// Z方向
				if (uvw.z < 0.0f) {
					/// 手前のチャンクからサンプリング
					uvw.z += 1.0f;
					textureId = max(0, int(_chunkTextureId) - int(voxelTerrainInfo.chunkCountXZ.x));
				} else if (uvw.z > 1.0f) {
					/// 奥のチャンクからサンプリング
					uvw.z -= 1.0f;
					textureId = min(int(_chunkTextureId) + int(voxelTerrainInfo.chunkCountXZ.x), int(voxelTerrainInfo.maxChunkCount) - 1);
				}
				
				
				vcc.colors[x + 1][y + 1][z + 1] = voxelChunkTextures[textureId].SampleLevel(texSampler, uvw, 0);
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

	uint used[4] = {
		FRONT_LEFT_TOP,
		FRONT_RIGHT_TOP,
		BACK_RIGHT_TOP,
		BACK_LEFT_TOP
	};

	uint3 indis[2] = {
		/// 上面
		uint3(0, 1, 2),
		uint3(0, 2, 3)
	};
	

	/// データを適用　(color, position, normalは呼び出し元で設定する)
	RenderingData rd;
	for (int i = 0; i < 4; i++) {
		rd.verts[i].worldPosition = kDefaultVertices[used[i]];
	}
	
	for (int i = 0; i < 2; i++) {
		rd.indis[i] = indis[i];
	}
	
	return rd;
}

/// パターン2の頂点、インデックスを生成 [000011]
RenderingData GenerateRenderingDataPattern2() {
	/// パターン2(対向する2方向(3通りの回転)) デフォBit: 000011
	/// デフォBitより 上下方向にボクセルが存在するパターン
	
	uint3 indis[4] = {
		/// 上面
		kVoxelTriangles[8],
		kVoxelTriangles[9],
		/// 下面
		kVoxelTriangles[10],
		kVoxelTriangles[11]
	};
	
	
	RenderingData rd;
	for (int i = 0; i < 8; i++) {
		rd.verts[i].worldPosition = kDefaultVertices[i];
	}
	
	for (int i = 0; i < 4; i++) {
		rd.indis[i] = indis[i];
	}
	
	return rd;
}

/// パターン3の頂点、インデックスを生成 [000101]
RenderingData GenerateRenderingDataPattern3() {
	/// パターン3(垂直な2方向(12通りの回転)) デフォBit: 000101
	/// デフォBitより 上方向、右方向にボクセルが存在するパターン
	
	uint used[4] = {
		BACK_RIGHT_BOTTOM,
		FRONT_RIGHT_BOTTOM,
		BACK_LEFT_TOP,
		FRONT_LEFT_TOP
	};

	uint3 indis[2] = {
		/// 斜め面
		uint3(2, 1, 0),
		uint3(2, 3, 1)
	};
	
	
	RenderingData rd;
	for (int i = 0; i < 4; i++) {
		rd.verts[i].worldPosition = kDefaultVertices[used[i]];
	}
	
	for (int i = 0; i < 2; i++) {
		rd.indis[i] = indis[i];
	}
	
	return rd;
}

/// パターン4の頂点、インデックスを生成 [000111]
RenderingData GenerateRenderingDataPattern4() {
	/// パターン4(T字型3方向(24通りの回転)) デフォBit: 000111
	/// デフォBitより 上下方向、右方向にボクセルが存在するパターン

	/// 8頂点なのでデフォルト頂点をそのまま使用
	
	uint3 indis[6] = {
		/// 上面
		uint3(0, 1, 2),
		uint3(1, 3, 2),
		/// 右面
		uint3(0, 7, 1),
		uint3(0, 6, 7),
		/// 下面
		uint3(4, 6, 7),
		uint3(4, 7, 5)
	};
	
	
	RenderingData rd;
	for (int i = 0; i < 8; i++) {
		rd.verts[i].worldPosition = kDefaultVertices[i];
	}
	
	for (int i = 0; i < 6; i++) {
		rd.indis[i] = indis[i];
	}
	
	return rd;
}

/// パターン5の頂点、インデックスを生成 [001111]
RenderingData GenerateRenderingDataPattern5() {
	/// パターン5(平面4方向(6通りの回転)) デフォBit: 001111
	/// デフォBitより 上下左右方向にボクセルが存在するパターン

	/// 8頂点なのでデフォルト頂点をそのまま使用
	
	uint3 indis[8] = {
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
	
	
	RenderingData rd;
	for (int i = 0; i < 8; i++) {
		rd.verts[i].worldPosition = kDefaultVertices[i];
	}

	for (int i = 0; i < 6; ++i) {
		rd.indis[i] = indis[i];
	}
	
	return rd;
}

/// パターン6の頂点、インデックスを生成 [010101]
RenderingData GenerateRenderingDataPattern6() {
	/// パターン6(3軸各1方向(8通りの回転)) デフォBit: 010101
	/// デフォBitより 上方向、右方向、奥方向にボクセルが存在するパターン
	
	uint used[6] = {
		BACK_LEFT_TOP,
		BACK_RIGHT_TOP,
		BACK_RIGHT_BOTTOM,
		FRONT_LEFT_TOP,
		FRONT_LEFT_BOTTOM,
		FRONT_RIGHT_BOTTOM
	};
	
	uint3 indis[4] = {
		/// 奥(三角)
		uint3(0, 2, 1),
		/// 左(三角)
		uint3(0, 3, 4),
		/// 下(三角)
		uint3(4, 5, 2),
		/// 斜め(三角)
		uint3(0, 2, 4)
	};
	
	
	RenderingData rd;
	for (int i = 0; i < 6; i++) {
		rd.verts[i].worldPosition = kDefaultVertices[used[i]];
	}
	for (int i = 0; i < 4; ++i) {
		rd.indis[i] = indis[i];
	}
	
	return rd;
}

/// パターン7の頂点、インデックスを生成 [010111]
RenderingData GenerateRenderingDataPattern7() {
	/// パターン7(L字+対向(24通りの回転)) デフォBit: 010111
	/// デフォBitより 上下方向、右方向、前方向にボクセルが存在するパターン
	
	/// 8頂点なのでデフォルト頂点をそのまま使用
	uint3 indis[8] = {
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
	for (int i = 0; i < 8; i++) {
		rd.verts[i].worldPosition = kDefaultVertices[i];
	}
	for (int i = 0; i < 8; ++i) {
		rd.indis[i] = indis[i];
	}
	
	return rd;
}

/// パターン8の頂点、インデックスを生成 [011111]
RenderingData GenerateRenderingDataPattern8() {
	/// パターン8(斜め4方向(8通りの回転)) デフォBit: 011111
	/// デフォBitより 上下、左右、前方向にボクセルが存在するパターン
	
	/// 8頂点なのでデフォルト頂点をそのまま使用
	uint3 indis[10] = {
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
	for (int i = 0; i < 8; i++) {
		rd.verts[i].worldPosition = kDefaultVertices[i];
	}
	for (int i = 0; i < 10; ++i) {
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


RenderingData GenerateVoxelRenderingData(uint _patternIndex, uint3 _voxelPos, uint3 _rotation) {
	RenderingData rd = GetPatternRenderingData(_patternIndex);

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
		}
	}

	/// Y 逆回転
	for (uint i = 0; i < cntY; ++i) {
		for (int v = 0; v < 8; ++v) {
			float3 pos = rd.verts[v].worldPosition.xyz;
			rd.verts[v].worldPosition.xyz = RotateY90Pos(pos);
		}
	}

	/// X 逆回転
	for (uint i = 0; i < cntX; ++i) {
		for (int v = 0; v < 8; ++v) {
			float3 pos = rd.verts[v].worldPosition.xyz;
			rd.verts[v].worldPosition.xyz = RotateX90Pos(pos);
		}
	}

    // ======================================================
    // 展開して色、座標、法線を再計算
    // ======================================================
    [unroll]
	for (int i = 0; i < 8; i++) {
		float3 wp = rd.verts[i].worldPosition.xyz + float3(_voxelPos);
		rd.verts[i].worldPosition.xyz = wp;

		rd.verts[i].color = kPatternColor[_patternIndex];
		rd.verts[i].position = mul(rd.verts[i].worldPosition, viewProjection.matVP);

		float3 normal = wp - float3(_voxelPos);
		rd.verts[i].normal = normalize(normal);
	}

	return rd;
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

	
	AABB aabb;
	aabb.min = asPayload.chunkOrigin + DTid;
	aabb.max = aabb.min + asPayload.subChunkSize;
	if (IsVisible(aabb, CreateFrustumFromMatrix(viewProjection.matVP))) {

		for (int z = 0; z < 2; z++) {
			for (int y = 0; y < 2; y++) {
				for (int x = 0; x < 2; x++) {
					uint3 voxelPos = uint3(x, y, z) + DTid;
				
					/// ボクセルの色クラスタを取得
					uint chunkTextureId = chunks[asPayload.chunkIndex].textureId;
					VoxelColorCluter vcc = GetVoxelColorCluster(voxelPos, chunkTextureId);
					
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
					
					//if (patternIndex != 7) {
					//	continue;
					//}
					
					/// 描画するボクセルとして登録
					DrawInstanceInfo dii;
					dii.voxelPos = voxelPos;
					dii.rotation = rotation;
						
					dii.patternIndex = patternIndex;
					dii.normalizedPattern = normalizedPattern;

					dii.vertexStartIndex = numVertices;
					dii.indexStartIndex = numPrimitives;

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
		uint3 voxelPos = diis[i].voxelPos;
		float3 worldPos = float3(voxelPos) + asPayload.chunkOrigin;
		
		RenderingData rd = GenerateVoxelRenderingData(diis[i].patternIndex, worldPos, diis[i].rotation);

		uint vIndex = diis[i].vertexStartIndex;
		uint iIndex = diis[i].indexStartIndex;
		
		for (int j = 0; j < kPatternVertexCount[diis[i].patternIndex]; j++) {
			verts[vIndex + j] = rd.verts[j];
		}
		
		for (int j = 0; j < kPatternPrimitiveCount[diis[i].patternIndex]; ++j) {
			indices[iIndex + j] = rd.indis[j] + uint3(vIndex, vIndex, vIndex);
		}
		
	}

}
