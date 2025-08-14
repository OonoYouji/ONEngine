#include "SceneFactory.h"

//#include "../Scenes/GameScene.h"
//#include "../Scenes/LoadLevelTestScene.h"

SceneFactory::SceneFactory() {}
SceneFactory::~SceneFactory() {}

void SceneFactory::Initialize() {

	/// 初期化時のシーン名を設定する
	SetStartupSceneName("TitleScene");

	/// シーンの登録
	//RegisterScene<GameScene>("GameScene");
	//RegisterScene<LoadLevelTestScene>("LoadLevelTest");
}