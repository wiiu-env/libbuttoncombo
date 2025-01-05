#include "buttoncombo/ButtonCombo.h"
#include "buttoncombo/api.h"
#include "buttoncombo/defines.h"

#include <optional>
#include <stdexcept>

namespace ButtonComboModule {
    std::string_view GetStatusStr(const ButtonComboModule_Error status) {
        return ButtonComboModule_GetStatusStr(status);
    }

    std::string_view GetControllerTypeStr(const ButtonComboModule_ControllerTypes controller) {
        return ButtonComboModule_GetControllerTypeStr(controller);
    }

    std::optional<ButtonCombo> CreateComboPressDownEx(const std::string_view label,
                                                      const ButtonComboModule_ControllerTypes controllerMask,
                                                      const ButtonComboModule_Buttons combo,
                                                      const ButtonComboModule_ComboCallback callback,
                                                      void *context,
                                                      const bool observer,
                                                      ButtonComboModule_ComboStatus &outStatus,
                                                      ButtonComboModule_Error &outError) noexcept {
        ButtonComboModule_ComboOptions options               = {};
        options.metaOptions.label                            = label.data();
        options.callbackOptions                              = {.callback = callback, .context = context};
        options.buttonComboOptions.type                      = observer ? BUTTON_COMBO_MODULE_TYPE_PRESS_DOWN_OBSERVER : BUTTON_COMBO_MODULE_TYPE_PRESS_DOWN;
        options.buttonComboOptions.basicCombo.combo          = combo;
        options.buttonComboOptions.basicCombo.controllerMask = controllerMask;

        return ButtonCombo::Create(options, outStatus, outError);
    }

    std::optional<ButtonCombo> CreateComboPressDown(const std::string_view label,
                                                    const ButtonComboModule_Buttons combo,
                                                    const ButtonComboModule_ComboCallback callback,
                                                    void *context,
                                                    ButtonComboModule_ComboStatus &outStatus,
                                                    ButtonComboModule_Error &outError) noexcept {
        return CreateComboPressDownEx(label, BUTTON_COMBO_MODULE_CONTROLLER_ALL, combo, callback, context, false, outStatus, outError);
    }

    std::optional<ButtonCombo> CreateComboPressDownObserver(const std::string_view label,
                                                            const ButtonComboModule_Buttons combo,
                                                            const ButtonComboModule_ComboCallback callback,
                                                            void *context,
                                                            ButtonComboModule_ComboStatus &outStatus,
                                                            ButtonComboModule_Error &outError) noexcept {
        return CreateComboPressDownEx(label, BUTTON_COMBO_MODULE_CONTROLLER_ALL, combo, callback, context, true, outStatus, outError);
    }


    std::optional<ButtonCombo> CreateComboHoldEx(const std::string_view label,
                                                 const ButtonComboModule_ControllerTypes controllerMask,
                                                 const ButtonComboModule_Buttons combo,
                                                 const uint32_t holdDurationInMs,
                                                 const ButtonComboModule_ComboCallback callback,
                                                 void *context,
                                                 const bool observer,
                                                 ButtonComboModule_ComboStatus &outStatus,
                                                 ButtonComboModule_Error &outError) noexcept {
        ButtonComboModule_ComboOptions options               = {};
        options.metaOptions.label                            = label.data();
        options.callbackOptions                              = {.callback = callback, .context = context};
        options.buttonComboOptions.type                      = observer ? BUTTON_COMBO_MODULE_TYPE_HOLD_OBSERVER : BUTTON_COMBO_MODULE_TYPE_HOLD;
        options.buttonComboOptions.basicCombo.combo          = combo;
        options.buttonComboOptions.basicCombo.controllerMask = controllerMask;
        options.buttonComboOptions.optionalHoldForXMs        = holdDurationInMs;

        return ButtonCombo::Create(options, outStatus, outError);
    }

    std::optional<ButtonCombo> CreateComboHold(const std::string_view label,
                                               const ButtonComboModule_Buttons combo,
                                               const uint32_t holdDurationInMs,
                                               const ButtonComboModule_ComboCallback callback,
                                               void *context,
                                               ButtonComboModule_ComboStatus &outStatus,
                                               ButtonComboModule_Error &outError) noexcept {
        return CreateComboHoldEx(label, BUTTON_COMBO_MODULE_CONTROLLER_ALL, combo, holdDurationInMs, callback, context, false, outStatus, outError);
    }

