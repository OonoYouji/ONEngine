
public class PrefabTest : MonoBehavior {

	Vector3 oneRotateValue = new Vector3(0.0f, 0.01f, 0.0f);

	public override void Initialize() {
		Debug.Log("PrefabTest initialized.");

		//MeshRenderer mr = entity.GetComponent<MeshRenderer>();

	}

	public override void Update() {
		Debug.Log("PrefabTest Update called.");
			
		if(transform == null) {
			Debug.Log("Transform is null in PrefabTest.");
			return;
		}

		Transform t = transform;
		t.scale = Vector3.one * Mathf.Sin(Time.time);
	}
}
