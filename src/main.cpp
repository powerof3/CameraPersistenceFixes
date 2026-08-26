#include "Debug.h"
#include "Hooks.h"
#include "Serialization.h"

void OnInit(SKSE::MessagingInterface::Message* a_msg)
{
	switch (a_msg->type) {
	case SKSE::MessagingInterface::kPostLoad:
		{
			Camera::InstallHooks();
			Camera::Serializer::GetSingleton()->LoadCameraData();
			Debug::Install();
		}
		break;
	case SKSE::MessagingInterface::kSaveGame:
		{
			const std::string savePath = { static_cast<char*>(a_msg->data), a_msg->dataLen };
			Camera::Serializer::GetSingleton()->SetCurrentSavePath(savePath);
		}
		break;
	case SKSE::MessagingInterface::kPreLoadGame:
		{
			std::string savePath({ static_cast<char*>(a_msg->data), a_msg->dataLen });
			REX::STR::REPLACE_LAST_INSTANCE(savePath, ".ess", "");

			Camera::Serializer::GetSingleton()->SetCurrentSavePath(savePath);
		}
		break;
	case SKSE::MessagingInterface::kDeleteGame:
		{
			const std::string savePath({ static_cast<char*>(a_msg->data), a_msg->dataLen });
			Camera::Serializer::GetSingleton()->DeleteSavePath(savePath);
		}
		break;
	default:
		break;
	}
}

#ifdef SKYRIM_SUPPORT_AE
SKSE_PLUGIN_VERSION = []() {
	SKSE::PluginVersionData v;
	v.PluginVersion(REL::Version{ Version::MAJOR, Version::MINOR, Version::PATCH });
	v.PluginName("Camera Persistence Fixes");
	v.AuthorName("powerofthree");
	v.UsesAddressLibrary();
	v.UsesUpdatedStructs();
	v.CompatibleVersions({ SKSE::RUNTIME_SSE_LATEST });

	if constexpr (SKSE::RUNTIME_SSE_LATEST < Runtime::MIN_ADDRESS_LIBRARY_V5) {
		v.MinimumRequiredXSEVersion(REL::Version{ 2, 2, 5 });
	} else {
		v.MinimumRequiredXSEVersion(REL::Version{ 2, 3, 0 });
	}

	return v;
}();
#else
SKSE_PLUGIN_QUERY(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
{
	a_info->infoVersion = SKSE::PluginInfo::kVersion;
	a_info->name = "Camera Persistence Fixes";
	a_info->version = Version::MAJOR;

	if (a_skse->IsEditor()) {
		return false;
	}

	const auto ver = a_skse->RuntimeVersion();
	if (ver < SKSE::RUNTIME_SSE_1_5_39) {
		REX::CRITICAL("Unsupported runtime version {}", ver.string());
		return false;
	}

	return true;
}
#endif

SKSE_PLUGIN_LOAD(const SKSE::LoadInterface* a_skse)
{
	SKSE::Init(a_skse, { .log = true,
						   .logName = Version::PROJECT.data()});

	auto runtimeVersion = a_skse->RuntimeVersion();

	REX::INFO("Game version : {}", runtimeVersion);

#ifdef SKYRIM_SUPPORT_AE
	if constexpr (SKSE::RUNTIME_SSE_LATEST < Runtime::MIN_ADDRESS_LIBRARY_V5) {
		if (runtimeVersion >= Runtime::MIN_ADDRESS_LIBRARY_V5) {
			REX::FAIL(
				"You are using a newer version of Skyrim than this version of {0} supports.\n"
				"Install the correct version of {0} for your game version.\n"
				"Runtime: {1}\n"
				"Supported: 1.6.1170 (Steam) / 1.6.1179 (GOG)",
				Version::PROJECT, runtimeVersion);
		}
	}
#endif

	auto messaging = SKSE::GetMessagingInterface();
	messaging->RegisterListener("SKSE", OnInit);

	return true;
}
