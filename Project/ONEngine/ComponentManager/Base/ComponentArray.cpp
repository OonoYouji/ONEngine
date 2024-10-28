#include "ComponentArray.h"

#include "../MeshInstancingRenderer/MeshInstancingRenderer.h"
#include "../MeshRenderer/MeshRenderer.h"
#include "../Transform/Transform.h"
#include "../Collider/BaseCollider.h"
#include "../Collider/BoxCollider.h"
#include "../Collider/SphereCollider.h"

template<>
unsigned __int64 ComponentArray<MeshRenderer, void>::componentId_ = 0;

template<>
unsigned __int64 ComponentArray<MeshInstancingRenderer, void>::componentId_ = 0;

template<>
unsigned __int64 ComponentArray<Transform, void>::componentId_ = 0;

template<>
unsigned __int64 ComponentArray<BaseCollider, void>::componentId_ = 0;

template<>
unsigned __int64 ComponentArray<BoxCollider, void>::componentId_ = 0;

template<>
unsigned __int64 ComponentArray<SphereCollider, void>::componentId_ = 0;