    std::optional<ButtonCombo> CreateComboHoldObserver(const std::string_view label,
                                                       const ButtonComboModule_Buttons combo,
                                                       const uint32_t holdDurationInMs,
                                                       const ButtonComboModule_ComboCallback callback,
                                                       void *context,
                                                       ButtonComboModule_ComboStatus &outStatus,
                                                       ButtonComboModule_Error &outError) noexcept {
        return CreateComboHoldEx(label, BUTTON_COMBO_MODULE_CONTROLLER_ALL, combo, holdDurationInMs, callback, context, true, outStatus, outError);
    }

    ButtonCombo CreateComboPressDownEx(const std::string_view label,
                                       const ButtonComboModule_ControllerTypes controllerMask,
                                       const ButtonComboModule_Buttons combo,
                                       const ButtonComboModule_ComboCallback callback,
                                       void *context,
                                       const bool observer,
                                       ButtonComboModule_ComboStatus &outStatus) {
        ButtonComboModule_Error error;
        auto res = CreateComboPressDownEx(label, controllerMask, combo, callback, context, observer, outStatus, error);
        if (!res) {
            throw std::runtime_error{std::string("Failed to create press down ex button combo: ").append(ButtonComboModule_GetStatusStr(error))};
        }
        return std::move(*res);
    }

    ButtonCombo CreateComboPressDown(const std::string_view label,
                                     const ButtonComboModule_Buttons combo,
                                     const ButtonComboModule_ComboCallback callback,
                                     void *context,
                                     ButtonComboModule_ComboStatus &outStatus) {
        return CreateComboPressDownEx(label, BUTTON_COMBO_MODULE_CONTROLLER_ALL, combo, callback, context, false, outStatus);
    }

    ButtonCombo CreateComboPressDownObserver(const std::string_view label,
                                             const ButtonComboModule_Buttons combo,
                                             const ButtonComboModule_ComboCallback callback,
                                             void *context,
                                             ButtonComboModule_ComboStatus &outStatus) {
        return CreateComboPressDownEx(label, BUTTON_COMBO_MODULE_CONTROLLER_ALL, combo, callback, context, true, outStatus);
    }

    ButtonCombo CreateComboHoldEx(const std::string_view label,
                                  const ButtonComboModule_ControllerTypes controllerMask,
                                  const ButtonComboModule_Buttons combo,
                                  const uint32_t holdDurationInMs,
                                  const ButtonComboModule_ComboCallback callback,
                                  void *context,
                                  const bool observer,
                                  ButtonComboModule_ComboStatus &outStatus) {
        ButtonComboModule_Error error;
        auto res = CreateComboHoldEx(label, controllerMask, combo, holdDurationInMs, callback, context, observer, outStatus, error);
        if (!res) {
            throw std::runtime_error{std::string("Failed to create press down ex button combo: ").append(ButtonComboModule_GetStatusStr(error))};
        }
        return std::move(*res);
    }

    ButtonCombo CreateComboHold(const std::string_view label,
                                const ButtonComboModule_Buttons combo,
                                const uint32_t holdDurationInMs,
                                const ButtonComboModule_ComboCallback callback,
                                void *context,
                                ButtonComboModule_ComboStatus &outStatus) {
        return CreateComboHoldEx(label, BUTTON_COMBO_MODULE_CONTROLLER_ALL, combo, holdDurationInMs, callback, context, false, outStatus);
    }

    ButtonCombo CreateComboHoldObserver(const std::string_view label,
                                        const ButtonComboModule_Buttons combo,
                                        const uint32_t holdDurationInMs,
                                        const ButtonComboModule_ComboCallback callback,
                                        void *context,
                                        ButtonComboModule_ComboStatus &outStatus) {
        return CreateComboHoldEx(label, BUTTON_COMBO_MODULE_CONTROLLER_ALL, combo, holdDurationInMs, callback, context, true, outStatus);
    }

    ButtonComboModule_Error CheckComboAvailable(const ButtonComboModule_ButtonComboOptions &options,
                                                ButtonComboModule_ComboStatus &outStatus) {
        return ButtonComboModule_CheckComboAvailable(&options, &outStatus);
    }

    ButtonComboModule_Error DetectButtonCombo_Blocking(const ButtonComboModule_DetectButtonComboOptions &options,
                                                       ButtonComboModule_Buttons &outButtons) {
        return ButtonComboModule_DetectButtonCombo_Blocking(&options, &outButtons);
    }
} // namespace ButtonComboModule