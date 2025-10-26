#pragma once

#include <Windows.h>

/// std
#include <string>
#include <thread>
#include <atomic>
#include <deque>
#include <mutex>
#include <vector>


/// @brief ファイルイベントの構造体
struct FileEvent {
	enum class Type {
		Added,
		Removed,
		Modified,
		RenamedOld,
		RenamedNew
	};

	Type type;
	std::wstring path;
};



/// ///////////////////////////////////////////////////
/// ファイルの変更を監視するクラス
/// ///////////////////////////////////////////////////
class FileWatcher {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================
	
	FileWatcher();
	~FileWatcher();

	bool Start(const std::wstring& _dir);
	void Stop();

	std::vector<FileEvent> ConsumeEvents();


private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	void Run();


	/// ===================================================
	/// private : objects
	/// ===================================================
	
	std::wstring dirPath_;
	std::atomic<bool> isRunning_;
	std::thread th_;
	HANDLE hDir_;

	std::mutex mutex_;
	std::deque<FileEvent> fileEvents_;

	
};