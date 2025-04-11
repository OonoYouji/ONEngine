#pragma once

/// ///////////////////////////////////////////////////
/// ECSの基盤クラス
/// ///////////////////////////////////////////////////
class EntityComponentSystem final {
public:
	/// ===================================================
	/// public : sub class
	/// ====================================================

	/// ///////////////////////////////////////////////////
	/// 
	/// ///////////////////////////////////////////////////
	class IComponentArray {};

	template<typename Comp>
	class ComponentArray final : public IComponentArray {

	};

public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	EntityComponentSystem();
	~EntityComponentSystem();

	void Initialize();
	void Update();
	void Shutdown();

private:
	/// ===================================================
	/// private : objects
	/// ===================================================


};

