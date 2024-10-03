#pragma once

#include <string>

class BaseGameObject;

/// ===================================================
/// すべてのコンポーネントの基底クラス
/// ===================================================
class BaseComponent {
public:
	/// ===================================================
	/// public : virtual methods
	/// ===================================================

	virtual ~BaseComponent() {}

	virtual void Initialize() {}

	virtual void Update() {}
	virtual void LastUpdate() {}

	virtual void Draw() {}

	virtual void Debug() {}

	/// ===================================================
	/// public : methods
	/// ===================================================

	BaseGameObject* GetOwner() const { return owner_; }
	void SetOwner(BaseGameObject* owner);

	const std::string& GetName() const { return name_; }
	void SetName(const std::string& name);

public:
	/// ===================================================
	/// public : objects
	/// ===================================================

	bool isActive = true;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	BaseGameObject* owner_ = nullptr;
	std::string     name_;
};