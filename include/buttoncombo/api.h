/**
 * @file api.h
 * @brief Main entry point for the libbuttoncombo library.
 *
 * This file contains the C and C++ APIs for interacting with the ButtonComboModule.
 */

#pragma once

#include "defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Returns a string representation of the provided ButtonComboModule_Error.
 *
 * @param status The status code to convert.
 * @return A pointer to a string literal describing the provided status.
 */
const char *ButtonComboModule_GetStatusStr(ButtonComboModule_Error status);

/**
 * @brief Returns a string representation of the provided ButtonComboModule_ControllerTypes.
 *
 * @param controller The controller mask to convert.
 * @return A pointer to a string literal describing the provided controller type.
 */
const char *ButtonComboModule_GetControllerTypeStr(ButtonComboModule_ControllerTypes controller);

/**
 * @brief Returns a string representation of the provided ButtonComboModule_ComboStatus.
 *
 * @param status The combo status to convert.
 * @return A pointer to a string literal describing the provided combo status.
 */
const char *ButtonComboModule_GetComboStatusStr(ButtonComboModule_ComboStatus status);


/**
 * @brief Initializes the ButtonComboModule library.
 *
 * This function must be called before any other function in this library (except ButtonComboModule_GetVersion).
 * It locates the WUMS module and resolves the function pointers.
 *
 * @retval BUTTON_COMBO_MODULE_ERROR_SUCCESS                 The library was initialized successfully.
 * @retval BUTTON_COMBO_MODULE_ERROR_MODULE_NOT_FOUND        The ButtonComboModule.wms could not be found. Ensure it is running.
 * @retval BUTTON_COMBO_MODULE_ERROR_MODULE_MISSING_EXPORT   The module is running but is missing expected exports.
 * @retval BUTTON_COMBO_MODULE_ERROR_UNSUPPORTED_API_VERSION The loaded module version is incompatible with this client library.
 */
ButtonComboModule_Error ButtonComboModule_InitLibrary();

/**
 * @brief Deinitializes the ButtonComboModule library.
 *
 * @retval BUTTON_COMBO_MODULE_ERROR_SUCCESS       Deinitialization was successful.
 * @retval BUTTON_COMBO_MODULE_ERROR_UNKNOWN_ERROR Deinitialization failed.
 */
ButtonComboModule_Error ButtonComboModule_DeInitLibrary();

/**
 * @brief Retrieves the API Version of the loaded ButtonComboModule.
 *
 * **Requires ButtonComboModule API version 1 or higher.**
 *
 * @param[out] outVersion Pointer to storage for the version number. Must not be NULL.
 *
 * @retval BUTTON_COMBO_MODULE_ERROR_SUCCESS                 The version was successfully stored in outVersion.
 * @retval BUTTON_COMBO_MODULE_ERROR_MODULE_NOT_FOUND        The module could not be found.
 * @retval BUTTON_COMBO_MODULE_ERROR_MODULE_MISSING_EXPORT   The module is missing the version export.
 * @retval BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT        outVersion is NULL.
 * @retval BUTTON_COMBO_MODULE_ERROR_UNKNOWN_ERROR           An internal error occurred.
 */
ButtonComboModule_Error ButtonComboModule_GetVersion(ButtonComboModule_APIVersion *outVersion);

