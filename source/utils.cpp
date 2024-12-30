#include "logger.h"
#include <buttoncombo/defines.h>
#include <buttoncombo/manager.h>
#include <coreinit/debug.h>
#include <coreinit/dynload.h>
#include <cstdarg>

static OSDynLoad_Module sModuleHandle = nullptr;

static ButtonComboModule_Error (*sBCMGetVersionFn)(ButtonComboModule_APIVersion *) = nullptr;

static ButtonComboModule_Error (*sBCMAddButtonComboFn)(const ButtonComboModule_ComboOptions *options, ButtonComboModule_ComboHandle *outHandle, ButtonComboModule_ComboStatus *outStatus)    = nullptr;
static ButtonComboModule_Error (*sBCMRemoveButtonComboFn)(ButtonComboModule_ComboHandle handle)                                                                                              = nullptr;
static ButtonComboModule_Error (*sBCMGetButtonComboStatus)(ButtonComboModule_ComboHandle handle, ButtonComboModule_ComboStatus *outStatus)                                                   = nullptr;
static ButtonComboModule_Error (*sBCMUpdateButtonComboMeta)(ButtonComboModule_ComboHandle handle, const ButtonComboModule_MetaOptions *options)                                              = nullptr;
static ButtonComboModule_Error (*sBCMUpdateButtonComboCallback)(ButtonComboModule_ComboHandle handle, const ButtonComboModule_CallbackOptions *options)                                      = nullptr;
static ButtonComboModule_Error (*sBCMUpdateControllerMask)(ButtonComboModule_ComboHandle handle, ButtonComboModule_ControllerTypes controllerMask, ButtonComboModule_ComboStatus *outStatus) = nullptr;
static ButtonComboModule_Error (*sBCMUpdateButtonCombo)(ButtonComboModule_ComboHandle handle, ButtonComboModule_Buttons combo, ButtonComboModule_ComboStatus *outStatus)                     = nullptr;
static ButtonComboModule_Error (*sBCMUpdateHoldDuration)(ButtonComboModule_ComboHandle handle, uint32_t holdDurationInMs)                                                                    = nullptr;
static ButtonComboModule_Error (*sBCMGetButtonComboMeta)(ButtonComboModule_ComboHandle handle, ButtonComboModule_MetaOptionsOut *outOptions)                                                 = nullptr;
static ButtonComboModule_Error (*sBCMGetButtonComboCallback)(ButtonComboModule_ComboHandle handle, ButtonComboModule_CallbackOptions *outOptions)                                            = nullptr;
static ButtonComboModule_Error (*sBCMGetButtonComboInfoEx)(ButtonComboModule_ComboHandle handle, ButtonComboModule_ButtonComboInfoEx *outOptions)                                            = nullptr;

static ButtonComboModule_Error (*sBCMCheckComboAvailable)(const ButtonComboModule_ButtonComboOptions *options, ButtonComboModule_ComboStatus *outStatus)              = nullptr;
static ButtonComboModule_Error (*sBCMDetectButtonComboBlocking)(const ButtonComboModule_DetectButtonComboOptions *options, ButtonComboModule_Buttons *outButtonCombo) = nullptr;

static bool sLibInitDone = false;

static ButtonComboModule_APIVersion sButtonComboModuleVersion = BUTTON_COMBO_MODULE_API_VERSION_ERROR;

