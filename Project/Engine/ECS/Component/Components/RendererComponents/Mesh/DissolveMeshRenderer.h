#pragma once

/// externals
#include <nlohmann/json.hpp>

/// engine
#include "../../Interface/IComponent.h"
#include "Engine/Asset/Guid/Guid.h"
#include "Engine/Asset/Assets/Mateiral/Material.h"

namespace ONEngine {

/// ///////////////////////////////////////////////////
/// コンポーネントのデバッグ用関数
/// ///////////////////////////////////////////////////
class DissolveMeshRenderer;
namespace ComponentDebug {
void ShowGUI(DissolveMeshRenderer* _dmr);
}


/// ///////////////////////////////////////////////////
/// メッシュをディゾルブ表現で表示するためのコンポーネント
/// ///////////////////////////////////////////////////
class DissolveMeshRenderer : public IRenderComponent {
	friend void from_json(const nlohmann::json& _j, DissolveMeshRenderer& _dmr);
	friend void to_json(nlohmann::json& _j, const DissolveMeshRenderer& _dmr);
public:
	/// ===========================================
	/// public : methods
	/// ===========================================

	DissolveMeshRenderer();
	~DissolveMeshRenderer();

private:
	/// ===========================================
	/// private : objects
	/// ===========================================

	Guid meshGuid_;
	Material material_;
	Guid dissolveTexture_;
};

} /// namespace ONEngine