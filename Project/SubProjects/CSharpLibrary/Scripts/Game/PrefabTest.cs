
public class PrefabTest : MonoBehavior {

	Vector3 oneRotateValue = new Vector3(0.0f, 0.01f, 0.0f);

	public override void Initialize() {
		Log.WriteLine("PrefabTest initialized.");

		//MeshRenderer mr = entity.GetComponent<MeshRenderer>();

	}

	public override void Update() {
		Log.WriteLine("PrefabTest Update called.");

		if(transform == null) {
			Log.WriteLine("Transform is null in PrefabTest.");
			return;
		}

		Transform t = transform;
		t.scale = Vector3.one * Mathf.Sin(Time.time);
		//t.rotate *= Quaternion.FromEuler(oneRotateValue * Time.deltaTime);
		//t.rotate = t.rotate.Normalized();
	}
}