const char *ButtonComboModule_GetStatusStr(const ButtonComboModule_Error status) {
    switch (status) {
        case BUTTON_COMBO_MODULE_ERROR_SUCCESS:
            return "BUTTON_COMBO_MODULE_ERROR_SUCCESS";
        case BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT:
            return "BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT";
        case BUTTON_COMBO_MODULE_ERROR_INVALID_COMBO:
            return "BUTTON_COMBO_MODULE_ERROR_INVALID_COMBO";
        case BUTTON_COMBO_MODULE_ERROR_INVALID_COMBO_TYPE:
            return "BUTTON_COMBO_MODULE_ERROR_INVALID_COMBO_TYPE";
        case BUTTON_COMBO_MODULE_ERROR_DURATION_MISSING:
            return "BUTTON_COMBO_MODULE_ERROR_DURATION_MISSING";
        case BUTTON_COMBO_MODULE_ERROR_INCOMPATIBLE_OPTIONS_VERSION:
            return "BUTTON_COMBO_MODULE_ERROR_INCOMPATIBLE_OPTIONS_VERSION";
        case BUTTON_COMBO_MODULE_ERROR_MODULE_NOT_FOUND:
            return "BUTTON_COMBO_MODULE_ERROR_MODULE_NOT_FOUND";
        case BUTTON_COMBO_MODULE_ERROR_MODULE_MISSING_EXPORT:
            return "BUTTON_COMBO_MODULE_ERROR_MODULE_MISSING_EXPORT";
        case BUTTON_COMBO_MODULE_ERROR_UNSUPPORTED_API_VERSION:
            return "BUTTON_COMBO_MODULE_ERROR_UNSUPPORTED_API_VERSION";
        case BUTTON_COMBO_MODULE_ERROR_UNKNOWN_ERROR:
            return "BUTTON_COMBO_MODULE_ERROR_UNKNOWN_ERROR";
        case BUTTON_COMBO_MODULE_ERROR_UNSUPPORTED_COMMAND:
            return "BUTTON_COMBO_MODULE_ERROR_UNSUPPORTED_COMMAND";
        case BUTTON_COMBO_MODULE_ERROR_LIB_UNINITIALIZED:
            return "BUTTON_COMBO_MODULE_ERROR_LIB_UNINITIALIZED";
        case BUTTON_COMBO_MODULE_ERROR_HANDLE_NOT_FOUND:
            return "BUTTON_COMBO_MODULE_ERROR_HANDLE_NOT_FOUND";
        case BUTTON_COMBO_MODULE_ERROR_ABORTED:
            return "BUTTON_COMBO_MODULE_ERROR_ABORTED";
    }
    return "BUTTON_COMBO_MODULE_ERROR_UNKNOWN_ERROR";
}

