
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <windows.h>
#include <commctrl.h>
#include <roapi.h>

#include "listview.h"
#include "winrt_gamepad.h"
#include "arraylist.h"

#define IDT_TIMER 0x1000

HWND hwndListView;

void OnCreate(HWND hwnd, CREATESTRUCTW *cs) {
    hwndListView =
        CreateWindowExW(0, WC_LISTVIEWW, L"ListView",
                        WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_TABSTOP |
                            LVS_SINGLESEL | LVS_REPORT,
                        0, 0, 250, 500, hwnd, NULL, cs->hInstance, NULL);

    LVAddColumn(hwndListView, 0, L"#", 100);
    LVAddColumn(hwndListView, 1, L"LX", 100);
    LVAddColumn(hwndListView, 2, L"LY", 100);
    LVAddColumn(hwndListView, 3, L"RX", 100);
    LVAddColumn(hwndListView, 4, L"RY", 100);
    LVAddColumn(hwndListView, 5, L"LT", 100);
    LVAddColumn(hwndListView, 6, L"RT", 100);
    LVAddColumn(hwndListView, 7, L"Buttons", 400);

    InitializeGamingInput();
    SetTimer(hwnd, IDT_TIMER, 50, NULL);
}

void OnResize(HWND hwnd, WPARAM wParam, WORD client_width, WORD client_height) {
    MoveWindow(hwndListView, 0, 0, client_width, client_height, TRUE);
}

void OnGamepadAdded(GamingInputGamepad *gamepad) {
    int index = LVFindItemByParam(hwndListView, -1, gamepad->id);
    wchar_t tmp[16];
    if (index == -1) {

        wsprintfW(tmp, L"%d", gamepad->id);
        index = LVAddItem(hwndListView, -1, tmp, gamepad->id);
    }
}

void OnGamepadRemoved(GamingInputGamepad *gamepad) {
    int index = LVFindItemByParam(hwndListView, -1, gamepad->id);
    if (index > -1) {
        LVDeleteItem(hwndListView, index);
    }
}

void ParseButtonString(GamingInputGamepad *gamepad, wchar_t *str) {
    str[0] = 0;
    uint64_t buttons = gamepad->state.buttons;

    wsprintfW(str, L"(%lx) ", gamepad->state.buttons);

    if (buttons & GamepadButton_Menu)
        wcscat(str, L"Menu ");
    if (buttons & GamepadButton_View)
        wcscat(str, L"View ");
    if (buttons & GamepadButton_A)
        wcscat(str, L"A ");
    if (buttons & GamepadButton_B)
        wcscat(str, L"B ");
    if (buttons & GamepadButton_X)
        wcscat(str, L"X ");
    if (buttons & GamepadButton_Y)
        wcscat(str, L"Y ");
    if (buttons & GamepadButton_DPadUp)
        wcscat(str, L"DPadUp ");
    if (buttons & GamepadButton_DPadDown)
        wcscat(str, L"DPadDown ");
    if (buttons & GamepadButton_DPadLeft)
        wcscat(str, L"DPadLeft ");
    if (buttons & GamepadButton_DPadRight)
        wcscat(str, L"DPadRight ");
    if (buttons & GamepadButton_LB)
        wcscat(str, L"LB ");
    if (buttons & GamepadButton_RB)
        wcscat(str, L"RB ");
    if (buttons & GamepadButton_LS)
        wcscat(str, L"LS ");
    if (buttons & GamepadButton_RS)
        wcscat(str, L"RS ");
    if (buttons & GamepadButton_Paddle1)
        wcscat(str, L"Paddle1 ");
    if (buttons & GamepadButton_Paddle2)
        wcscat(str, L"Paddle2 ");
    if (buttons & GamepadButton_Paddle3)
        wcscat(str, L"Paddle3 ");
    if (buttons & GamepadButton_Paddle4)
        wcscat(str, L"Paddle4 ");
    if (buttons & GamepadButton_Guide)
        wcscat(str, L"Guide ");
}

void OnGamepadUpdate(GamingInputGamepad *gamepad) {
    int index = LVFindItemByParam(hwndListView, -1, gamepad->id);
    if (index == -1)
        return;

    wchar_t tmp[32768];

    wsprintfW(tmp, L"%d", gamepad->state.LX);
    LVSetItemText(hwndListView, index, 1, tmp);

    wsprintfW(tmp, L"%d", gamepad->state.LY);
    LVSetItemText(hwndListView, index, 2, tmp);

    wsprintfW(tmp, L"%d", gamepad->state.RX);
    LVSetItemText(hwndListView, index, 3, tmp);

    wsprintfW(tmp, L"%d", gamepad->state.RY);
    LVSetItemText(hwndListView, index, 4, tmp);

    wsprintfW(tmp, L"%d", gamepad->state.LT);
    LVSetItemText(hwndListView, index, 5, tmp);

    wsprintfW(tmp, L"%d", gamepad->state.RT);
    LVSetItemText(hwndListView, index, 6, tmp);

    ParseButtonString(gamepad, tmp);
    LVSetItemText(hwndListView, index, 7, tmp);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE:
        OnCreate(hwnd, (CREATESTRUCTW *)lParam);
        break;
    case WM_SIZE:
        OnResize(hwnd, wParam, LOWORD(lParam), HIWORD(lParam));
        break;
    case WM_TIMER:
        switch (wParam) {
        case IDT_TIMER:
            UpdateReading(OnGamepadUpdate);
            break;
        }

        break;
    case WM_CLOSE:
        KillTimer(hwnd, IDT_TIMER);
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPWSTR lpCmdLine, int nCmdShow) {

    HRESULT hr;
    hr = RoInitialize(RO_INIT_SINGLETHREADED);
    if (!SUCCEEDED(hr)) {
        return -1;
    }

    WNDCLASSEXW wndClass = {0};
    wndClass.cbSize = sizeof(WNDCLASSEXW);
    wndClass.hInstance = hInstance;
    wndClass.lpszClassName = L"GamingInputTest";
    wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wndClass.lpfnWndProc = WndProc;

    RegisterClassExW(&wndClass);
    HWND hwnd = CreateWindowExW(0, L"GamingInputTest", L"GamingInputTest",
                                WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
                                CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                NULL, NULL, hInstance, NULL);

    if (hwnd == NULL) {
        return 1;
    }

    ShowWindow(hwnd, nCmdShow);

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    RoUninitialize();
    return 0;
}
