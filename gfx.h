#ifndef _CORE_GFX_H_
#define _CORE_GFX_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "core.h"

typedef void *WindowHandle;

typedef enum MouseButton {
    MOUSE_BUTTON_LEFT = 0,
    MOUSE_BUTTON_RIGHT,
    MOUSE_BUTTON_MIDDLE,
    MOUSE_BUTTON_4,
    MOUSE_BUTTON_5,
    MOUSE_BUTTON_PRIMARY = MOUSE_BUTTON_LEFT,
    MOUSE_BUTTON_SECONDARY = MOUSE_BUTTON_RIGHT,
    MOUSE_BUTTON_COUNT,
}MouseButton;

typedef enum Key {
    KEY_SPACE          =     32,
    KEY_APOSTROPHE     =     39,  /* ' */
    KEY_COMMA          =     44,  /* , */
    KEY_MINUS          =     45,  /* - */
    KEY_PERIOD         =     46,  /* . */
    KEY_SLASH          =     47,  /* / */
    KEY_0              =     48,
    KEY_1              =     49,
    KEY_2              =     50,
    KEY_3              =     51,
    KEY_4              =     52,
    KEY_5              =     53,
    KEY_6              =     54,
    KEY_7              =     55,
    KEY_8              =     56,
    KEY_9              =     57,
    KEY_SEMICOLON      =     59,  /* ; */
    KEY_EQUAL          =     61,  /* = */
    KEY_A              =     65,
    KEY_B              =     66,
    KEY_C              =     67,
    KEY_D              =     68,
    KEY_E              =     69,
    KEY_F              =     70,
    KEY_G              =     71,
    KEY_H              =     72,
    KEY_I              =     73,
    KEY_J              =     74,
    KEY_K              =     75,
    KEY_L              =     76,
    KEY_M              =     77,
    KEY_N              =     78,
    KEY_O              =     79,
    KEY_P              =     80,
    KEY_Q              =     81,
    KEY_R              =     82,
    KEY_S              =     83,
    KEY_T              =     84,
    KEY_U              =     85,
    KEY_V              =     86,
    KEY_W              =     87,
    KEY_X              =     88,
    KEY_Y              =     89,
    KEY_Z              =     90,
    KEY_LEFT_BRACKET   =     91,  /* [ */
    KEY_BACKSLASH      =     92,  /* \ */
    KEY_RIGHT_BRACKET  =     93,  /* ] */
    KEY_GRAVE_ACCENT   =     96,  /* ` */
    KEY_WORLD_1        =    161, /* non-US #1 */
    KEY_WORLD_2        =    162, /* non-US #2 */

    /* Function keys */
    KEY_ESCAPE          =    256,
    KEY_ENTER           =    257,
    KEY_TAB             =    258,
    KEY_BACKSPACE       =    259,
    KEY_INSERT          =    260,
    KEY_DELETE          =    261,
    KEY_RIGHT           =    262,
    KEY_LEFT            =    263,
    KEY_DOWN            =    264,
    KEY_UP              =    265,
    KEY_PAGE_UP         =    266,
    KEY_PAGE_DOWN       =    267,
    KEY_HOME            =    268,
    KEY_END             =    269,
    KEY_CAPS_LOCK       =    280,
    KEY_SCROLL_LOCK     =    281,
    KEY_NUM_LOCK        =    282,
    KEY_PRINT_SCREEN    =    283,
    KEY_PAUSE           =    284,
    KEY_F1              =    290,
    KEY_F2              =    291,
    KEY_F3              =    292,
    KEY_F4              =    293,
    KEY_F5              =    294,
    KEY_F6              =    295,
    KEY_F7              =    296,
    KEY_F8              =    297,
    KEY_F9              =    298,
    KEY_F10             =    299,
    KEY_F11             =    300,
    KEY_F12             =    301,
    KEY_F13             =    302,
    KEY_F14             =    303,
    KEY_F15             =    304,
    KEY_F16             =    305,
    KEY_F17             =    306,
    KEY_F18             =    307,
    KEY_F19             =    308,
    KEY_F20             =    309,
    KEY_F21             =    310,
    KEY_F22             =    311,
    KEY_F23             =    312,
    KEY_F24             =    313,
    KEY_F25             =    314,
    KEY_KP_0            =    320,
    KEY_KP_1            =    321,
    KEY_KP_2            =    322,
    KEY_KP_3            =    323,
    KEY_KP_4            =    324,
    KEY_KP_5            =    325,
    KEY_KP_6            =    326,
    KEY_KP_7            =    327,
    KEY_KP_8            =    328,
    KEY_KP_9            =    329,
    KEY_KP_DECIMAL      =    330,
    KEY_KP_DIVIDE       =    331,
    KEY_KP_MULTIPLY     =    332,
    KEY_KP_SUBTRACT     =    333,
    KEY_KP_ADD          =    334,
    KEY_KP_ENTER        =    335,
    KEY_KP_EQUAL        =    336,
    KEY_LEFT_SHIFT      =    340,
    KEY_LEFT_CONTROL    =    341,
    KEY_LEFT_ALT        =    342,
    KEY_LEFT_SUPER      =    343,
    KEY_RIGHT_SHIFT     =    344,
    KEY_RIGHT_CONTROL   =    345,
    KEY_RIGHT_ALT       =    346,
    KEY_RIGHT_SUPER     =    347,
    KEY_MENU            =    348,

    KEY_LAST            = KEY_MENU,
}Key;

