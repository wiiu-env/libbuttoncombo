#pragma once

#include "defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Returns a ButtonComboModule_Status as a string
 * @param status
 * @return String representation of a given status
**/
const char *ButtonComboModule_GetStatusStr(ButtonComboModule_Error status);

/**
 * This function has to be called before any other function of this lib (except ButtonComboModule_GetVersion) can be used.
 *
 * @return  BUTTON_COMBO_MODULE_ERROR_SUCCESS:                 The library has been initialized successfully. Other functions can now be used.<br>
 *          BUTTON_COMBO_MODULE_ERROR_MODULE_NOT_FOUND:        The module could not be found. Make sure the module is loaded.<br>
 *          BUTTON_COMBO_MODULE_ERROR_MODULE_MISSING_EXPORT:   The module is missing an expected export.<br>
 *          BUTTON_COMBO_MODULE_ERROR_UNSUPPORTED_API_VERSION: The version of the loaded module is not compatible with this version of the lib.
**/
ButtonComboModule_Error ButtonComboModule_InitLibrary();

/**
 * Deinitializes the ButtonComboModule lib
 * @return BUTTON_COMBO_MODULE_ERROR_SUCCESS or BUTTON_COMBO_MODULE_ERROR_UNKNOWN_ERROR
 */
ButtonComboModule_Error ButtonComboModule_DeInitLibrary();

/**
 * Retrieves the API Version of the loaded ButtonComboModule. <br>
 * <br>
 * Requires ButtonComboModule API version 1 or higher
 * @param outVersion pointer to the variable where the version will be stored.
 *
 * @return BUTTON_COMBO_MODULE_ERROR_SUCCESS:               The API version has been store in the version ptr.<br>
 *         BUTTON_COMBO_MODULE_ERROR_MODULE_NOT_FOUND:      The module could not be found. Make sure the module is loaded.<br>
 *         BUTTON_COMBO_MODULE_ERROR_MODULE_MISSING_EXPORT: The module is missing an expected export.<br>
 *         BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT:      Invalid version pointer.<br>
 *         BUTTON_COMBO_MODULE_ERROR_UNKNOWN_ERROR:         Retrieving the module version failed.
**/
ButtonComboModule_Error ButtonComboModule_GetVersion(ButtonComboModule_APIVersion *outVersion);


ButtonComboModule_Error ButtonComboModule_AddButtonComboSimplePressDownEx(const char *label,
                                                                          ButtonComboModule_Buttons buttonCombo,
                                                                          ButtonComboModule_ControllerTypes controllerMask,
                                                                          ButtonComboModule_ComboCallback callback,
                                                                          void *context,
                                                                          ButtonComboModule_ComboHandle *handleOut);

ButtonComboModule_Error ButtonComboModule_AddButtonComboSimplePressDown(const char *label,
                                                                        ButtonComboModule_Buttons buttonCombo,
                                                                        ButtonComboModule_ComboCallback callback,
                                                                        void *context,
                                                                        ButtonComboModule_ComboHandle *handleOut);

ButtonComboModule_Error ButtonComboModule_AddButtonComboSimpleHold(const char *label,
                                                                   ButtonComboModule_Buttons buttonCombo,
                                                                   uint32_t holdDurationInMs,
                                                                   ButtonComboModule_ComboCallback callback,
                                                                   void *context,
                                                                   ButtonComboModule_ComboHandle *handleOut);

/**
 * Registers a button combo. <br>
 * <br>
 * Requires ButtonComboModule API version 1 or higher
 * @param options pointer which holds information about button combo that should be added
 * @param outHandle pointer where the handle of the added combo will be written to
 * @param outStatus pointer where the status of this combo will be written to
 *
 * @return BUTTON_COMBO_MODULE_ERROR_SUCCESS:                       The API version has been store in the version ptr.<br>
 *         BUTTON_COMBO_MODULE_ERROR_LIB_UNINITIALIZED:             Library was not initialized. Call ButtonComboModule_InitLibrary() before using this function.<br>
 *         BUTTON_COMBO_MODULE_ERROR_UNSUPPORTED_COMMAND:           Command not supported by the currently loaded ButtonComboModule version.<br>
 *         BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT:              Invalid input pointers or values.<br>
 *         BUTTON_COMBO_MODULE_ERROR_INCOMPATIBLE_OPTIONS_VERSION:  Invalid input pointers or values.<br>
 *         BUTTON_COMBO_MODULE_ERROR_UNKNOWN_ERROR:                 Adding the button combo failed.
**/
ButtonComboModule_Error ButtonComboModule_AddButtonCombo(const ButtonComboModule_ComboOptions *options,
                                                         ButtonComboModule_ComboHandle *outHandle,
                                                         ButtonComboModule_ComboStatus *outStatus);

ButtonComboModule_Error ButtonComboModule_RemoveButtonCombo(ButtonComboModule_ComboHandle handle);

ButtonComboModule_Error ButtonComboModule_GetButtonComboStatus(ButtonComboModule_ComboHandle handle,
                                                               ButtonComboModule_ComboStatus *outStatus);

ButtonComboModule_Error ButtonComboModule_UpdateButtonComboMeta(ButtonComboModule_ComboHandle handle,
                                                                const ButtonComboModule_MetaOptions *options);

ButtonComboModule_Error ButtonComboModule_UpdateButtonComboCallback(ButtonComboModule_ComboHandle handle,
                                                                    const ButtonComboModule_CallbackOptions *options);

ButtonComboModule_Error ButtonComboModule_UpdateControllerMask(ButtonComboModule_ComboHandle handle,
                                                               ButtonComboModule_ControllerTypes controllerMask,
                                                               ButtonComboModule_ComboStatus *outStatus);

ButtonComboModule_Error ButtonComboModule_UpdateButtonCombo(ButtonComboModule_ComboHandle handle,
                                                            ButtonComboModule_Buttons combo,
                                                            ButtonComboModule_ComboStatus *outStatus);

ButtonComboModule_Error ButtonComboModule_UpdateHoldDuration(ButtonComboModule_ComboHandle handle,
                                                             uint32_t holdDurationInMs);

ButtonComboModule_Error ButtonComboModule_GetButtonComboMeta(ButtonComboModule_ComboHandle handle,
                                                             ButtonComboModule_MetaOptionsOut *outOptions);

ButtonComboModule_Error ButtonComboModule_GetButtonComboCallback(ButtonComboModule_ComboHandle handle,
                                                                 ButtonComboModule_CallbackOptions *outOptions);

ButtonComboModule_Error ButtonComboModule_GetButtonComboInfoEx(ButtonComboModule_ComboHandle handle,
                                                               ButtonComboModule_ButtonComboInfoEx *outOptions);

ButtonComboModule_Error ButtonComboModule_CheckComboAvailable(const ButtonComboModule_ButtonComboOptions *options,
                                                              ButtonComboModule_ComboStatus *outStatus);


ButtonComboModule_Error ButtonComboModule_DetectButtonCombo_Blocking(const ButtonComboModule_DetectButtonComboOptions *options,
                                                                     ButtonComboModule_Buttons *outButtonCombo);

#ifdef __cplusplus
}
#endif
