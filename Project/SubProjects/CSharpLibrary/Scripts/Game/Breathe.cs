
public class Breathe : MonoBehavior {

	public override void Initialize() {
		Log.WriteLine("Breathe initialized.");
	}

	public override void Update() {
		Log.WriteLine("Breathe Update called.");
		if (transform == null) {
			Log.WriteLine("Transform is null in Breathe.");
			return;
		}

		Transform t = transform;
		// Mathf.Sin(...)を 0~1 の範囲に変換
		t.scale = Vector3.one * (Mathf.Sin(Time.time) * 0.5f + 0.5f);
	}

}
