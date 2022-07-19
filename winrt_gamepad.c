#include <stddef.h>
#include <stdio.h>
#include <assert.h>

#include <initguid.h>
#include <unknwn.h>

#include <windows.h>
#include <roapi.h>
#include <winstring.h>

#include <windows.foundation.h>
#include <windows.foundation.collections.h>
#include <windows.gaming.input.h>

#include "arraylist.h"
#include "winrt_gamepad.h"

const GUID IID_FIEventHandler_Windows_Gaming_Input_Gamepad = {
    0x8a7639ee,
    0x624a,
    0x501a,
    {0xbb, 0x53, 0x56, 0x2d, 0x1e, 0xc1, 0x1b, 0x52}};

const GUID IID_ABI_Windows_Gaming_Input_IGamepadStatics = {
    0x8bbce529,
    0xd49c,
    0x39e9,
    {0x95, 0x60, 0xe4, 0x7d, 0xde, 0x96, 0xb7, 0xc8}};

ArrayList *gamepadList;
int next_gamepad_id = 0;
__x_ABI_CWindows_CGaming_CInput_CIGamepadStatics *gamepadStatics;

/* Implement the COM interface for IEventHandler<Gamepad>
 * Used in Gamepad add/remove events */
typedef struct EventHandler_Gamepad {
    __FIEventHandler_1_Windows__CGaming__CInput__CGamepad base;
    HRESULT(*handler)
    (IInspectable *sender, __x_ABI_CWindows_CGaming_CInput_CIGamepad *args);

} EventHandler_Gamepad;

