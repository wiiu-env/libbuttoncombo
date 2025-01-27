#pragma once

#include "defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This function returns a string representation of the provided ButtonComboModule_Error.
 *
 * @param status The status to get the string representation for.
 * @return A pointer to a string describing the provided status.
**/
const char *ButtonComboModule_GetStatusStr(ButtonComboModule_Error status);

/**
 * This function returns a string representation of the provided ButtonComboModule_ControllerTypes.
 *
 * @param controller The controller to get the string representation for.
 * @return A pointer to a string describing the provided controller.
**/
const char *ButtonComboModule_GetControllerTypeStr(ButtonComboModule_ControllerTypes controller);

/**
 * This function returns a string representation of the provided ButtonComboModule_ComboStatus.
 *
 * @param status The combo status to get the string representation for.
 * @return A pointer to a string describing the provided combo status.
**/
const char *ButtonComboModule_GetComboStatusStr(ButtonComboModule_ComboStatus status);


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


/**
 * Creates a button combo which triggers a callback if this combo is detected.
 *
 * **Requires ButtonComboModule API version 1 or higher**
 *
 * This function takes a generic `ButtonComboModule_ComboOptions` which defines how the combo should be checked.
 *
 * Depending on the given type, the combo will either check for holding (for X ms) or pressing a button on for a given
 * controller mask. The type also defines if it's an "observer" or not. Observers won't check for combo conflicts.
 *
 * If the given type is not an `observer` and any other (valid) button combination overlaps with new combo, then the
 * `outStatus` will be set to `BUTTON_COMBO_MODULE_COMBO_STATUS_VALID` and the combo will be inactive. The original
 * ButtonCombo which had the combo first won't be affected at all.
 * In conflict state the callback will not be triggered even if the combination is pressed. If `observer` is set to
 * "true", the combo won't check for conflicts.
 *
 * To resolve a BUTTON_COMBO_MODULE_COMBO_STATUS_VALID combo state you **always** have to update combo information
 * via @ButtonComboModule_UpdateControllerMask or @ButtonComboModule_UpdateButtonCombo. The state won't update itself,
 * even it the combo has no conflicts in a later point in time (e.g. due to other conflicting combos being removed in the meantime)
 *
 * Conflict example (only relevant if combo type is not an observer):
 * It's not possible to add any new valid button combo containing "L+R" (e.g. "X+L+R"), if there already is a button
 * combination "L+R". Furthermore, it's also not possible to add a "L" or "R" combo if there already is a button
 * combination "L+R".
 *
 * See @ButtonComboModule_RemoveButtonCombo to remove an added button combo.
 *
 * @param options options of this button combo
 * @param outHandle The handle of the button combo will be stored here on success.  Must not be nullptr.
 * @param outStatus (optional) The status of the combo will be stored here. Only if the status is BUTTON_COMBO_MODULE_COMBO_STATUS_VALID the combo is valid. Can be NULL.
 * @return Returns BUTTON_COMBO_MODULE_ERROR_SUCCESS on success. Please check the outStatus as well.
*/
ButtonComboModule_Error ButtonComboModule_AddButtonCombo(const ButtonComboModule_ComboOptions *options,
                                                         ButtonComboModule_ComboHandle *outHandle,
                                                         ButtonComboModule_ComboStatus *outStatus);

