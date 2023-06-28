#include "Hooks.h"
#include "Serialization.h"

namespace Camera
{
	struct SaveGame
	{
		static void thunk(RE::FreeCameraState* a_this, RE::BGSSaveFormBuffer* a_buf)
		{
            Camera::Serializer::GetSingleton()->SaveGame(a_this);
		    func(a_this, a_buf);
		}
		static inline REL::Relocation<decltype(thunk)> func;
		static inline std::size_t size = 0x06;
	};

	struct LoadGame
	{
		static void thunk(RE::FreeCameraState* a_this, RE::BGSLoadFormBuffer* a_buf)
		{
            Camera::Serializer::GetSingleton()->LoadGame(a_this);
			func(a_this, a_buf);
		}
		static inline REL::Relocation<decltype(thunk)> func;
		static inline std::size_t size = 0x07;
	};

	struct Revert
	{
		static void thunk(RE::FreeCameraState* a_this, RE::BGSLoadFormBuffer* a_buf)
		{
			a_this->translation = RE::NiPoint3();
			a_this->rotation = RE::BSTPoint2<float>();
			a_this->zUpDown = RE::BSTPoint2<float>();
			a_this->useRunSpeed = false;
			a_this->lockToZPlane = false;

			func(a_this, a_buf);
		}
		static inline REL::Relocation<decltype(thunk)> func;
		static inline std::size_t size = 0x08;
	};

	void InstallHooks()
	{
		stl::write_vfunc<RE::FreeCameraState, SaveGame>();
		stl::write_vfunc<RE::FreeCameraState, LoadGame>();
		stl::write_vfunc<RE::FreeCameraState, Revert>();
	}
}
