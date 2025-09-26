using System;

namespace Ease {
	public static class In {
		public static float Sine(float t) => 1f - (float)Math.Cos((t * Math.PI) / 2f);
		public static float Quad(float t) => t * t;
		public static float Cubic(float t) => t * t * t;
		public static float Quart(float t) => t * t * t * t;
		public static float Quint(float t) => t * t * t * t * t;
		public static float Expo(float t) => (t == 0f) ? 0f : (float)Math.Pow(2f, 10f * (t - 1f));
		public static float Circ(float t) => 1f - (float)Math.Sqrt(1f - t * t);

		public static float Back(float t) {
			const float c1 = 1.70158f;
			const float c3 = c1 + 1f;
			return c3 * t * t * t - c1 * t * t;
		}

		public static float Elastic(float t) {
			if (t == 0f || t == 1f) return t;
			const float c4 = (float)(2 * Math.PI / 3);
			return -(float)Math.Pow(2f, 10f * t - 10f) * (float)Math.Sin((t * 10f - 10.75f) * c4);
		}

		public static float Bounce(float t) => 1f - Out.Bounce(1f - t);
	}

	public static class Out {
		public static float Sine(float t) => (float)Math.Sin((t * Math.PI) / 2f);
		public static float Quad(float t) => 1f - (1f - t) * (1f - t);

		public static float Cubic(float t) {
			float f = t - 1f;
			return f * f * f + 1f;
		}

		public static float Quart(float t) {
			float f = t - 1f;
			return 1f - f * f * f * f;
		}

		public static float Quint(float t) {
			float f = t - 1f;
			return f * f * f * f * f + 1f;
		}

		public static float Expo(float t) => (t == 1f) ? 1f : 1f - (float)Math.Pow(2f, -10f * t);

		public static float Circ(float t) {
			float f = t - 1f;
			return (float)Math.Sqrt(1f - f * f);
		}

		public static float Back(float t) {
			const float c1 = 1.70158f;
			const float c3 = c1 + 1f;
			float f = t - 1f;
			return 1f + c3 * f * f * f + c1 * f * f;
		}

		public static float Elastic(float t) {
			if (t == 0f || t == 1f) return t;
			const float c4 = (float)(2 * Math.PI / 3);
			return (float)Math.Pow(2f, -10f * t) * (float)Math.Sin((t * 10f - 0.75f) * c4) + 1f;
		}

		public static float Bounce(float t) {
			const float n1 = 7.5625f;
			const float d1 = 2.75f;

			if (t < 1f / d1) return n1 * t * t;
			else if (t < 2f / d1) {
				t -= 1.5f / d1;
				return n1 * t * t + 0.75f;
			} else if (t < 2.5f / d1) {
				t -= 2.25f / d1;
				return n1 * t * t + 0.9375f;
			} else {
				t -= 2.625f / d1;
				return n1 * t * t + 0.984375f;
			}
		}
	}

	public static class InOut {
		public static float Sine(float t) => -(float)(Math.Cos(Math.PI * t) - 1f) / 2f;
		public static float Quad(float t) => (t < 0.5f) ? 2f * t * t : 1f - (float)Math.Pow(-2f * t + 2f, 2f) / 2f;
		public static float Cubic(float t) => (t < 0.5f) ? 4f * t * t * t : 1f - (float)Math.Pow(-2f * t + 2f, 3f) / 2f;

		public static float Quart(float t) =>
			(t < 0.5f) ? 8f * t * t * t * t : 1f - (float)Math.Pow(-2f * t + 2f, 4f) / 2f;

		public static float Quint(float t) =>
			(t < 0.5f) ? 16f * t * t * t * t * t : 1f - (float)Math.Pow(-2f * t + 2f, 5f) / 2f;

		public static float Expo(float t) {
			if (t == 0f) return 0f;
			if (t == 1f) return 1f;
			return (t < 0.5f)
				? (float)Math.Pow(2f, 20f * t - 10f) / 2f
				: (2f - (float)Math.Pow(2f, -20f * t + 10f)) / 2f;
		}

		public static float Circ(float t) => (t < 0.5f)
			? (1f - (float)Math.Sqrt(1f - (2f * t) * (2f * t))) / 2f
			: ((float)Math.Sqrt(1f - (2f * t - 2f) * (2f * t - 2f)) + 1f) / 2f;

		public static float Back(float t) {
			const float c1 = 1.70158f;
			const float c2 = c1 * 1.525f;
			return (t < 0.5f)
				? (float)(Math.Pow(2f * t, 2f) * ((c2 + 1f) * 2f * t - c2)) / 2f
				: (float)(Math.Pow(2f * t - 2f, 2f) * ((c2 + 1f) * (t * 2f - 2f) + c2) + 2f) / 2f;
		}

		public static float Elastic(float t) {
			if (t == 0f || t == 1f) return t;
			const float c5 = (float)(2 * Math.PI / 4.5f);
			return (t < 0.5f)
				? -(float)(Math.Pow(2f, 20f * t - 10f) * Math.Sin((20f * t - 11.125f) * c5)) / 2f
				: (float)(Math.Pow(2f, -20f * t + 10f) * Math.Sin((20f * t - 11.125f) * c5)) / 2f + 1f;
		}

		public static float Bounce(float t) => (t < 0.5f)
			? (1f - Out.Bounce(1f - 2f * t)) / 2f
			: (1f + Out.Bounce(2f * t - 1f)) / 2f;
	}
}