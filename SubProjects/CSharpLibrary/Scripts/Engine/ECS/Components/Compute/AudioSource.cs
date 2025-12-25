using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

public class AudioSource : Component {
	///////////////////////////////////////////////////////////////////////////////////////////
	/// objects
	///////////////////////////////////////////////////////////////////////////////////////////
	public float volume = 1f;
	public float pitch = 1f;
	public string path = "";
	private bool isPlayRequest_;

	///////////////////////////////////////////////////////////////////////////////////////////
	/// method
	///////////////////////////////////////////////////////////////////////////////////////////
	//public override void Begin() {
	//	InternalSetParams(nativeHandle, volume, pitch);
	//}

	//public override void End() {
	//	InternalSetParams(nativeHandle, volume, pitch);
		
	//	if (isPlayRequest_) {
	//		isPlayRequest_ = false;
	//		InternalPlayOneShot(nativeHandle, volume, pitch, path);
	//	}
	//}

	public void Play() {
		isPlayRequest_ = true;
	}

	public void OneShotPlay(float _volume, float _pitch, string _path) {
		Debug.Log("AudioSource.OneShotPlay - Playing " + _path);
		InternalPlayOneShot(nativeHandle, _volume, _pitch, _path);
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	/// internal methods
	///////////////////////////////////////////////////////////////////////////////////////////
	[MethodImpl(MethodImplOptions.InternalCall)]
	static private extern void InternalGetParams(ulong _nativeHandle, out float _volume, out float _pitch);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static private extern void InternalSetParams(ulong _nativeHandle, float volume, float pitch);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static private extern void InternalPlayOneShot(ulong _nativeHandle, float _volume, float _pitch, string _path);
}