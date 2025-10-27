#include "FileWatcher.h"

/// engine
#include "Engine/Core/Utility/Utility.h"

FileWatcher::FileWatcher() : isRunning_(false) {}
FileWatcher::~FileWatcher() {
	Stop();
}

bool FileWatcher::Start(const std::vector<std::wstring>& _dirs) {
	if (isRunning_) {
		return false;
	}

	isRunning_ = true;

	for (const auto& dir : _dirs) {
		if (!std::filesystem::exists(dir)) {
			// ログ: dir が存在しないことを表示
			Console::LogError(ConvertString(L"Directory does not exist: " + dir));
			continue;
		}

		auto watchTarget = std::make_unique<WatchTarget>();
		watchTarget->dirPath = dir;
		watchTarget->hDir = CreateFileW(
			dir.c_str(),
			FILE_LIST_DIRECTORY,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			nullptr,
			OPEN_EXISTING,
			FILE_FLAG_BACKUP_SEMANTICS,
			nullptr
		);

		if (watchTarget->hDir == INVALID_HANDLE_VALUE) {
			// ログ: dir のハンドル取得に失敗したことを表示
			DWORD err = GetLastError();
			Console::LogError(ConvertString(L"Failed to get handle for directory: " + dir + L", Error: " + std::to_wstring(err)));
			continue;
		}

		watchTarget->thread = std::thread(&FileWatcher::WatchDirectory, this, dir);
		watchers_.push_back(std::move(watchTarget));
	}


	return !watchers_.empty();
}

void FileWatcher::Stop() {
	/// ---- ファイル監視の停止 ---- ///

	/// CloseHandleで別スレッドと競合しないようにロック
	std::lock_guard<std::mutex> lock(mutex_);

	if (!isRunning_) {
		return;
	}

	isRunning_ = false;


	for (auto& ctx : watchers_) {
		if (ctx->hDir != INVALID_HANDLE_VALUE) {
			CloseHandle(ctx->hDir);
		}

		if (ctx->thread.joinable()) {
			ctx->thread.join();
		}
	}

}

std::vector<FileEvent> FileWatcher::ConsumeEvents() {
	/// ----- 監視イベントの取得とクリア ----- ///

	std::lock_guard<std::mutex> lock(mutex_);
	std::vector<FileEvent> out(fileEvents_.begin(), fileEvents_.end());
	fileEvents_.clear();
	return out;
}