/**
 * @brief Registers a new button combo with the ButtonComboModule.
 *
 * **Requires ButtonComboModule API version 1 or higher**
 *
 * This function registers a new combo based on the provided generic `ButtonComboModule_ComboOptions`.
 * Depending on the options, the combo can detect simple presses or hold durations on specific controllers.
 *
 * @section Conflict Handling
 * The behavior regarding conflicts depends on the `type` set in the options:
 *
 * - **Non-Observer (Standard):**
 * The module validates if the new combo overlaps with any existing registered combos (e.g. registering "L+R" when "X+L+R" exists).
 * - If a conflict is detected, the new combo is registered but `outStatus` is set to @ref BUTTON_COMBO_MODULE_COMBO_STATUS_CONFLICT.
 * - In this state, the **callback will not trigger** even if the buttons are pressed.
 * - The existing combo that caused the conflict remains valid and unaffected.
 *
 * - **Observer:**
 * The module does **not** check for conflicts. The callback will trigger regardless of other existing combos.
 *
 * @par Conflict example (only relevant if combo type is not an observer):
 * It's not possible to add any new valid button combo containing "L+R" (e.g. "X+L+R"), if there already is a button
 * combination "L+R". Furthermore, it's also not possible to add a "L" or "R" combo if there already is a button
 * combination "L+R".
 *
 * @section Resolving Conflicts
 * If a combo enters the @ref BUTTON_COMBO_MODULE_COMBO_STATUS_CONFLICT state, it remains inactive until manually updated.
 *
 * To resolve this (even after the conflicting combo is removed), you **must** manually update the combo using:
 * - @ref ButtonComboModule_UpdateControllerMask or
 * - @ref ButtonComboModule_UpdateButtonCombo
 *
 * **Note:** The status does *not* automatically update to `VALID` just because the conflicting combo was removed.
 * You must trigger one of the update functions above to force a re-evaluation.
 *
 * @sa ButtonComboModule_RemoveButtonCombo to remove an added button combo.
 *
 * @param[in]  options   Configuration for the new combo. Must not be NULL.
 * @param[out] outHandle Storage for the new combo's handle. Must not be NULL.
 * @param[out] outStatus (Optional) Storage for the initial status. If provided, it will be set to
 * @ref BUTTON_COMBO_MODULE_COMBO_STATUS_VALID if active, or
 * @ref BUTTON_COMBO_MODULE_COMBO_STATUS_CONFLICT if inactive due to overlap.
 *
 * @retval BUTTON_COMBO_MODULE_ERROR_SUCCESS                       Combo successfully created. Check outStatus for validity.
 * @retval BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT              options or outHandle is NULL, or the callback in options is NULL.
 * @retval BUTTON_COMBO_MODULE_ERROR_LIB_UNINITIALIZED             The library is not initialized.
 * @retval BUTTON_COMBO_MODULE_ERROR_INCOMPATIBLE_OPTIONS_VERSION  The options struct version is incorrect.
 * @retval BUTTON_COMBO_MODULE_ERROR_INVALID_COMBO                 The button combination or controller mask in options is empty/0.
 * @retval BUTTON_COMBO_MODULE_ERROR_DURATION_MISSING              The type is HOLD but holdDuration is 0.
 * @retval BUTTON_COMBO_MODULE_ERROR_INVALID_COMBO_TYPE            The combo type in options is unknown.
 * @retval BUTTON_COMBO_MODULE_ERROR_UNKNOWN_ERROR                 The module is in an invalid state.
 */
ButtonComboModule_Error ButtonComboModule_AddButtonCombo(const ButtonComboModule_ComboOptions *options,
                                                         ButtonComboModule_ComboHandle *outHandle,
                                                         ButtonComboModule_ComboStatus *outStatus);

/**
 * @brief Helper to create a "PressDown" combo with extended options.
 *
 * **Requires ButtonComboModule API version 1 or higher.**
 *
 * A "PressDown" combo triggers immediately when the buttons are pressed.
 *
 * @sa ButtonComboModule_AddButtonCombo for detailed information about button combos.
 *
 * @param[in]  label          A string label for debugging.
 * @param[in]  controllerMask Bitmask of controllers to listen to. Must not be empty.
 * @param[in]  combo          Bitmask of buttons to detect. Must not be empty.
 * @param[in]  callback       Function to call when detected. Must not be NULL.
 * @param[in]  context        User data passed to the callback. Can be NULL.
 * @param[in]  observer       If true, ignores conflicts with other combos.
 * @param[out] outHandle      Storage for the new handle. Must not be NULL.
 * @param[out] outStatus      (Optional) Storage for the initial status.
 *
 * @retval BUTTON_COMBO_MODULE_ERROR_SUCCESS           Combo successfully created.
 * @retval BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT  Pointers are NULL or masks/combos are empty.
 * @retval BUTTON_COMBO_MODULE_ERROR_LIB_UNINITIALIZED The library is not initialized.
 * @retval BUTTON_COMBO_MODULE_ERROR_INVALID_COMBO     The combo or controller mask is 0.
 * @retval BUTTON_COMBO_MODULE_ERROR_UNKNOWN_ERROR     Internal module error.
 */
ButtonComboModule_Error ButtonComboModule_AddButtonComboPressDownEx(const char *label,
                                                                    ButtonComboModule_ControllerTypes controllerMask,
                                                                    ButtonComboModule_Buttons combo,
                                                                    ButtonComboModule_ComboCallback callback,
                                                                    void *context,
                                                                    bool observer,
                                                                    ButtonComboModule_ComboHandle *outHandle,
                                                                    ButtonComboModule_ComboStatus *outStatus);
/**
 * @brief Helper to create a "PressDown" combo on ALL controllers (Conflict Checked).
 *
 * **Requires ButtonComboModule API version 1 or higher.**
 *
 * Equivalent to calling `ButtonComboModule_AddButtonComboPressDownEx` with:
 * - `controllerMask` = `BUTTON_COMBO_MODULE_CONTROLLER_ALL`
 * - `observer` = `false`
 *
 * @see ButtonComboModule_AddButtonComboPressDownEx
 */