ButtonComboModule_Error ButtonComboModule_InitLibrary() {
    if (sLibInitDone) {
        return BUTTON_COMBO_MODULE_ERROR_SUCCESS;
    }
    if (OSDynLoad_Acquire("homebrew_buttoncombo", &sModuleHandle) != OS_DYNLOAD_OK) {
        DEBUG_FUNCTION_LINE_ERR("OSDynLoad_Acquire failed.");
        return BUTTON_COMBO_MODULE_ERROR_MODULE_NOT_FOUND;
    }

    if (OSDynLoad_FindExport(sModuleHandle, OS_DYNLOAD_EXPORT_FUNC, "ButtonComboModule_GetVersion", reinterpret_cast<void **>(&sBCMGetVersionFn)) != OS_DYNLOAD_OK) {
        DEBUG_FUNCTION_LINE_ERR("FindExport ButtonComboModule_GetVersion failed.");
        return BUTTON_COMBO_MODULE_ERROR_MODULE_MISSING_EXPORT;
    }
    if (OSDynLoad_FindExport(sModuleHandle, OS_DYNLOAD_EXPORT_FUNC, "ButtonComboModule_AddButtonCombo", reinterpret_cast<void **>(&sBCMAddButtonComboFn)) != OS_DYNLOAD_OK) {
        DEBUG_FUNCTION_LINE_ERR("FindExport ButtonComboModule_AddButtonCombo failed.");
        return BUTTON_COMBO_MODULE_ERROR_MODULE_MISSING_EXPORT;
    }
    if (OSDynLoad_FindExport(sModuleHandle, OS_DYNLOAD_EXPORT_FUNC, "ButtonComboModule_RemoveButtonCombo", reinterpret_cast<void **>(&sBCMRemoveButtonComboFn)) != OS_DYNLOAD_OK) {
        DEBUG_FUNCTION_LINE_ERR("FindExport ButtonComboModule_RemoveButtonCombo failed.");
        return BUTTON_COMBO_MODULE_ERROR_MODULE_MISSING_EXPORT;
    }
    if (OSDynLoad_FindExport(sModuleHandle, OS_DYNLOAD_EXPORT_FUNC, "ButtonComboModule_GetButtonComboStatus", reinterpret_cast<void **>(&sBCMGetButtonComboStatus)) != OS_DYNLOAD_OK) {
        DEBUG_FUNCTION_LINE_ERR("FindExport ButtonComboModule_GetButtonComboStatus failed.");
        return BUTTON_COMBO_MODULE_ERROR_MODULE_MISSING_EXPORT;
    }
    if (OSDynLoad_FindExport(sModuleHandle, OS_DYNLOAD_EXPORT_FUNC, "ButtonComboModule_UpdateButtonComboMeta", reinterpret_cast<void **>(&sBCMUpdateButtonComboMeta)) != OS_DYNLOAD_OK) {
        DEBUG_FUNCTION_LINE_ERR("FindExport ButtonComboModule_UpdateButtonComboMeta failed.");
        return BUTTON_COMBO_MODULE_ERROR_MODULE_MISSING_EXPORT;
    }
    if (OSDynLoad_FindExport(sModuleHandle, OS_DYNLOAD_EXPORT_FUNC, "ButtonComboModule_UpdateButtonComboCallback", reinterpret_cast<void **>(&sBCMUpdateButtonComboCallback)) != OS_DYNLOAD_OK) {
        DEBUG_FUNCTION_LINE_ERR("FindExport ButtonComboModule_UpdateButtonComboCallback failed.");
        return BUTTON_COMBO_MODULE_ERROR_MODULE_MISSING_EXPORT;
    }
    if (OSDynLoad_FindExport(sModuleHandle, OS_DYNLOAD_EXPORT_FUNC, "ButtonComboModule_UpdateControllerMask", reinterpret_cast<void **>(&sBCMUpdateControllerMask)) != OS_DYNLOAD_OK) {
        DEBUG_FUNCTION_LINE_ERR("FindExport ButtonComboModule_UpdateControllerMask failed.");
        return BUTTON_COMBO_MODULE_ERROR_MODULE_MISSING_EXPORT;
    }
    if (OSDynLoad_FindExport(sModuleHandle, OS_DYNLOAD_EXPORT_FUNC, "ButtonComboModule_UpdateButtonCombo", reinterpret_cast<void **>(&sBCMUpdateButtonCombo)) != OS_DYNLOAD_OK) {
        DEBUG_FUNCTION_LINE_ERR("FindExport ButtonComboModule_UpdateButtonCombo failed.");
        return BUTTON_COMBO_MODULE_ERROR_MODULE_MISSING_EXPORT;
    }
    if (OSDynLoad_FindExport(sModuleHandle, OS_DYNLOAD_EXPORT_FUNC, "ButtonComboModule_UpdateHoldDuration", reinterpret_cast<void **>(&sBCMUpdateHoldDuration)) != OS_DYNLOAD_OK) {
        DEBUG_FUNCTION_LINE_ERR("FindExport ButtonComboModule_UpdateHoldDuration failed.");
        return BUTTON_COMBO_MODULE_ERROR_MODULE_MISSING_EXPORT;
    }
    if (OSDynLoad_FindExport(sModuleHandle, OS_DYNLOAD_EXPORT_FUNC, "ButtonComboModule_GetButtonComboMeta", reinterpret_cast<void **>(&sBCMGetButtonComboMeta)) != OS_DYNLOAD_OK) {
        DEBUG_FUNCTION_LINE_ERR("FindExport ButtonComboModule_GetButtonComboMeta failed.");
        return BUTTON_COMBO_MODULE_ERROR_MODULE_MISSING_EXPORT;
    }
    if (OSDynLoad_FindExport(sModuleHandle, OS_DYNLOAD_EXPORT_FUNC, "ButtonComboModule_GetButtonComboCallback", reinterpret_cast<void **>(&sBCMGetButtonComboCallback)) != OS_DYNLOAD_OK) {
        DEBUG_FUNCTION_LINE_ERR("FindExport ButtonComboModule_GetButtonComboCallback failed.");
        return BUTTON_COMBO_MODULE_ERROR_MODULE_MISSING_EXPORT;
    }
    if (OSDynLoad_FindExport(sModuleHandle, OS_DYNLOAD_EXPORT_FUNC, "ButtonComboModule_GetButtonComboInfoEx", reinterpret_cast<void **>(&sBCMGetButtonComboInfoEx)) != OS_DYNLOAD_OK) {
        DEBUG_FUNCTION_LINE_ERR("FindExport ButtonComboModule_GetButtonComboInfoEx failed.");
        return BUTTON_COMBO_MODULE_ERROR_MODULE_MISSING_EXPORT;
    }
    if (OSDynLoad_FindExport(sModuleHandle, OS_DYNLOAD_EXPORT_FUNC, "ButtonComboModule_CheckComboAvailable", reinterpret_cast<void **>(&sBCMCheckComboAvailable)) != OS_DYNLOAD_OK) {
        DEBUG_FUNCTION_LINE_ERR("FindExport ButtonComboModule_CheckComboAvailable failed.");
        return BUTTON_COMBO_MODULE_ERROR_MODULE_MISSING_EXPORT;
    }
    if (OSDynLoad_FindExport(sModuleHandle, OS_DYNLOAD_EXPORT_FUNC, "ButtonComboModule_DetectButtonCombo_Blocking", reinterpret_cast<void **>(&sBCMDetectButtonComboBlocking)) != OS_DYNLOAD_OK) {
        DEBUG_FUNCTION_LINE_ERR("FindExport ButtonComboModule_DetectButtonCombo_Blocking failed.");
        return BUTTON_COMBO_MODULE_ERROR_MODULE_MISSING_EXPORT;
    }

    if (ButtonComboModule_GetVersion(&sButtonComboModuleVersion) != BUTTON_COMBO_MODULE_ERROR_SUCCESS) {
        sButtonComboModuleVersion = BUTTON_COMBO_MODULE_API_VERSION_ERROR;
        return BUTTON_COMBO_MODULE_ERROR_UNSUPPORTED_API_VERSION;
    }

    sLibInitDone = true;
    return BUTTON_COMBO_MODULE_ERROR_SUCCESS;
}

