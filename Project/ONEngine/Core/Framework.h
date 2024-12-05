#pragma once



/// ===================================================
/// フレームワークの基礎
/// ===================================================
class Framework {
public:
	virtual ~Framework() {}

	virtual void Initialize() = 0;
	virtual void Update()     = 0;
	virtual void Draw()       = 0;
	virtual void Finalize()   = 0;

	void Run();
};