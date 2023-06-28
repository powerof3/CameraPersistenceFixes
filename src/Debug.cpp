#include "Debug.h"

namespace Debug
{
	namespace detail
	{
		constexpr auto LONG_NAME = "SetCameraTarget"sv;
		constexpr auto SHORT_NAME = "sct"sv;

		[[nodiscard]] const std::string& HelpString()
		{
			static auto help = []() {
				std::string buf;
				buf += "SetCameraTarget\n";
				return buf;
			}();
			return help;
		}

		bool Execute(const RE::SCRIPT_PARAMETER*, RE::SCRIPT_FUNCTION::ScriptData*, RE::TESObjectREFR* a_obj, RE::TESObjectREFR*, RE::Script*, RE::ScriptLocals*, double&, std::uint32_t&)
		{
			constexpr auto print = [](const char* a_fmt) {
				if (RE::ConsoleLog::IsConsoleMode()) {
					RE::ConsoleLog::GetSingleton()->Print(a_fmt);
				}
			};

			if (a_obj == nullptr || a_obj->IsNot(RE::FormType::ActorCharacter)) {
				RE::PlayerCamera::GetSingleton()->cameraTarget = RE::PlayerCharacter::GetSingleton()->CreateRefHandle();
				return false;
			} else {
				RE::PlayerCamera::GetSingleton()->cameraTarget = a_obj->As<RE::Actor>()->CreateRefHandle();
				return true;
			}
		}
	}

	void Install()
	{
		if (const auto function = RE::SCRIPT_FUNCTION::LocateConsoleCommand("TestDegrade"); function) {
			static RE::SCRIPT_PARAMETER params[] = {
				{ "Actor", RE::SCRIPT_PARAM_TYPE::kActor, true }
			};

			function->functionName = detail::LONG_NAME.data();
			function->shortName = detail::SHORT_NAME.data();
			function->helpString = detail::HelpString().data();
			function->referenceFunction = false;
			function->SetParameters(params);
			function->executeFunction = &detail::Execute;
			function->conditionFunction = nullptr;

			logger::debug("installed {}", detail::LONG_NAME);
		}
	}
}