ButtonComboModule_Error ButtonComboModule_AddButtonComboPressDown(const char *label,
                                                                  ButtonComboModule_Buttons combo,
                                                                  ButtonComboModule_ComboCallback callback,
                                                                  void *context,
                                                                  ButtonComboModule_ComboHandle *outHandle,
                                                                  ButtonComboModule_ComboStatus *outStatus);

/**
  * @brief Helper to create a "PressDown" combo on ALL controllers (Observer).
  *
  * **Requires ButtonComboModule API version 1 or higher.**
  *
  * Equivalent to calling `ButtonComboModule_AddButtonComboPressDownEx` with:
  * - `controllerMask` = `BUTTON_COMBO_MODULE_CONTROLLER_ALL`
  * - `observer` = `true`
  *
  * @see ButtonComboModule_AddButtonComboPressDownEx
  */
ButtonComboModule_Error ButtonComboModule_AddButtonComboPressDownObserver(const char *label,
                                                                          ButtonComboModule_Buttons combo,
                                                                          ButtonComboModule_ComboCallback callback,
                                                                          void *context,
                                                                          ButtonComboModule_ComboHandle *outHandle,
                                                                          ButtonComboModule_ComboStatus *outStatus);


/**
  * @brief Helper to create a "Hold" combo with extended options.
  *
  * **Requires ButtonComboModule API version 1 or higher.**
  *
  * A "Hold" combo triggers only after the buttons have been held for `holdDurationInMs`.
  *
  * @sa ButtonComboModule_AddButtonCombo for detailed information about button combos.
  *
  * @param[in]  label             A string label for debugging.
  * @param[in]  controllerMask    Bitmask of controllers to listen to. Must not be empty.
  * @param[in]  combo             Bitmask of buttons to detect. Must not be empty.
  * @param[in]  holdDurationInMs  Duration in milliseconds the buttons must be held. Must be > 0.
  * @param[in]  callback          Function to call when detected. Must not be NULL.
  * @param[in]  context           User data passed to the callback. Can be NULL.
  * @param[in]  observer          If true, ignores conflicts with other combos.
  * @param[out] outHandle         Storage for the new handle. Must not be NULL.
  * @param[out] outStatus         (Optional) Storage for the initial status.
  *
  * @retval BUTTON_COMBO_MODULE_ERROR_SUCCESS             Combo successfully created.
  * @retval BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT    Pointers are NULL.
  * @retval BUTTON_COMBO_MODULE_ERROR_LIB_UNINITIALIZED   The library is not initialized.
  * @retval BUTTON_COMBO_MODULE_ERROR_INVALID_COMBO       The combo or controller mask is 0.
  * @retval BUTTON_COMBO_MODULE_ERROR_DURATION_MISSING    holdDurationInMs is 0.
  * @retval BUTTON_COMBO_MODULE_ERROR_UNKNOWN_ERROR       Internal module error.
  */
ButtonComboModule_Error ButtonComboModule_AddButtonComboHoldEx(const char *label,
                                                               ButtonComboModule_ControllerTypes controllerMask,
                                                               ButtonComboModule_Buttons combo,
                                                               uint32_t holdDurationInMs,
                                                               ButtonComboModule_ComboCallback callback,
                                                               void *context,
                                                               bool observer,
                                                               ButtonComboModule_ComboHandle *outHandle,
                                                               ButtonComboModule_ComboStatus *outStatus);

/**
 * @brief Helper to create a "Hold" combo on ALL controllers (Conflict Checked).
 *
 * **Requires ButtonComboModule API version 1 or higher.**
 *
 * Equivalent to calling `ButtonComboModule_AddButtonComboHoldEx` with:
 * - `controllerMask` = `BUTTON_COMBO_MODULE_CONTROLLER_ALL`
 * - `observer` = `false`
 *
 * @see ButtonComboModule_AddButtonComboHoldEx
 */
ButtonComboModule_Error ButtonComboModule_AddButtonComboHold(const char *label,
                                                             ButtonComboModule_Buttons combo,
                                                             uint32_t holdDurationInMs,
                                                             ButtonComboModule_ComboCallback callback,
                                                             void *context,
                                                             ButtonComboModule_ComboHandle *outHandle,
                                                             ButtonComboModule_ComboStatus *outStatus);

