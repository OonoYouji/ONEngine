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

	static public Quaternion LookAt(Vector3 _position, Vector3 _target, Vector3 _up) {

		// forward
		Vector3 forward = Vector3.Normalize(_target - _position);

		// forward と up の平行対策
		float dot = Vector3.Dot(forward, _up);
		if (Mathf.Abs(dot) > 0.999f) {
			// 上すぎ問題の応急処置
			_up = new Vector3(0, 0, 1);
		}

		// 左手系の LookTo 行列を作る
		Matrix4x4 view = Matrix4x4.CreateLookToLH(_position, forward, _up);

		// カメラのワールド行列 = view の逆行列
		Matrix4x4 world = Matrix4x4.Inverse(view);

		// 回転部分をクォータニオンへ
		Quaternion rot = Quaternion.CreateFromRotationMatrix(world);
		return Quaternion.Normalized(rot);
	}


	static public Quaternion CreateFromRotationMatrix(Matrix4x4 _m) {
		float trace = _m.m00 + _m.m11 + _m.m22;

		Quaternion q = new Quaternion();

		if (trace > 0.0f) {
			float s = Mathf.Sqrt(trace + 1.0f) * 2.0f; // 4 * w
			q.w = 0.25f * s;
			q.x = (_m.m21 - _m.m12) / s;
			q.y = (_m.m02 - _m.m20) / s;
			q.z = (_m.m10 - _m.m01) / s;
		} else if (_m.m00 > _m.m11 && _m.m00 > _m.m22) {
			float s = Mathf.Sqrt(1.0f + _m.m00 - _m.m11 - _m.m22) * 2.0f; // 4 * x
			q.w = (_m.m21 - _m.m12) / s;
			q.x = 0.25f * s;
			q.y = (_m.m01 + _m.m10) / s;
			q.z = (_m.m02 + _m.m20) / s;
		} else if (_m.m11 > _m.m22) {
			float s = Mathf.Sqrt(1.0f + _m.m11 - _m.m00 - _m.m22) * 2.0f; // 4 * y
			q.w = (_m.m02 - _m.m20) / s;
			q.x = (_m.m01 + _m.m10) / s;
			q.y = 0.25f * s;
			q.z = (_m.m12 + _m.m21) / s;
		} else {
			float s = Mathf.Sqrt(1.0f + _m.m22 - _m.m00 - _m.m11) * 2.0f; // 4 * z
			q.w = (_m.m10 - _m.m01) / s;
			q.x = (_m.m02 + _m.m20) / s;
			q.y = (_m.m12 + _m.m21) / s;
			q.z = 0.25f * s;
		}

		// 任意のQuaternion型にNormalizeが無い場合は自前で処理
		float len = Mathf.Sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
		if (len > 1e-8f) {
			float inv = 1.0f / len;
			q.x *= inv;
			q.y *= inv;
			q.z *= inv;
			q.w *= inv;
		}

		return q;
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

		Quaternion q;
		q.x = cosYaw * sinPitch * cosRoll + sinYaw * cosPitch * sinRoll;
		q.y = sinYaw * cosPitch * cosRoll - cosYaw * sinPitch * sinRoll;
		q.z = cosYaw * cosPitch * sinRoll - sinYaw * sinPitch * cosRoll;
		q.w = cosYaw * cosPitch * cosRoll + sinYaw * sinPitch * sinRoll;

		return q;
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