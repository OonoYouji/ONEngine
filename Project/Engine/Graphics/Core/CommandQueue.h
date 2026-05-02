#pragma once

namespace Engine::Graphics {
class RenderDevice;
}

namespace Engine::Graphics {

///
/// DX12のコマンドキューを管理するクラス
///
class CommandQueue final {
public:

	CommandQueue();
	~CommandQueue();

	void Initialize(RenderDevice* renderDevice);
	void Shutdown();

private:

};

} /// namespace Engine::Graphics