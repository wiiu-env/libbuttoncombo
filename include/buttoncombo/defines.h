#pragma once

#include <stdint.h>
#include <wut_types.h>

//! Wii U GamePad buttons.
typedef enum ButtonComboModule_Buttons {
    //! The A button.
    BCMPAD_BUTTON_A = 0x8000,
    //! The B button.
    BCMPAD_BUTTON_B = 0x4000,
    //! The X button.
    BCMPAD_BUTTON_X = 0x2000,
    //! The Y button.
    BCMPAD_BUTTON_Y = 0x1000,
    //! The left button of the D-pad.
    BCMPAD_BUTTON_LEFT = 0x0800,
    //! The right button of the D-pad.
    BCMPAD_BUTTON_RIGHT = 0x0400,
    //! The up button of the D-pad.
    BCMPAD_BUTTON_UP = 0x0200,
    //! The down button of the D-pad.
    BCMPAD_BUTTON_DOWN = 0x0100,
    //! The ZL button.
    BCMPAD_BUTTON_ZL = 0x0080,
    //! The ZR button.
    BCMPAD_BUTTON_ZR = 0x0040,
    //! The L button.
    BCMPAD_BUTTON_L = 0x0020,
    //! The R button.
    BCMPAD_BUTTON_R = 0x0010,
    //! The + button.
    BCMPAD_BUTTON_PLUS = 0x0008,
    //! The - button.
    BCMPAD_BUTTON_MINUS = 0x0004,
    //! The right stick button.
    BCMPAD_BUTTON_STICK_R = 0x00020000,
    //! The left stick button.
    BCMPAD_BUTTON_STICK_L = 0x00040000,
    //! The TV button.
    BCMPAD_BUTTON_TV = 0x00010000,
    //! The reserved bit
    BCMPAD_BUTTON_RESERVED_BIT = 0x80000,
    //! The 1 button on the wiimote (exclusive to Wiimotes)
    BCMPAD_BUTTON_1 = 0x0001,
    //! The 2 button on the wiimote (exclusive to Wiimotes)
    BCMPAD_BUTTON_2 = 0x0002,
    //! The C button on the nunchuck (exclusive to Wiimotes)
    BCMPAD_BUTTON_C = 0x100000,
    //! The Z button on the nunchuck (exclusive to Wiimotes)
    BCMPAD_BUTTON_Z = 0x200000,
} ButtonComboModule_Buttons;
WUT_ENUM_BITMASK_TYPE(ButtonComboModule_Buttons);

typedef enum ButtonComboModule_Error {
    BUTTON_COMBO_MODULE_ERROR_SUCCESS                      = 0,
    BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT             = -1,
    BUTTON_COMBO_MODULE_ERROR_HANDLE_NOT_FOUND             = -2,
    BUTTON_COMBO_MODULE_ERROR_INVALID_COMBO                = -3,
    BUTTON_COMBO_MODULE_ERROR_INVALID_COMBO_TYPE           = -4,
    BUTTON_COMBO_MODULE_ERROR_DURATION_MISSING             = -5,
    BUTTON_COMBO_MODULE_ERROR_INCOMPATIBLE_OPTIONS_VERSION = -6,
    BUTTON_COMBO_MODULE_ERROR_MODULE_NOT_FOUND             = -7,
    BUTTON_COMBO_MODULE_ERROR_MODULE_MISSING_EXPORT        = -8,
    BUTTON_COMBO_MODULE_ERROR_UNSUPPORTED_API_VERSION      = -9,
    BUTTON_COMBO_MODULE_ERROR_UNSUPPORTED_COMMAND          = -10,
    BUTTON_COMBO_MODULE_ERROR_LIB_UNINITIALIZED            = -11,
    BUTTON_COMBO_MODULE_ERROR_ABORTED                      = -12,
    BUTTON_COMBO_MODULE_ERROR_UNKNOWN_ERROR                = -0xFF,
} ButtonComboModule_Error;

