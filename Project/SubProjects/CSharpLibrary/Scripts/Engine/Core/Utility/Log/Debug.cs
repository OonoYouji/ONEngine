using System.Runtime.CompilerServices;

static public class Debug {
	static public void Log(string message) {
#if DEBUG
		InternalConsoleLog("[script] " + message);
#endif
	}

	static public void LogInfo(string message) {
		Log("[info] " + message);
	}

	static public void LogWarning(string message) {
		Log("[warning] " + message);
	}

	static public void LogError(string message) {
		Log("[error] " + message);
	}


	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalConsoleLog(string s);
}