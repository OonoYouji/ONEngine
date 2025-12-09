#pragma once

#include <Windows.h>
#include <string>
#include <thread>
#include <atomic>
#include <deque>
#include <mutex>
#include <vector>
#include <memory>


/// @brief ファイルのイベント情報
struct FileEvent {
	enum class Action {
		Added,
		Removed,
		Modified,
		RenamedOld,
		RenamedNew
	};

	enum class Type {
		File,
		Directory
	};

	Action action;
	Type type;
	std::wstring path;
	std::wstring watchedDir;
};


/// @brief 監視ディレクトリ情報
struct WatchTarget {
	std::wstring dirPath;
	HANDLE hDir = INVALID_HANDLE_VALUE;
	HANDLE hEvent = nullptr;
	std::thread thread;
};

/// ///////////////////////////////////////////////////
/// ファイルの監視システム
/// ///////////////////////////////////////////////////
class FileWatcher {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	FileWatcher();
	~FileWatcher();

	bool Start(const std::vector<std::wstring>& _dirs);
	void Stop();

	std::vector<FileEvent> ConsumeEvents();

private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	void WatchDirectory(std::shared_ptr<WatchTarget> _ctx);

	
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::atomic<bool> isRunning_{ false };
	std::mutex mutex_;
	std::deque<FileEvent> fileEvents_;

	std::vector<std::shared_ptr<WatchTarget>> watchers_;
};
