#pragma once

#include <wrl/client.h>

#include <dinput.h>

#include "Math/Vector2.h"
#include "Math/Vector3.h"

namespace ONE { class WinApp; }


enum class MouseCode {
	Left ,
	Right,
	Whell,	Center = Whell,
	Side1,
	Side2,
	Side3,
	Side4,
	Count, // 使用禁止
};

/// ===================================================
/// マウス入力
/// ===================================================
class Mouse final {
public:

	Mouse();
	~Mouse();

	/// ===================================================
	/// public : methods
	/// ===================================================

	void Initialize(IDirectInput8* directInput, ONE::WinApp* winApp);

	void Begin();

	void ConfineCursorToWindow(HWND _hwnd);


	bool Press(MouseCode code) const;
	bool Trigger(MouseCode code) const;
	bool Release(MouseCode code) const;

	float GetScroll() const { return static_cast<float>(state_.lZ); }

	inline const Vec2& GetPosition() const { return position_; }
	inline const Vec2& GetVelocity() const { return velocity_; }

	inline bool GetIsCursorConfined() const { return isCursorConfined_; }

	Vec3 MouseRay(float distance);
	Vec3 MouseNearPosition();
	Vec3 MouseFarPosition();

private:
	
	/// ===================================================
	/// private : objects
	/// ===================================================

	ONE::WinApp* pWinApp_ = nullptr;

	Microsoft::WRL::ComPtr<IDirectInputDevice8> mouse_;

	DIMOUSESTATE2 state_;
	DIMOUSESTATE2 preState_;

	Vec2 position_;
	Vec2 velocity_;

	bool isCursorConfined_ = false; /// カーソルがウィンドウ内に制限されているか

private:
	Mouse(const Mouse&) = delete;
	Mouse(Mouse&&) = delete;
	Mouse& operator= (const Mouse&) = delete;
	Mouse& operator= (Mouse&&) = delete;
};