using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class SceneLoader : MonoBehavior {
	[SerializeField] private bool isLoading_ = false;
	[SerializeField] public string sceneName = "";

	public override void Initialize() {
		
	}

	public override void Update() {
		if (isLoading_) {
			SceneManager.LoadScene(sceneName);
		}
	}

	public void Load(string _sceneName) {
		isLoading_ = true;
		sceneName = _sceneName;
	}
	
}