/**
 * @brief Helper to create a "Hold" combo on ALL controllers (Observer).
 *
 * **Requires ButtonComboModule API version 1 or higher.**
 *
 * Equivalent to calling `ButtonComboModule_AddButtonComboHoldEx` with:
 * - `controllerMask` = `BUTTON_COMBO_MODULE_CONTROLLER_ALL`
 * - `observer` = `true`
 *
 * @see ButtonComboModule_AddButtonComboHoldEx
 */
ButtonComboModule_Error ButtonComboModule_AddButtonComboHoldObserver(const char *label,
                                                                     ButtonComboModule_Buttons combo,
                                                                     uint32_t holdDurationInMs,
                                                                     ButtonComboModule_ComboCallback callback,
                                                                     void *context,
                                                                     ButtonComboModule_ComboHandle *outHandle,
                                                                     ButtonComboModule_ComboStatus *outStatus);

/**
* @brief Removes a previously registered button combo.
*
* **Requires ButtonComboModule API version 1 or higher.**
*
* This function allows removing a combo using its handle. If the handle is not found,
* the function still returns SUCCESS (idempotent).
*
* @param[in] handle The handle of the combo to remove. Must not be NULL.
*
* @retval BUTTON_COMBO_MODULE_ERROR_SUCCESS            The combo was removed or was not found.
* @retval BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT   The handle was NULL.
* @retval BUTTON_COMBO_MODULE_ERROR_LIB_UNINITIALIZED  The library is not initialized.
* @retval BUTTON_COMBO_MODULE_ERROR_UNKNOWN_ERROR      Internal module error.
*/
ButtonComboModule_Error ButtonComboModule_RemoveButtonCombo(ButtonComboModule_ComboHandle handle);

/**
 * @brief Retrieves the current status of a combo.
 *
 * **Requires ButtonComboModule API version 1 or higher.**
 *
 * @param[in]  handle    The handle of the combo. Must not be NULL.
 * @param[out] outStatus Storage for the status. Must not be NULL.
 *
 * @retval BUTTON_COMBO_MODULE_ERROR_SUCCESS            Status retrieved successfully.
 * @retval BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT   handle or outStatus is NULL, or **handle not found**.
 * @retval BUTTON_COMBO_MODULE_ERROR_LIB_UNINITIALIZED  The library is not initialized.
 * @retval BUTTON_COMBO_MODULE_ERROR_UNKNOWN_ERROR      Internal module error.
 */
ButtonComboModule_Error ButtonComboModule_GetButtonComboStatus(ButtonComboModule_ComboHandle handle,
                                                               ButtonComboModule_ComboStatus *outStatus);

/**
* @brief Updates the metadata (label) for a specific combo.
*
* **Requires ButtonComboModule API version 1 or higher.**
*
* @param[in] handle      The handle of the combo. Must not be NULL.
* @param[in] metaOptions The new metadata. Must not be NULL.
*
* @retval BUTTON_COMBO_MODULE_ERROR_SUCCESS            Metadata updated.
* @retval BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT   handle or metaOptions is NULL, or **handle not found**.
* @retval BUTTON_COMBO_MODULE_ERROR_LIB_UNINITIALIZED  The library is not initialized.
* @retval BUTTON_COMBO_MODULE_ERROR_UNKNOWN_ERROR      Internal module error.
*/
ButtonComboModule_Error ButtonComboModule_UpdateButtonComboMeta(ButtonComboModule_ComboHandle handle,
                                                                const ButtonComboModule_MetaOptions *metaOptions);

/**
* @brief Updates the callback function and context for a combo.
*
* **Requires ButtonComboModule API version 1 or higher.**
*
* @param[in] handle          The handle of the combo. Must not be NULL.
* @param[in] callbackOptions The new callback and context. Must not be NULL.
*
* @retval BUTTON_COMBO_MODULE_ERROR_SUCCESS            Callback options updated.
* @retval BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT   handle or callbackOptions is NULL, or **handle not found**.
* @retval BUTTON_COMBO_MODULE_ERROR_LIB_UNINITIALIZED  The library is not initialized.
* @retval BUTTON_COMBO_MODULE_ERROR_UNKNOWN_ERROR      Internal module error.
*/
ButtonComboModule_Error ButtonComboModule_UpdateButtonComboCallback(ButtonComboModule_ComboHandle handle,
                                                                    const ButtonComboModule_CallbackOptions *callbackOptions);

