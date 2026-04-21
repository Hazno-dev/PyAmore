// Hazno - 2026

module;

#include <libdetour.h>

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
			FnType m_trampoline = nullptr;
			FnType m_target = nullptr;
            uint8 saved_bytes[LIBDETOUR_JMP_SZ_] = {};

		public:
			constexpr Hook() = default;
			constexpr Hook(const FnType trampoline, const FnType target) : m_trampoline(trampoline), m_target(target) {}
			explicit constexpr Hook(const FnType trampoline, const FnType target, const bool enable) : m_trampoline(trampoline), m_target(target)
			{
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
				return *this;
			}

			template <Cpt_Ptr Fn>
			Hook& WithTarget(Fn target)
			{
				m_target = reinterpret_cast<FnType>(target);
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

			Ret Call(Args... args) const
			{
				if (!m_target) {
					Log("Hook Node called without trampoline or target set");
					return Ret();
				}

			    Disable();

			    if (constexpr bool is_void = std::is_same_v<Ret, void>) {
                    m_target(args...);
                    Enable();
                    return void();
                }

			    auto res = m_target(args...);
			    Enable();
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