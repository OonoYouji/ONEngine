#pragma once

/// std
#include <vector>

/// engine
#include "Engine/Core/Utility/Math/Vector3.h"

/// ///////////////////////////////////////////////////
/// 川のコントロールポイント
/// ///////////////////////////////////////////////////
struct RiverControlPoint {
	Vector3 position;
	float width;
};

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

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	/// splineのコントロールポイント
	std::vector<RiverControlPoint> controlPoints_;
};