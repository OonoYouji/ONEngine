#pragma once
#include <wrl/client.h>
/// ===================================================
/// ComPtrをusingして簡略化
/// ===================================================
template<typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;