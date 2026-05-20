using System;

public static class MathUtils {
    private static Random random = new Random();

    /// <summary>
    /// 現在の値から目標値へ、最大速度を超えない範囲で近づけます。
    /// </summary>
    public static float MoveTowards(float current, float target, float maxDelta) {
        if (Mathf.Abs(target - current) <= maxDelta) {
            return target;
        }
        return current + Mathf.Sign(target - current) * maxDelta;
    }

    /// <summary>
    /// 現在の座標から目標座標へ、最大距離を超えない範囲で近づけます。
    /// </summary>
    public static Vector3 MoveTowards(Vector3 current, Vector3 target, float maxDistanceDelta) {
        Vector3 a = target - current;
        float magnitude = a.Length();
        if (magnitude <= maxDistanceDelta || magnitude == 0f) {
            return target;
        }
        return current + a / magnitude * maxDistanceDelta;
    }

    /// <summary>
    /// 値を別の範囲に再マッピングします。
    /// </summary>
    public static float Remap(float value, float fromSource, float toSource, float fromTarget, float toTarget) {
        return (value - fromSource) / (toSource - fromSource) * (toTarget - fromTarget) + fromTarget;
    }

    /// <summary>
    /// 徐々に値を変化させます。
    /// </summary>
    public static float SmoothDamp(float current, float target, ref float currentVelocity, float smoothTime, float maxSpeed, float deltaTime) {
        smoothTime = Math.Max(0.0001f, smoothTime);
        float num = 2f / smoothTime;
        float num2 = num * deltaTime;
        float num3 = 1f / (1f + num2 + 0.48f * num2 * num2 + 0.235f * num2 * num2 * num2);
        float num4 = current - target;
        float num5 = target;
        float num6 = maxSpeed * smoothTime;
        num4 = Mathf.Clamp(num4, -num6, num6);
        target = current - num4;
        float num7 = (currentVelocity + num * num4) * deltaTime;
        currentVelocity = (currentVelocity - num * num7) * num3;
        float num8 = target + (num4 + num7) * num3;
        if (num5 - current > 0f == num8 > num5) {
            num8 = num5;
            currentVelocity = (num8 - num5) / deltaTime;
        }
        return num8;
    }

    /// <summary>
    /// 徐々に座標を変化させます。
    /// </summary>
    public static Vector3 SmoothDamp(Vector3 current, Vector3 target, ref Vector3 currentVelocity, float smoothTime, float maxSpeed, float deltaTime) {
        float velocityX = currentVelocity.x;
        float velocityY = currentVelocity.y;
        float velocityZ = currentVelocity.z;
        float x = SmoothDamp(current.x, target.x, ref velocityX, smoothTime, maxSpeed, deltaTime);
        float y = SmoothDamp(current.y, target.y, ref velocityY, smoothTime, maxSpeed, deltaTime);
        float z = SmoothDamp(current.z, target.z, ref velocityZ, smoothTime, maxSpeed, deltaTime);
        currentVelocity = new Vector3(velocityX, velocityY, velocityZ);
        return new Vector3(x, y, z);
    }

    /// <summary>
    /// 半径1の球体上のランダムな座標を返します。
    /// </summary>
    public static Vector3 RandomOnUnitSphere() {
        float z = (float)(random.NextDouble() * 2.0 - 1.0);
        float phi = (float)(random.NextDouble() * 2.0 * Math.PI);
        float r = (float)Math.Sqrt(1.0 - z * z);
        return new Vector3(r * (float)Math.Cos(phi), r * (float)Math.Sin(phi), z);
    }
}