typedef enum ButtonComboModule_ControllerTypes {
    BUTTON_COMBO_MODULE_CONTROLLER_NONE   = 0,
    BUTTON_COMBO_MODULE_CONTROLLER_VPAD_0 = 1 << 0,
    BUTTON_COMBO_MODULE_CONTROLLER_VPAD_1 = 1 << 1,
    BUTTON_COMBO_MODULE_CONTROLLER_WPAD_0 = 1 << 2,
    BUTTON_COMBO_MODULE_CONTROLLER_WPAD_1 = 1 << 3,
    BUTTON_COMBO_MODULE_CONTROLLER_WPAD_2 = 1 << 4,
    BUTTON_COMBO_MODULE_CONTROLLER_WPAD_3 = 1 << 5,
    BUTTON_COMBO_MODULE_CONTROLLER_WPAD_4 = 1 << 6,
    BUTTON_COMBO_MODULE_CONTROLLER_WPAD_5 = 1 << 7,
    BUTTON_COMBO_MODULE_CONTROLLER_WPAD_6 = 1 << 8,
    BUTTON_COMBO_MODULE_CONTROLLER_VPAD   = BUTTON_COMBO_MODULE_CONTROLLER_VPAD_0 | BUTTON_COMBO_MODULE_CONTROLLER_VPAD_1,
    BUTTON_COMBO_MODULE_CONTROLLER_WPAD   = (BUTTON_COMBO_MODULE_CONTROLLER_WPAD_0 |
                                           BUTTON_COMBO_MODULE_CONTROLLER_WPAD_1 |
                                           BUTTON_COMBO_MODULE_CONTROLLER_WPAD_2 |
                                           BUTTON_COMBO_MODULE_CONTROLLER_WPAD_3 |
                                           BUTTON_COMBO_MODULE_CONTROLLER_WPAD_4 |
                                           BUTTON_COMBO_MODULE_CONTROLLER_WPAD_5 |
                                           BUTTON_COMBO_MODULE_CONTROLLER_WPAD_6),
    BUTTON_COMBO_MODULE_CONTROLLER_ALL    = BUTTON_COMBO_MODULE_CONTROLLER_VPAD | BUTTON_COMBO_MODULE_CONTROLLER_WPAD,
} ButtonComboModule_ControllerTypes;
WUT_ENUM_BITMASK_TYPE(ButtonComboModule_ControllerTypes);

typedef struct ButtonComboModule_ComboHandle {
    void *handle;
#ifdef __cplusplus
    ButtonComboModule_ComboHandle() {
        handle = nullptr;
    }
    explicit ButtonComboModule_ComboHandle(void *handle) : handle(handle) {}
    bool operator==(const ButtonComboModule_ComboHandle other) const {
        return handle == other.handle;
    }
    bool operator==(const void *other) const {
        return handle == other;
    }
#endif
} ButtonComboModule_ComboHandle;

typedef int32_t ButtonComboModule_APIVersion;

typedef enum ButtonComboModule_ComboType {
    BUTTON_COMBO_MODULE_COMBO_TYPE_INVALID             = 0,
    BUTTON_COMBO_MODULE_COMBO_TYPE_HOLD                = 1, // Checks if a combo has been hold for X ms. Does check for conflicts
    BUTTON_COMBO_MODULE_COMBO_TYPE_HOLD_OBSERVER       = 2, // Checks if a combo has been hold for X ms. Does not check for conflicts
    BUTTON_COMBO_MODULE_COMBO_TYPE_PRESS_DOWN          = 3, // Checks if a combo has been pressed down on a controller. Does check for conflicts
    BUTTON_COMBO_MODULE_COMBO_TYPE_PRESS_DOWN_OBSERVER = 4, // Checks if a combo has been pressed down on a controller. Does not check for conflicts
} ButtonComboModule_ComboType;

