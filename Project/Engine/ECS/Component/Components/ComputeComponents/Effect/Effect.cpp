#include "Effect.h"

/// engine
#include "Engine/Core/Utility/Math/Vector4.h"

Effect::Effect() {
	isCreateParticle_ = true;
	emitInstanceCount_ = 10;
	//emittedElementColor_ = Vector4::kWhite;
	/*startData_.size = Vector3::kOne;
	startData_.rotate = Vector3::kZero;
	startData_.color.first = Vector4::kWhite;
	startData_.color.second = Vector4::kWhite;*/
}

void Effect::CreateElement(const Vector3& _position, const Color& _color) {
	Element element;
	element.transform.position = _position;
	element.transform.scale = Vector3::kOne;
	element.transform.rotate = Vector3::kZero;
	element.transform.Update();

	element.color = _color;
	element.lifeTime = mainModule_.lifeLeftTime_;
	element.velocity = Vector3::kZero;
	elements_.push_back(element);
}

void Effect::CreateElement(const Vector3& _position, const Vector3& _velocity, const Color& _color) {
	Element element;
	element.transform.position = _position;
	element.transform.scale = Vector3::kOne;
	element.transform.rotate = Vector3::kZero;
	element.transform.Update();

	element.color = _color;
	element.lifeTime = mainModule_.lifeLeftTime_;
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
	element.lifeTime = mainModule_.lifeLeftTime_;
	element.velocity = _velocity;
	elements_.push_back(element);
}

void Effect::RemoveElement(size_t _index) {
	if (_index < elements_.size()) {
		elements_.erase(elements_.begin() + _index);
	}
}

void Effect::SetMainModule(const EffectMainModule& _module) {
	mainModule_ = _module;
}

void Effect::SetEmitShape(const EffectEmitShape& _shape) {
	emitShape_ = _shape;
}

void Effect::SetEmitType(EmitType _type) {
	emitType_ = _type;
}

void Effect::SetMaxEffectCount(size_t _maxCount) {
	maxEffectCount_ = _maxCount;
	elements_.reserve(maxEffectCount_);
}

void Effect::SetEmitTypeDistance(float _interval, size_t _emitInstanceCount) {
	emitType_ = EmitType::Distance;
	distanceEmitData_.emitDistance = _interval;
	distanceEmitData_.emitInterval = _interval;
	emitInstanceCount_ = _emitInstanceCount;
}

void Effect::SetEmitTypeDistance(const DistanceEmitData& _data) {
	distanceEmitData_ = _data;
}

void Effect::SetEmitTypeTime(const TimeEmitData& _data, size_t _emitInstanceCount) {
	emitType_ = EmitType::Time;
	timeEmitData_ = _data;
	emitInstanceCount_ = _emitInstanceCount;
}

void Effect::SetEmitTypeTime(const TimeEmitData& _data) {
	timeEmitData_ = _data;
}

void Effect::SetEmitInstanceCount(size_t _emitInstanceCount) {
	emitInstanceCount_ = _emitInstanceCount;
}

void Effect::SetLifeLeftTime(float _time) {
	mainModule_.lifeLeftTime_ = _time;
}

void Effect::SetElementUpdateFunc(std::function<void(Element*)> _func) {
	elementUpdateFunc_ = _func;
}

void Effect::SetUseBillboard(bool _use) {
	useBillboard_ = _use;
}

void Effect::SetIsCreateParticle(bool _isCreateParticle) {
	isCreateParticle_ = _isCreateParticle;
}

void Effect::SetBlendMode(BlendMode _blendMode) {
	blendMode_ = _blendMode;
}

void Effect::SetStartSize(const Vector3& _size) {
	mainModule_.SetSizeStartData(_size);
}

void Effect::SetStartSize(const Vector3& _size1, const Vector3& _size2) {
	mainModule_.SetSizeStartData(std::make_pair(_size1, _size2));
}

void Effect::SetStartRotate(const Vector3& _rotate) {
	mainModule_.SetRotateStartData(_rotate);
}

void Effect::SetStartRotate(const Vector3& _rotate1, const Vector3& _rotate2) {
	mainModule_.SetRotateStartData(std::make_pair(_rotate1, _rotate2));
}

void Effect::SetStartColor(const Color& _color) {
	mainModule_.SetColorStartData(_color);
}

void Effect::SetStartColor(const Color& _color1, const Color& _color2) {
	mainModule_.SetColorStartData(std::make_pair(_color1, _color2));
}

void Effect::SetStartSpeed(float _speed) {
	mainModule_.SetSpeedStartData(_speed);
}

void Effect::SetStartSpeed(float _speed1, float _speed2) {
	mainModule_.SetSpeedStartData(std::make_pair(_speed1, _speed2));
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

bool Effect::IsCreateParticle() const {
	return isCreateParticle_;
}

size_t Effect::GetMaxEffectCount() const {
	return maxEffectCount_;
}

const std::string& Effect::GetMeshPath() const {
	return meshPath_;
}

const std::string& Effect::GetTexturePath() const {
	return texturePath_;
}

const std::vector<Effect::Element>& Effect::GetElements() const {
	return elements_;
}

Effect::BlendMode Effect::GetBlendMode() const {
	return blendMode_;
}

EffectMainModule* Effect::GetMainModule() {
	return &mainModule_;
}

const EffectMainModule& Effect::GetMainModule() const {
	return mainModule_;
}

EffectEmitShape* Effect::GetEmitShape() {
	return &emitShape_;
}

const EffectEmitShape& Effect::GetEmitShape() const {
	return emitShape_;
}

int Effect::GetEmitType() const {
	return static_cast<int>(emitType_);
}

const Effect::DistanceEmitData& Effect::GetDistanceEmitData() const {
	return distanceEmitData_;
}

const Effect::TimeEmitData& Effect::GetTimeEmitData() const {
	return timeEmitData_;
}

size_t Effect::GetEmitInstanceCount() const {
	return emitInstanceCount_;
}
