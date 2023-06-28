#include "Serialization.h"

namespace Camera
{
	FreeCameraState::FreeCameraState(const RE::FreeCameraState* a_state) :
		translation(a_state->translation),
		rotation(a_state->rotation.x, a_state->rotation.y),
		zUpDown(a_state->zUpDown.x, a_state->zUpDown.y),
		verticalDirection(a_state->verticalDirection),
		useRunSpeed(a_state->useRunSpeed),
		lockToZPlane(a_state->lockToZPlane)
	{}

	void FreeCameraState::LoadState(RE::FreeCameraState* a_state) const
	{
		a_state->translation = translation;

		a_state->rotation.x = rotation.x;
		a_state->rotation.y = rotation.y;

		a_state->zUpDown.x = zUpDown.x;
		a_state->zUpDown.y = zUpDown.y;

		a_state->verticalDirection = verticalDirection;
		a_state->lockToZPlane = lockToZPlane;

		// player can become invisible
		if (const auto process = RE::PlayerCharacter::GetSingleton()->currentProcess) {
			if (const auto high = process->high) {
				high->fadeState = RE::HighProcessData::FADE_STATE::kNormal;
				high->fadeAlpha = 1.0f;
			}
		}
	}

	CameraData::CameraData(const RE::PlayerCamera* a_playerCamera, const RE::FreeCameraState* a_state)
	{
        const auto currentCamera = a_playerCamera->currentState->id;
		if (currentCamera == RE::CameraState::kFree) {
			freeCameraState = a_state;
		}
		if (currentCamera != RE::CameraState::kMount && currentCamera != RE::CameraState::kDragon) {
            if (const auto target = a_playerCamera->cameraTarget.get(); target && !target->IsPlayerRef()) {
				RE::BGSNumericIDIndex numericID{};
				numericID.SetNumericID(target->GetFormID());
				cameraTargetHandle = numericID;
			}
		}
	}

	void CameraData::LoadCameraData(RE::FreeCameraState* a_state) const
	{
		if (freeCameraState) {
			freeCameraState->LoadState(a_state);
		}

		if (cameraTargetHandle) {
            if (const auto actor = RE::TESForm::LookupByID<RE::Actor>(cameraTargetHandle->GetNumericID())) {
				RE::PlayerCamera::GetSingleton()->cameraTarget = actor->CreateRefHandle();
			}
		}
	}

    bool CameraData::DoSerialize() const
	{
		return freeCameraState || cameraTargetHandle;
	}


    void Serializer::SetCurrentSavePath(const std::string& a_save)
	{
		currentSave = a_save;
	}

	void Serializer::DeleteSavePath(const std::string& a_save)
	{
		cameraDataMap.erase(a_save);
	}

	void Serializer::SaveGame(const RE::FreeCameraState* a_this)
	{
		CameraData cameraData(RE::PlayerCamera::GetSingleton(), a_this);
		if (cameraData.DoSerialize() && cameraDataMap.emplace(currentSave, cameraData).second) {
			SerializeCameraData();
		}
	}

	void Serializer::LoadGame(RE::FreeCameraState* a_this)
	{
		if (const auto it = cameraDataMap.find(currentSave); it != cameraDataMap.end()) {
			it->second.LoadCameraData(a_this);
		}
	}

	std::optional<std::filesystem::path> Serializer::save_directory()
	{
		wchar_t*                                               buffer{ nullptr };
		const auto                                             result = ::SHGetKnownFolderPath(::FOLDERID_Documents, ::KNOWN_FOLDER_FLAG::KF_FLAG_DEFAULT, nullptr, std::addressof(buffer));
		std::unique_ptr<wchar_t[], decltype(&::CoTaskMemFree)> knownPath(buffer, ::CoTaskMemFree);
		if (!knownPath || result != S_OK) {
			logger::error("failed to get known folder path"sv);
			return std::nullopt;
		}

		std::filesystem::path path = knownPath.get();
		path /= "My Games"sv;
		path /= *REL::Relocation<const char**>(RELOCATION_ID(508778, 380738)).get();

		if (const auto sLocalSavePath = RE::GetINISetting("sLocalSavePath:General"))
			path /= sLocalSavePath->GetString();
		else {
			logger::error("failed to get local save path ini setting"sv);
			return std::nullopt;
		}

		path /= "CameraSaveData.json"sv;

		return path;
	}

	void Serializer::SerializeCameraData()
	{
		if (jsonPath.empty()) {
			if (const auto saveDirectory = save_directory()) {
				jsonPath = saveDirectory->string();
			}
		}

        [[maybe_unused]] auto ec = glz::write_file(cameraDataMap, jsonPath, std::string());
	}

	void Serializer::LoadCameraData()
	{
		if (jsonPath.empty()) {
			if (const auto saveDirectory = save_directory()) {
				jsonPath = saveDirectory->string();
			}
		}

		glz::read_file(cameraDataMap, jsonPath, std::string());
	}
}
