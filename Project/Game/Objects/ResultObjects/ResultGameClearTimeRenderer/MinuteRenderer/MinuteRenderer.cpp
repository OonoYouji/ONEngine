#include "MinuteRenderer.h"

#include "ComponentManager/NumberRenderer/NumberRenderer.h"

MinuteRenderer::MinuteRenderer(float _minute)
	: minute_(_minute) {
	CreateTag(this);
}

MinuteRenderer::~MinuteRenderer() {}

void MinuteRenderer::Initialize() {

	size_t digit = std::to_string(static_cast<int>(minute_)).size();
	auto renderer = AddComponent<NumberRenderer>(static_cast<uint32_t>(digit));
	renderer->SetScore(static_cast<uint32_t>(minute_));

	pTransform_->position.x = -1.0f * static_cast<float>(digit);

}

void MinuteRenderer::Update() {

}

