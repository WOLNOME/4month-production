#pragma once
#include "json.hpp"
#include <string>

using json = nlohmann::json;

class JsonUtil {
public:
	/// <summary>
	/// Jsonデータを出力する
	/// </summary>
	/// <param name="filePath">ファイルパス</param>
	/// <returns>jsonデータ</returns>
	static const json& GetJsonData(const std::string& filePath);
	/// <summary>
	/// Jsonファイルを編集する
	/// </summary>
	/// <param name="filePath">ファイルパス</param>
	/// <param name="jsonData">jsonデータ</param>
	static bool EditJson(const std::string& filePath, const json& jsonData);
	/// <summary>
	/// Jsonファイルを新しく作成する
	/// </summary>
	/// <param name="name">ファイル名</param>
	/// <param name="targetDir">作成するフォルダ名</param>
	/// <param name="jsonData">jsonデータ</param>
	static bool CreateJson(const std::string& name,const std::string& targetDir, const json& jsonData);
};

