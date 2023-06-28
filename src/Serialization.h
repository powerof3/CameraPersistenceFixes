#pragma once

template <>
struct glz::meta<RE::NiPoint3>
{
	using T = RE::NiPoint3;
	static constexpr auto value = array(&T::x, &T::y, &T::z);
};

template <>
struct glz::meta<RE::NiPoint2>
{
	using T = RE::NiPoint2;
	static constexpr auto value = array(&T::x, &T::y);
};

template <>
struct glz::meta<RE::BGSNumericIDIndex>
{
	using T = RE::BGSNumericIDIndex;
	static constexpr auto value = array(&T::data1, &T::data2, &T::data3);
};

namespace Camera
{
	struct FreeCameraState
	{
		FreeCameraState() = default;
		FreeCameraState(const RE::FreeCameraState* a_state);

		void LoadState(RE::FreeCameraState* a_state) const;

		// members
		RE::NiPoint3 translation;        // 30
		RE::NiPoint2 rotation;           // 3C
		RE::NiPoint2 zUpDown;            // 44
		std::int16_t verticalDirection;  // 4C
		bool         useRunSpeed;        // 4E
		bool         lockToZPlane;       // 4F

		struct glaze
		{
			using T = FreeCameraState;
			static constexpr auto value = glz::object(
				"translation", &T::translation,
				"rotation", &T::rotation,
				"zUpDown", &T::zUpDown,
				"verticalDir", &T::verticalDirection,
				"lockToZPlane", &T::lockToZPlane);
		};
	};

	struct CameraData
	{
		CameraData() = default;
		CameraData(const RE::PlayerCamera* a_playerCamera, const RE::FreeCameraState* a_state);

		void LoadCameraData(RE::FreeCameraState* a_state) const;
		bool DoSerialize() const;

		// members
		std::optional<FreeCameraState>       freeCameraState{ std::nullopt };
		std::optional<RE::BGSNumericIDIndex> cameraTargetHandle{ std::nullopt };

		struct glaze
		{
			using T = CameraData;
			static constexpr auto value = glz::object(
				"freeCameraState", &T::freeCameraState,
				"cameraTarget", &T::cameraTargetHandle);
		};
	};

	class Serializer : public ISingleton<Serializer>
	{
	public:
		void SetCurrentSavePath(const std::string& a_save);
		void DeleteSavePath(const std::string& a_save);

		void SaveGame(const RE::FreeCameraState* a_this);
		void LoadGame(RE::FreeCameraState* a_this);

		void LoadCameraData();

	private:
		static std::optional<std::filesystem::path> save_directory();
		void                                        SerializeCameraData();

		// members
		std::string currentSave{};

		ankerl::unordered_dense::map<std::string, CameraData>      cameraDataMap{};
		ankerl::unordered_dense::map<std::uint32_t, std::uint32_t> modIndexMap{};

		std::string jsonPath{};
	};
}
