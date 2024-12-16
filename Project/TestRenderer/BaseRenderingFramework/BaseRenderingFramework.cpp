#include "BaseRenderingFramework.h"

#include "../BaseRenderer/BaseRenderer.h"


void BaseRenderingFramework::AddRenderer(const std::string& _name, std::unique_ptr<class BaseRenderer> _renderer) {
	renderers_[_name] = std::move(_renderer);
}
