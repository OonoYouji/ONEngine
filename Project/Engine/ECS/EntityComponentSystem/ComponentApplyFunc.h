#pragma once

/// std
#include <string>

/// externals
#include <mono/jit/jit.h>

/*
* C#側のComponentアタッチ関数が呼び出されたときにC++側で処理を行うComponentごとの関数群
*/

namespace ONEngine {

class ECSGroup;

using ComponentApplyFunc = void(*)(void* _data, ECSGroup* _ecsGroup);
using ComponentFetchFunc = void(*)(void* _data, ECSGroup* _ecsGroup);
//using ComponentApplyFunc = void(*)(MonoObject* _element, MonoClass* _class, ECSGroup* _ecsGroup);

namespace ComponentApplyFuncs {

/// Apply

void ApplyTransform(void* _element, ECSGroup* _ecsGroup);
void ApplyMeshRenderer(void* _element, ECSGroup* _ecsGroup);


/// Fetch

void FetchTransform(void* _element, ECSGroup* _ecsGroup);
void FetchMeshRenderer(void* _element, ECSGroup* _ecsGroup);



ComponentApplyFunc GetApplyFunc(MonoClass* _monoClass);
ComponentFetchFunc GetFetchFunc(MonoClass* _monoClass);

size_t GetBatchElementSize(MonoClass* _monoClass);

void Initialize(MonoImage* _monoImage);
}


} /// namespace ONEngine