#include "GamePostEffectPipelineRegistry.h"

/// engine
#include "GraphicManager/PostEffect/Grayscale/Grayscale.h"


GamePostEffectPipelineRegistry::GamePostEffectPipelineRegistry() {
	Register("grayscale", std::make_unique<Grayscale>());
}

GamePostEffectPipelineRegistry::~GamePostEffectPipelineRegistry() {}
