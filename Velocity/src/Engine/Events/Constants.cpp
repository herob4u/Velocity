#include "vctPCH.h"
#include "Constants.h"

#include "GLFW/glfw3.h"

using namespace Vct;

// KEY CODES
const int KeyCode::A = GLFW_KEY_A;
const int KeyCode::B = GLFW_KEY_B;
const int KeyCode::C = GLFW_KEY_C;
const int KeyCode::D = GLFW_KEY_D;
const int KeyCode::E = GLFW_KEY_E;
const int KeyCode::F = GLFW_KEY_F;
const int KeyCode::G = GLFW_KEY_G;
const int KeyCode::H = GLFW_KEY_H;
const int KeyCode::I = GLFW_KEY_I;
const int KeyCode::J = GLFW_KEY_J;
const int KeyCode::K = GLFW_KEY_K;
const int KeyCode::L = GLFW_KEY_L;
const int KeyCode::M = GLFW_KEY_M;
const int KeyCode::N = GLFW_KEY_N;
const int KeyCode::O = GLFW_KEY_O;
const int KeyCode::P = GLFW_KEY_P;
const int KeyCode::Q = GLFW_KEY_Q;
const int KeyCode::R = GLFW_KEY_R;
const int KeyCode::S = GLFW_KEY_S;
const int KeyCode::T = GLFW_KEY_T;
const int KeyCode::U = GLFW_KEY_U;
const int KeyCode::V = GLFW_KEY_V;
const int KeyCode::W = GLFW_KEY_W;
const int KeyCode::X = GLFW_KEY_X;
const int KeyCode::Y = GLFW_KEY_Y;
const int KeyCode::Z = GLFW_KEY_Z;

const int KeyCode::Space = GLFW_KEY_SPACE;
const int KeyCode::Aphostrophe = GLFW_KEY_APOSTROPHE;
const int KeyCode::Comma = GLFW_KEY_COMMA;
const int KeyCode::Minus = GLFW_KEY_MINUS;
const int KeyCode::Period = GLFW_KEY_PERIOD;
const int KeyCode::Slash = GLFW_KEY_SLASH;

const int KeyCode::NUM_0 = GLFW_KEY_0;
const int KeyCode::NUM_1 = GLFW_KEY_1;
const int KeyCode::NUM_2 = GLFW_KEY_2;
const int KeyCode::NUM_3 = GLFW_KEY_3;
const int KeyCode::NUM_4 = GLFW_KEY_4;
const int KeyCode::NUM_5 = GLFW_KEY_5;
const int KeyCode::NUM_6 = GLFW_KEY_6;
const int KeyCode::NUM_7 = GLFW_KEY_7;
const int KeyCode::NUM_8 = GLFW_KEY_8;
const int KeyCode::NUM_9 = GLFW_KEY_9;

const int KeyCode::Semicolon = GLFW_KEY_SEMICOLON;
const int KeyCode::Equal = GLFW_KEY_EQUAL;
const int KeyCode::LeftBracket = GLFW_KEY_LEFT_BRACKET;
const int KeyCode::Backslash = GLFW_KEY_BACKSLASH;
const int KeyCode::RightBracket = GLFW_KEY_RIGHT_BRACKET;
const int KeyCode::GraveAccent = GLFW_KEY_GRAVE_ACCENT;
const int KeyCode::World1 = GLFW_KEY_WORLD_1;
const int KeyCode::World2 = GLFW_KEY_WORLD_2;

const int KeyCode::Escape = GLFW_KEY_ESCAPE;
const int KeyCode::Enter = GLFW_KEY_ENTER;
const int KeyCode::Tab = GLFW_KEY_TAB;
const int KeyCode::Backspace = GLFW_KEY_BACKSPACE;
const int KeyCode::Insert = GLFW_KEY_INSERT;
const int KeyCode::Delete = GLFW_KEY_DELETE;
const int KeyCode::Right = GLFW_KEY_RIGHT;
const int KeyCode::Left = GLFW_KEY_LEFT;
const int KeyCode::Down = GLFW_KEY_DOWN;
const int KeyCode::Up = GLFW_KEY_UP;
const int KeyCode::PageUp = GLFW_KEY_PAGE_UP;
const int KeyCode::PageDown = GLFW_KEY_PAGE_DOWN;
const int KeyCode::Home = GLFW_KEY_HOME;
const int KeyCode::End = GLFW_KEY_END;
const int KeyCode::CapsLock = GLFW_KEY_CAPS_LOCK;
const int KeyCode::ScrollLock = GLFW_KEY_SCROLL_LOCK;
const int KeyCode::NumLock = GLFW_KEY_NUM_LOCK;
const int KeyCode::PrintScr = GLFW_KEY_PRINT_SCREEN;
const int KeyCode::Pause = GLFW_KEY_PAUSE;

