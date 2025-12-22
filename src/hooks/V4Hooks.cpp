#include "CustomJSONDataHooks.h"
#include "HookUtils.hpp"

#include "BeatmapDataLoaderVersion4/BeatmapDataLoader.hpp"
#include "BeatmapLevelSaveDataVersion4/AudioSaveData.hpp"
#include "BeatmapSaveDataVersion4/BeatmapSaveData.hpp"
#include "BeatmapSaveDataVersion4/LightshowSaveData.hpp"

#include "GlobalNamespace/BpmTimeProcessor.hpp"
#include "GlobalNamespace/EnvironmentKeywords.hpp"
#include "GlobalNamespace/IEnvironmentInfo.hpp"
#include "GlobalNamespace/IEnvironmentLightGroups.hpp"
#include "GlobalNamespace/EnvironmentLightGroups.hpp"
#include "GlobalNamespace/DefaultEnvironmentEventsFactory.hpp"
#include "GlobalNamespace/BeatmapObjectData.hpp"
#include "GlobalNamespace/BeatmapData.hpp"
#include "GlobalNamespace/BeatmapEventData.hpp"
#include "GlobalNamespace/BeatmapDataBasicInfo.hpp"
#include "GlobalNamespace/EnvironmentEffectsFilterPreset.hpp"
#include "GlobalNamespace/GameplayModifiers.hpp"
#include "GlobalNamespace/PlayerSpecificSettings.hpp"
#include "GlobalNamespace/IBeatmapLightEventConverter.hpp"

#include "UnityEngine/JsonUtility.hpp"
#include "System/String.hpp"

#include "beatsaber-hook/shared/utils/typedefs-list.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"
// for rapidjson error parsing
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/error/en.h"

#include "cpp-semver/shared/cpp-semver.hpp"
#include "paper2_scotland2/shared/Profiler.hpp"
#include "custom-types/shared/register.hpp"

#include "CustomBeatmapData.h"
#include "misc/BeatmapFieldUtils.hpp"
#include "misc/BeatmapDataLoaderUtils.hpp"
#include "CustomJSONDataHooks.h"
#include "CJDLogger.h"
#include "VList.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace GlobalNamespace;
using namespace CustomJSONData;
using namespace BeatmapLevelSaveDataVersion4;
using namespace BeatmapSaveDataVersion4;

// clang-format on
MAKE_PAPER_HOOK_MATCH(BeatmapDataLoader_GetBeatmapDataFromSaveDataJson_v4,
                      &BeatmapDataLoaderVersion4::BeatmapDataLoader::GetBeatmapDataFromSaveDataJson,
                      GlobalNamespace::BeatmapData*, ::StringW audioDataJson, ::StringW beatmapJson,
                      ::StringW lightshowJson, ::StringW defaultLightshowJson,
                      ::GlobalNamespace::BeatmapDifficulty beatmapDifficulty, bool loadingForDesignatedEnvironment,
                      ::GlobalNamespace::IEnvironmentInfo* targetEnvironmentInfo,
                      ::GlobalNamespace::IEnvironmentInfo* originalEnvironmentInfo,
                      ::GlobalNamespace::BeatmapLevelDataVersion beatmapLevelDataVersion,
                      ::GlobalNamespace::GameplayModifiers* gameplayModifiers,
                      ::GlobalNamespace::PlayerSpecificSettings* playerSpecificSettings,
                      ::GlobalNamespace::IBeatmapLightEventConverter* lightEventConverter) {
  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Loading V4 Beatmap Data from JSON");

  // Check for custom JSON data in beatmap
  auto sharedDoc = parseDocument(std::string(beatmapJson));

  if (!sharedDoc) {
    return BeatmapDataLoader_GetBeatmapDataFromSaveDataJson_v4(
        audioDataJson, beatmapJson, lightshowJson, defaultLightshowJson, beatmapDifficulty,
        loadingForDesignatedEnvironment, targetEnvironmentInfo, originalEnvironmentInfo, beatmapLevelDataVersion,
        gameplayModifiers, playerSpecificSettings, lightEventConverter);
  }

  // For now, delegate to original method since we need to implement V4 custom save data parsing
  // TODO: Implement V4 custom save data parsing similar to V2/V3
  return BeatmapDataLoader_GetBeatmapDataFromSaveDataJson_v4(
      audioDataJson, beatmapJson, lightshowJson, defaultLightshowJson, beatmapDifficulty,
      loadingForDesignatedEnvironment, targetEnvironmentInfo, originalEnvironmentInfo, beatmapLevelDataVersion,
      gameplayModifiers, playerSpecificSettings, lightEventConverter);
}

