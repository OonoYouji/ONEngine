#include "Light.h"

DirectionalLight::DirectionalLight() {
	SetDirection({ 0.0f, -1.0f, 0.0f });
	SetIntensity(1.0f);
	SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
}
