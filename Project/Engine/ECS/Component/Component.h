#pragma once

/// ///////////////////////////////////////////////////
/// component include
/// ///////////////////////////////////////////////////

/// computes
#include "Components/ComputeComponents/Transform/Transform.h"
#include "Components/ComputeComponents/Light/Light.h"
#include "Components/ComputeComponents/Audio/AudioSource.h"
#include "Components/ComputeComponents/Variables/Variables.h"
#include "Components/ComputeComponents/Effect/Effect.h"
#include "Components/ComputeComponents/Script/Script.h"

/// renderer
#include "Components/RendererComponents/Mesh/MeshRenderer.h"
#include "Components/RendererComponents/Mesh/CustomMeshRenderer.h"
#include "Components/RendererComponents/SkinMesh/SkinMeshRenderer.h"
#include "Components/RendererComponents/Sprite/SpriteRenderer.h"
#include "Components/RendererComponents/Primitive/Line2DRenderer.h"
#include "Components/RendererComponents/Primitive/Line3DRenderer.h"
#include "Components/RendererComponents/ScreenPostEffectTag/ScreenPostEffectTag.h"

/// collision
#include "Components/ComputeComponents/Collision/CollisionCheck/CollisionCheck.h"
#include "Components/ComputeComponents/Collision/SphereCollider.h"
#include "Components/ComputeComponents/Collision/BoxCollider.h"