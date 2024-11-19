#include "MyFileSystem.h"

namespace fs = std::filesystem;

std::list<std::pair<std::string,std::string>> MyFileSystem::SearchFile(const std::string &directory,const std::string &extension) {
	std::list<std::pair<std::string,std::string>> fileList;
	for(const auto &entry : fs::recursive_directory_iterator(directory)) {
		if(entry.is_regular_file() && entry.path().extension() == ('.' + extension)) {
			fileList.push_back({entry.path().parent_path().string(),entry.path().filename().stem().string()});
		}
	}
	return fileList;
}

std::list<std::string> MyFileSystem::SearchSubFolder(const std::string &directory) {
	std::list<std::string> subFolders;
	for(const auto &entry : fs::recursive_directory_iterator(directory)) {
		if(fs::is_regular_file(entry)) {
			subFolders.push_back(entry.path().string());
		}
	}
	return subFolders;
}

bool MyFileSystem::CreateFolder(const std::string &directory) {
	return std::filesystem::create_directories(directory);
}

bool MyFileSystem::removeEmptyFolder(const std::string &directory) {
	return fs::remove(directory);
}

std::uintmax_t MyFileSystem::deleteFolder(const std::string &path) {
	return fs::remove_all(path);
}