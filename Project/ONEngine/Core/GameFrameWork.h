#pragma once


class GameFrameWork final {
public:
	GameFrameWork() {}
	~GameFrameWork() {}

	void Initialize();
	void Update();
	void Draw();
	void Finalize();

	void Run();

private:


	class SceneManager* pSceneManager_ = nullptr;
};