//
//void FileWatcher::Run() {
//	/// ----- ファイル監視のメインループ ----- ///
//
//	hDir_ = CreateFileW(
//		dirPath_.c_str(),
//		FILE_LIST_DIRECTORY,
//		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
//		nullptr,
//		OPEN_EXISTING,
//		FILE_FLAG_BACKUP_SEMANTICS,
//		nullptr
//	);
//
//
//	/// ハンドルの取得に失敗した場合は終了
//	if (hDir_ == INVALID_HANDLE_VALUE) {
//		isRunning_ = false;
//		return;
//	}
//
//
//	const DWORD bufferSize = 64 * 1024;
//	std::vector<char> buffer(bufferSize);
//
//	while (isRunning_) {
//		DWORD bytesReturned = 0;
//
//		/// ブロッキング呼び出し
//		BOOL ok = ReadDirectoryChangesW(
//			hDir_,
//			buffer.data(), bufferSize,
//			TRUE,
//			FILE_NOTIFY_CHANGE_FILE_NAME |
//			FILE_NOTIFY_CHANGE_DIR_NAME |
//			FILE_NOTIFY_CHANGE_ATTRIBUTES |
//			FILE_NOTIFY_CHANGE_SIZE |
//			FILE_NOTIFY_CHANGE_LAST_WRITE |
//			FILE_NOTIFY_CHANGE_CREATION,
//			&bytesReturned,
//			nullptr, nullptr
//		);
//
//		if (!ok) {
//			/// ----- エラー発生時の処理 ----- ///
//			DWORD error = GetLastError();
//			Console::LogError(ConvertString(error));
//
//			if (!isRunning_) {
//				break;
//			}
//
//			std::this_thread::sleep_for(std::chrono::milliseconds(200));
//			continue;
//		}
//
//
//		if (bytesReturned == 0) {
//			continue;
//		}
//
//
//		char* p = buffer.data();
//		while (true) {
//			FILE_NOTIFY_INFORMATION* fni = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(p);
//			std::wstring name(fni->FileName, fni->FileName + (fni->FileNameLength / sizeof(WCHAR)));
//
//			FileEvent event;
//			switch (fni->Action) {
//			case FILE_ACTION_ADDED:            event.type = FileEvent::Type::Added;      break;
//			case FILE_ACTION_REMOVED:          event.type = FileEvent::Type::Removed;    break;
//			case FILE_ACTION_MODIFIED:         event.type = FileEvent::Type::Modified;   break;
//			case FILE_ACTION_RENAMED_OLD_NAME: event.type = FileEvent::Type::RenamedOld; break;
//			case FILE_ACTION_RENAMED_NEW_NAME: event.type = FileEvent::Type::RenamedNew; break;
//			default: event.type = FileEvent::Type::Modified; break;
//			}
//
//
//			std::wstring full = dirPath_;
//			if (!full.empty() && full.back() != L'\\') {
//				full.push_back(L'\\');
//			}
//
//			full += name;
//			event.path = full;
//
//			{
//				std::lock_guard<std::mutex> lock(mutex_);
//				fileEvents_.push_back(event);
//			}
//
//			if (fni->NextEntryOffset == 0) {
//				break;
//			}
//
//			p += fni->NextEntryOffset;
//
//		}
//
//	}	/// while (isRunning_)
//
//
//	if (hDir_ && hDir_ != INVALID_HANDLE_VALUE) {
//		CloseHandle(hDir_);
//		hDir_ = INVALID_HANDLE_VALUE;
//	}
//
//}


void FileWatcher::WatchDirectory(const std::wstring& _dir) {
	HANDLE hDir = CreateFileW(
		_dir.c_str(),
		FILE_LIST_DIRECTORY,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS,
		NULL
	);

	if (hDir == INVALID_HANDLE_VALUE)
		return;

	BYTE buffer[4096];
	DWORD bytesReturned;

	while (isRunning_) {
		if (ReadDirectoryChangesW(
			hDir,
			buffer,
			sizeof(buffer),
			TRUE,
			FILE_NOTIFY_CHANGE_FILE_NAME |
			FILE_NOTIFY_CHANGE_DIR_NAME |
			FILE_NOTIFY_CHANGE_LAST_WRITE,
			&bytesReturned,
			NULL,
			NULL
		)) {
			FILE_NOTIFY_INFORMATION* fni = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(buffer);
			do {
				FileEvent ev{};
				switch (fni->Action) {
				case FILE_ACTION_ADDED: ev.type = FileEvent::Type::Added; break;
				case FILE_ACTION_REMOVED: ev.type = FileEvent::Type::Removed; break;
				case FILE_ACTION_MODIFIED: ev.type = FileEvent::Type::Modified; break;
				case FILE_ACTION_RENAMED_OLD_NAME: ev.type = FileEvent::Type::RenamedOld; break;
				case FILE_ACTION_RENAMED_NEW_NAME: ev.type = FileEvent::Type::RenamedNew; break;
				}

				ev.path = _dir + L"\\" + std::wstring(fni->FileName, fni->FileNameLength / sizeof(WCHAR));
				ev.watchedDir = _dir;

				{
					std::lock_guard<std::mutex> lock(mutex_);
					fileEvents_.push_back(std::move(ev));
				}

				if (fni->NextEntryOffset == 0) break;
				fni = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(
					reinterpret_cast<BYTE*>(fni) + fni->NextEntryOffset);
			} while (true);
		}
	}

	CloseHandle(hDir);
}