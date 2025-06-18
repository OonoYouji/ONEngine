using System.Runtime.CompilerServices;

static public class Input {

	static public bool TriggerKey(DIK keyCode) {
		return InternalTriggerKey((int)keyCode);
	}

	static public bool PressKey(DIK keyCode) {
		return InternalPressKey((int)keyCode);
	}

	static public bool ReleaseKey(DIK keyCode) {
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


}
