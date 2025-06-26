using System.Runtime.CompilerServices;

public static class Log {
	static public void WriteLine(string message) {
		InternalConsoleLog(message);
	}

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalConsoleLog(string s);
}