/**
* @brief Updates the controller mask for a combo.
*
* **Requires ButtonComboModule API version 1 or higher.**
*
* Changing the mask triggers a re-check for conflicts. `outStatus` will reflect
* whether the combo is now valid or conflicting.
*
* @param[in]  handle         The handle of the combo. Must not be NULL.
* @param[in]  controllerMask The new controller mask. Must not be empty.
* @param[out] outStatus      (Optional) Storage for the new status.
*
* @retval BUTTON_COMBO_MODULE_ERROR_SUCCESS            Mask updated (check outStatus for Validity).
* @retval BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT   handle is NULL or **handle not found**.
* @retval BUTTON_COMBO_MODULE_ERROR_LIB_UNINITIALIZED  The library is not initialized.
* @retval BUTTON_COMBO_MODULE_ERROR_UNKNOWN_ERROR      Internal module error.
*/
ButtonComboModule_Error ButtonComboModule_UpdateControllerMask(ButtonComboModule_ComboHandle handle,
                                                               ButtonComboModule_ControllerTypes controllerMask,
                                                               ButtonComboModule_ComboStatus *outStatus);

/**
* @brief Updates the button combination for a combo.
*
* **Requires ButtonComboModule API version 1 or higher.**
*
* Changing the buttons triggers a re-check for conflicts.
*
* @param[in]  handle    The handle of the combo. Must not be NULL.
* @param[in]  combo     The new button bitmask. Must not be empty.
* @param[out] outStatus (Optional) Storage for the new status.
*
* @retval BUTTON_COMBO_MODULE_ERROR_SUCCESS            Buttons updated (check outStatus for Validity).
* @retval BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT   handle is NULL or **handle not found**.
* @retval BUTTON_COMBO_MODULE_ERROR_LIB_UNINITIALIZED  The library is not initialized.
* @retval BUTTON_COMBO_MODULE_ERROR_UNKNOWN_ERROR      Internal module error.
*/
ButtonComboModule_Error ButtonComboModule_UpdateButtonCombo(ButtonComboModule_ComboHandle handle,
                                                            ButtonComboModule_Buttons combo,
                                                            ButtonComboModule_ComboStatus *outStatus);

/**
* @brief Updates the hold duration for a "Hold" combo.
*
* **Requires ButtonComboModule API version 1 or higher.**
*
* This is only valid for combos created as `HOLD` or `HOLD_OBSERVER`.
*
* @param[in] handle           The handle of the combo. Must not be NULL.
* @param[in] holdDurationInMs The new duration in milliseconds. Must be > 0.
*
* @retval BUTTON_COMBO_MODULE_ERROR_SUCCESS            Duration updated.
* @retval BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT   handle is NULL or **handle not found**.
* @retval BUTTON_COMBO_MODULE_ERROR_LIB_UNINITIALIZED  The library is not initialized.
* @retval BUTTON_COMBO_MODULE_ERROR_UNKNOWN_ERROR      Internal module error.
*/
ButtonComboModule_Error ButtonComboModule_UpdateHoldDuration(ButtonComboModule_ComboHandle handle,
                                                             uint32_t holdDurationInMs);

/**
* @brief Retrieves the metadata (label) for a specific combo.
*
* @param[in]  handle     The handle of the combo. Must not be NULL.
* @param[out] outOptions Storage for the metadata. Must not be NULL.
*
* @retval BUTTON_COMBO_MODULE_ERROR_SUCCESS            Metadata retrieved.
* @retval BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT   handle/outOptions is NULL or **handle not found**.
* @retval BUTTON_COMBO_MODULE_ERROR_LIB_UNINITIALIZED  The library is not initialized.
* @retval BUTTON_COMBO_MODULE_ERROR_UNKNOWN_ERROR      Internal module error.
*/
ButtonComboModule_Error ButtonComboModule_GetButtonComboMeta(ButtonComboModule_ComboHandle handle,
                                                             ButtonComboModule_MetaOptionsOut *outOptions);

/**
* @brief Retrieves the callback options for a specific combo.
*
* **Requires ButtonComboModule API version 1 or higher.**
*
* @param[in]  handle     The handle of the combo. Must not be NULL.
* @param[out] outOptions Storage for the callback options. Must not be NULL.
*
* @retval BUTTON_COMBO_MODULE_ERROR_SUCCESS            Callback options retrieved.
* @retval BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT   handle/outOptions is NULL or **handle not found**.
* @retval BUTTON_COMBO_MODULE_ERROR_LIB_UNINITIALIZED  The library is not initialized.
* @retval BUTTON_COMBO_MODULE_ERROR_UNKNOWN_ERROR      Internal module error.
*/
ButtonComboModule_Error ButtonComboModule_GetButtonComboCallback(ButtonComboModule_ComboHandle handle,
                                                                 ButtonComboModule_CallbackOptions *outOptions);

