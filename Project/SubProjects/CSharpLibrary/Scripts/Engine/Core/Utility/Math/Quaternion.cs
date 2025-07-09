using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;


[StructLayout(LayoutKind.Sequential)]
public struct Quaternion {

	public float x, y, z, w;

	public Quaternion(float x, float y, float z, float w) {
		this.x = x;
		this.y = y;
		this.z = z;
		this.w = w;
	}

	/// -----------------------------------------
	/// static public 定数
	/// -----------------------------------------

	static public Quaternion identity {
		get {
			return new Quaternion(0, 0, 0, 1);
		}
	}


	/// -----------------------------------------
	/// static public methods
	/// -----------------------------------------

	static public float Length(Quaternion _q) {
		return _q.Length();
	}

	static public Quaternion Normalized(Quaternion _q) {
		return _q.Normalized();
	}

	static public Quaternion Conjugate(Quaternion _q) {
		return _q.Conjugate();
	}

	static public Quaternion Inverse(Quaternion _q) {
		return _q.Inverse();
	}

	static public Quaternion Lerp(Quaternion _q1, Quaternion _q2, float _t) {
		// 線形補間
		return new Quaternion(
			Mathf.Lerp(_q1.x, _q2.x, _t),
			Mathf.Lerp(_q1.y, _q2.y, _t),
			Mathf.Lerp(_q1.z, _q2.z, _t),
			Mathf.Lerp(_q1.w, _q2.w, _t)
		);
	}

	static public Quaternion MakeFromAxis(Vector3 _axis, float _angle) {
		/// _angle はラジアンで指定
		float halfAngle = _angle * 0.5f;
		float sinHalfAngle = Mathf.Sin(halfAngle);

		Vector3 normalizedAxis = _axis.Normalized();

		return new Quaternion(
			normalizedAxis.x * sinHalfAngle,
			normalizedAxis.y * sinHalfAngle,
			normalizedAxis.z * sinHalfAngle,
			Mathf.Cos(halfAngle)
		);
	}


	/// -----------------------------------------
	/// public methods
	/// -----------------------------------------

	public float Length() {
		return Mathf.Sqrt(x * x + y * y + z * z + w * w);
	}

	public Quaternion Normalized() {
		float length = Length();
		if (length == 0.0f) {
			return identity;
		}

		return new Quaternion(x / length, y / length, z / length, w / length);
	}

	public Quaternion Conjugate() {
		return new Quaternion(-x, -y, -z, w);
	}

	public Quaternion Inverse() {
		Quaternion conjugate = Conjugate(); // 共役を計算
		float norm = Length();                // ノルムを計算
		if (norm == 0.0f) {
			return identity; // ノルムが0の場合は単位クォータニオンを返す
		}

		float normSquared = norm * norm;    // ノルムの二乗
		return conjugate / normSquared;
	}

	public Vector3 ToEuler() {
		Vector3 euler = new Vector3();

		// Pitch (X軸)
		float sinp = 2.0f * (w * x + y * z);
		float cosp = 1.0f - 2.0f * (x * x + y * y);
		euler.x = Mathf.Atan2(sinp, cosp);

		// Yaw (Y軸)
		float siny = 2.0f * (w * y - z * x);
		if (Mathf.Abs(siny) >= 1.0f) {
			euler.y = Mathf.CopySign(Mathf.PI / 2.0f, siny); // クランプ
		} else {
			euler.y = Mathf.Asin(siny);
		}

		// Roll (Z軸)
		float sinr = 2.0f * (w * z + x * y);
		float cosr = 1.0f - 2.0f * (y * y + z * z);
		euler.z = Mathf.Atan2(sinr, cosr);

		return euler;
	}

	static public Quaternion FromEuler(Vector3 _euler) {
		float pitch = _euler.x * 0.5f; // X回転
		float yaw = _euler.y * 0.5f; // Y回転
		float roll = _euler.z * 0.5f; // Z回転

		float sinPitch = Mathf.Sin(pitch);
		float cosPitch = Mathf.Cos(pitch);
		float sinYaw = Mathf.Sin(yaw);
		float cosYaw = Mathf.Cos(yaw);
		float sinRoll = Mathf.Sin(roll);
		float cosRoll = Mathf.Cos(roll);

		return new Quaternion(
			cosYaw * cosPitch * cosRoll + sinYaw * sinPitch * sinRoll,
			cosYaw * sinPitch * cosRoll + sinYaw * cosPitch * sinRoll,
			sinYaw * cosPitch * cosRoll - cosYaw * sinPitch * sinRoll,
			cosYaw * cosPitch * sinRoll - sinYaw * sinPitch * cosRoll
		);
	}


	/// -------------------------------------------
	/// public operators
	/// - -----------------------------------------

	static public Quaternion operator *(Quaternion _q1, Quaternion _q2) {
		return new Quaternion(
			_q1.w * _q2.x + _q1.x * _q2.w + _q1.y * _q2.z - _q1.z * _q2.y,
			_q1.w * _q2.y + _q1.y * _q2.w + _q1.z * _q2.x - _q1.x * _q2.z,
			_q1.w * _q2.z + _q1.z * _q2.w + _q1.x * _q2.y - _q1.y * _q2.x,
			_q1.w * _q2.w - _q1.x * _q2.x - _q1.y * _q2.y - _q1.z * _q2.z
		);
	}

	static public Quaternion operator /(Quaternion _q, float _scalar) {
		return new Quaternion(
			_q.w / _scalar,
			_q.x / _scalar,
			_q.y / _scalar,
			_q.z / _scalar
		);
	}

}