/**
 * Creates a "PressDown" button combo which triggers a callback when given combo for a given controller has been pressed
 *
 * **Requires ButtonComboModule API version 1 or higher**
 *
 * See @ButtonComboModule_AddButtonCombo for detailed information about button combos.
 * See @ButtonComboModule_RemoveButtonCombo to remove the added button combo.
 *
 * @param label label of the button combo
 * @param controllerMask Mask of controllers which should be checked. Must not be empty.
 * @param combo Combo which should be checked. Must not be empty
 * @param callback Callback that will be called if a button combo is detected. Must not be nullptr.
 * @param context Context for the callback. Can be nullptr.
 * @param observer Defines if this combo should check for conflicts. Set it to "true" to be an observer and ignore conflicts.
 * @param outHandle The handle of the button combo will be stored here on success.  Must not be nullptr.
 * @param outStatus (optional) The status of the combo will be stored here. Only if the status is BUTTON_COMBO_MODULE_COMBO_STATUS_VALID the combo is valid. Can be NULL.
 * @return Returns BUTTON_COMBO_MODULE_ERROR_SUCCESS on success. Please check the outStatus as well.
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
 * Wrapper for `ButtonComboModule_AddButtonComboPressDownEx` with
 * - `observer` set to "false"
 * - `controllerMask` set to "BUTTON_COMBO_MODULE_CONTROLLER_ALL"
 *
 * **Requires ButtonComboModule API version 1 or higher**
 *
 * See: @ButtonComboModule_AddButtonComboPressDownEx for more information.
*/
ButtonComboModule_Error ButtonComboModule_AddButtonComboPressDown(const char *label,
                                                                  ButtonComboModule_Buttons combo,
                                                                  ButtonComboModule_ComboCallback callback,
                                                                  void *context,
                                                                  ButtonComboModule_ComboHandle *outHandle,
                                                                  ButtonComboModule_ComboStatus *outStatus);

/**
  * Wrapper for `ButtonComboModule_AddButtonComboPressDownEx` with
  * - `observer` set to "true"
  * - `controllerMask` set to "BUTTON_COMBO_MODULE_CONTROLLER_ALL"
  *
  * **Requires ButtonComboModule API version 1 or higher**
  *
  * See: @ButtonComboModule_AddButtonComboPressDownEx for more information.
 */
ButtonComboModule_Error ButtonComboModule_AddButtonComboPressDownObserver(const char *label,
                                                                          ButtonComboModule_Buttons combo,
                                                                          ButtonComboModule_ComboCallback callback,
                                                                          void *context,
                                                                          ButtonComboModule_ComboHandle *outHandle,
                                                                          ButtonComboModule_ComboStatus *outStatus);


/**
  * Creates a "Hold" button combo which triggers a callback when given combo for a given controller has been hold for X ms
  *
  * **Requires ButtonComboModule API version 1 or higher**
  *
  * See @ButtonComboModule_AddButtonCombo for detailed information about button combos.
  * See @ButtonComboModule_RemoveButtonCombo to remove the added button combo.
  *
  * @param label label of the button combo
  * @param controllerMask Mask of controllers which should be checked. Must not be empty.
  * @param combo Combo which should be checked. Must not be empty
  * @param holdDurationInMs Defines how long the button combination need to be hold down. Must not be 0.
  * @param callback Callback that will be called if a button combo is detected. Must not be nullptr.
  * @param context Context for the callback. Can be nullptr.
  * @param observer Defines if this combo should check for conflicts. Set it to "true" to be an observer and ignore conflicts.
  * @param outHandle The handle of the button combo will be stored here on success.  Must not be nullptr.
  * @param outStatus (optional) The status of the combo will be stored here. Only if the status is BUTTON_COMBO_MODULE_COMBO_STATUS_VALID the combo is valid. Can be NULL.
  * @return Returns BUTTON_COMBO_MODULE_ERROR_SUCCESS on success. Please check the outStatus as well.
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
 * Wrapper for `ButtonComboModule_AddButtonComboHoldEx` with
 * - `observer` set to "false"
 * - `controllerMask` set to "BUTTON_COMBO_MODULE_CONTROLLER_ALL"
 *
 * **Requires ButtonComboModule API version 1 or higher**
 *
 * See: @ButtonComboModule_AddButtonComboHoldEx for more information.
*/
ButtonComboModule_Error ButtonComboModule_AddButtonComboHold(const char *label,
                                                             ButtonComboModule_Buttons combo,
                                                             uint32_t holdDurationInMs,
                                                             ButtonComboModule_ComboCallback callback,
                                                             void *context,
                                                             ButtonComboModule_ComboHandle *outHandle,
                                                             ButtonComboModule_ComboStatus *outStatus);

