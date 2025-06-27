using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class Puzzle : MonoBehavior {

	public override void Initialize() {
	}

	public override void Update() {

		Transform t = transform;

		Vector3 rotate = t.rotate;
		Vector3 position = t.position;

		position.y = Mathf.Sin(Time.time) * 3f + 3f; // Y軸を上下に動かす
		rotate.y = Mathf.Sin(Time.time) * Mathf.Sin(Time.time); // Y軸を回転

		t.rotate = rotate;
		t.position = position;

	}


	public override void OnCollisionEnter(Entity collision) {
		// パズルが他のエンティティと衝突したときの処理
		//Log.WriteLine("collided:" + collision.name);
	}



}
