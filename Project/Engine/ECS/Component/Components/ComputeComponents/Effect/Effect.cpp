#include "Effect.h"

/// engine
#include "Engine/Core/Utility/Math/Vector4.h"

Effect::Effect() {
	isCreateParticle_ = true;
	emitInstanceCount_ = 10;
	//emittedElementColor_ = Vector4::kWhite;
	startData_.size = Vector3::kOne;
	startData_.rotate = Vector3::kZero;
	startData_.color.first = Vector4::kWhite;
	startData_.color.second = Vector4::kWhite;
}

void Effect::CreateElement(const Vector3& _position, const Color& _color) {
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

void Effect::CreateElement(const Vector3& _position, const Vector3& _velocity, const Color& _color) {
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

void Effect::CreateElement(const Vector3& _position, const Vector3& _scale, const Vector3& _rotate, const Vector3& _velocity, const Color& _color) {
	Element element;
	element.transform.position = _position;
	element.transform.scale = _scale;
	element.transform.rotate = _rotate;
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

void Effect::SetEmitTypeTime(const TimeEmitData& _data, size_t _emitInstanceCount) {
	emitType_ = EmitType::Time;
	timeEmitData_ = _data;
	emitInstanceCount_ = _emitInstanceCount;
}

void Effect::SetEmitShape(const Vector3& _center, float _radius) {
	emitShape_.SetShapeType(EffectEmitShape::ShapeType::Sphere);
	emitShape_.SetSphere(_center, _radius);
}

void Effect::SetEmitShape(const Vector3& _center, const Vector3& _size) {
	emitShape_.SetShapeType(EffectEmitShape::ShapeType::Cube);
	emitShape_.SetCube(_center, _size);
}

void Effect::SetEmitShape(const Vector3& _apex, float _angle, float _radius, float _height) {
	emitShape_.SetShapeType(EffectEmitShape::ShapeType::Cone);
	emitShape_.SetCone(_apex, _angle, _radius, _height);
}
