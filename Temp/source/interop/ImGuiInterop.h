#pragma once
#include <cstdint>

#ifdef _WIN32
#define EXPORT extern "C" __declspec(dllexport)
#else
#define EXPORT extern "C"
#endif

// 簡易的な ImGui ブリッジ (アーキテクチャ実証用)
EXPORT bool igBegin(const char* name, bool* p_open);
EXPORT void igEnd();
EXPORT void igText(const char* fmt);
EXPORT bool igButton(const char* label);
EXPORT void igCheckbox(const char* label, bool* v);
EXPORT void igSliderFloat(const char* label, float* v, float v_min, float v_max);
