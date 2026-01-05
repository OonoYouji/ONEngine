using System.Runtime.CompilerServices;

static public class Input {

	// ===================================================================
	// キーボード入力
	// ===================================================================

	static public bool TriggerKey(KeyCode keyCode) {
		return InternalTriggerKey((int)keyCode);
	}

	static public bool PressKey(KeyCode keyCode) {
		return InternalPressKey((int)keyCode);
	}

	static public bool ReleaseKey(KeyCode keyCode) {
		return InternalReleaseKey((int)keyCode);
	}

	// ===================================================================
	// ゲームパッド入力
	// ===================================================================

	static public bool TriggerGamepad(Gamepad gamepad) {
		return InternalTriggerGamepad((int)gamepad);
	}

	static public bool PressGamepad(Gamepad gamepad) {
		return InternalPressGamepad((int)gamepad);
	}

	static public bool ReleaseGamepad(Gamepad gamepad) {
		return InternalReleaseGamepad((int)gamepad);
	}

	static public Vector2 GamepadThumb(GamepadAxis axis) {
		int axisIndex = (int)axis;
		Debug.Log("GamepadThumb: " + axisIndex);
		Vector2 output;
		InternalGetGamepadThumb(axisIndex, out output.x, out output.y);
		return output;
	}

	static public Vector2 KeyboardAxis(KeyboardAxis axis) {
		Vector2 output = new Vector2(0.0f, 0.0f);
		if (axis == global::KeyboardAxis.WASD) {
			if (PressKey(KeyCode.W)) output.y += 1.0f;
			if (PressKey(KeyCode.S)) output.y -= 1.0f;
			if (PressKey(KeyCode.A)) output.x -= 1.0f;
			if (PressKey(KeyCode.D)) output.x += 1.0f;
		} else if (axis == global::KeyboardAxis.Arrow) {
			if (PressKey(KeyCode.UpArrow)) output.y += 1.0f;
			if (PressKey(KeyCode.DownArrow)) output.y -= 1.0f;
			if (PressKey(KeyCode.LeftArrow)) output.x -= 1.0f;
			if (PressKey(KeyCode.RightArrow)) output.x += 1.0f;
		}
		// 正規化
		if (output.x != 0.0f || output.y != 0.0f) {
			float length = Mathf.Sqrt(output.x * output.x + output.y * output.y);
			output.x /= length;
			output.y /= length;
		}
		return output;
	}

	// ===================================================================
	// マウス入力
	// ===================================================================

	static public bool TriggerMouse(Mouse _mouse) {
		return InternalTriggerMouse((int)_mouse);
	}

	static public bool PressMouse(Mouse _mouse) {
		return InternalPressMouse((int)_mouse);
	}

	static public bool ReleaseMouse(Mouse _mouse) {
		return InternalReleaseMouse((int)_mouse);
	}

	static public Vector2 MouseVelocity() {
		Vector2 output;
		InternalGetMouseVelocity(out output.x, out output.y);
		return output;
	}

	static public float MouseWheel() {
		float wheel;
		InternalGetMouseWheel(out wheel);
		return wheel;
	}

	static public Vector2 MousePosition() {
		Vector2 output;
		InternalGetMousePosition(out output.x, out output.y);
		return output;
	}

	/// ==================================
	/// c++側で実装される関数
	/// ==================================

	[MethodImpl(MethodImplOptions.InternalCall)]
	static private extern bool InternalTriggerKey(int dik);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static private extern bool InternalPressKey(int dik);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static private extern bool InternalReleaseKey(int dik);


	[MethodImpl(MethodImplOptions.InternalCall)]
	static private extern bool InternalTriggerGamepad(int gamepad);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static private extern bool InternalPressGamepad(int gamepad);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static private extern bool InternalReleaseGamepad(int gamepad);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalGetGamepadThumb(int axisIndex, out float _x, out float _y);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern bool InternalTriggerMouse(int _mouse);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern bool InternalPressMouse(int _mouse);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern bool InternalReleaseMouse(int _mouse);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalGetMouseVelocity(out float _x, out float _y);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalGetMouseWheel(out float _w);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalGetMousePosition(out float _x, out float _y);

}
