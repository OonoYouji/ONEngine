#include "FrameResource.h"
#include "RenderDevice.h"
#include "Engine/Common/Assert.h"

namespace Engine::Graphics {

void FrameResource::Initialize(RenderDevice* device) {
    HRESULT hr = device->GetDevice()->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT, 
        IID_PPV_ARGS(&commandAllocator_)
    );
    Assert(SUCCEEDED(hr), "Failed to create CommandAllocator for FrameResource");

    sceneCB_.Create(device, sizeof(GeneratedSchema::SceneData));
}

} // namespace Engine::Graphics
