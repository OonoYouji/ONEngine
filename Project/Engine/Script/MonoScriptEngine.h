#pragma once

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/debug-helpers.h"

class MonoScriptEngine {
public:

	MonoScriptEngine();
	~MonoScriptEngine();

	void Initialize();

private:
	MonoDomain* domain;
	MonoImage* image;
	MonoAssembly* assembly = nullptr;
};

