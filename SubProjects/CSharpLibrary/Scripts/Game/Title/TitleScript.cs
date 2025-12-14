using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class TitleScript : MonoScript {
	
	
	public override void Initialize() {
	}

	public override void Update() {
		Debug.Log("TitleScript.Update - call update");
		
		if (Input.TriggerKey(KeyCode.Space) || Input.TriggerGamepad(Gamepad.A)) {
			SceneLoader sceneLoader = entity.GetScript<SceneLoader>();
			if (sceneLoader) {
				Debug.Log("TitleScript.Update - SceneLoading");
				sceneLoader.Load("GameScene");
			}
		}
	}

}