/**
 * Wrapper for `ButtonComboModule_AddButtonComboHoldEx` with
 * - `observer` set to "true"
 * - `controllerMask` set to "BUTTON_COMBO_MODULE_CONTROLLER_ALL"
 *
 * **Requires ButtonComboModule API version 1 or higher**
 *
 * See: @ButtonComboModule_AddButtonComboHoldEx for more information.
*/
ButtonComboModule_Error ButtonComboModule_AddButtonComboHoldObserver(const char *label,
                                                                     ButtonComboModule_Buttons combo,
                                                                     uint32_t holdDurationInMs,
                                                                     ButtonComboModule_ComboCallback callback,
                                                                     void *context,
                                                                     ButtonComboModule_ComboHandle *outHandle,
                                                                     ButtonComboModule_ComboStatus *outStatus);

/**
* Removes a button combo for the given handle.
*
* **Requires ButtonComboModule API version 1 or higher**
*
* @param handle handle of the button combo that should be removed.
* @return Returns BUTTON_COMBO_MODULE_ERROR_SUCCESS on success.
*/
ButtonComboModule_Error ButtonComboModule_RemoveButtonCombo(ButtonComboModule_ComboHandle handle);

/**
 * Gets a button combo status for the given handle.
 *
 * **Requires ButtonComboModule API version 1 or higher**
 *
 * @param handle Handle of the button combo
 * @param outStatus The status of the combo will be stored here.
 * @return Returns BUTTON_COMBO_MODULE_ERROR_SUCCESS on success.
 */
ButtonComboModule_Error ButtonComboModule_GetButtonComboStatus(ButtonComboModule_ComboHandle handle,
                                                               ButtonComboModule_ComboStatus *outStatus);

/**
* Updates the meta options for the given handle
*
* **Requires ButtonComboModule API version 1 or higher**
*
* @param handle Handle of the button
* @param metaOptions new meta options
* @return Returns BUTTON_COMBO_MODULE_ERROR_SUCCESS on success.
*/
ButtonComboModule_Error ButtonComboModule_UpdateButtonComboMeta(ButtonComboModule_ComboHandle handle,
                                                                const ButtonComboModule_MetaOptions *metaOptions);

/**
* Updates the callback and context for the given handle
*
* **Requires ButtonComboModule API version 1 or higher**
*
* @param handle Handle of the button combo
* @param callbackOptions new callback options
* @return Returns BUTTON_COMBO_MODULE_ERROR_SUCCESS on success.
*/
ButtonComboModule_Error ButtonComboModule_UpdateButtonComboCallback(ButtonComboModule_ComboHandle handle,
                                                                    const ButtonComboModule_CallbackOptions *callbackOptions);

/**
* Updates the controller mask for the given handle
*
* **Requires ButtonComboModule API version 1 or higher**
*
* This will also re-check for conflicts and updates the combo status
*
* @param handle Handle of the button combo
* @param controllerMask new controller mask. must not be empty
* @param outStatus the new combo status after setting the mask will be written here.
* @return Returns BUTTON_COMBO_MODULE_ERROR_SUCCESS on success.
*/
ButtonComboModule_Error ButtonComboModule_UpdateControllerMask(ButtonComboModule_ComboHandle handle,
                                                               ButtonComboModule_ControllerTypes controllerMask,
                                                               ButtonComboModule_ComboStatus *outStatus);

/**
* Updates the combo for the given handle
*
* **Requires ButtonComboModule API version 1 or higher**
*
* This will also re-check for conflicts and updates the combo status
*
* @param handle Handle of the button combo
* @param combo new combo. must not be empty.
* @param outStatus the new combo status after setting the mask will be written here.
* @return Returns BUTTON_COMBO_MODULE_ERROR_SUCCESS on success.
*/
ButtonComboModule_Error ButtonComboModule_UpdateButtonCombo(ButtonComboModule_ComboHandle handle,
                                                            ButtonComboModule_Buttons combo,
                                                            ButtonComboModule_ComboStatus *outStatus);

