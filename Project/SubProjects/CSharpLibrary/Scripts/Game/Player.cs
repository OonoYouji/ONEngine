using System.Diagnostics;

public class Player : MonoBehavior {

	public override void Initialize() {
		entityId = 5;
		//entityId = Internal_GetEntityId(typeof(Player));
	}

	public override void Update() {
		//Debug.WriteLine("Player Update");

		Transform t = transform;
		t.position += Vector3.forward;
		transform = t;
	}

}
