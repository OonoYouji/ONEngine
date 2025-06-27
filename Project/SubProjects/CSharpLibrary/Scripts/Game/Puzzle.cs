using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class Puzzle : MonoBehavior {

	public override void Initialize() {
	}

	public override void Update() {
	}


	public override void OnCollisionEnter(Entity collision) {
		// パズルが他のエンティティと衝突したときの処理
		Log.WriteLine("collided:" + collision.name);
	}



}