ButtonComboModule_Error ButtonComboModule_DeInitLibrary() {
    if (sLibInitDone) {
        sBCMGetVersionFn          = nullptr;
        sButtonComboModuleVersion = BUTTON_COMBO_MODULE_API_VERSION_ERROR;
        OSDynLoad_Release(sModuleHandle);
        sModuleHandle = nullptr;
        sLibInitDone  = false;
    }
    return BUTTON_COMBO_MODULE_ERROR_SUCCESS;
}

ButtonComboModule_Error ButtonComboModule_GetVersion(ButtonComboModule_APIVersion *outVersion) {
    if (sBCMGetVersionFn == nullptr) {
        if (OSDynLoad_Acquire("homebrew_buttoncombo", &sModuleHandle) != OS_DYNLOAD_OK) {
            DEBUG_FUNCTION_LINE_WARN("OSDynLoad_Acquire failed.");
            return BUTTON_COMBO_MODULE_ERROR_MODULE_NOT_FOUND;
        }

        if (OSDynLoad_FindExport(sModuleHandle, OS_DYNLOAD_EXPORT_FUNC, "GetVersion", reinterpret_cast<void **>(&sBCMGetVersionFn)) != OS_DYNLOAD_OK) {
            DEBUG_FUNCTION_LINE_WARN("FindExport sBCMGetVersion failed.");
            return BUTTON_COMBO_MODULE_ERROR_MODULE_MISSING_EXPORT;
        }
    }

    return sBCMGetVersionFn(outVersion);
}

namespace {
    ButtonComboModule_ComboOptions getBaseComboOptions(const char *label,
                                                       const ButtonComboModule_ComboCallback callback,
                                                       void *context) {
        ButtonComboModule_ComboOptions comboOptions = {};
        comboOptions.version                        = BUTTON_COMBO_MODULE_COMBO_OPTIONS_VERSION;
        comboOptions.metaOptions.label              = label;
        comboOptions.callbackOptions.callback       = callback;
        comboOptions.callbackOptions.context        = context;
        return comboOptions;
    }
} // namespace

