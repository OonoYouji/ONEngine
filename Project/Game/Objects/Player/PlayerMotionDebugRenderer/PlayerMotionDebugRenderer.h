#pragma once

#include "../Motion/BaseMotion.h"

/// base class
#include "GameObjectManager/BaseGameObject.h"
#include <vector>

class PlayerMotionDebugRenderer : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	PlayerMotionDebugRenderer(class Player* _player);
	~PlayerMotionDebugRenderer();

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

private:

	/// <summary>
	/// 
	/// </summary>
	void SelectEditMotion();

	/// <summary>
	/// モーションの編集
	/// </summary>
	/// <param name="_motion"></param>
	void MotionEdit(const std::string& _key, BaseMotion* _motion);

	/// <summary>
	/// キーフレームの編集
	/// </summary>
	/// <param name="_keyframes"></param>
	void KeyframeEdit(std::vector<MotionKeyframe>& _keyframes);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="_fileName"></param>
	/// <param name="_motion"></param>
	void SaveMotionToJson(const std::string& _fileName, BaseMotion* _motion);


private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class SplinePathRenderer* splinePathRenderer_ = nullptr;
	class MeshInstancingRenderer* meshInstancingRenderer_ = nullptr;


	class Player* pPlayer_ = nullptr;
	class PlayerBehaviorManager* pBehaviorManage_ = nullptr;

};