/**
* Updates hold duration for a given handle
*
* **Requires ButtonComboModule API version 1 or higher**
*
* This only possible to "Hold"-button combos.
*
* @param handle Handle of the button combo
* @param holdDurationInMs the new hold duration in milliseconds
* @return Returns BUTTON_COMBO_MODULE_ERROR_SUCCESS on success.
*/
ButtonComboModule_Error ButtonComboModule_UpdateHoldDuration(ButtonComboModule_ComboHandle handle,
                                                             uint32_t holdDurationInMs);

/**
* Returns the current metadata for the given handle
*
* @param handle Handle of the button combo
* @param outOptions struct where the result will be written to. Must not be nullptr.
* @return Returns BUTTON_COMBO_MODULE_ERROR_SUCCESS on success.
*/
ButtonComboModule_Error ButtonComboModule_GetButtonComboMeta(ButtonComboModule_ComboHandle handle,
                                                             ButtonComboModule_MetaOptionsOut *outOptions);

/**
* Returns the current callback and context for the given handle
*
* **Requires ButtonComboModule API version 1 or higher**
*
* @param handle Handle of the button combo
* @param outOptions struct where the result will be written to. Must not be nullptr.
* @return Returns BUTTON_COMBO_MODULE_ERROR_SUCCESS on success.
*/
ButtonComboModule_Error ButtonComboModule_GetButtonComboCallback(ButtonComboModule_ComboHandle handle,
                                                                 ButtonComboModule_CallbackOptions *outOptions);

/**
* Returns the information about the controller mask and combo for the given handle
*
* **Requires ButtonComboModule API version 1 or higher**
*
* @param handle Handle of the button combo
* @param outOptions struct where the result will be written to. Must not be nullptr.
* @return Returns BUTTON_COMBO_MODULE_ERROR_SUCCESS on success.
*/
ButtonComboModule_Error ButtonComboModule_GetButtonComboInfoEx(ButtonComboModule_ComboHandle handle,
                                                               ButtonComboModule_ButtonComboInfoEx *outOptions);

/**
* Helper function to check the combo status for a given button combo option struct.
*
* **Requires ButtonComboModule API version 1 or higher**
*
* This can be used to check if a certain button combination is still "free" and won't cause any conflicts.
*
* The input for this function is a "ButtonComboModule_ButtonComboOptions" struct ptr. Fill in the values like this:
* `controllerMask` - Mask of which controllers would be checked for the button combo.
* `combo` - The button combo that should be checked.
*
* @param options Holds information about how the button combo should be detected.
* @param outStatus On success this will store the status of provided combo options.
* @return Returns "BUTTON_COMBO_MODULE_ERROR_SUCCESS" on success
**/
ButtonComboModule_Error ButtonComboModule_CheckComboAvailable(const ButtonComboModule_ButtonComboOptions *options,
                                                              ButtonComboModule_ComboStatus *outStatus);

/**
* Helper function to detect a pressed button combo.
*
* **Requires ButtonComboModule API version 1 or higher**
*
* This function is blocking the current thread until it return, call it in an appropriate place.
*
* The input for this function is a "ButtonComboModule_DetectButtonComboOptions" struct ptr. Fill in the values like this:
* `controllerMask` - Mask of which controller should be checked for a button combo. Must not be empty
* `holdComboForInMs` - Defines how many ms a combo needs to be hold to be detected as a combo
* `holdAbortForInMs` - Defines how many ms the abort combo needs to be hold so the detection will be aborted.
* `abortButtonCombo` - Defines the combo that will trigger an abort.
*
* The abort button combo is checked on all controller, if they are not part of the `controllerMask`
*
* @param options Holds information about how the button combo should be detected.
* @param outButtons The detected button combo will be stored here if the functions returns BUTTON_COMBO_MODULE_ERROR_SUCCESS.
* @return Returns "BUTTON_COMBO_MODULE_ERROR_SUCCESS" on success, and "BUTTON_COMBO_MODULE_ERROR_ABORTED" if the detection was aborted.
**/
ButtonComboModule_Error ButtonComboModule_DetectButtonCombo_Blocking(const ButtonComboModule_DetectButtonComboOptions *options,
                                                                     ButtonComboModule_Buttons *outButtons);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

