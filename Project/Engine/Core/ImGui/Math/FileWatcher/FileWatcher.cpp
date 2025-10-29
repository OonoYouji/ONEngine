#include "FileWatcher.h"

/// std
#include <filesystem>

/// engine
#include "Engine/Core/Utility/Utility.h"

FileWatcher::FileWatcher() = default;
FileWatcher::~FileWatcher() {
	Stop();
}

bool FileWatcher::Start(const std::vector<std::wstring>& _dirs) {
	if (isRunning_) return false;
	isRunning_ = true;

	for (const auto& dir : _dirs) {
		if (!std::filesystem::exists(dir)) continue;

		auto ctx = std::make_shared<WatchTarget>();
		ctx->dirPath = dir;

		ctx->hDir = CreateFileW(
			dir.c_str(),
			FILE_LIST_DIRECTORY,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			nullptr,
			OPEN_EXISTING,
			FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
			nullptr
		);

		if (ctx->hDir == INVALID_HANDLE_VALUE) continue;

		ctx->hEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (!ctx->hEvent) {
			CloseHandle(ctx->hDir);
			continue;
		}

		ctx->thread = std::thread(&FileWatcher::WatchDirectory, this, ctx);

		std::lock_guard<std::mutex> lock(mutex_);
		watchers_.push_back(ctx);
	}

	return !watchers_.empty();
}

void FileWatcher::Stop() {
	std::vector<std::shared_ptr<WatchTarget>> localWatchers;

	{	/// isRunning_ の変更と watchers_ のコピー
		std::lock_guard<std::mutex> lock(mutex_);
		if (!isRunning_) return;
		isRunning_ = false;
		localWatchers = watchers_;
	}

	/// CancelIoEx + SetEvent でスレッドを抜けさせる
	for (auto& ctx : localWatchers) {
		if (ctx && ctx->hDir != INVALID_HANDLE_VALUE) {
			CancelIoEx(ctx->hDir, nullptr);
			if (ctx->hEvent) SetEvent(ctx->hEvent);
		}
	}

	/// join は mutex 外で安全に
	for (auto& ctx : localWatchers) {
		if (ctx && ctx->thread.joinable()) {
			ctx->thread.join();
		}
	}

	{	/// リソースの破棄
		std::lock_guard<std::mutex> lock(mutex_);
		for (auto& ctx : watchers_) {
			if (ctx) {
				if (ctx->hEvent) {
					CloseHandle(ctx->hEvent);
					ctx->hEvent = nullptr;
				}
				if (ctx->hDir != INVALID_HANDLE_VALUE) {
					CloseHandle(ctx->hDir);
					ctx->hDir = INVALID_HANDLE_VALUE;
				}
			}
		}
		watchers_.clear();
	}
}

std::vector<FileEvent> FileWatcher::ConsumeEvents() {
	std::lock_guard<std::mutex> lock(mutex_);
	std::vector<FileEvent> out(fileEvents_.begin(), fileEvents_.end());
	fileEvents_.clear();
	return out;
}

void FileWatcher::WatchDirectory(std::shared_ptr<WatchTarget> _ctx) {
	if (!_ctx || _ctx->hDir == INVALID_HANDLE_VALUE || !_ctx->hEvent) {
		return;
	}

	BYTE buffer[4096];
	OVERLAPPED overlapped = {};
	overlapped.hEvent = _ctx->hEvent;

	while (isRunning_) {
		DWORD bytesReturned = 0;
		BOOL ok = ReadDirectoryChangesW(
			_ctx->hDir,
			buffer,
			sizeof(buffer),
			TRUE,
			FILE_NOTIFY_CHANGE_FILE_NAME |
			FILE_NOTIFY_CHANGE_DIR_NAME |
			FILE_NOTIFY_CHANGE_LAST_WRITE,
			&bytesReturned,
			&overlapped,
			nullptr
		);

		if (!ok) {
			break;
		}

		DWORD wait = WaitForSingleObject(_ctx->hEvent, 100); // タイムアウト付き
		if (!isRunning_) {
			break;
		}

		if (wait != WAIT_OBJECT_0) {
			continue;
		}

		DWORD bytesTransferred = 0;
		if (!GetOverlappedResult(_ctx->hDir, &overlapped, &bytesTransferred, FALSE)) {
			/// ----- error ----- ///

			DWORD gle = GetLastError();
			if (gle == ERROR_OPERATION_ABORTED) {
				Console::LogError(ConvertString(gle));
			}

			break;
		}

		FILE_NOTIFY_INFORMATION* fni = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(buffer);
		while (true) {

			/// Eventごとに処理
			FileEvent ev{};
			switch (fni->Action) {
			case FILE_ACTION_ADDED:            ev.type = FileEvent::Type::Added;      break;
			case FILE_ACTION_REMOVED:          ev.type = FileEvent::Type::Removed;    break;
			case FILE_ACTION_MODIFIED:         ev.type = FileEvent::Type::Modified;   break;
			case FILE_ACTION_RENAMED_OLD_NAME: ev.type = FileEvent::Type::RenamedOld; break;
			case FILE_ACTION_RENAMED_NEW_NAME: ev.type = FileEvent::Type::RenamedNew; break;
			}

			/// Pathの構築
			ev.path = _ctx->dirPath + L"\\" + std::wstring(fni->FileName, fni->FileNameLength / sizeof(WCHAR));
			ev.watchedDir = _ctx->dirPath;

			{
				std::lock_guard<std::mutex> lock(mutex_);
				fileEvents_.push_back(std::move(ev));
			}

			/// 次のエントリがないならループ終了
			if (fni->NextEntryOffset == 0) {
				break;
			}

			/// 次のエントリへ
			fni = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(reinterpret_cast<BYTE*>(fni) + fni->NextEntryOffset);
		}
	}
}
