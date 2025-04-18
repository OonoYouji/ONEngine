#include "Effect.h"

/// engine
#include "Engine/Core/Utility/Math/Vector4.h"

void Effect::CreateElement(const Vector3& _position, const Vector4& _color) {
	Element element;
	element.transform.position = _position;
	element.transform.scale    = Vector3::kOne;
	element.transform.rotate   = Vector3::kZero;
	element.transform.Update();

	element.color = _color;
	element.lifeTime = lifeLeftTime_;
	element.velocity = Vector3::kZero;
	elements_.push_back(element);
}

void Effect::CreateElement(const Vector3& _position, const Vector3& _velocity, const Vector4& _color) {
	Element element;
	element.transform.position = _position;
	element.transform.scale    = Vector3::kOne;
	element.transform.rotate   = Vector3::kZero;
	element.transform.Update();

	element.color = _color;
	element.lifeTime = lifeLeftTime_;
	element.velocity = _velocity;
	elements_.push_back(element);
}

void Effect::SetEmitTypeDistance(float _interval, size_t _emitInstanceCount) {
	emitType_ = EmitType::Distance;
	distanceEmitData_.emitDistance = _interval;
	distanceEmitData_.emitInterval = _interval;
	emitInstanceCount_ = _emitInstanceCount;
}

void Effect::SetEmitType(const TimeEmitData& _data) {
	emitType_ = EmitType::Time;
	timeEmitData_ = _data;
}