#include <buttoncombo/ButtonCombo.h>
#include <optional>
#include <string_view>

namespace ButtonComboModule {
    /**
     * Wrapper for @ButtonComboModule_GetStatusStr
    **/
    const char *GetStatusStr(ButtonComboModule_Error status);

    /**
     * Wrapper for @ButtonComboModule_GetControllerTypeStr
    **/
    const char *GetControllerTypeStr(ButtonComboModule_ControllerTypes controller);

    /**
     * Wrapper for @ButtonComboModule_GetComboStatusStr
    **/
    const char *GetComboStatusStr(ButtonComboModule_ComboStatus status);

    /**
     * Creates a button combo which triggers a callback if this combo is detected.
     *
     * This function takes a generic `ButtonComboModule_ComboOptions` which defines how the combo should be checked.
     *
     * Depending on the given type, the combo will either check for holding (for X ms) or pressing a button on for a given
     * controller mask. The type also defines if it's an "observer" or not. Observers won't check for combo conflicts.
     *
     * If the given type is not an `observer` and any other (valid) button combination overlaps with new combo, then the
     * `outStatus` will be set to `BUTTON_COMBO_MODULE_COMBO_STATUS_CONFLICT` and the combo will be inactive. The original
     * ButtonCombo which had the combo first won't be affected at all.
     * In conflict state the callback will not be triggered even if the combination is pressed. If `observer` is set to
     * "true", the combo won't check for conflicts.
     *
     * To resolve a BUTTON_COMBO_MODULE_COMBO_STATUS_CONFLICT combo state you **always** have to update combo information
     * via @ButtonCombo::UpdateControllerMask or @ButtonCombo::UpdateButtonCombo. The state won't update itself,
     * even it the combo has no conflicts in a later point in time (e.g. due to other conflicting combos being removed in the meantime)
     *
     * Conflict example (only relevant if combo type is not an observer):
     * It's not possible to add any new valid button combo containing "L+R" (e.g. "X+L+R"), if there already is a button
     * combination "L+R". Furthermore, it's also not possible to add a "L" or "R" combo if there already is a button
     * combination "L+R".
     *
     * @param options options of this button combo
     * @param outStatus (optional) The status of the combo will be stored here. Only if the status is BUTTON_COMBO_MODULE_COMBO_STATUS_VALID the combo is valid.
     * @param outError The error of this operation will be stored here. Only if the error is BUTTON_COMBO_MODULE_ERROR_SUCCESS creating the combo was successful.
     * @return An optional `ButtonCombo` object if the combo registration succeeds; otherwise, an empty optional.
    */
    std::optional<ButtonCombo> CreateComboEx(const ButtonComboModule_ComboOptions &options,
                                             ButtonComboModule_ComboStatus &outStatus,
                                             ButtonComboModule_Error &outError) noexcept;

