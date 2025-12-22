#include <regex>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"

#include "misc/BeatmapDataLoaderUtils.hpp"
#include "misc/BeatmapFieldUtils.hpp"

// for rapidjson error parsing
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/error/en.h"

namespace CustomJSONData {

template <typename T> constexpr void addAllToVector(std::vector<T>& vec, auto const& listPtr) {
  ListW<typename std::remove_reference_t<decltype(vec)>::value_type> vList(listPtr);

  std::copy(vList.begin(), vList.end(), std::back_inserter(vec));
};

template <typename T> constexpr void sortInPlace(std::span<T> vec) {
  std::stable_sort(vec.begin(), vec.end(), TimeCompare<typename std::span<T>::const_reference>);
};

template <typename T> constexpr void cleanAndSort(std::vector<T>& vec) {
  // remove nulls
  for (auto it = vec.begin(); it != vec.end();) {
    auto const& v = *it;
    if (!v) {
      it = vec.erase(it);
      continue;
    }

    it++;
  }

  sortInPlace<T>({vec.begin(), vec.end()});
};

static std::optional<std::shared_ptr<rapidjson::Document>> parseDocument(std::string_view stringData) {
  auto sharedDoc = std::make_shared<rapidjson::Document>();
  rapidjson::Document& doc = *sharedDoc;
  rapidjson::ParseResult result = doc.Parse(stringData.data());

  if (!result || doc.IsNull() || doc.HasParseError()) {
    std::string errorCodeStr(rapidjson::GetParseError_En(result.Code()));
    CJDLogger::Logger.fmtLog<LogLevel::DBG>("Unable to parse json due to {}", errorCodeStr);
    return std::nullopt;
  }

  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Parsing json success");

  return sharedDoc;
}

static std::u16string GetVersionFromPath(std::u16string_view path) {
  // SongCore has a fallback so i guess i do too
  static std::u16string_view const fallback = u"2.0.0";

  auto truncatedText = path.substr(0, 50);
  static std::regex const versionRegex(R"("_?version"\s*:\s*"[0-9]+\.[0-9]+\.?[0-9]?")");
  std::match_results<std::u16string_view::const_iterator> matches;
  if (std::regex_search(truncatedText.begin(), truncatedText.end(), matches, versionRegex)) {
    if (!matches.empty()) {
      auto version = matches[0].str();
      version = version.substr(0, version.length() - 1);
      version = version.substr(version.find_last_of('\"') + 1, version.length());

      return version;
    }
  }

  return std::u16string(fallback);
}
}  // namespace CustomJSONData