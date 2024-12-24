#pragma once

/// ===================================================
/// gameのmain loop interface class
/// ===================================================
class IGameLoop {
public:
	
	/// ===================================================
	/// public : methods
	/// ===================================================

	virtual ~IGameLoop() {}
	
	virtual void Initialize() = 0;
	virtual void Update()     = 0;
	virtual void Draw()       = 0;
	virtual void Finalize()   = 0;

	virtual bool GetIsEnd()   = 0;
};