/**
* @brief Retrieves detailed info (type, mask, buttons, duration) for a specific combo.
*
* **Requires ButtonComboModule API version 1 or higher.**
*
* @param[in]  handle     The handle of the combo. Must not be NULL.
* @param[out] outOptions Storage for the detailed info. Must not be NULL.
*
* @retval BUTTON_COMBO_MODULE_ERROR_SUCCESS            Info retrieved.
* @retval BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT   handle/outOptions is NULL or **handle not found**.
* @retval BUTTON_COMBO_MODULE_ERROR_LIB_UNINITIALIZED  The library is not initialized.
* @retval BUTTON_COMBO_MODULE_ERROR_UNKNOWN_ERROR      Internal module error.
*/
ButtonComboModule_Error ButtonComboModule_GetButtonComboInfoEx(ButtonComboModule_ComboHandle handle,
                                                               ButtonComboModule_ButtonComboInfoEx *outOptions);

/**
* @brief Checks if a proposed combo would cause a conflict.
*
* **Requires ButtonComboModule API version 1 or higher.**
*
* This does not register the combo, it only checks if `outStatus` would be VALID or CONFLICT.
*
* @param[in]  options   The proposed combo options (mask and buttons). Must not be NULL.
* @param[out] outStatus Storage for the resulting status. Must not be NULL.
*
* @retval BUTTON_COMBO_MODULE_ERROR_SUCCESS            Check completed successfully.
* @retval BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT   options or outStatus is NULL.
* @retval BUTTON_COMBO_MODULE_ERROR_LIB_UNINITIALIZED  The library is not initialized.
* @retval BUTTON_COMBO_MODULE_ERROR_UNKNOWN_ERROR      Internal module error.
*/
ButtonComboModule_Error ButtonComboModule_CheckComboAvailable(const ButtonComboModule_ButtonComboOptions *options,
                                                              ButtonComboModule_ComboStatus *outStatus);

/**
* @brief Blocks execution until a specific combo (or abort combo) is detected.
*
* **Requires ButtonComboModule API version 1 or higher.**
*
* @warning **This function blocks the calling thread** until the combo is detected or aborted.
* Do not call this from the main UI thread (e.g., inside a ProcUI loop) or the application will freeze.
*
* @param[in]  options    Configuration for detection (mask, durations, abort buttons). Must not be NULL.
* @param[out] outButtons Storage for the detected buttons. Must not be NULL.
*
* @note The abort button combo is checked on all controllers, even those not included in the `controllerMask`.
*
* @retval BUTTON_COMBO_MODULE_ERROR_SUCCESS            The target combo was detected.
* @retval BUTTON_COMBO_MODULE_ERROR_ABORTED            The abort combo was pressed.
* @retval BUTTON_COMBO_MODULE_ERROR_INVALID_ARGUMENT   options/outButtons is NULL, or options contain invalid values (e.g. 0 duration, empty mask).
* @retval BUTTON_COMBO_MODULE_ERROR_LIB_UNINITIALIZED  The library is not initialized.
* @retval BUTTON_COMBO_MODULE_ERROR_UNKNOWN_ERROR      Internal module error.
*/
ButtonComboModule_Error ButtonComboModule_DetectButtonCombo_Blocking(const ButtonComboModule_DetectButtonComboOptions *options,
                                                                     ButtonComboModule_Buttons *outButtons);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

#include <buttoncombo/ButtonCombo.h>
#include <optional>
#include <stdexcept>
#include <string_view>

/**
 * @namespace ButtonComboModule
 * @brief C++ RAII wrapper API.
 *
 * This namespace provides a safer, object-oriented interface for the ButtonComboModule.
 * Most functions here are wrappers around the C API, returning `std::optional` or throwing exceptions
 * instead of returning raw error codes.
 */
namespace ButtonComboModule {
    /**
     * @brief Wrapper for @ref ButtonComboModule_GetStatusStr.
     */
    const char *GetStatusStr(ButtonComboModule_Error status);

    /**
     * @brief Wrapper for @ref ButtonComboModule_GetControllerTypeStr.
     */
    const char *GetControllerTypeStr(ButtonComboModule_ControllerTypes controller);

    /**
     * @brief Wrapper for @ref ButtonComboModule_GetComboStatusStr.
     */
    const char *GetComboStatusStr(ButtonComboModule_ComboStatus status);

