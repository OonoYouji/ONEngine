#pragma once

namespace Engine::ECS {

class Registry;

///
/// ECSのシステムの基底クラス
///
class System {
public:
	virtual ~System() = default;
	virtual void Update(Registry& registry) = 0;
};

} // namespace Engine::ECS
