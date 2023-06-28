#pragma once

#define NOMINMAX

#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"

#include <shlobj.h>

#include <ClibUtil/singleton.hpp>
#include <ClibUtil/string.hpp>
#include <ankerl/unordered_dense.h>
#include <glaze/glaze.hpp>
#include <spdlog/sinks/basic_file_sink.h>
#include <xbyak/xbyak.h>

#define DLLEXPORT __declspec(dllexport)

namespace logger = SKSE::log;
namespace string = clib_util::string;

using namespace clib_util::singleton;
using namespace std::literals;

namespace stl
{
	using namespace SKSE::stl;

	template <class F, class T>
	void write_vfunc()
	{
		REL::Relocation<std::uintptr_t> vtbl{ F::VTABLE[0] };
		T::func = vtbl.write_vfunc(T::size, T::thunk);
	}

	template <class T>
	void write_thunk_call(std::uintptr_t a_src)
	{
		auto& trampoline = SKSE::GetTrampoline();
		SKSE::AllocTrampoline(14);

		T::func = trampoline.write_call<5>(a_src, T::thunk);
	}

}

#ifdef SKYRIM_AE
#	define OFFSET(se, ae) ae
#else
#	define OFFSET(se, ae) se
#endif

#include "Version.h"