MAKE_PAPER_HOOK_MATCH(BeatmapDataLoader_GetBeatmapDataBasicInfoFromSaveDataJson_v4,
                      &BeatmapDataLoaderVersion4::BeatmapDataLoader::GetBeatmapDataBasicInfoFromSaveDataJson,
                      GlobalNamespace::BeatmapDataBasicInfo*, ::StringW beatmapJson) {
  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Getting V4 Beatmap Basic Info from JSON");

  if (String::IsNullOrEmpty(beatmapJson)) {
    return nullptr;
  }

  auto sharedDoc = parseDocument(std::string(beatmapJson));

  if (!sharedDoc) {
    return BeatmapDataLoader_GetBeatmapDataBasicInfoFromSaveDataJson_v4(beatmapJson);
  }

  // For now, delegate to original method
  // TODO: Implement custom V4 beatmap basic info parsing
  return BeatmapDataLoader_GetBeatmapDataBasicInfoFromSaveDataJson_v4(beatmapJson);
}

/*
// Future implementation for V4 save data processing
// This would be similar to the C# implementation in the comments:

MAKE_PAPER_HOOK_MATCH(BeatmapDataLoader_GetBeatmapDataFromSaveData_v4,
                      &BeatmapDataLoaderVersion4::BeatmapDataLoader::GetBeatmapDataFromSaveData,
                      GlobalNamespace::BeatmapData*,
                      ::BeatmapLevelSaveDataVersion4::AudioSaveData* audioSaveData,
                      ::BeatmapSaveDataVersion4::BeatmapSaveData* beatmapSaveData,
                      ::BeatmapSaveDataVersion4::LightshowSaveData* lightshowSaveData,
                      ::BeatmapSaveDataVersion4::LightshowSaveData* defaultLightshowSaveData,
                      ::GlobalNamespace::BeatmapDifficulty beatmapDifficulty,
                      bool loadingForDesignatedEnvironment,
                      ::GlobalNamespace::EnvironmentKeywords* environmentKeywords,
                      ::GlobalNamespace::IEnvironmentLightGroups* environmentLightGroups,
                      ::GlobalNamespace::GameplayModifiers* gameplayModifiers,
                      ::GlobalNamespace::PlayerSpecificSettings* playerSpecificSettings,
                      ::GlobalNamespace::IBeatmapLightEventConverter* lightEventConverter) {

  CJDLogger::Logger.fmtLog<LogLevel::DBG>("Processing V4 Save Data");

  // Implementation would go here following the C# logic:
  // 1. Create BeatmapData(4)
  // 2. Create BpmTimeProcessor from audioSaveData
  // 3. Check for zen mode
  // 4. Convert beatmap objects if not zen mode
  // 5. Convert BPM events from audioSaveData
  // 6. Handle lightshow data based on flags
  // 7. Process remaining data and sort

  return BeatmapDataLoader_GetBeatmapDataFromSaveData_v4(
      audioSaveData, beatmapSaveData, lightshowSaveData, defaultLightshowSaveData,
      beatmapDifficulty, loadingForDesignatedEnvironment, environmentKeywords,
      environmentLightGroups, gameplayModifiers, playerSpecificSettings, lightEventConverter);
}
*/

void CustomJSONData::v4::InstallHooks() {
  // Install V4 hooks
  INSTALL_HOOK_ORIG(CJDLogger::Logger, BeatmapDataLoader_GetBeatmapDataFromSaveDataJson_v4)
  INSTALL_HOOK_ORIG(CJDLogger::Logger, BeatmapDataLoader_GetBeatmapDataBasicInfoFromSaveDataJson_v4)
  // INSTALL_HOOK_ORIG(CJDLogger::Logger, BeatmapDataLoader_GetBeatmapDataFromSaveData_v4)
}
