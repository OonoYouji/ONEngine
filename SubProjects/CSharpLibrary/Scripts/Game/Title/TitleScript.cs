using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class TitleScript : MonoBehavior {
	
	
	public override void Initialize() {
	}

	public override void Update() {
		if (Input.TriggerKey(KeyCode.Space)) {
			SceneLoader sceneLoader = entity.GetScript<SceneLoader>();
			if (sceneLoader) {
				sceneLoader.Load("GameScene");
			}
		}
	}

}