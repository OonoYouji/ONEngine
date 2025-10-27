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
	std::wstring watchedDir;
};

/// @brief 監視対象のディレクトリ構造体
struct WatchTarget {
	std::wstring dirPath;
	HANDLE hDir;
	std::thread thread;
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

	bool Start(const std::vector<std::wstring>& _dirs);
	//bool Start(const std::wstring& _dir);
	void Stop();

	std::vector<FileEvent> ConsumeEvents();


private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	//void Run();

	void WatchDirectory(const std::wstring& _dir);


	/// ===================================================
	/// private : objects
	/// ===================================================

	//std::wstring dirPath_;
	std::atomic<bool> isRunning_;
	//std::thread th_;
	//HANDLE hDir_;

	std::mutex mutex_;
	std::deque<FileEvent> fileEvents_;


	std::vector<std::unique_ptr<WatchTarget>> watchers_;


};