ButtonComboModule_Error ButtonComboModule_AddButtonComboSimplePressDownEx(
        const char *label,
        const ButtonComboModule_Buttons buttonCombo,
        const ButtonComboModule_ControllerTypes controllerMask,
        const ButtonComboModule_ComboCallback callback,
        void *context,
        ButtonComboModule_ComboHandle *outHandle,
        ButtonComboModule_ComboStatus *outStatus) {
    ButtonComboModule_ComboOptions options               = getBaseComboOptions(label, callback, context);
    options.buttonComboOptions.type                      = BUTTON_COMBO_MODULE_TYPE_PRESS_DOWN;
    options.buttonComboOptions.basicCombo.combo          = buttonCombo;
    options.buttonComboOptions.basicCombo.controllerMask = controllerMask;

    return ButtonComboModule_AddButtonCombo(&options, outHandle, outStatus);
}

ButtonComboModule_Error ButtonComboModule_AddButtonComboSimplePressDown(
        const char *label,
        const ButtonComboModule_Buttons buttonCombo,
        const ButtonComboModule_ComboCallback callback,
        void *context,
        ButtonComboModule_ComboHandle *handleOut) {
    return ButtonComboModule_AddButtonComboSimplePressDownEx(label,
                                                             buttonCombo,
                                                             BUTTON_COMBO_MODULE_CONTROLLER_ALL,
                                                             callback,
                                                             context,
                                                             handleOut);
}

ButtonComboModule_Error ButtonComboModule_AddButtonComboSimpleHold(
        const char *label,
        const ButtonComboModule_Buttons buttonCombo,
        const uint32_t holdDurationInMs,
        const ButtonComboModule_ComboCallback callback,
        void *context,
        ButtonComboModule_ComboHandle *outHandle,
        ButtonComboModule_ComboStatus *outStatus) {
    ButtonComboModule_ComboOptions options               = getBaseComboOptions(label, callback, context);
    options.buttonComboOptions.type                      = BUTTON_COMBO_MODULE_TYPE_HOLD;
    options.buttonComboOptions.basicCombo.combo          = buttonCombo;
    options.buttonComboOptions.basicCombo.controllerMask = BUTTON_COMBO_MODULE_CONTROLLER_ALL;
    options.buttonComboOptions.optionalHoldForXMs        = holdDurationInMs;

    return ButtonComboModule_AddButtonCombo(&options, outHandle, outStatus);
}

ButtonComboModule_Error ButtonComboModule_AddButtonCombo(const ButtonComboModule_ComboOptions *options,
                                                         ButtonComboModule_ComboHandle *outHandle,
                                                         ButtonComboModule_ComboStatus *outStatus) {
    if (sButtonComboModuleVersion == BUTTON_COMBO_MODULE_API_VERSION_ERROR) {
        return BUTTON_COMBO_MODULE_ERROR_LIB_UNINITIALIZED;
    }
    if (sBCMAddButtonComboFn == nullptr || sButtonComboModuleVersion < 1) {
        return BUTTON_COMBO_MODULE_ERROR_UNSUPPORTED_COMMAND;
    }

    if (outHandle == nullptr) {
        return BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT;
    }

    return sBCMAddButtonComboFn(options, outHandle, outStatus);
}

ButtonComboModule_Error ButtonComboModule_RemoveButtonCombo(const ButtonComboModule_ComboHandle handle) {
    if (sButtonComboModuleVersion == BUTTON_COMBO_MODULE_API_VERSION_ERROR) {
        return BUTTON_COMBO_MODULE_ERROR_LIB_UNINITIALIZED;
    }
    if (sBCMRemoveButtonComboFn == nullptr || sButtonComboModuleVersion < 1) {
        return BUTTON_COMBO_MODULE_ERROR_UNSUPPORTED_COMMAND;
    }

    if (handle == 0) {
        return BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT;
    }

    return sBCMRemoveButtonComboFn(handle);
}