    /**
     * Creates a button combo which triggers a callback if this combo is detected.
     *
     * This function creates a "PressDown"-combo. This means the callback is triggered once the combo is pressed down
     * on one of the provided controllers. The provided controller mask can be a combination of any
     * `ButtonComboModule_ControllerTypes` values.
     *
     * The "observer" parameter defines if this button combo should check for conflicts with other button combos.
     *
     * See @CreateComboEx for more information.
     *
     * The returned objected will automatically remove the button combo in the destructor. Make sure to keep it around
     * along as the button combo should be valid! You have to use `std::move` to move it around.
     *
     * @param label Label of this button combo
     * @param controllerMask Mask of controllers which should be checked. Must not be empty.
     * @param combo Combination which should be checked. Must not be empty.
     * @param callback Callback that will be called if a button combo is detected. Must not be nullptr.
     * @param context Context for the callback. Can be nullptr.
     * @param observer Defines if this combo should check for conflicts. Set to "true" to be an observer and ignore conflicts.
     * @param outStatus The status of the combo will be stored here. Only if the status is BUTTON_COMBO_MODULE_COMBO_STATUS_VALID the combo is valid.
     * @param outError The error of this operation will be stored here. Only if the error is BUTTON_COMBO_MODULE_ERROR_SUCCESS creating the combo was successful.
     * @return An optional `ButtonCombo` object if the combo registration succeeds; otherwise, an empty optional.
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
     * Wrapper for `CreateComboPressDownEx` with `observer` set to "false" and `controllerMask` set to
     * "BUTTON_COMBO_MODULE_CONTROLLER_ALL"
     *
     * See: @CreateComboPressDownEx for more information.
     *
     * The returned objected will automatically remove the button combo in the destructor. Make sure to keep it around
     * along as the button combo should be valid! You have to use `std::move` to move it around.
    */
    std::optional<ButtonCombo> CreateComboPressDown(std::string_view label,
                                                    ButtonComboModule_Buttons combo,
                                                    ButtonComboModule_ComboCallback callback,
                                                    void *context,
                                                    ButtonComboModule_ComboStatus &outStatus,
                                                    ButtonComboModule_Error &outError) noexcept;

    /**
     * Wrapper for `CreateComboPressDownEx` with `observer` set to "true" and `controllerMask` set to
     * "BUTTON_COMBO_MODULE_CONTROLLER_ALL"
     *
     * See: @CreateComboPressDownEx for more information.
     *
     * The returned objected will automatically remove the button combo in the destructor. Make sure to keep it around
     * along as the button combo should be valid! You have to use `std::move` to move it around.
    */
    std::optional<ButtonCombo> CreateComboPressDownObserver(std::string_view label,
                                                            ButtonComboModule_Buttons combo,
                                                            ButtonComboModule_ComboCallback callback,
                                                            void *context,
                                                            ButtonComboModule_ComboStatus &outStatus,
                                                            ButtonComboModule_Error &outError) noexcept;

    /**
     * Creates a button combo which triggers a callback if a given button Combination has been hold for given duration.
     *
     * This function creates a "Hold"-combo. This means the callback is triggered once the combo is hold for a given duration
     * on one of the provided controllers. The provided controller mask can be a combination of any `ButtonComboModule_ControllerTypes` values.
     *
     *  The "observer" parameter defines if this button combo should check for conflicts with other button combos.
     *
     *  See: @CreateComboEx for more information about the details.
     *
     * The returned objected will automatically remove the button combo in the destructor. Make sure to keep it around
     * along as the button combo should be valid! You have to use `std::move` to move it around.
     *
     * @param label Label of this button combo
     * @param controllerMask Mask of controllers which should be checked. Must not be empty.
     * @param combo Combination which should be checked. Must not be empty.
     * @param holdDurationInMs Defines how long the button combination need to be hold down. Must not be 0.
     * @param callback Callback that will be called if a button combo is detected. Must not be nullptr.
     * @param context Context for the callback
     * @param observer Defines if this combo should check for conflicts. Set to "true" to be an observer and ignore conflicts.
     * @param outStatus The status of the combo will be stored here. Only if the status is BUTTON_COMBO_MODULE_COMBO_STATUS_VALID the combo is valid. Must not be nullptr.
     * @param outError The error of this operation will be stored here. Only if the error is BUTTON_COMBO_MODULE_ERROR_SUCCESS creating the combo was successful. Must not be nullptr.
     * @return An optional `ButtonCombo` object if the combo registration succeeds; otherwise, an empty optional.
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
     * Wrapper for `CreateComboHoldEx` with `observer` set to "false" and `controllerMask` set to
     * "BUTTON_COMBO_MODULE_CONTROLLER_ALL"
     *
     * See: @CreateComboHoldEx for more information.
     *
     * The returned objected will automatically remove the button combo in the destructor. Make sure to keep it around
     * along as the button combo should be valid! You have to use `std::move` to move it around.
    */
    std::optional<ButtonCombo> CreateComboHold(std::string_view label,
                                               ButtonComboModule_Buttons combo,
                                               uint32_t holdDurationInMs,
                                               ButtonComboModule_ComboCallback callback,
                                               void *context,
                                               ButtonComboModule_ComboStatus &outStatus,
                                               ButtonComboModule_Error &outError) noexcept;

