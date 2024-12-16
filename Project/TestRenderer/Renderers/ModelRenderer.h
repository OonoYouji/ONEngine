#pragma once


/// base class
#include "../BaseRenderer/BaseRenderer.h"


/// ===================================================
/// modelの表示に使うRenderer
/// ===================================================
class ModelRenderer : public BaseRenderer {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================
	
	ModelRenderer(ID3D12GraphicsCommandList* _commandList);
	~ModelRenderer();
	
	void Initialize() override;
	void PreDraw()    override;
	void PostDraw()   override;


private:

	/// ===================================================
	/// private : objects
	/// ===================================================

};