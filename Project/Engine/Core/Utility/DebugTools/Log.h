#pragma once

/// std
#include <format>

/// <summary>
/// 引数の文字列をログに出力する
/// </summary>
/// <param name="_message"> :　出力する文字列 </param>
void Log(const std::string& _message);

/// <summary>
/// 引数の文字列をログに出力する
/// </summary>
/// <param name="_message"> :　出力する文字列 </param>
void Log(const std::wstring& _message);



/*##########################################################
	TODO : COMMENT
	この関数を移動する先の.hを考える
##########################################################*/

std::string ConvertString(const std::wstring& _wstr);
std::wstring ConvertString(const std::string& _str);