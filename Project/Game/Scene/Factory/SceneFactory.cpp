#include "SceneFactory.h"

#include "../Scenes/GameScene.h"


SceneFactory::SceneFactory() {}
SceneFactory::~SceneFactory() {}

void SceneFactory::Initialize() {

	/// 初期化時のシーン名を設定する
	SetStartupSceneName("Game");

	/// シーンの登録
	RegisterScene<GameScene>("Game");
}