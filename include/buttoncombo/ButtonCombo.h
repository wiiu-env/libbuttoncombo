#pragma once

#ifdef __cplusplus

#include "defines.h"

#include <optional>

namespace ButtonComboModule {
    class ButtonCombo {
    public:
        static std::optional<ButtonCombo> Create(const ButtonComboModule_ComboOptions &options,
                                                 ButtonComboModule_ComboStatus &outStatus,
                                                 ButtonComboModule_Error &outError) noexcept;

        static ButtonCombo Create(const ButtonComboModule_ComboOptions &options,
                                  ButtonComboModule_ComboStatus &outStatus);

        ~ButtonCombo();
        ButtonCombo(const ButtonCombo &) = delete;

        ButtonCombo(ButtonCombo &&src) noexcept;

        ButtonCombo &operator=(const ButtonCombo &) = delete;

        ButtonCombo &operator=(ButtonCombo &&src) noexcept;

        [[nodiscard]] ButtonComboModule_ComboHandle getHandle() const;

        ButtonComboModule_Error GetButtonComboStatus(ButtonComboModule_ComboStatus &outStatus) const;

        [[nodiscard]] ButtonComboModule_Error UpdateButtonComboMeta(const ButtonComboModule_MetaOptions &metaOptions) const;

        [[nodiscard]] ButtonComboModule_Error UpdateButtonComboCallback(const ButtonComboModule_CallbackOptions &callbackOptions) const;

        [[nodiscard]] ButtonComboModule_Error UpdateControllerMask(ButtonComboModule_ControllerTypes controllerMask,
                                                                   ButtonComboModule_ComboStatus &outStatus) const;

        [[nodiscard]] ButtonComboModule_Error UpdateButtonCombo(ButtonComboModule_Buttons combo,
                                                                ButtonComboModule_ComboStatus &outStatus) const;

        [[nodiscard]] ButtonComboModule_Error UpdateHoldDuration(uint32_t holdDurationInFrames) const;

        [[nodiscard]] ButtonComboModule_Error GetButtonComboMeta(ButtonComboModule_MetaOptionsOut &outOptions) const;

        ButtonComboModule_Error GetButtonComboCallback(ButtonComboModule_CallbackOptions &outOptions) const;

        ButtonComboModule_Error GetButtonComboInfoEx(ButtonComboModule_ButtonComboInfoEx &outOptions) const;

    private:
        void ReleaseButtonComboHandle();

        explicit ButtonCombo(ButtonComboModule_ComboHandle handle);

        ButtonComboModule_ComboHandle mHandle = ButtonComboModule_ComboHandle(nullptr);
    };
} // namespace ButtonComboModule
#endif