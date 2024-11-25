#include "SceneFactory.h"

#include "../Scene_Game.h"
#include "../Scene_Title.h"
#include "../Scene_Result.h"
#include "../Scene_Clear.h"


SceneFactory::SceneFactory(const std::string& _startupScene) : AbstructSceneFactory(_startupScene){

	/// シーンの生成関数を登録
	RegisterSceneCreator("TitleScene", []() { return new Scene_Title(); });
	RegisterSceneCreator("GameScene", []() { return new Scene_Game(); });
	RegisterSceneCreator("ResultScene", []() { return new Scene_Result(); });
	RegisterSceneCreator("ClearScene", []() { return new Scene_Clear(); });

}

SceneFactory::~SceneFactory() {}