const int KeyCode::F1 = GLFW_KEY_F1;
const int KeyCode::F2 = GLFW_KEY_F2;
const int KeyCode::F3 = GLFW_KEY_F3;
const int KeyCode::F4 = GLFW_KEY_F4;
const int KeyCode::F5 = GLFW_KEY_F5;
const int KeyCode::F6 = GLFW_KEY_F6;
const int KeyCode::F7 = GLFW_KEY_F7;
const int KeyCode::F8 = GLFW_KEY_F8;
const int KeyCode::F9 = GLFW_KEY_F9;
const int KeyCode::F10 = GLFW_KEY_F10;
const int KeyCode::F11 = GLFW_KEY_F11;
const int KeyCode::F12 = GLFW_KEY_F12;
const int KeyCode::F13 = GLFW_KEY_F13;
const int KeyCode::F14 = GLFW_KEY_F14;
const int KeyCode::F15 = GLFW_KEY_F15;
const int KeyCode::F16 = GLFW_KEY_F16;
const int KeyCode::F17 = GLFW_KEY_F17;
const int KeyCode::F18 = GLFW_KEY_F18;
const int KeyCode::F19 = GLFW_KEY_F19;
const int KeyCode::F20 = GLFW_KEY_F20;
const int KeyCode::F21 = GLFW_KEY_F21;
const int KeyCode::F22 = GLFW_KEY_F22;
const int KeyCode::F23 = GLFW_KEY_F23;
const int KeyCode::F24 = GLFW_KEY_F24;
const int KeyCode::F25 = GLFW_KEY_F25;

const int KeyCode::KEYPAD_0 = GLFW_KEY_KP_0;
const int KeyCode::KEYPAD_1 = GLFW_KEY_KP_1;
const int KeyCode::KEYPAD_2 = GLFW_KEY_KP_2;
const int KeyCode::KEYPAD_3 = GLFW_KEY_KP_3;
const int KeyCode::KEYPAD_4 = GLFW_KEY_KP_4;
const int KeyCode::KEYPAD_5 = GLFW_KEY_KP_5;
const int KeyCode::KEYPAD_6 = GLFW_KEY_KP_6;
const int KeyCode::KEYPAD_7 = GLFW_KEY_KP_7;
const int KeyCode::KEYPAD_8 = GLFW_KEY_KP_8;
const int KeyCode::KEYPAD_9 = GLFW_KEY_KP_9;
const int KeyCode::KEYPAD_Decimal = GLFW_KEY_KP_DECIMAL;
const int KeyCode::KEYPAD_Divide = GLFW_KEY_KP_DIVIDE;
const int KeyCode::KEYPAD_Multiply = GLFW_KEY_KP_MULTIPLY;
const int KeyCode::KEYPAD_Subtract = GLFW_KEY_KP_SUBTRACT;
const int KeyCode::KEYPAD_Add = GLFW_KEY_KP_ADD;
const int KeyCode::KEYPAD_Enter = GLFW_KEY_KP_ENTER;
const int KeyCode::KEYPAD_Equal = GLFW_KEY_KP_EQUAL;

const int KeyCode::LeftShift = GLFW_KEY_LEFT_SHIFT;
const int KeyCode::LeftControl = GLFW_KEY_LEFT_CONTROL;
const int KeyCode::LeftAlt = GLFW_KEY_LEFT_ALT;
const int KeyCode::LeftSuper = GLFW_KEY_LEFT_SUPER;
const int KeyCode::RightShift = GLFW_KEY_RIGHT_SHIFT;
const int KeyCode::RightControl = GLFW_KEY_RIGHT_CONTROL;
const int KeyCode::RightAlt = GLFW_KEY_RIGHT_ALT;
const int KeyCode::RightSuper = GLFW_KEY_RIGHT_SUPER;

const int KeyCode::Menu = GLFW_KEY_MENU;
const int KeyCode::Invalid = GLFW_KEY_UNKNOWN;


// Mouse Buttons
const int MouseButton::LeftMouse = GLFW_MOUSE_BUTTON_1;
const int MouseButton::RightMouse = GLFW_MOUSE_BUTTON_2;
const int MouseButton::MiddleMouse = GLFW_MOUSE_BUTTON_3;