typedef enum ButtonComboModule_ComboStatus {
    BUTTON_COMBO_MODULE_COMBO_STATUS_INVALID_STATUS = 0, // Invalid status, this only happens on errors.
    BUTTON_COMBO_MODULE_COMBO_STATUS_VALID          = 1, // The Combo is valid and active
    BUTTON_COMBO_MODULE_COMBO_STATUS_CONFLICT       = 2, // The Combo is already used by a different combo. Update to combo by updating the combo or controller mask.
} ButtonComboModule_ComboStatus;

typedef struct ButtonComboModule_DetectButtonComboOptions {
    ButtonComboModule_ControllerTypes controllerMask;
    uint32_t holdComboForInMs;
    uint32_t holdAbortForInMs;
    ButtonComboModule_Buttons abortButtonCombo;
} ButtonComboModule_DetectButtonComboOptions;

/**
 * @typedef ButtonComboModule_ComboCallback
 * @brief Callback function type for handling button combo events.
 *
 * This callback is invoked when a button combo is triggered.
 *
 * @param triggeredBy
 *        The type of controller that triggered the button combination.
 *        See @ref ButtonComboModule_ControllerTypes for possible values.
 *
 * @param handle
 *        A handle representing the button combo that was triggered.
 *        This can be used to identify the specific combo that invoked the callback.
 *
 * @param context
 *        A user-defined context pointer passed during the setup of the callback.
 *        This can be used to provide additional information or state relevant to the callback.
 */
typedef void (*ButtonComboModule_ComboCallback)(ButtonComboModule_ControllerTypes triggeredBy, ButtonComboModule_ComboHandle handle, void *context);

#define BUTTON_COMBO_MODULE_COMBO_OPTIONS_VERSION 1
#define BUTTON_COMBO_MODULE_API_VERSION_ERROR     (-0xFF)

typedef struct ButtonComboModule_MetaOptions {
    const char *label; // Label that identifies a button combo, currently only used for debugging
} ButtonComboModule_MetaOptions;

typedef struct ButtonComboModule_MetaOptionsOut {
    char *labelBuffer;
    uint32_t labelBufferLength;
} ButtonComboModule_MetaOptionsOut;

typedef struct ButtonComboModule_CallbackOptions {
    ButtonComboModule_ComboCallback callback; // Must not be NULL. Defines which callback should be called once a combo is detected
    void *context;                            // Passed into the callback when it's triggered. Can be NULL
} ButtonComboModule_CallbackOptions;

typedef struct ButtonComboModule_ButtonComboOptions {
    ButtonComboModule_ControllerTypes controllerMask; // Defines a mask for which controller should be checked. Must not be empty.
    ButtonComboModule_Buttons combo;                  // Defines which combo should be detected.Note : Not all button are available on all controllers.Must not be empty.
} ButtonComboModule_ButtonComboOptions;

typedef struct ButtonComboModule_ButtonComboInfoEx {
    ButtonComboModule_ComboType type;                // Defines the type of the combo AND if it will check for conflicts.
    ButtonComboModule_ButtonComboOptions basicCombo; // Defines which combo should be checked on which controller
    uint32_t optionalHoldForXMs;                     // Only mandatory if the type is set to COMBO_TYPE_HOLD or COMBO_TYPE_HOLD_OBSERVER
} ButtonComboModule_ButtonComboInfoEx;

typedef struct ButtonComboModule_ComboOptions {
    int version;                                            // Has to be set to BUTTON_COMBO_MODULE_COMBO_OPTIONS_VERSION
    ButtonComboModule_MetaOptions metaOptions;              // Defines the meta information about the combo e.g. the label
    ButtonComboModule_CallbackOptions callbackOptions;      // Defines the callback that should be called once the combo is detected
    ButtonComboModule_ButtonComboInfoEx buttonComboOptions; // Defines how and when which combo should be detected
} ButtonComboModule_ComboOptions;