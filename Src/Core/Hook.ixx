// Hazno - 2026

module;

#include <type_traits>
#include <Python.h>
#include <ceval.h>

#include "Hook.h"

export module Core:Hook;
import :Types;
import :Log;

export {
    template<typename Fn>
	struct Hook;

	template<typename Ret, typename... Args>
	Hook(Ret(*)(Args...)) -> Hook<Ret(*)(Args...)>;

	template<typename Ret, typename... Args>
	struct Hook<Ret(*)(Args...)>
	{
		private:
			using FnType = Ret(*)(Args...);

	        bool m_detoured = false;
			FnType m_target = nullptr;
			FnType m_trampoline = nullptr;
            uint8 m_saved_bytes[LIBDETOUR_JMP_SZ_] = {};

		public:
			constexpr Hook() = default;
			constexpr Hook(const FnType trampoline, const FnType target) : m_target(target), m_trampoline(trampoline)
			{
			    if (m_target) {
			        memcpy(m_saved_bytes, reinterpret_cast<void*>(m_target), sizeof(m_saved_bytes));
			    }
			}

			explicit constexpr Hook(const FnType trampoline, const FnType target, const bool enable) : m_target(target), m_trampoline(trampoline)
			{
			    if (m_target) {
			        memcpy(m_saved_bytes, reinterpret_cast<void*>(m_target), sizeof(m_saved_bytes));
			    }

				if (enable) {
					Enable();
				}
			}

			explicit constexpr Hook(const FnType trampoline) : m_trampoline(trampoline) {}

			Hook& WithTrampoline(FnType trampoline)
			{
				m_trampoline = trampoline;
				return *this;
			}

			Hook& WithTarget(FnType target)
			{
				m_target = target;
			    memcpy(m_saved_bytes, reinterpret_cast<void*>(m_target), sizeof(m_saved_bytes));
				return *this;
			}

			template <Cpt_Ptr Fn>
			Hook& WithTarget(Fn target)
			{
				m_target = reinterpret_cast<FnType>(target);
			    memcpy(m_saved_bytes, reinterpret_cast<void*>(m_target), sizeof(m_saved_bytes));
				return *this;
			}

	        Hook& WithTarget(PyCFunctionObject* pyFunc)
            {
                m_target = reinterpret_cast<FnType>(PyCFunction_GET_FUNCTION(pyFunc));
			    memcpy(m_saved_bytes, reinterpret_cast<void*>(m_target), sizeof(m_saved_bytes));
                return *this;
            }

			bool TryEnable(const char* name = "Unnamed Node")
			{
				if (!m_trampoline) {
					Log("Failed to enable {}, trampoline isn't set", name);
					return false;
				}

				if (!m_target) {
					Log("Failed to enable {}, target isn't set", name);
					return false;
				}

				if (!detour_enable(reinterpret_cast<detour_ctx_t*>(this))) {
				    Log("Failed to enable {}, libDetour returned false", name);
				    return false;
				}

			    //Log("Enabled {} on target {}", name, reinterpret_cast<void*>(m_target));
			    return true;
			}

			Hook& Enable(const char* name = "Unnamed Node")
			{
				TryEnable(name);
				return *this;
			}

			void Disable(const char* name = "Unnamed Node")
			{
				if (!m_target) {
					Log("Cannot disable {} as target is null.", name);
					return;
				}

			    if (!detour_disable(reinterpret_cast<detour_ctx_t*>(this))) {
			        Log("Failed to enable {}, libDetour returned false", name);
			    }
			}

			Ret Call(Args... args)
			{
				if (!m_target) {
					Log("Hook Node called without trampoline or target set");
					return Ret();
				}

			    const auto shouldToggle = m_detoured;
			    if (shouldToggle) {
			        Disable();
			    }

			    if constexpr (std::is_same_v<Ret, void>) {
                    m_target(args...);
			        if (shouldToggle) {
			            Enable();
			        }
                    return void();
                }

			    auto res = m_target(args...);
			    if (shouldToggle) {
			        Enable();
			    }
			    return res;
			}

	        [[nodiscard]] void* GetTarget() const
			{
			    return reinterpret_cast<void*>(m_target);
			}

	        [[nodiscard]] void* GetTrampoline() const
			{
			    return reinterpret_cast<void*>(m_trampoline);
			}
	};
}