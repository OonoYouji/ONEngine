#include "Submarine.h"

#include <ModelManager.h>
#include <Input.h>


/// <summary>
/// 初期化
/// </summary>
void Submarine::Initialize() {
	model_ = ModelManager::CreateCube();



}


/// <summary>
/// 更新処理
/// </summary>
void Submarine::Update() {

	/// オブジェクト上でマウスをクリックし始めたか
	if(Input::TriggerMouse(MouseCode::Left)) {

	}

}


/// <summary>
/// 描画
/// </summary>
void Submarine::Draw() {
	model_->Draw(&transform_);
}
