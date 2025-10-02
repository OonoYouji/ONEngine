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
#include "Engine/Graphics/Buffer/Data/Material.h"

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

	void DrawSplineCurve();

	void CreateBuffers(class DxDevice* _dxDevice, class DxSRVHeap* _dxSRVHeap, class DxCommand* _dxCommand);
	void SetBufferData();

	void SetMaterialData(int32_t _entityId, int32_t _texIndex);

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
	ConstantBuffer<Material> materialBuffer_;
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

	ConstantBuffer<Param>& GetParamBufRef();
	ConstantBuffer<Material>& GetMaterialBufRef();
	StructuredBuffer<RiverVertex>& GetRwVerticesRef();
	StructuredBuffer<uint32_t>& GetRwIndicesRef();
	StructuredBuffer<RiverControlPoint>& GetControlPointBufRef();
	bool GetIsCreatedBuffers() const;
	UINT GetTotalIndices() const;
	UINT GetTotalVertices() const;
};