    /**
     * @brief Creates a button combo (Generic).
     *
     * @details This is the core C++ factory function. It validates the input and registers the combo.
     * Refer to @ref ButtonComboModule_AddButtonCombo for detailed behavior of specific options.
     *
     * @param options    Configuration options (see @ref ButtonComboModule_ComboOptions).
     * @param[out] outStatus  Resulting status (VALID or CONFLICT).
     * @param[out] outError   Resulting error code.
     * @return A `ButtonCombo` object on success, or `std::nullopt` on failure.
     * @sa ButtonComboModule_AddButtonCombo
     */
    std::optional<ButtonCombo> CreateComboEx(const ButtonComboModule_ComboOptions &options,
                                             ButtonComboModule_ComboStatus &outStatus,
                                             ButtonComboModule_Error &outError) noexcept;

    /**
     * @brief Creates a "Press Down" combo.
     *
     * @details Triggers immediately when buttons are pressed.
     * Refer to @ref ButtonComboModule_AddButtonComboPressDownEx for parameter details.
     *
     * @param label          Debug label.
     * @param controllerMask Controllers to listen to.
     * @param combo          Button bitmask.
     * @param callback       Function to call on trigger.
     * @param context        User data for callback.
     * @param observer       If true, ignore conflicts.
     * @param[out] outStatus Resulting status.
     * @param[out] outError  Resulting error code.
     * @return A `ButtonCombo` object on success, or `std::nullopt` on failure.
     * @sa ButtonComboModule_AddButtonComboPressDownEx
     */
    std::optional<ButtonCombo> CreateComboPressDownEx(std::string_view label,
                                                      ButtonComboModule_ControllerTypes controllerMask,
                                                      ButtonComboModule_Buttons combo,
                                                      ButtonComboModule_ComboCallback callback,
                                                      void *context,
                                                      bool observer,
                                                      ButtonComboModule_ComboStatus &outStatus,
                                                      ButtonComboModule_Error &outError) noexcept;

    /**
     * @brief Creates a "Press Down" combo on ALL controllers (Conflict Checked).
     *
     * Wrapper for @ref CreateComboPressDownEx with `observer=false` and `mask=ALL`.
     * @sa ButtonComboModule_AddButtonComboPressDown
     */
    std::optional<ButtonCombo> CreateComboPressDown(std::string_view label,
                                                    ButtonComboModule_Buttons combo,
                                                    ButtonComboModule_ComboCallback callback,
                                                    void *context,
                                                    ButtonComboModule_ComboStatus &outStatus,
                                                    ButtonComboModule_Error &outError) noexcept;

    /**
     * @brief Creates a "Press Down" combo on ALL controllers (Observer).
     *
     * Wrapper for @ref CreateComboPressDownEx with `observer=true` and `mask=ALL`.
     * @sa ButtonComboModule_AddButtonComboPressDownEx
     */
    std::optional<ButtonCombo> CreateComboPressDownObserver(std::string_view label,
                                                            ButtonComboModule_Buttons combo,
                                                            ButtonComboModule_ComboCallback callback,
                                                            void *context,
                                                            ButtonComboModule_ComboStatus &outStatus,
                                                            ButtonComboModule_Error &outError) noexcept;

    /**
     * @brief Creates a "Hold" combo.
     *
     * @details Triggers after buttons are held for a duration.
     * Refer to @ref ButtonComboModule_AddButtonComboHoldEx for parameter details.
     *
     * @param label             Debug label.
     * @param controllerMask    Controllers to listen to.
     * @param combo             Button bitmask.
     * @param holdDurationInMs  Hold time in ms.
     * @param callback          Function to call.
     * @param context           User data.
     * @param observer          If true, ignore conflicts.
     * @param[out] outStatus    Resulting status.
     * @param[out] outError     Resulting error code.
     * @return A `ButtonCombo` object on success, or `std::nullopt` on failure.
     * @sa ButtonComboModule_AddButtonComboHoldEx
     */
    std::optional<ButtonCombo> CreateComboHoldEx(std::string_view label,
                                                 ButtonComboModule_ControllerTypes controllerMask,
                                                 ButtonComboModule_Buttons combo,
                                                 uint32_t holdDurationInMs,
                                                 ButtonComboModule_ComboCallback callback,
                                                 void *context,
                                                 bool observer,
                                                 ButtonComboModule_ComboStatus &outStatus,
                                                 ButtonComboModule_Error &outError) noexcept;

    /**
     * @brief Creates a "Hold" combo on ALL controllers (Conflict Checked).
     *
     * Wrapper for @ref CreateComboHoldEx with `observer=false` and `mask=ALL`.
     * @sa ButtonComboModule_AddButtonComboHold
     */
    std::optional<ButtonCombo> CreateComboHold(std::string_view label,
                                               ButtonComboModule_Buttons combo,
                                               uint32_t holdDurationInMs,
                                               ButtonComboModule_ComboCallback callback,
                                               void *context,
                                               ButtonComboModule_ComboStatus &outStatus,
                                               ButtonComboModule_Error &outError) noexcept;

