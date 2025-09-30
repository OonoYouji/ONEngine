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
		uint32_t segments;
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

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	/// splineのコントロールポイント
	std::vector<RiverControlPoint> controlPoints_;
	int samplePerSegment_;

	/// buffer
	ConstantBuffer<Param> paramBuf_;
	StructuredBuffer<RiverControlPoint> controlPointBuf_;
	StructuredBuffer<RiverVertex> rwVertices_;
	bool isCreatedBuffers_;

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
	StructuredBuffer<RiverVertex>& GetRwVerticesRef();
	StructuredBuffer<RiverControlPoint>& GetControlPointBufRef();
	bool GetIsCreatedBuffers() const;
};