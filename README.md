[![Publish Docker Image](https://github.com/wiiu-env/libbuttoncombo/actions/workflows/push_image.yml/badge.svg)](https://github.com/wiiu-env/libbuttoncombo/actions/workflows/push_image.yml)

# libbuttoncombo

**libbuttoncombo** is the client library for the [ButtonComboModule](https://github.com/wiiu-env/ButtonComboModule). It
allows Wii U homebrew applications to easily register and detect button combinations (presses or holds) system-wide by
interfacing with the Wii U Module System (WUMS).

**Note:** If you are developing a **Wii U Plugin System (WUPS)** plugin, you should **not** use this library directly.
Instead, use the `WUPSButtonComboAPI_` functionality provided by
the [WiiUPluginSystem (WUPS)](https://github.com/wiiu-env/WiiUPluginSystem).

## Features

* **System-wide Detection**: Detects input even when your application is running in the background (via the module).
* **Conflict Management**: Automatically handles overlapping combos (e.g., prevents "A" from triggering if "A+B" is
  registered), unless registered as an Observer.
* **Flexible Inputs**: Supports simple button presses and time-based "Hold" interactions.
* **Modern C++ API**: Provides RAII wrappers (`ButtonComboModule::ButtonCombo`) for automatic resource management.
* **C API**: Full support for C projects.

## Prerequisites

* **ButtonComboModule**: Requires the [ButtonComboModule](https://github.com/wiiu-env/ButtonComboModule) to be running
  via [WUMSLoader](https://github.com/wiiu-env/WUMSLoader).
* **Build Tools**: Requires [wut](https://github.com/devkitPro/wut) for building.

## Building

To build this library, you must have **devkitPro** (specifically `wut` and `devkitPPC`) installed.

You also need to ensure the `WUMS_ROOT` environment variable is set to your WUMS installation directory. Typically, this
is inside your devkitPro directory.

### Local Build
```
  make
  make install
```
### Docker Build

A prebuilt version of this lib can be found on dockerhub. To use it for your projects, add this to your Dockerfile:
```
  COPY --from=ghcr.io/wiiu-env/libbuttoncombo:[tag] /artifacts $DEVKITPRO
```
Replace `[tag]` with the version you want to use. It's highly recommended to pin the version to the **latest date**
instead of using `latest`.

## Usage

### 1. Makefile Configuration

To use `libbuttoncombo` in your own project, add it to your `LIBS` and ensure `WUMS_ROOT` is defined so the compiler can
find the headers and libraries.
```
# In your Makefile
WUMS_ROOT := $(DEVKITPRO)/wums

# Add include and library paths
INCLUDE   += -I$(WUMS_ROOT)/include
LIBDIRS   += -L$(WUMS_ROOT)/lib

# Link the library
LIBS      += -lbuttoncombo
```
### 2. Initialization

Before using any API functions, you must initialize the library. This verifies that the backend module is loaded.
```
#include <buttoncombo/api.h>

void InitMyPlugin() {
    // Initialize the library
    auto res = ButtonComboModule_InitLibrary();

    if (res != BUTTON_COMBO_MODULE_ERROR_SUCCESS) {
        // Handle error (e.g., module not loaded)
        return;
    }
}

void DeinitMyPlugin() {
    ButtonComboModule_DeInitLibrary();
}
```
### 3. C++ API Example

The C++ API uses a `ButtonCombo` class that automatically unregisters the combo when the object is destroyed. To keep
combos alive for the duration of your application, store them in a container like `std::vector`.

```
#include <buttoncombo/ButtonCombo.h>
#include <vector>

// Global vector to store active combos
std::vector<ButtonComboModule::ButtonCombo> gCombos;

// Callback function
void OnCombo(ButtonComboModule_ControllerTypes triggeredBy, ButtonComboModule_ComboHandle handle, void *context) {
    // 'triggeredBy' indicates which controller (e.g., GamePad, Pro Controller) pressed the buttons.
}

void SetupCombos() {
    ButtonComboModule_ComboStatus status;
    ButtonComboModule_Error error;

    // 1. Create a "Press Down" combo (Triggers immediately)
    // We use ButtonComboModule::CreateComboPressDown directly.
    // std::move is used because ButtonCombo cannot be copied, only moved.
    auto myPressComboOpt = ButtonComboModule::CreateComboPressDown(
            "MyPressCombo",                    // Label
            BCMPAD_BUTTON_A | BCMPAD_BUTTON_B, // Button Combination
            OnCombo,                           // Callback
            nullptr,                           // Context
            status,                            // Out: Status
            error                              // Out: Error
    );

    if (myPressComboOpt) {
        if (status == BUTTON_COMBO_MODULE_COMBO_STATUS_VALID) {
            // Move the combo into the vector to keep it alive
            gCombos.push_back(std::move(*myPressComboOpt));
        } else {
            OSReport("Failed to create combo. Status: %s (%d)\n", ButtonComboModule::GetComboStatusStr(status), status);
            // Handle conflict (BUTTON_COMBO_MODULE_COMBO_STATUS_CONFLICT)
        }
    } else {
        OSReport("Failed to create combo. Error: %s (%d)\n", ButtonComboModule::GetStatusStr(error), error);
    }

    // 2. Create a "Hold" combo (Triggers after 2 seconds)
    auto myHoldComboOpt = ButtonComboModule::CreateComboHold(
            "MyHoldCombo",
            BCMPAD_BUTTON_L,
            2000, // Hold duration in milliseconds
            OnCombo,
            nullptr,
            status,
            error);

    if (myHoldComboOpt && status == BUTTON_COMBO_MODULE_COMBO_STATUS_VALID && error == BUTTON_COMBO_MODULE_ERROR_SUCCESS) {
        gCombos.push_back(std::move(*myHoldComboOpt));
    } else {
        OSReport("Failed to create combo. Error: %s (%d) Status %s (%d)\n", ButtonComboModule::GetStatusStr(error), error, ButtonComboModule::GetComboStatusStr(status), status);
    }
}

void CleanupCombos() {
    // Clearing the vector destroys the ButtonCombo objects,
    // which automatically unregisters them from the module.
    gCombos.clear();
}
```

### 4. C API Example

If you are using C, you must manually manage the handle and memory.

```
#include <buttoncombo/api.h>
#include <coreinit/debug.h>

ButtonComboModule_ComboHandle myHandle;

void OnCombo(ButtonComboModule_ControllerTypes triggeredBy, ButtonComboModule_ComboHandle handle, void *context) {
    // 'triggeredBy' indicates which controller (e.g., GamePad, Pro Controller) pressed the buttons.
}

void RegisterCCombo() {
    ButtonComboModule_ComboStatus status;

    ButtonComboModule_Error err = ButtonComboModule_AddButtonComboPressDown(
            "C_Combo",
            BCMPAD_BUTTON_X | BCMPAD_BUTTON_Y,
            OnCombo,
            NULL,
            &myHandle,
            &status);

    if (err == BUTTON_COMBO_MODULE_ERROR_SUCCESS && status == BUTTON_COMBO_MODULE_COMBO_STATUS_VALID) {
        // Success
    } else {
        OSReport("Failed to create combo. Error: %s (%d) Status %s (%d)\n", ButtonComboModule_GetStatusStr(err), err, ButtonComboModule_GetComboStatusStr(status), status);
    }
}

void UnregisterCCombo() {
    if (myHandle.handle != NULL) {
        ButtonComboModule_RemoveButtonCombo(myHandle);
        myHandle.handle = NULL;
    }
}
```

## Formatting

You can format the code via docker:
```
docker run --rm -v ${PWD}:/src ghcr.io/wiiu-env/clang-format:13.0.0-2 -r ./source ./include -i
```
## License

This library is licensed under the **LGPL-3.0**.  
The underlying **ButtonComboModule** is licensed under the **GPL-3.0**.