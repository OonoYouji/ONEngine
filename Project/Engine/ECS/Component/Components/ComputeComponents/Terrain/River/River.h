#pragma once

/// std
#include <vector>
#include <cstdint>

/// engine
#include "Engine/Core/Utility/Math/Vector4.h"
#include "Engine/Core/Utility/Math/Vector3.h"
#include "Engine/Core/Utility/Math/Vector2.h"
#include "Engine/Graphics/Buffer/ConstantBuffer.h"
#include "Engine/Graphics/Buffer/StructuredBuffer.h"
#include "Engine/Graphics/Buffer/Data/GPUMaterial.h"

/// ///////////////////////////////////////////////////
/// 川のコントロールポイント
/// ///////////////////////////////////////////////////
struct RiverControlPoint {
	Vector3 position;
	float width;
};

/// ///////////////////////////////////////////////////
/// 川のメッシュ頂点
/// ///////////////////////////////////////////////////
struct RiverVertex {
	Vector4 position;
	Vector2 uv;
	Vector3 normal;
};


/// ///////////////////////////////////////////////////
/// spline曲線の計算関数
/// ///////////////////////////////////////////////////
Vector3 CatmullRomPosition(
	const Vector3& _p0,
	const Vector3& _p1,
	const Vector3& _p2,
	const Vector3& _p3,
	float _t
);

RiverControlPoint CatmullRom(
	const RiverControlPoint& _p0,
	const RiverControlPoint& _p1,
	const RiverControlPoint& _p2,
	const RiverControlPoint& _p3,
	float _t
);

std::vector<RiverControlPoint> SampleRiverSpline(
	const std::vector<RiverControlPoint>& _points,
	int _samplePerSegment
);


/// ///////////////////////////////////////////////////
/// 川
/// ///////////////////////////////////////////////////
class River {
public:

	struct Param {
		uint32_t totalSegments;
		uint32_t totalVertices;
		uint32_t totalSamples;
		uint32_t samplePerSegment;
	};

public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	River();
	~River();

	/// ----- edit ----- ///
	void Edit(class EntityComponentSystem* _ecs);
	void SaveToJson(const std::string& _name);
	void LoadFromJson(const std::string& _name);


	/// @brief Spline曲線をGizmoで描画する
	void DrawSplineCurve();

	/// @brief Bufferを生成する
	/// @param _dxDevice DxDeviceへのポインタ
	/// @param _dxSRVHeap DxSRVHeapへのポインタ
	/// @param _dxCommand DxCommandへのポインタ
	void CreateBuffers(class DxDevice* _dxDevice, class DxSRVHeap* _dxSRVHeap, class DxCommand* _dxCommand);

	/// @brief Bufferデータをセットする
	void SetBufferData();


	/// @brief MaterialDataをセットする
	/// @param _entityId OwnerEntityのID
	/// @param _texIndex 川に使用するテクスチャのIndex
	void SetMaterialData(int32_t _entityId, int32_t _texIndex);


	/// @brief VBVとIBVのバリアを生成する(描画用に)
	/// @param _dxCommand DxCommandへのポインタ
	void CreateRenderingBarriers(class DxCommand* _dxCommand);

	/// @brief VBVとIBVのバリアを復元する(計算用に)
	/// @param _dxCommand DxCommandへのポインタ
	void RestoreResourceBarriers(class DxCommand* _dxCommand);


	/// @brief 描画用にVBVを生成する
	D3D12_VERTEX_BUFFER_VIEW CreateVBV();
	/// @brief 描画用にIBVを生成する
	D3D12_INDEX_BUFFER_VIEW CreateIBV();

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	/// splineのコントロールポイント
	std::vector<RiverControlPoint> controlPoints_;
	std::vector<RiverControlPoint> createdPoints_;
	int samplePerSegment_;

	/// buffer
	ConstantBuffer<Param> paramBuf_;
	ConstantBuffer<GPUMaterial> materialBuffer_;
	StructuredBuffer<RiverControlPoint> controlPointBuf_;
	StructuredBuffer<RiverVertex> rwVertices_;
	StructuredBuffer<uint32_t> rwIndices_;
	bool isCreatedBuffers_;
	UINT totalVertices_;
	UINT totalIndices_;

	/// edit
	bool isGenerateMeshRequest_;

public:
	/// ==================================================
	/// public : accessors
	/// ==================================================

	int GetSamplePerSegment() const;
	int GetNumControlPoint() const;
	bool GetIsGenerateMeshRequest() const;
	void SetIsGenerateMeshRequest(bool _request);

	const ConstantBuffer<Param>& GetParamBuffer() const;
	const ConstantBuffer<GPUMaterial>& GetMaterialBuffer() const;
	const StructuredBuffer<RiverVertex>& GetRwVertices() const;
	const StructuredBuffer<uint32_t>& GetRwIndices() const;
	const StructuredBuffer<RiverControlPoint>& GetControlPointBuffer() const;
	bool GetIsCreatedBuffers() const;
	UINT GetTotalIndices() const;
	UINT GetTotalVertices() const;
};