    /**
     * Wrapper for `CreateComboHoldEx` with `observer` set to "true" and `controllerMask` set to
     * "BUTTON_COMBO_MODULE_CONTROLLER_ALL"
     *
     * See: @CreateComboHoldEx for more information.
     *
     * The returned objected will automatically remove the button combo in the destructor. Make sure to keep it around
     * along as the button combo should be valid! You have to use `std::move` to move it around.
    */
    std::optional<ButtonCombo> CreateComboHoldObserver(std::string_view label,
                                                       ButtonComboModule_Buttons combo,
                                                       uint32_t holdDurationInMs,
                                                       ButtonComboModule_ComboCallback callback,
                                                       void *context,
                                                       ButtonComboModule_ComboStatus &outStatus,
                                                       ButtonComboModule_Error &outError) noexcept;

    /**
    *  Same as @CreateComboEx, but throwing an exception on error instead of returning an optional.
    */
    ButtonCombo CreateComboPressDownEx(std::string_view label,
                                       ButtonComboModule_ControllerTypes controllerMask,
                                       ButtonComboModule_Buttons combo,
                                       ButtonComboModule_ComboCallback callback,
                                       void *context,
                                       bool observer,
                                       ButtonComboModule_ComboStatus &outStatus);
    /**
    *  Same as @CreateComboEx, but throwing an exception on error instead of returning an optional.
    */
    ButtonCombo CreateComboPressDown(std::string_view label,
                                     ButtonComboModule_Buttons combo,
                                     ButtonComboModule_ComboCallback callback,
                                     void *context,
                                     ButtonComboModule_ComboStatus &outStatus);
    /**
    *  Same as @CreateComboPressDownObserver, but throwing an exception on error instead of returning an optional.
    */
    ButtonCombo CreateComboPressDownObserver(std::string_view label,
                                             ButtonComboModule_Buttons combo,
                                             ButtonComboModule_ComboCallback callback,
                                             void *context,
                                             ButtonComboModule_ComboStatus &outStatus);
    /**
    *  Same as @CreateComboHoldEx, but throwing an exception on error instead of returning an optional.
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
    *  Same as @CreateComboHold, but throwing an exception on error instead of returning an optional.
    */
    ButtonCombo CreateComboHold(std::string_view label,
                                ButtonComboModule_Buttons combo,
                                uint32_t holdDurationInMs,
                                ButtonComboModule_ComboCallback callback,
                                void *context,
                                ButtonComboModule_ComboStatus &outStatus);
    /**
    *  Same as @CreateComboHoldObserver, but throwing an exception on error instead of returning an optional.
    */
    ButtonCombo CreateComboHoldObserver(std::string_view label,
                                        ButtonComboModule_Buttons combo,
                                        uint32_t holdDurationInMs,
                                        ButtonComboModule_ComboCallback callback,
                                        void *context,
                                        ButtonComboModule_ComboStatus &outStatus);

    /**
    * See @ButtonComboModule_CheckComboAvailable
    */
    ButtonComboModule_Error CheckComboAvailable(const ButtonComboModule_ButtonComboOptions &options,
                                                ButtonComboModule_ComboStatus &outStatus);

    /**
    * See @ButtonComboModule_DetectButtonCombo_Blocking
    */
    ButtonComboModule_Error DetectButtonCombo_Blocking(const ButtonComboModule_DetectButtonComboOptions &options,
                                                       ButtonComboModule_Buttons &outButtons);
} // namespace ButtonComboModule
#endif
