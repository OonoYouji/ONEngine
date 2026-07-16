#pragma once

namespace ONEngine {

/// ///////////////////////////////////////////////////
/// CPU上のタイムスタンプID
/// ///////////////////////////////////////////////////
enum class CPUTimeStampID {
	CSharpScriptUpdate,
	ECSUpdate,
	RenderUpdate,
	PhysicsUpdate,
	Count, /// 要素数カウント用
};

} /// namespace ONEngine