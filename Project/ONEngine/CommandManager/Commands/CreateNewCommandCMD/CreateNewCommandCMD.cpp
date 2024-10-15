#include "CreateNewCommandCMD.h"

#include <iostream>
#include <fstream>
#include <filesystem>

void CreateNewCommandCMD::Execution(const std::string& args) {
	std::string folder = directoryPath_ + args;

	/// フォルダが存在しない場合は作成
	std::filesystem::create_directories(folder);

	/// ヘッダファイル (.h) の作成
	std::string headerFilePath = folder + "/" + args + ".h";
	if(!std::filesystem::exists(headerFilePath)) { // ファイルが存在しないか確認
		std::ofstream headerFile(headerFilePath);
		if(headerFile.is_open()) {
			headerFile << "#pragma once\n\n";
			headerFile << "#include \"CommandLineInterface/Commands/BaseCommand.h\"\n\n";
			headerFile << "/// ===================================================\n";
			headerFile << "/// \n";
			headerFile << "/// ===================================================\n";
			headerFile << "class " << args << " : BaseCommand {\n";
			headerFile << "public:\n\n";
			headerFile << "    /// ===================================================\n";
			headerFile << "    /// public : methods\n";
			headerFile << "    /// ===================================================\n\n";
			headerFile << "    " << args << "();\n";
			headerFile << "    ~" << args << "();\n\n";
			headerFile << "    void Execution(const std::string& args) override;\n";
			headerFile << "private:\n\n";
			headerFile << "    /// ===================================================\n";
			headerFile << "    /// private : objects\n";
			headerFile << "    /// ===================================================\n\n";
			headerFile << "};\n";
			headerFile.close();
			std::cout << "	Created: " << headerFilePath << std::endl;
		} else {
			std::cerr << "	Error creating file: " << headerFilePath << std::endl;
		}
	} else {
		std::cout << "	File already exists: " << headerFilePath << std::endl;
	}

	/// ソースファイル (.cpp) の作成
	std::string cppFilePath = folder + "/" + args + ".cpp";
	if(!std::filesystem::exists(cppFilePath)) { // ファイルが存在しないか確認
		std::ofstream cppFile(cppFilePath);
		if(cppFile.is_open()) {
			cppFile << "#include \"" << args << ".h\"\n\n";
			cppFile << "#include <iostream>\n\n";
			cppFile << args << "::" << args << "() {}\n";
			cppFile << args << "::~" << args << "() {}\n\n";
			cppFile << "void " << args << "::Execution(const std::string& args) {\n";
			cppFile << "    std::cout << \"No definition\" << std::endl\n";
			cppFile << "}\n\n";
			cppFile.close();
			std::cout << "	Created: " << cppFilePath << std::endl;
		} else {
			std::cerr << "	Error creating file: " << cppFilePath << std::endl;
		}
	} else {
		std::cout << "	File already exists: " << cppFilePath << std::endl;
	}

}
