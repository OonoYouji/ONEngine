#pragma once

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/debug-helpers.h"

class CSharpScript {
public:

	CSharpScript();
	~CSharpScript();

	void Initialize();

private:
	MonoDomain* domain;
	MonoImage* image;
	MonoAssembly* assembly = nullptr;
};