ButtonComboModule_Error ButtonComboModule_GetButtonComboStatus(const ButtonComboModule_ComboHandle handle,
                                                               ButtonComboModule_ComboStatus *outStatus) {
    if (sButtonComboModuleVersion == BUTTON_COMBO_MODULE_API_VERSION_ERROR) {
        return BUTTON_COMBO_MODULE_ERROR_LIB_UNINITIALIZED;
    }
    if (sBCMGetButtonComboStatus == nullptr || sButtonComboModuleVersion < 1) {
        return BUTTON_COMBO_MODULE_ERROR_UNSUPPORTED_COMMAND;
    }

    if (handle == 0 || outStatus == nullptr) {
        return BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT;
    }

    return sBCMGetButtonComboStatus(handle, outStatus);
}


ButtonComboModule_Error ButtonComboModule_UpdateButtonComboMeta(const ButtonComboModule_ComboHandle handle,
                                                                const ButtonComboModule_MetaOptions *options) {
    if (sButtonComboModuleVersion == BUTTON_COMBO_MODULE_API_VERSION_ERROR) {
        return BUTTON_COMBO_MODULE_ERROR_LIB_UNINITIALIZED;
    }
    if (sBCMUpdateButtonComboMeta == nullptr || sButtonComboModuleVersion < 1) {
        return BUTTON_COMBO_MODULE_ERROR_UNSUPPORTED_COMMAND;
    }

    if (handle == 0 || options == nullptr) {
        return BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT;
    }

    return sBCMUpdateButtonComboMeta(handle, options);
}

ButtonComboModule_Error ButtonComboModule_UpdateButtonComboCallback(const ButtonComboModule_ComboHandle handle,
                                                                    const ButtonComboModule_CallbackOptions *options) {
    if (sButtonComboModuleVersion == BUTTON_COMBO_MODULE_API_VERSION_ERROR) {
        return BUTTON_COMBO_MODULE_ERROR_LIB_UNINITIALIZED;
    }
    if (sBCMUpdateButtonComboCallback == nullptr || sButtonComboModuleVersion < 1) {
        return BUTTON_COMBO_MODULE_ERROR_UNSUPPORTED_COMMAND;
    }

    if (handle == 0 || options == nullptr) {
        return BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT;
    }

    return sBCMUpdateButtonComboCallback(handle, options);
}

ButtonComboModule_Error ButtonComboModule_UpdateControllerMask(const ButtonComboModule_ComboHandle handle,
                                                               const ButtonComboModule_ControllerTypes controllerMask,
                                                               ButtonComboModule_ComboStatus *outStatus) {
    if (sButtonComboModuleVersion == BUTTON_COMBO_MODULE_API_VERSION_ERROR) {
        return BUTTON_COMBO_MODULE_ERROR_LIB_UNINITIALIZED;
    }
    if (sBCMUpdateControllerMask == nullptr || sButtonComboModuleVersion < 1) {
        return BUTTON_COMBO_MODULE_ERROR_UNSUPPORTED_COMMAND;
    }

    if (handle == 0) {
        return BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT;
    }

    return sBCMUpdateControllerMask(handle, controllerMask, outStatus);
}
ButtonComboModule_Error ButtonComboModule_UpdateButtonCombo(const ButtonComboModule_ComboHandle handle,
                                                            const ButtonComboModule_Buttons combo,
                                                            ButtonComboModule_ComboStatus *outStatus) {
    if (sButtonComboModuleVersion == BUTTON_COMBO_MODULE_API_VERSION_ERROR) {
        return BUTTON_COMBO_MODULE_ERROR_LIB_UNINITIALIZED;
    }
    if (sBCMUpdateButtonCombo == nullptr || sButtonComboModuleVersion < 1) {
        return BUTTON_COMBO_MODULE_ERROR_UNSUPPORTED_COMMAND;
    }

    if (handle == 0) {
        return BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT;
    }

    return sBCMUpdateButtonCombo(handle, combo, outStatus);
}

ButtonComboModule_Error ButtonComboModule_UpdateHoldDuration(const ButtonComboModule_ComboHandle handle,
                                                             const uint32_t holdDurationInMs) {
    if (sButtonComboModuleVersion == BUTTON_COMBO_MODULE_API_VERSION_ERROR) {
        return BUTTON_COMBO_MODULE_ERROR_LIB_UNINITIALIZED;
    }
    if (sBCMUpdateHoldDuration == nullptr || sButtonComboModuleVersion < 1) {
        return BUTTON_COMBO_MODULE_ERROR_UNSUPPORTED_COMMAND;
    }

    if (handle == 0) {
        return BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT;
    }

    return sBCMUpdateHoldDuration(handle, holdDurationInMs);
}

