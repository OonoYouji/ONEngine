
using System.Runtime.InteropServices;

[StructLayout(LayoutKind.Sequential)]
public struct Flag {
	public bool current;
	public bool prev;
	
	public void Update() {
		prev = current;
	}

	public bool Press() {
		return current;
	}

	public bool Trigger() {
		return current && !prev;
	}

	public bool Release() {
		return !current && prev;
	}

	public void Set(bool value) {
		current = value;
	}	
}