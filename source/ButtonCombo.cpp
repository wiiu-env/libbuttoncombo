#include <buttoncombo/ButtonCombo.h>
#include <buttoncombo/api.h>
#include <buttoncombo/defines.h>

#include <coreinit/debug.h>
#include <stdexcept>

namespace ButtonComboModule {

    std::optional<ButtonCombo> ButtonCombo::Create(const ButtonComboModule_ComboOptions &options,
                                                   ButtonComboModule_ComboStatus &outStatus,
                                                   ButtonComboModule_Error &outError) noexcept {
        ButtonComboModule_ComboHandle handle;
        if (outError = ButtonComboModule_AddButtonCombo(&options, &handle, &outStatus); outError == BUTTON_COMBO_MODULE_ERROR_SUCCESS) {
            return ButtonCombo(handle);
        }
        return {};
    }

    ButtonCombo ButtonCombo::Create(const ButtonComboModule_ComboOptions &options,
                                    ButtonComboModule_ComboStatus &outStatus) {
        ButtonComboModule_Error error;
        auto res = Create(options, outStatus, error);
        if (!res) {
            throw std::runtime_error{std::string("Failed to create button combo: ").append(ButtonComboModule_GetStatusStr(error))};
        }
        return std::move(*res);
    }

    ButtonCombo::~ButtonCombo() {
        ReleaseButtonComboHandle();
    }

    void ButtonCombo::ReleaseButtonComboHandle() {
        if (mHandle != nullptr) {
            if (const auto res = ButtonComboModule_RemoveButtonCombo(mHandle); res != BUTTON_COMBO_MODULE_ERROR_SUCCESS) {
                OSReport("ButtonCombo::ReleaseButtonComboHandle(): ButtonComboModule_RemoveButtonCombo for %p returned: %s\n", mHandle.handle, ButtonComboModule_GetStatusStr(res));
            }
            mHandle = ButtonComboModule_ComboHandle(nullptr);
        }
    }

    ButtonCombo::ButtonCombo(ButtonCombo &&src) noexcept {
        ReleaseButtonComboHandle();

        mHandle = src.mHandle;

        src.mHandle = ButtonComboModule_ComboHandle(nullptr);
    }

    ButtonCombo &ButtonCombo::operator=(ButtonCombo &&src) noexcept {
        if (this != &src) {
            ReleaseButtonComboHandle();

            mHandle = src.mHandle;

            src.mHandle = ButtonComboModule_ComboHandle(nullptr);
        }
        return *this;
    }

    [[nodiscard]] ButtonComboModule_ComboHandle ButtonCombo::getHandle() const {
        return mHandle;
    }

    ButtonComboModule_Error ButtonCombo::GetButtonComboStatus(ButtonComboModule_ComboStatus &outStatus) const {
        return ButtonComboModule_GetButtonComboStatus(mHandle, &outStatus);
    }

    [[nodiscard]] ButtonComboModule_Error ButtonCombo::UpdateButtonComboMeta(const ButtonComboModule_MetaOptions &metaOptions) const {
        return ButtonComboModule_UpdateButtonComboMeta(mHandle, &metaOptions);
    }

    [[nodiscard]] ButtonComboModule_Error ButtonCombo::UpdateButtonComboCallback(const ButtonComboModule_CallbackOptions &callbackOptions) const {
        return ButtonComboModule_UpdateButtonComboCallback(mHandle, &callbackOptions);
    }

    [[nodiscard]] ButtonComboModule_Error ButtonCombo::UpdateControllerMask(const ButtonComboModule_ControllerTypes controllerMask,
                                                                            ButtonComboModule_ComboStatus &outStatus) const {
        return ButtonComboModule_UpdateControllerMask(mHandle, controllerMask, &outStatus);
    }

    [[nodiscard]] ButtonComboModule_Error ButtonCombo::UpdateButtonCombo(const ButtonComboModule_Buttons combo,
                                                                         ButtonComboModule_ComboStatus &outStatus) const {
        return ButtonComboModule_UpdateButtonCombo(mHandle, combo, &outStatus);
    }

    [[nodiscard]] ButtonComboModule_Error ButtonCombo::UpdateHoldDuration(const uint32_t holdDurationInFrames) const {
        return ButtonComboModule_UpdateHoldDuration(mHandle, holdDurationInFrames);
    }

    [[nodiscard]] ButtonComboModule_Error ButtonCombo::GetButtonComboMeta(ButtonComboModule_MetaOptionsOut &outOptions) const {
        return ButtonComboModule_GetButtonComboMeta(mHandle, &outOptions);
    }

    ButtonComboModule_Error ButtonCombo::GetButtonComboCallback(ButtonComboModule_CallbackOptions &outOptions) const {
        return ButtonComboModule_GetButtonComboCallback(mHandle, &outOptions);
    }

    ButtonComboModule_Error ButtonCombo::GetButtonComboInfoEx(ButtonComboModule_ButtonComboInfoEx &outOptions) const {
        return ButtonComboModule_GetButtonComboInfoEx(mHandle, &outOptions);
    }

    ButtonCombo::ButtonCombo(const ButtonComboModule_ComboHandle handle) : mHandle(handle) {
    }
} // namespace ButtonComboModule
