#include "JsonUtil.h"
#include <fstream>
#include <filesystem>
#include <iostream>
#include <cassert>

namespace fs = std::filesystem;

const json& JsonUtil::GetJsonData(const std::string& filePath) {
	std::ifstream file(filePath);
	if (!file.is_open()) {
		assert(0 && "Jsonファイルが開けませんでした");
		return json{};
	}

	json jsonData;
	file >> jsonData;
	file.close();
	return jsonData;
}

bool JsonUtil::EditJson(const std::string& filePath, const json& jsonData) {
	std::ofstream file(filePath);
	if (!file.is_open()) {
		// ファイルが開けなかったらfalseを返す
		return false;
	}

	file << jsonData.dump(4);  // 4スペースでシリアライズ(整列)
	file.close();
	// 成功したらtrueを返す
	return true;
}

bool JsonUtil::CreateJson(const std::string& name, const std::string& targetDir, const json& jsonData) {
	//すでにファイルが存在しているか確認
	std::string fullPath = targetDir + "/" + name;
	if (fs::exists(fullPath)) {
		//すでにファイルが存在していたらfalseを返す
		return false;
	}


}