typedef enum KeyMod {
    MOD_SHIFT           = CORE_BIT(0),
    MOD_CONTROL         = CORE_BIT(1),
    MOD_ALT             = CORE_BIT(2),
    MOD_SUPER           = CORE_BIT(3),
    MOD_CAPS_LOCK       = CORE_BIT(4),
    MOD_NUM_LOCK        = CORE_BIT(5),
}KeyMod;

typedef enum KeyState {
    KEY_STATE_DOWN,
    KEY_STATE_UP,
}KeyState;

typedef struct WindowEvent {
    enum {
        WINDOW_EVENT_NONE = 0,
        WINDOW_EVENT_MOUSE,
        WINDOW_EVENT_MOUSE_DOUBLE_CLICK,
        WINDOW_EVENT_CLOSE,
        WINDOW_EVENT_CLOSE_REQUESTED,
        WINDOW_EVENT_CREATED,
        WINDOW_EVENT_FOCUS_GAINED,
        WINDOW_EVENT_FOCUS_LOST,
        WINDOW_EVENT_MOUSE_MOVE,
        WINDOW_EVENT_MOUSE_ENTER,
        WINDOW_EVENT_MOUSE_LEAVE,
        WINDOW_EVENT_SCROLL,
        WINDOW_EVENT_KEY,
        WINDOW_EVENT_CHAR,
        WINDOW_EVENT_RESIZED,
        WINDOW_EVENT_MOVED,
        WINDOW_EVENT_MINIMIZED,
        WINDOW_EVENT_MAXIMIZED,
        WINDOW_EVENT_RESTORED,
        WINDOW_EVENT_COUNT,
    }kind;
    union {
        struct WindowMouseEvent { MouseButton button; KeyState state; size_t click_count; } mouse;
        struct WindowDoubleClickEvent { MouseButton button; } mouse_double_click;
        struct WindowMouseMoveEvent { i32 x; i32 y; } mouse_move;
        struct WindowScrollEvent { f32 x; f32 y; } scroll;
        struct WindowKeyEvent { Key key ; KeyState state; i32 mods; } key;
        struct WindowCharEvent { u32 codepoint; } character;
        struct WindowResizedEvent { i32 w; i32 h; } resized;
        struct WindowMovedEvent { i32 x; i32 y; } moved;
    };
}WindowEvent;

typedef u32 WindowStyle_;
typedef enum WindowStyle {
    WINDOW_NOT_MOVEABLE         = CORE_BIT(0),
    WINDOW_NOT_RESIZEABLE       = CORE_BIT(1),
    WINDOW_VISILE               = CORE_BIT(2),
}WindowStyle;

typedef void (*WindowEventCallback)(WindowHandle window, WindowEvent event, void *user_data);

typedef struct WindowCreateArgs {
    StringView name;
    size_t width, height;
    WindowStyle_ style;
    void *user_data;
    WindowEventCallback event_callback;
}WindowCreateArgs;

WindowEvent window_event_mouse(MouseButton button, KeyState state, size_t click_count);
WindowEvent window_event_double_click(MouseButton button);
WindowEvent window_event_mouse_move(i32 x, i32 y);
WindowEvent window_event_scroll(f32 x, f32 y);
WindowEvent window_event_key(Key key, KeyState state, i32 mods);
WindowEvent window_event_char(u32 codepoint);
WindowEvent window_event_resize(i32 width, i32 height);
WindowEvent window_event_move(i32 x, i32 y);
void window_event_print(WindowEvent *event);

