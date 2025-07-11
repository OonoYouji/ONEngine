using System;

public class Enemy : MonoBehavior {

	float hp;
	bool isAlive;

	public override void Initialize() {
		hp = 100f;
		isAlive = true;
	}

	public override void Update() {

		if (hp <= 0f) {
			isAlive = false;
			Debug.Log("Enemy defeated." + entity.Id);
			return;
		}
	}


	public bool IsAlive { 
		get {
			return isAlive;
		}
	}

}