using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class ProfileTest : MonoScript {

	float time = 0f;
	//Matrix4x4[] mat;

	public override void Initialize() {
		//mat = new Matrix4x4[1024];
	}

	public override void Update() {
		time += 1.0f / 60.0f;
		for (int i = 0; i < 1024; i++) {
			Rotate();
			//mat[i] = Matrix4x4.Rotate(transform.rotate);
		}
		//Rotate();
	}

	public void Rotate() {
		float rotateY = time * 20.0f;
		transform.rotate = Quaternion.FromEuler(new Vector3(0f, rotateY, 0f));
	}
}
