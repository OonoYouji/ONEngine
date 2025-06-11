#include "MonoScriptEngine.h"

#include "Engine/Core/Utility/Utility.h"

MonoScriptEngine::MonoScriptEngine() {}
MonoScriptEngine::~MonoScriptEngine() {
	if (domain) {
		mono_jit_cleanup(domain);
		domain = nullptr;
	}
}

void MonoScriptEngine::Initialize() {
	mono_set_dirs("./Externals/mono/lib", "./Externals/mono/etc");
	domain = mono_jit_init("MyDomain");
	if (!domain) {
		Console::Log("Failed to initialize Mono JIT");
		return;
	}

	assembly = mono_domain_assembly_open(domain, "./Assets/Scripts/CSharpLibrary.dll");
	if (!assembly) {
		Console::Log("Failed to load CSharpLibrary.dll");
		return;
	}

	image = mono_assembly_get_image(assembly);
	if (!image) {
		Console::Log("Failed to get image from assembly");
		return;
	}
}
