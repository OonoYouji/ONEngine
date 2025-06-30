using System.Runtime.CompilerServices;

static public class Input {

	static public bool TriggerKey(KeyCode keyCode) {
		return InternalTriggerKey((int)keyCode);
	}

	static public bool PressKey(KeyCode keyCode) {
		return InternalPressKey((int)keyCode);
	}

	static public bool ReleaseKey(KeyCode keyCode) {
		return InternalReleaseKey((int)keyCode);
	}


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
		Log.WriteLine("GamepadThumb: " + axisIndex);
		Vector2 output;
		InternalGetGamepadThumb(axisIndex, out output.x, out output.y);
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


}