HRESULT STDMETHODCALLTYPE FIEventHandler_Gamepad_QueryInterface(
    __FIEventHandler_1_Windows__CGaming__CInput__CGamepad *this, REFIID riid,
    void **ppvObject) {

    if (!ppvObject)
        return E_INVALIDARG;
    *ppvObject = NULL;
    if (IsEqualIID(riid, &IID_IUnknown) ||
        IsEqualIID(riid, &IID_FIEventHandler_Windows_Gaming_Input_Gamepad)) {
        *ppvObject = (LPVOID)this;
        this->lpVtbl->AddRef(this);
        return S_OK;
    }
    return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE FIEventHandler_Gamepad_AddRef(
    __FIEventHandler_1_Windows__CGaming__CInput__CGamepad *this) {
    /* The event object would be static, no need to implement reference
     * counting. */
    return 1;
}

ULONG STDMETHODCALLTYPE FIEventHandler_Gamepad_Release(
    __FIEventHandler_1_Windows__CGaming__CInput__CGamepad *this) {
    /* The event object would be static, no need to implement reference
     * counting. */
    return 1;
}

HRESULT STDMETHODCALLTYPE FIEventHandler_Gamepad_Invoke(
    __FIEventHandler_1_Windows__CGaming__CInput__CGamepad *this,
    IInspectable *sender, __x_ABI_CWindows_CGaming_CInput_CIGamepad *args) {
    EventHandler_Gamepad *self = (EventHandler_Gamepad *)this;
    if (self->handler)
        return self->handler(sender, args);
    else
        return S_OK;
}

__FIEventHandler_1_Windows__CGaming__CInput__CGamepadVtbl
    handler_gamepad_vtabl = {.QueryInterface =
                                 FIEventHandler_Gamepad_QueryInterface,
                             .AddRef = FIEventHandler_Gamepad_AddRef,
                             .Release = FIEventHandler_Gamepad_Release,
                             .Invoke = FIEventHandler_Gamepad_Invoke};

int FindGamepadPredicate(void *p, void *gamepad) {
    GamingInputGamepad *item = (GamingInputGamepad *)p;
    return item->pad == gamepad;
}

/* Gamepad Added event */
HRESULT
handler_func_gamepad_added(IInspectable *sender,
                           __x_ABI_CWindows_CGaming_CInput_CIGamepad *gamepad) {

    if (ArrayList_FindIndex(gamepadList, FindGamepadPredicate, gamepad) == -1) {
        GamingInputGamepad new_gamepad = {.id = next_gamepad_id,
                                          .pad = gamepad};
        next_gamepad_id++;
        int i = ArrayList_Add(gamepadList, &new_gamepad);

        GamingInputGamepad *added_gamepad;
        ArrayList_GetRef(gamepadList, i, (void **)&added_gamepad);
        OnGamepadAdded(added_gamepad);
    }
    return S_OK;
}
EventRegistrationToken token_gamepad_added = {0};
EventHandler_Gamepad handler_gamepad_added = {
    .base = {.lpVtbl = &handler_gamepad_vtabl},
    .handler = handler_func_gamepad_added};

/* Gamepad removed event */
HRESULT
handler_func_gamepad_removed(
    IInspectable *sender, __x_ABI_CWindows_CGaming_CInput_CIGamepad *gamepad) {
    int index = ArrayList_FindIndex(gamepadList, FindGamepadPredicate, gamepad);

    if (index > -1) {
        GamingInputGamepad *removed_gamepad;
        ArrayList_GetRef(gamepadList, index, (void **)&removed_gamepad);
        OnGamepadRemoved(removed_gamepad);

        ArrayList_Del(gamepadList, index);
    }
    return S_OK;
}
EventRegistrationToken token_gamepad_removed = {0};
EventHandler_Gamepad handler_gamepad_removed = {
    .base = {.lpVtbl = &handler_gamepad_vtabl},
    .handler = handler_func_gamepad_removed};

void InitializeGamingInput() {
    HRESULT hr;
    {
        const wchar_t *className = RuntimeClass_Windows_Gaming_Input_Gamepad;
        HSTRING_HEADER className_header;
        HSTRING className_hstr;
        hr = WindowsCreateStringReference(className, (UINT32)wcslen(className),
                                          &className_header, &className_hstr);
        assert(SUCCEEDED(hr));

        hr = RoGetActivationFactory(
            className_hstr, &IID_ABI_Windows_Gaming_Input_IGamepadStatics,
            (void **)&gamepadStatics);
        assert(SUCCEEDED(hr));
    }

    hr = gamepadStatics->lpVtbl->add_GamepadAdded(
        gamepadStatics,
        (__FIEventHandler_1_Windows__CGaming__CInput__CGamepad
             *)&handler_gamepad_added,
        &token_gamepad_added);
    assert(SUCCEEDED(hr));

    hr = gamepadStatics->lpVtbl->add_GamepadRemoved(
        gamepadStatics,
        (__FIEventHandler_1_Windows__CGaming__CInput__CGamepad
             *)&handler_gamepad_removed,
        &token_gamepad_removed);
    assert(SUCCEEDED(hr));

    gamepadList = ArrayList_Init(sizeof(GamingInputGamepad), 16);
}

void UpdateReading(void (*handler)(GamingInputGamepad *gamepad)) {
    for (int i = 0; i < gamepadList->count; i++) {
        GamingInputGamepad *item;
        ArrayList_GetRef(gamepadList, i, (void **)&item);

        __x_ABI_CWindows_CGaming_CInput_CIGamepad *gamepad = item->pad;
        __x_ABI_CWindows_CGaming_CInput_CGamepadReading reading = {0};
        HRESULT hr = gamepad->lpVtbl->GetCurrentReading(gamepad, &reading);
        if (!SUCCEEDED(hr))
            continue;

        item->state.LX = (int)(reading.LeftThumbstickX * 32768);
        item->state.LY = (int)(reading.LeftThumbstickY * 32768);
        item->state.RX = (int)(reading.RightThumbstickX * 32768);
        item->state.RY = (int)(reading.RightThumbstickY * 32768);
        item->state.LT = (int)(reading.LeftTrigger * 32768);
        item->state.RT = (int)(reading.RightTrigger * 32768);
        item->state.buttons = (GamingInputGamepadButtons)(reading.Buttons);

        handler(item);
    }
}