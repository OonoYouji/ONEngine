#include "BaseRenderable.h"



void BaseRenderable::SettingRenderer(const std::unordered_map<std::string, std::unique_ptr<BaseRenderable>>& _rendererMap) {
	renderer_ = _rendererMap.at(rendererName_).get();
}
