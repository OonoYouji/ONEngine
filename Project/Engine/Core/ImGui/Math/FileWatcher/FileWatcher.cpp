#include "FileWatcher.h"

FileWatcher::FileWatcher() : isRunning_(false), hDir_(INVALID_HANDLE_VALUE) {}
FileWatcher::~FileWatcher() {
	Stop();
}

bool FileWatcher::Start(const std::wstring& _dir) {
	/// ---- ファイル監視の開始 ---- ///

	if (isRunning_) {
		return false;
	}

	dirPath_ = _dir;
	isRunning_ = true;
	th_ = std::thread([this] { Run(); });

	return true;
}

void FileWatcher::Stop() {
	/// ---- ファイル監視の停止 ---- ///

	/// CloseHandleで別スレッドと競合しないようにロック
	std::lock_guard<std::mutex> lock(mutex_);

	if (!isRunning_) {
		return;
	}

	isRunning_ = false;

	if (hDir_ && hDir_ != INVALID_HANDLE_VALUE) {
		CancelIoEx(hDir_, nullptr);
		CloseHandle(hDir_);
		hDir_ = INVALID_HANDLE_VALUE;
	}

	if (th_.joinable()) {
		th_.join();
	}
}

std::vector<FileEvent> FileWatcher::ConsumeEvents() {
	/// ----- 監視イベントの取得とクリア ----- ///

	std::lock_guard<std::mutex> lock(mutex_);
	std::vector<FileEvent> out(fileEvents_.begin(), fileEvents_.end());
	fileEvents_.clear();
	return out;
}


void FileWatcher::Run() {
	/// ----- ファイル監視のメインループ ----- ///

	hDir_ = CreateFileW(
		dirPath_.c_str(),
		FILE_LIST_DIRECTORY,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		nullptr,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS,
		nullptr
	);


	/// ハンドルの取得に失敗した場合は終了
	if (hDir_ == INVALID_HANDLE_VALUE) {
		isRunning_ = false;
		return;
	}


	const DWORD bufferSize = 64 * 1024;
	std::vector<char> buffer(bufferSize);

	while (isRunning_) {
		DWORD bytesReturned = 0;

		/// ブロッキング呼び出し
		BOOL ok = ReadDirectoryChangesW(
			hDir_,
			buffer.data(), bufferSize,
			TRUE,
			FILE_NOTIFY_CHANGE_FILE_NAME |
			FILE_NOTIFY_CHANGE_DIR_NAME |
			FILE_NOTIFY_CHANGE_ATTRIBUTES |
			FILE_NOTIFY_CHANGE_SIZE |
			FILE_NOTIFY_CHANGE_LAST_WRITE |
			FILE_NOTIFY_CHANGE_CREATION,
			&bytesReturned,
			nullptr, nullptr
		);

		if (!ok) {
			/// ----- エラー発生時の処理 ----- ///
			DWORD error = GetLastError();
			if (!isRunning_) {
				break;
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(200));
			continue;
		}


		if (bytesReturned == 0) {
			continue;
		}


		char* p = buffer.data();
		while (true) {
			FILE_NOTIFY_INFORMATION* fni = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(p);
			std::wstring name(fni->FileName, fni->FileName + (fni->FileNameLength / sizeof(WCHAR)));

			FileEvent event;
			switch (fni->Action) {
			case FILE_ACTION_ADDED:            event.type = FileEvent::Type::Added;      break;
			case FILE_ACTION_REMOVED:          event.type = FileEvent::Type::Removed;    break;
			case FILE_ACTION_MODIFIED:         event.type = FileEvent::Type::Modified;   break;
			case FILE_ACTION_RENAMED_OLD_NAME: event.type = FileEvent::Type::RenamedOld; break;
			case FILE_ACTION_RENAMED_NEW_NAME: event.type = FileEvent::Type::RenamedNew; break;
			default: event.type = FileEvent::Type::Modified; break;
			}


			std::wstring full = dirPath_;
			if (!full.empty() && full.back() != L'\\') {
				full.push_back(L'\\');
			}

			full += name;
			event.path = full;

			{
				std::lock_guard<std::mutex> lock(mutex_);
				fileEvents_.push_back(event);
			}

			if (fni->NextEntryOffset == 0) {
				break;
			}

			p += fni->NextEntryOffset;

		}

	}	/// while (isRunning_)


	if (hDir_ && hDir_ != INVALID_HANDLE_VALUE) {
		CloseHandle(hDir_);
		hDir_ = INVALID_HANDLE_VALUE;
	}

}