using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.CompilerServices;

static public class SceneManager {

	static string currentSceneName = string.Empty;
	static List<string> sceneNames;


	/// <summary>
	/// シーンの追加
	/// </summary>
	/// <param name="_sceneName"></param>
	static public void LoadScene(string _sceneName) {
		sceneNames.Add(_sceneName);
		currentSceneName = _sceneName;

		InternalLoadScene(_sceneName);
		// シーンをロードする処理を実装
		Console.WriteLine($"Loading scene: {_sceneName}");
	}

	/// <summary>
	/// シーンのアンロード
	/// </summary>
	/// <param name="_sceneName"></param>
	static public void UnloadScene(string _sceneName) {
		/// シーンがあるかチェック
		if (!sceneNames.Contains(_sceneName)) {
			Console.WriteLine($"Scene {_sceneName} does not exist.");
			return;
		}

		sceneNames.Remove(_sceneName);
		if (currentSceneName == _sceneName) {
			currentSceneName = string.Empty; // 現在のシーンをクリア
		}

		InternalUnloadScene(_sceneName);
		// シーンをアンロードする処理を実装
		Console.WriteLine($"Unloading scene: {_sceneName}");
	}


	/// ===================================
	/// Internal methods
	/// ===================================

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalLoadScene(string sceneName);

	[MethodImpl(MethodImplOptions.InternalCall)]
	static extern void InternalUnloadScene(string sceneName);

}