    /**
     * @brief Creates a "Hold" combo on ALL controllers (Observer).
     *
     * Wrapper for @ref CreateComboHoldEx with `observer=true` and `mask=ALL`.
     * @sa ButtonComboModule_AddButtonComboHoldEx
     */
    std::optional<ButtonCombo> CreateComboHoldObserver(std::string_view label,
                                                       ButtonComboModule_Buttons combo,
                                                       uint32_t holdDurationInMs,
                                                       ButtonComboModule_ComboCallback callback,
                                                       void *context,
                                                       ButtonComboModule_ComboStatus &outStatus,
                                                       ButtonComboModule_Error &outError) noexcept;

    /**
     * @brief Creates a "Press Down" combo (Throwing).
     *
     * @details Same as @ref CreateComboPressDownEx but throws on error.
     * @throws std::runtime_error if creation fails (e.g. invalid arguments or uninitialized lib).
     */
    ButtonCombo CreateComboPressDownEx(std::string_view label,
                                       ButtonComboModule_ControllerTypes controllerMask,
                                       ButtonComboModule_Buttons combo,
                                       ButtonComboModule_ComboCallback callback,
                                       void *context,
                                       bool observer,
                                       ButtonComboModule_ComboStatus &outStatus);

    /**
     * @brief Creates a "Press Down" combo on ALL controllers (Throwing).
     * @sa CreateComboPressDownEx
     */
    ButtonCombo CreateComboPressDown(std::string_view label,
                                     ButtonComboModule_Buttons combo,
                                     ButtonComboModule_ComboCallback callback,
                                     void *context,
                                     ButtonComboModule_ComboStatus &outStatus);

    /**
     * @brief Creates a "Press Down" Observer combo on ALL controllers (Throwing).
     * @sa CreateComboPressDownObserver
     */
    ButtonCombo CreateComboPressDownObserver(std::string_view label,
                                             ButtonComboModule_Buttons combo,
                                             ButtonComboModule_ComboCallback callback,
                                             void *context,
                                             ButtonComboModule_ComboStatus &outStatus);

    /**
     * @brief Creates a "Hold" combo (Throwing).
     *
     * @details Same as @ref CreateComboHoldEx but throws on error.
     * @throws std::runtime_error if creation fails.
     */
    ButtonCombo CreateComboHoldEx(std::string_view label,
                                  ButtonComboModule_ControllerTypes controllerMask,
                                  ButtonComboModule_Buttons combo,
                                  uint32_t holdDurationInMs,
                                  ButtonComboModule_ComboCallback callback,
                                  void *context,
                                  bool observer,
                                  ButtonComboModule_ComboStatus &outStatus);

    /**
     * @brief Creates a "Hold" combo on ALL controllers (Throwing).
     * @sa CreateComboHold
     */
    ButtonCombo CreateComboHold(std::string_view label,
                                ButtonComboModule_Buttons combo,
                                uint32_t holdDurationInMs,
                                ButtonComboModule_ComboCallback callback,
                                void *context,
                                ButtonComboModule_ComboStatus &outStatus);

    /**
     * @brief Creates a "Hold" Observer combo on ALL controllers (Throwing).
     * @sa CreateComboHoldObserver
     */
    ButtonCombo CreateComboHoldObserver(std::string_view label,
                                        ButtonComboModule_Buttons combo,
                                        uint32_t holdDurationInMs,
                                        ButtonComboModule_ComboCallback callback,
                                        void *context,
                                        ButtonComboModule_ComboStatus &outStatus);

    /**
     * @brief Checks if a combo is available.
     *
     * Wrapper for @ref ButtonComboModule_CheckComboAvailable.
     * @sa ButtonComboModule_CheckComboAvailable
     */
    ButtonComboModule_Error CheckComboAvailable(const ButtonComboModule_ButtonComboOptions &options,
                                                ButtonComboModule_ComboStatus &outStatus);

    /**
     * @brief Blocks execution until a combo is detected.
     *
     * Wrapper for @ref ButtonComboModule_DetectButtonCombo_Blocking.
     * @sa ButtonComboModule_DetectButtonCombo_Blocking
     */
    ButtonComboModule_Error DetectButtonCombo_Blocking(const ButtonComboModule_DetectButtonComboOptions &options,
                                                       ButtonComboModule_Buttons &outButtons);
} // namespace ButtonComboModule
#endif