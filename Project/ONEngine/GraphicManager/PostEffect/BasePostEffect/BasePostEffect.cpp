#include "BasePostEffect.h"

BasePostEffect::BasePostEffect(const std::string& _uavTextureName) 
	: uavTextureName_(_uavTextureName) {

}

void BasePostEffect::SetUAVTextureName(const std::string& _uavTextureName) {
	uavTextureName_ = _uavTextureName;
}