struct WindowMouseEvent *window_mouse_event(WindowEvent *event);

WindowHandle window_create_opt(WindowCreateArgs args);
#define window_create(...) window_create_opt((WindowCreateArgs){__VA_ARGS__})
void window_destroy(WindowHandle window);
void window_make_current(WindowHandle window);
bool window_should_close(WindowHandle window);
//void window_clear_flag(WindowHandle window, WindowStyle flag);
//void window_set_flag(WindowHandle window, WindowStyle flag);
bool window_poll_events(WindowHandle window);
bool window_wait_events(void);

#ifdef PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

typedef struct Win32Window {
    HWND hwnd;
    HINSTANCE hInstance;
    bool should_close;
    bool mouse_tracked;
    void *user_data;
    WindowEventCallback event_callback;
}Win32Window, Window;

static WindowEvent window_msg_to_event(Win32Window *window, HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    Win32Window *window = GetProp(hwnd, "CORE_WINDOW");
    if(window) {
        WindowEvent event = window_msg_to_event(window, hwnd, msg, wParam, lParam);
        if(event.kind != WINDOW_EVENT_NONE) {
            (window->event_callback)((WindowHandle)window, event, window->user_data);
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

WindowHandle window_create_opt(WindowCreateArgs args) {
    HINSTANCE hinstance = GetModuleHandle(NULL);
    if(!hinstance) {
        return NULL;
    }

    UINT style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    if(FLAG_HAS(args.style, WINDOW_NOT_MOVEABLE)) {

    }
    if(FLAG_HAS(args.style, WINDOW_NOT_RESIZEABLE)) {
        style |= WS_MINIMIZEBOX | WS_SIZEBOX;
    }

    WNDCLASSEX wc = {
        .cbSize = sizeof(WNDCLASSEX),
        .style = style,
        .lpfnWndProc = WndProc,
        .hInstance = hinstance,
        .lpszClassName = args.name.data,
    };

    if(!RegisterClassEx(&wc)) {
        return NULL;
    }

    HWND hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        args.name.data,
        args.name.data,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        args.width,
        args.height,
        NULL,
        NULL,
        hinstance,
        NULL
    );

    if(!hwnd) {
        return NULL;
    }

    ShowWindow(hwnd, SW_SHOWNORMAL);
    UpdateWindow(hwnd);

    CORE_ASSERT(args.event_callback && "event_callback cannot be NULL");

    Win32Window *self = allocator_alloc(&default_allocator, sizeof(Win32Window));
    *self = (Win32Window) {
        .hwnd = hwnd,
        .hInstance = hinstance,
        .should_close = false,
        .mouse_tracked = false,
        .user_data = args.user_data,
        .event_callback = args.event_callback,
    };
    SetProp(self->hwnd, "CORE_WINDOW", self);
    return self;
}

void window_destroy(WindowHandle window) {
    Win32Window *self = window;
    RemoveProp(self->hwnd, "CORE_WINDOW");
    DestroyWindow(self->hwnd);
    allocator_free(&default_allocator, self);
}

void window_make_current(WindowHandle window) { CORE_UNUSED(window);}

bool window_should_close(WindowHandle window) {
    return ((Win32Window *)window)->should_close;
}

//void window_clear_flag(Win32Window *window, WindowStyle flag) {}
//void window_set_flag(Win32Window *window, WindowStyle flag) {}

static i32 get_key_mods(void) {
    i32 mods = 0;

    if (GetKeyState(VK_SHIFT) & 0x8000)
        mods |= MOD_SHIFT;
    if (GetKeyState(VK_CONTROL) & 0x8000)
        mods |= MOD_CONTROL;
    if (GetKeyState(VK_MENU) & 0x8000)
        mods |= MOD_ALT;
    if ((GetKeyState(VK_LWIN) | GetKeyState(VK_RWIN)) & 0x8000)
        mods |= MOD_SUPER;
    if (GetKeyState(VK_CAPITAL) & 1)
        mods |= MOD_CAPS_LOCK;
    if (GetKeyState(VK_NUMLOCK) & 1)
        mods |= MOD_NUM_LOCK;

    return mods;
}

static i32 win32_get_x_lparam(LPARAM lParam) {
    return (i32)(i16)LOWORD(lParam);
}

static i32 win32_get_y_lparam(LPARAM lParam) {
    return (i32)(i16)HIWORD(lParam);
}

static f32 win32_get_wheel_delta(WPARAM wParam) {
    return (f32)GET_WHEEL_DELTA_WPARAM(wParam) / (f32)WHEEL_DELTA;
}

static WindowEvent window_msg_to_event(Win32Window *window, HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    CORE_UNUSED(hwnd);

    //tables stolen from GLFW
    static Key keycodes[512] = {
        [0x00B] = KEY_0,
        [0x002] = KEY_1,
        [0x003] = KEY_2,
        [0x004] = KEY_3,
        [0x005] = KEY_4,
        [0x006] = KEY_5,
        [0x007] = KEY_6,
        [0x008] = KEY_7,
        [0x009] = KEY_8,
        [0x00A] = KEY_9,
        [0x01E] = KEY_A,
        [0x030] = KEY_B,
        [0x02E] = KEY_C,
        [0x020] = KEY_D,
        [0x012] = KEY_E,
        [0x021] = KEY_F,
        [0x022] = KEY_G,
        [0x023] = KEY_H,
        [0x017] = KEY_I,
        [0x024] = KEY_J,
        [0x025] = KEY_K,
        [0x026] = KEY_L,
        [0x032] = KEY_M,
        [0x031] = KEY_N,
        [0x018] = KEY_O,
        [0x019] = KEY_P,
        [0x010] = KEY_Q,
        [0x013] = KEY_R,
        [0x01F] = KEY_S,
        [0x014] = KEY_T,
        [0x016] = KEY_U,
        [0x02F] = KEY_V,
        [0x011] = KEY_W,
        [0x02D] = KEY_X,
        [0x015] = KEY_Y,
        [0x02C] = KEY_Z,

        [0x028] = KEY_APOSTROPHE,
        [0x02B] = KEY_BACKSLASH,
        [0x033] = KEY_COMMA,
        [0x00D] = KEY_EQUAL,
        [0x029] = KEY_GRAVE_ACCENT,
        [0x01A] = KEY_LEFT_BRACKET,
        [0x00C] = KEY_MINUS,
        [0x034] = KEY_PERIOD,
        [0x01B] = KEY_RIGHT_BRACKET,
        [0x027] = KEY_SEMICOLON,
        [0x035] = KEY_SLASH,
        [0x056] = KEY_WORLD_2,

        [0x00E] = KEY_BACKSPACE,
        [0x153] = KEY_DELETE,
        [0x14F] = KEY_END,
        [0x01C] = KEY_ENTER,
        [0x001] = KEY_ESCAPE,
        [0x147] = KEY_HOME,
        [0x152] = KEY_INSERT,
        [0x15D] = KEY_MENU,
        [0x151] = KEY_PAGE_DOWN,
        [0x149] = KEY_PAGE_UP,
        [0x045] = KEY_PAUSE,
        [0x039] = KEY_SPACE,
        [0x00F] = KEY_TAB,
        [0x03A] = KEY_CAPS_LOCK,
        [0x145] = KEY_NUM_LOCK,
        [0x046] = KEY_SCROLL_LOCK,
        [0x03B] = KEY_F1,
        [0x03C] = KEY_F2,
        [0x03D] = KEY_F3,
        [0x03E] = KEY_F4,
        [0x03F] = KEY_F5,
        [0x040] = KEY_F6,
        [0x041] = KEY_F7,
        [0x042] = KEY_F8,
        [0x043] = KEY_F9,
        [0x044] = KEY_F10,
        [0x057] = KEY_F11,
        [0x058] = KEY_F12,
        [0x064] = KEY_F13,
        [0x065] = KEY_F14,
        [0x066] = KEY_F15,
        [0x067] = KEY_F16,
        [0x068] = KEY_F17,
        [0x069] = KEY_F18,
        [0x06A] = KEY_F19,
        [0x06B] = KEY_F20,
        [0x06C] = KEY_F21,
        [0x06D] = KEY_F22,
        [0x06E] = KEY_F23,
        [0x076] = KEY_F24,
        [0x038] = KEY_LEFT_ALT,
        [0x01D] = KEY_LEFT_CONTROL,
        [0x02A] = KEY_LEFT_SHIFT,
        [0x15B] = KEY_LEFT_SUPER,
        [0x137] = KEY_PRINT_SCREEN,
        [0x138] = KEY_RIGHT_ALT,
        [0x11D] = KEY_RIGHT_CONTROL,
        [0x036] = KEY_RIGHT_SHIFT,
        [0x15C] = KEY_RIGHT_SUPER,
        [0x150] = KEY_DOWN,
        [0x14B] = KEY_LEFT,
        [0x14D] = KEY_RIGHT,
        [0x148] = KEY_UP,

        [0x052] = KEY_KP_0,
        [0x04F] = KEY_KP_1,
        [0x050] = KEY_KP_2,
        [0x051] = KEY_KP_3,
        [0x04B] = KEY_KP_4,
        [0x04C] = KEY_KP_5,
        [0x04D] = KEY_KP_6,
        [0x047] = KEY_KP_7,
        [0x048] = KEY_KP_8,
        [0x049] = KEY_KP_9,
        [0x04E] = KEY_KP_ADD,
        [0x053] = KEY_KP_DECIMAL,
        [0x135] = KEY_KP_DIVIDE,
        [0x11C] = KEY_KP_ENTER,
        [0x059] = KEY_KP_EQUAL,
        [0x037] = KEY_KP_MULTIPLY,
        [0x04A] = KEY_KP_SUBTRACT,
    };

    /*for (scancode = 0;  scancode < 512;  scancode++)
    {
        if (keycodes[scancode] > 0)
            _glfw.win32.scancodes[keycodes[scancode]] = scancode;
    }*/

    switch(msg)
    {
        case WM_CLOSE: {
            window->should_close = true;
            return (WindowEvent){ .kind = WINDOW_EVENT_CLOSE };
        }break;
        case WM_CREATE: {
            return (WindowEvent){ .kind = WINDOW_EVENT_CREATED };
        }break;
        case WM_QUIT: {
            window->should_close = true;
            return (WindowEvent){ .kind = WINDOW_EVENT_CLOSE_REQUESTED };
        }break;
        case WM_DESTROY: {
            window->should_close = true;
            PostQuitMessage(0);
            return (WindowEvent){ .kind = WINDOW_EVENT_CLOSE };
        }break;
        case WM_SETFOCUS: {
            return (WindowEvent){ .kind = WINDOW_EVENT_FOCUS_GAINED };
        }break;
        case WM_KILLFOCUS: {
            return (WindowEvent){ .kind = WINDOW_EVENT_FOCUS_LOST };
        }break;
        case WM_LBUTTONDOWN: {
            return window_event_mouse(MOUSE_BUTTON_LEFT, KEY_STATE_DOWN, 1);
        }break;
        case WM_LBUTTONUP: {
            return window_event_mouse(MOUSE_BUTTON_LEFT, KEY_STATE_UP, 1);
        }break;
        case WM_LBUTTONDBLCLK: {
            return window_event_double_click(MOUSE_BUTTON_LEFT);
        }break;
        case WM_RBUTTONDOWN: {
            return window_event_mouse(MOUSE_BUTTON_RIGHT, KEY_STATE_DOWN, 1);
        }break;
        case WM_RBUTTONUP: {
            return window_event_mouse(MOUSE_BUTTON_RIGHT, KEY_STATE_UP, 1);
        }break;
        case WM_RBUTTONDBLCLK: {
            return window_event_double_click(MOUSE_BUTTON_RIGHT);
        }break;
        case WM_MBUTTONDOWN: {
            return window_event_mouse(MOUSE_BUTTON_MIDDLE, KEY_STATE_DOWN, 1);
        }break;
        case WM_MBUTTONUP: {
            return window_event_mouse(MOUSE_BUTTON_MIDDLE, KEY_STATE_UP, 1);
        }break;
        case WM_MBUTTONDBLCLK: {
            return window_event_double_click(MOUSE_BUTTON_MIDDLE);
        }break;
        case WM_XBUTTONDOWN: {
            MouseButton button = HIWORD(wParam) == XBUTTON1 ? MOUSE_BUTTON_4 : MOUSE_BUTTON_5;
            return window_event_mouse(button, KEY_STATE_DOWN, 1);
        }break;
        case WM_XBUTTONUP: {
            MouseButton button = HIWORD(wParam) == XBUTTON1 ? MOUSE_BUTTON_4 : MOUSE_BUTTON_5;
            return window_event_mouse(button, KEY_STATE_UP, 1);
        }break;
        case WM_XBUTTONDBLCLK: {
            MouseButton button = HIWORD(wParam) == XBUTTON1 ? MOUSE_BUTTON_4 : MOUSE_BUTTON_5;
            return window_event_double_click(button);
        }break;
        case WM_KEYUP:
        case WM_SYSKEYUP:
        case WM_SYSKEYDOWN:
        case WM_KEYDOWN: {
            const KeyState action = (HIWORD(lParam) & KF_UP) ? KEY_STATE_UP : KEY_STATE_DOWN;
            const i32 mods = get_key_mods();
            i32 scancode = (HIWORD(lParam) & (KF_EXTENDED | 0xff));
            if(!scancode) {
                scancode = MapVirtualKeyW((UINT) wParam, MAPVK_VK_TO_VSC);
            }
            if (scancode == 0x54)
                scancode = 0x137;

            if (scancode == 0x146)
                scancode = 0x45;

            if (scancode == 0x136)
                scancode = 0x36;
            const Key key = keycodes[scancode];

            return window_event_key(key, action, mods);
        }break;
        case WM_CHAR: {
            return window_event_char((u32)wParam);
        }break;
        case WM_UNICHAR: {
            if(wParam == UNICODE_NOCHAR) {
                return (WindowEvent){0};
            }
            return window_event_char((u32)wParam);
        }break;
        case WM_MOUSEWHEEL: {
            return window_event_scroll(0.0f, win32_get_wheel_delta(wParam));
        }break;
        case WM_MOUSEHWHEEL: {
            return window_event_scroll(win32_get_wheel_delta(wParam), 0.0f);
        }break;
        case WM_SIZE: {
            if(wParam == SIZE_MINIMIZED) {
                return (WindowEvent){ .kind = WINDOW_EVENT_MINIMIZED };
            }
            if(wParam == SIZE_MAXIMIZED) {
                return (WindowEvent){ .kind = WINDOW_EVENT_MAXIMIZED };
            }
            if(wParam == SIZE_RESTORED) {
                return (WindowEvent){ .kind = WINDOW_EVENT_RESTORED };
            }
            return window_event_resize((i32)LOWORD(lParam), (i32)HIWORD(lParam));
        }break;
        case WM_MOVE: {
            return window_event_move(win32_get_x_lparam(lParam), win32_get_y_lparam(lParam));
        }break;
        case WM_MOUSEMOVE: {
            if(!window->mouse_tracked) {
                TRACKMOUSEEVENT track = {
                    .cbSize = sizeof(track),
                    .dwFlags = TME_LEAVE,
                    .hwndTrack = hwnd,
                };
                if(TrackMouseEvent(&track)) {
                    window->mouse_tracked = true;
                    return (WindowEvent){ .kind = WINDOW_EVENT_MOUSE_ENTER };
                }
            }
            return window_event_mouse_move(win32_get_x_lparam(lParam), win32_get_y_lparam(lParam));
        }break;
        case WM_MOUSELEAVE: {
            window->mouse_tracked = false;
            return (WindowEvent){ .kind = WINDOW_EVENT_MOUSE_LEAVE };
        }break;
        default: {
            return (WindowEvent){0};
        }break;
    }
    return (WindowEvent){0};
}

bool window_wait_events(void) {
    return WaitMessage();
}

bool window_poll_events(WindowHandle window) {
    MSG msg = {0};
    bool polled = false;
    while(PeekMessage(&msg, ((Win32Window *)window)->hwnd, 0, 0, PM_REMOVE)) {
        polled = true;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return polled;
}

WindowEvent window_event_mouse(MouseButton button, KeyState state, size_t click_count) {
    return (WindowEvent) {
        .kind = WINDOW_EVENT_MOUSE,
        .mouse = {
            .button = button,
            .state = state,
            .click_count = click_count,
        }
    };
}

WindowEvent window_event_double_click(MouseButton button) {
    return (WindowEvent) {
        .kind = WINDOW_EVENT_MOUSE_DOUBLE_CLICK,
        .mouse_double_click = {
            .button = button,
        }
    };
}

WindowEvent window_event_mouse_move(i32 x, i32 y) {
    return (WindowEvent) {
        .kind = WINDOW_EVENT_MOUSE_MOVE,
        .mouse_move = {x, y}
    };
}

WindowEvent window_event_scroll(f32 x, f32 y) {
    return (WindowEvent) {
        .kind = WINDOW_EVENT_SCROLL,
        .scroll = {x, y}
    };
}

WindowEvent window_event_key(Key key, KeyState state, i32 mods) {
    return (WindowEvent) {
        .kind = WINDOW_EVENT_KEY,
        .key = {key, state, mods}
    };
}

WindowEvent window_event_char(u32 codepoint) {
    return (WindowEvent) {
        .kind = WINDOW_EVENT_CHAR,
        .character = {codepoint}
    };
}

WindowEvent window_event_resize(i32 width, i32 height) {
    return (WindowEvent) {
        .kind = WINDOW_EVENT_RESIZED,
        .resized = {
            .w = width,
            .h = height,
        }
    };
}

WindowEvent window_event_move(i32 x, i32 y) {
    return (WindowEvent) {
        .kind = WINDOW_EVENT_MOVED,
        .moved = {x, y}
    };
}

void window_event_print(WindowEvent *event) {
    static const char *btn_tbl[] = { "Left", "Right", "Middle", "Button4", "Button5", };
    static const char *key_tbl[] = {
        [32] = "KEY_SPACE",
        [39] = "KEY_APOSTROPHE",  /* ' */
        [44] = "KEY_COMMA",  /* , */
        [45] = "KEY_MINUS",  /* - */
        [46] = "KEY_PERIOD",  /* . */
        [47] = "KEY_SLASH",  /* / */
        [48] = "KEY_0",
        [49] = "KEY_1",
        [50] = "KEY_2",
        [51] = "KEY_3",
        [52] = "KEY_4",
        [53] = "KEY_5",
        [54] = "KEY_6",
        [55] = "KEY_7",
        [56] = "KEY_8",
        [57] = "KEY_9",
        [59] = "KEY_SEMICOLON",  /* ; */
        [61] = "KEY_EQUAL",  /* = */
        [65] = "KEY_A",
        [66] = "KEY_B",
        [67] = "KEY_C",
        [68] = "KEY_D",
        [69] = "KEY_E",
        [70] = "KEY_F",
        [71] = "KEY_G",
        [72] = "KEY_H",
        [73] = "KEY_I",
        [74] = "KEY_J",
        [75] = "KEY_K",
        [76] = "KEY_L",
        [77] = "KEY_M",
        [78] = "KEY_N",
        [79] = "KEY_O",
        [80] = "KEY_P",
        [81] = "KEY_Q",
        [82] = "KEY_R",
        [83] = "KEY_S",
        [84] = "KEY_T",
        [85] = "KEY_U",
        [86] = "KEY_V",
        [87] = "KEY_W",
        [88] = "KEY_X",
        [89] = "KEY_Y",
        [90] = "KEY_Z",
        [91] = "KEY_LEFT_BRACKET",  /* [ */
        [92] = "KEY_BACKSLASH",  /* \ */
        [93] = "KEY_RIGHT_BRACKET",  /* ] */
        [96] = "KEY_GRAVE_ACCENT",  /* ` */
        [161] = "KEY_WORLD_1", /* non-US #1 */
        [162] = "KEY_WORLD_2", /* non-US #2 */
        [256] = "KEY_ESCAPE",
        [257] = "KEY_ENTER",
        [258] = "KEY_TAB",
        [259] = "KEY_BACKSPACE",
        [260] = "KEY_INSERT",
        [261] = "KEY_DELETE",
        [262] = "KEY_RIGHT",
        [263] = "KEY_LEFT",
        [264] = "KEY_DOWN",
        [265] = "KEY_UP",
        [266] = "KEY_PAGE_UP",
        [267] = "KEY_PAGE_DOWN",
        [268] = "KEY_HOME",
        [269] = "KEY_END",
        [280] = "KEY_CAPS_LOCK",
        [281] = "KEY_SCROLL_LOCK",
        [282] = "KEY_NUM_LOCK",
        [283] = "KEY_PRINT_SCREEN",
        [284] = "KEY_PAUSE",
        [290] = "KEY_F1",
        [291] = "KEY_F2",
        [292] = "KEY_F3",
        [293] = "KEY_F4",
        [294] = "KEY_F5",
        [295] = "KEY_F6",
        [296] = "KEY_F7",
        [297] = "KEY_F8",
        [298] = "KEY_F9",
        [299] = "KEY_F10",
        [300] = "KEY_F11",
        [301] = "KEY_F12",
        [302] = "KEY_F13",
        [303] = "KEY_F14",
        [304] = "KEY_F15",
        [305] = "KEY_F16",
        [306] = "KEY_F17",
        [307] = "KEY_F18",
        [308] = "KEY_F19",
        [309] = "KEY_F20",
        [310] = "KEY_F21",
        [311] = "KEY_F22",
        [312] = "KEY_F23",
        [313] = "KEY_F24",
        [314] = "KEY_F25",
        [320] = "KEY_KP_0",
        [321] = "KEY_KP_1",
        [322] = "KEY_KP_2",
        [323] = "KEY_KP_3",
        [324] = "KEY_KP_4",
        [325] = "KEY_KP_5",
        [326] = "KEY_KP_6",
        [327] = "KEY_KP_7",
        [328] = "KEY_KP_8",
        [329] = "KEY_KP_9",
        [330] = "KEY_KP_DECIMAL",
        [331] = "KEY_KP_DIVIDE",
        [332] = "KEY_KP_MULTIPLY",
        [333] = "KEY_KP_SUBTRACT",
        [334] = "KEY_KP_ADD",
        [335] = "KEY_KP_ENTER",
        [336] = "KEY_KP_EQUAL",
        [340] = "KEY_LEFT_SHIFT",
        [341] = "KEY_LEFT_CONTROL",
        [342] = "KEY_LEFT_ALT",
        [343] = "KEY_LEFT_SUPER",
        [344] = "KEY_RIGHT_SHIFT",
        [345] = "KEY_RIGHT_CONTROL",
        [346] = "KEY_RIGHT_ALT",
        [347] = "KEY_RIGHT_SUPER",
        [348] = "KEY_MENU",
    };
    switch (event->kind) {
        case WINDOW_EVENT_MOUSE: {
            println("Mouse { btn: %s, state: %s, click_count: %zu }", btn_tbl[event->mouse.button], event->mouse.state == KEY_STATE_DOWN ? "Down" : "Up", event->mouse.click_count);
        }break;
        case WINDOW_EVENT_MOUSE_DOUBLE_CLICK: {
            println("MouseDoubleClick { btn: %s }", btn_tbl[event->mouse_double_click.button]);
        }break;
        case WINDOW_EVENT_MOUSE_MOVE: {
            println("MouseMove { x: %d, y: %d }", event->mouse_move.x, event->mouse_move.y);
        }break;
        case WINDOW_EVENT_MOUSE_ENTER: {
            println("MouseEnter {}");
        }break;
        case WINDOW_EVENT_MOUSE_LEAVE: {
            println("MouseLeave {}");
        }break;
        case WINDOW_EVENT_SCROLL: {
            println("Scroll { x: %f, y: %f }", event->scroll.x, event->scroll.y);
        }break;
        case WINDOW_EVENT_KEY: {
            const char *key_name = event->key.key >= 0 && event->key.key <= KEY_LAST ? key_tbl[event->key.key] : NULL;
            println("Key { key: %s, state: %s, mods: %d }", key_name ? key_name : "KEY_UNKNOWN", event->key.state == KEY_STATE_DOWN ? "Down" : "Up", event->key.mods);
        }break;
        case WINDOW_EVENT_CHAR: {
            println("Char { codepoint: %u }", event->character.codepoint);
        }break;
        case WINDOW_EVENT_RESIZED: {
            println("Resize { w: %d, h: %d }", event->resized.w, event->resized.h);
        }break;
        case WINDOW_EVENT_MOVED: {
            println("Moved { x: %d, y: %d }", event->moved.x, event->moved.y);
        }break;
        case WINDOW_EVENT_MINIMIZED: {
            println("Minimized {}");
        }break;
        case WINDOW_EVENT_MAXIMIZED: {
            println("Maximized {}");
        }break;
        case WINDOW_EVENT_RESTORED: {
            println("Restored {}");
        }break;
        case WINDOW_EVENT_CREATED: {
            println("Created {}");
        }break;
        case WINDOW_EVENT_FOCUS_GAINED: {
            println("FocusGained {}");
        }break;
        case WINDOW_EVENT_FOCUS_LOST: {
            println("FocusLost {}");
        }break;
        case WINDOW_EVENT_CLOSE: {
            println("Close {}");
        }break;
        case WINDOW_EVENT_CLOSE_REQUESTED: {
            println("CloseRequested {}");
        }break;
        default: return;
    }
}

struct WindowMouseEvent *window_mouse_event(WindowEvent *event) {
    if(event->kind != WINDOW_EVENT_MOUSE) {
        return NULL;
    }
    return &event->mouse;
}

#elif defined(PLATFORM_POSIX)

#endif //PLATFORM_WIN32

#ifdef __cplusplus
}
#endif

#endif //_CORE_GFX_H_
