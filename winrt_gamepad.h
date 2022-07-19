#ifndef WINRT_GAMEPAD
#define WINRT_GAMEPAD

#include "arraylist.h"

typedef struct GamingInputGamepad {
    int id;
    void *pad;

    struct {
        int LX;
        int LY;
        int RX;
        int RY;
        int LT;
        int RT;
        uint64_t buttons;
    } state;
} GamingInputGamepad;

typedef enum GamingInputGamepadButtons {
    GamepadButton_None = 0x0,
    GamepadButton_Menu = 0x1,
    GamepadButton_View = 0x2,
    GamepadButton_A = 0x4,
    GamepadButton_B = 0x8,
    GamepadButton_X = 0x10,
    GamepadButton_Y = 0x20,
    GamepadButton_DPadUp = 0x40,
    GamepadButton_DPadDown = 0x80,
    GamepadButton_DPadLeft = 0x100,
    GamepadButton_DPadRight = 0x200,
    GamepadButton_LB = 0x400,
    GamepadButton_RB = 0x800,
    GamepadButton_LS = 0x1000,
    GamepadButton_RS = 0x2000,
    GamepadButton_Paddle1 = 0x4000,
    GamepadButton_Paddle2 = 0x8000,
    GamepadButton_Paddle3 = 0x10000,
    GamepadButton_Paddle4 = 0x20000,
    GamepadButton_Guide = 0x40000000
} GamingInputGamepadButtons;

extern ArrayList *gamepads;
void InitializeGamingInput();
void UpdateReading(void (*handler)(GamingInputGamepad *gamepad));

extern void OnGamepadAdded(GamingInputGamepad* gamepad);
extern void OnGamepadRemoved(GamingInputGamepad* gamepad);
#endif