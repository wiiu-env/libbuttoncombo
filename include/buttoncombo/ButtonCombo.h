#pragma once

#ifdef __cplusplus

#include "defines.h"
#include <optional>

namespace ButtonComboModule {

    /**
     * @class ButtonCombo
     * @brief RAII Wrapper for a ButtonComboModule handle.
     *
     * Manages a single button combo. The combo is automatically unregistered via
     * @ref ButtonComboModule_RemoveButtonCombo when this object is destroyed.
     */
    class ButtonCombo {
    public:
        /**
         * @brief Internal factory. Use `ButtonComboModule::Create...` instead.
         */
        static std::optional<ButtonCombo> Create(const ButtonComboModule_ComboOptions &options,
                                                 ButtonComboModule_ComboStatus &outStatus,
                                                 ButtonComboModule_Error &outError) noexcept;
        /**
         * @brief Internal factory (Throwing).
         */
        static ButtonCombo Create(const ButtonComboModule_ComboOptions &options,
                                  ButtonComboModule_ComboStatus &outStatus);

        /**
         * @brief Destructor. Calls @ref ButtonComboModule_RemoveButtonCombo.
         */
        ~ButtonCombo();

        // Movable, not copyable
        ButtonCombo(const ButtonCombo &) = delete;
        ButtonCombo(ButtonCombo &&src) noexcept;
        ButtonCombo &operator=(const ButtonCombo &) = delete;
        ButtonCombo &operator                       =(ButtonCombo &&src) noexcept;

        /**
         * @brief Returns the underlying C handle.
         */
        [[nodiscard]] ButtonComboModule_ComboHandle getHandle() const;

        /**
         * @brief Retrieves status.
         * @sa ButtonComboModule_GetButtonComboStatus
         */
        ButtonComboModule_Error GetButtonComboStatus(ButtonComboModule_ComboStatus &outStatus) const;

        /**
         * @brief Updates the label/metadata.
         * @sa ButtonComboModule_UpdateButtonComboMeta
         */
        [[nodiscard]] ButtonComboModule_Error UpdateButtonComboMeta(const ButtonComboModule_MetaOptions &metaOptions) const;

        /**
         * @brief Updates the callback function.
         * @sa ButtonComboModule_UpdateButtonComboCallback
         */
        [[nodiscard]] ButtonComboModule_Error UpdateButtonComboCallback(const ButtonComboModule_CallbackOptions &callbackOptions) const;

        /**
         * @brief Updates the controller mask and re-checks conflicts.
         * @details See @ref ButtonComboModule_UpdateControllerMask for details on how this affects combo status.
         * @sa ButtonComboModule_UpdateControllerMask
         */
        [[nodiscard]] ButtonComboModule_Error UpdateControllerMask(ButtonComboModule_ControllerTypes controllerMask,
                                                                   ButtonComboModule_ComboStatus &outStatus) const;

        /**
         * @brief Updates the button combination and re-checks conflicts.
         * @details See @ref ButtonComboModule_UpdateButtonCombo for details on how this affects combo status.
         * @sa ButtonComboModule_UpdateButtonCombo
         */
        [[nodiscard]] ButtonComboModule_Error UpdateButtonCombo(ButtonComboModule_Buttons combo,
                                                                ButtonComboModule_ComboStatus &outStatus) const;

        /**
         * @brief Updates the hold duration (Hold combos only).
         * @sa ButtonComboModule_UpdateHoldDuration
         */
        [[nodiscard]] ButtonComboModule_Error UpdateHoldDuration(uint32_t holdDurationInFrames) const;

        /**
         * @brief Retrieves metadata.
         * @sa ButtonComboModule_GetButtonComboMeta
         */
        [[nodiscard]] ButtonComboModule_Error GetButtonComboMeta(ButtonComboModule_MetaOptionsOut &outOptions) const;

        /**
         * @brief Retrieves callback options.
         * @sa ButtonComboModule_GetButtonComboCallback
         */
        ButtonComboModule_Error GetButtonComboCallback(ButtonComboModule_CallbackOptions &outOptions) const;

        /**
         * @brief Retrieves detailed combo info.
         * @sa ButtonComboModule_GetButtonComboInfoEx
         */
        ButtonComboModule_Error GetButtonComboInfoEx(ButtonComboModule_ButtonComboInfoEx &outOptions) const;

    private:
        void ReleaseButtonComboHandle();
        explicit ButtonCombo(ButtonComboModule_ComboHandle handle);
        ButtonComboModule_ComboHandle mHandle = ButtonComboModule_ComboHandle(nullptr);
    };
} // namespace ButtonComboModule
#endif