ButtonComboModule_Error ButtonComboModule_GetButtonComboMeta(const ButtonComboModule_ComboHandle handle,
                                                             ButtonComboModule_MetaOptionsOut *outOptions) {
    if (sButtonComboModuleVersion == BUTTON_COMBO_MODULE_API_VERSION_ERROR) {
        return BUTTON_COMBO_MODULE_ERROR_LIB_UNINITIALIZED;
    }
    if (sBCMGetButtonComboMeta == nullptr || sButtonComboModuleVersion < 1) {
        return BUTTON_COMBO_MODULE_ERROR_UNSUPPORTED_COMMAND;
    }

    if (handle == 0 || outOptions == nullptr) {
        return BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT;
    }

    return sBCMGetButtonComboMeta(handle, outOptions);
}

ButtonComboModule_Error ButtonComboModule_GetButtonComboCallback(const ButtonComboModule_ComboHandle handle,
                                                                 ButtonComboModule_CallbackOptions *outOptions) {
    if (sButtonComboModuleVersion == BUTTON_COMBO_MODULE_API_VERSION_ERROR) {
        return BUTTON_COMBO_MODULE_ERROR_LIB_UNINITIALIZED;
    }
    if (sBCMGetButtonComboCallback == nullptr || sButtonComboModuleVersion < 1) {
        return BUTTON_COMBO_MODULE_ERROR_UNSUPPORTED_COMMAND;
    }

    if (handle == 0 || outOptions == nullptr) {
        return BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT;
    }

    return sBCMGetButtonComboCallback(handle, outOptions);
}

ButtonComboModule_Error ButtonComboModule_GetButtonComboInfoEx(const ButtonComboModule_ComboHandle handle,
                                                               ButtonComboModule_ButtonComboInfoEx *outOptions) {
    if (sButtonComboModuleVersion == BUTTON_COMBO_MODULE_API_VERSION_ERROR) {
        return BUTTON_COMBO_MODULE_ERROR_LIB_UNINITIALIZED;
    }
    if (sBCMGetButtonComboInfoEx == nullptr || sButtonComboModuleVersion < 1) {
        return BUTTON_COMBO_MODULE_ERROR_UNSUPPORTED_COMMAND;
    }

    if (handle == 0 || outOptions == nullptr) {
        return BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT;
    }

    return sBCMGetButtonComboInfoEx(handle, outOptions);
}

ButtonComboModule_Error ButtonComboModule_CheckComboAvailable(const ButtonComboModule_ButtonComboOptions *options,
                                                              ButtonComboModule_ComboStatus *outStatus) {
    if (sButtonComboModuleVersion == BUTTON_COMBO_MODULE_API_VERSION_ERROR) {
        return BUTTON_COMBO_MODULE_ERROR_LIB_UNINITIALIZED;
    }
    if (sBCMCheckComboAvailable == nullptr || sButtonComboModuleVersion < 1) {
        return BUTTON_COMBO_MODULE_ERROR_UNSUPPORTED_COMMAND;
    }

    if (options == nullptr || outStatus == nullptr) {
        return BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT;
    }

    return sBCMCheckComboAvailable(options, outStatus);
}

ButtonComboModule_Error ButtonComboModule_DetectButtonCombo_Blocking(const ButtonComboModule_DetectButtonComboOptions *options,
                                                                     ButtonComboModule_Buttons *outButtonCombo) {
    if (sButtonComboModuleVersion == BUTTON_COMBO_MODULE_API_VERSION_ERROR) {
        return BUTTON_COMBO_MODULE_ERROR_LIB_UNINITIALIZED;
    }
    if (sBCMDetectButtonComboBlocking == nullptr || sButtonComboModuleVersion < 1) {
        return BUTTON_COMBO_MODULE_ERROR_UNSUPPORTED_COMMAND;
    }

    if (options == nullptr || outButtonCombo == nullptr) {
        return BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT;
    }

    return sBCMDetectButtonComboBlocking(options, outButtonCombo);
}