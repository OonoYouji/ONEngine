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

	virtual void BackSpriteDraw() {}
	virtual void Draw() {}
	virtual void FrontSpriteDraw() {}

	virtual void Debug() {}

	/// ===================================================
	/// public : methods
	/// ===================================================

	BaseGameObject* GetParent() const { return parent_; }
	void SetParent(BaseGameObject* parent);

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

	BaseGameObject* parent_;
	std::string name_;
};