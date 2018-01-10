/* All content(c) 2017 DigiPen(USA) Corporation, all rights reserved. */
/*****************************************************************************/
/*!
\file Input.h
\author Connor Deakin
\par E-mail: connor.deakin\@digipen.edu
\par Project: Onyx
\date 11/09/2017
\brief
  Contains the interface for the Input static class.
*/
/*****************************************************************************/
#pragma once
#ifndef INPUT_H
#define INPUT_H

// S_INPUT ////////////////////////////////////////////////////////////////////

#include <vector>
#include <utility>

#include "../Graphics/SDLContext.h"

//! All of the main keys that can be detected by the input class. 
// If a key is not listed  here, it will be registered as OTHERKEY.
enum Key
{
  ESCAPE, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
  TILDE, N1, N2, N3, N4, N5, N6, N7, N8, N9, N0, MINUS, EQUAL, BACKSPACE,
  TAB, Q, W, E, R, T, Y, U, I, O, P, BRACKETLEFT, BRACKETRIGHT, BACKSLASH,
  CAPSLOCK, A, S, D, F, G, H, J, K, L, SEMICOLON, APOSTROPHE, RETURN,
  SHIFTLEFT, Z, X, C, V, B, N, M, COMMA, PERIOD, FOWARDSLASH, SHIFTRIGHT,
  CTRLLEFT, GUILEFT, ALTLEFT, SPACE, ALTRIGHT, SELECT, CTRLRIGHT,
  PRINTSCREEN, SCROLLLOCK, PAUSE,
  INSERT, HOME, PAGEUP, DEL, END, PAGEDOWN,
  ARROWUP, ARROWDOWN, ARROWLEFT, ARROWRIGHT,
  OTHERKEY,
  NUMKEYS
};

extern const char* KeyStringNames[NUMKEYS];

//! All of the MouseButtons that can be detected by the input class. 
// If a button is not listed here, it will be registered as OTHERMBUTTON.
enum MButton
{
  LEFT,
  MIDDLE,
  RIGHT,
  OTHERMBUTTON,
  NUMMBUTTONS
};

//! All Controller buttons that can be detected by the input class.
enum CButton
{
  EX, CIRCLE, TRIANGLE, SQUARE,
  DPADUP, DPADDOWN, DPADLEFT, DPADRIGHT,
  BUMPERLEFT, BUMPERRIGHT,
  STICKLEFT, STICKRIGHT,
  RESET, START,
  OTHERCBUTTON,
  NUMCBUTTONS
};

//! All controller analogs that can be detected by the input class.
enum CAnalog
{
  TRIGGERLEFT, TRIGGERRIGHT,
  STICKLEFTX, STICKLEFTY, STICKRIGHTX, STICKRIGHTY,
  OTHERCANALOG,
  NUMCANALOGS
};

/*****************************************************************************/
/*!
\class Input
\brief
This is used to interface with the mouse, keyboard, and controller input
from the SDL context created by the SDLContext class. All of the keys, mouse
buttons, controller buttons, and controller analogs registered by this class
can be found in the Key, MButton, CButton, and CAnalog enums,
respectively.
*/
/*****************************************************************************/
class Input
{
public:
  static bool KeyDown(Key key);
  static bool KeyPressed(Key key);
  static bool AnyKeyPressed();
  static bool MouseButtonDown(MButton mouse_button);
  static bool MouseButtonPressed(MButton mouse_button);
  static bool AnyMouseButtonPressed();
  static const std::pair<int, int> & MouseMotion();
  static const std::pair<int, int> & MouseLocation();
  static int MouseWheelMotion();
public:
  /***************************************************************************/
  /*!
  \class Controller
  \brief
  Used for tracking the analog and input values on a single controller.
  This will also be the structer that is used to retrieve information about
  the inputs on a specific controller. Simply use the GetController function
  from the Input class.
  */
  /***************************************************************************/
  class Controller
  {
  public:
    bool ButtonDown(CButton button) const;
    bool ButtonPressed(CButton button) const;
    Sint16 AnalogValue(CAnalog analog) const;
    bool AnalogActive(CAnalog analog) const;
    float TriggerLeft() const;
    float TriggerRight() const;
    const std::pair<float, float> & StickLeft() const;
    const std::pair<float, float> & StickRight() const;
  private:
    Controller(Sint32 id, SDL_GameController * controller);
    void UpdateAnalog(CAnalog analog, Sint16 value);
    //! The SDL id for the controller.
    Sint32 _id;
    //! A pointer to the SDL game controller.
    SDL_GameController * _sdlController;
    //! Tracks which buttons are currently down on the controller.
    bool _buttonsDown[NUMCBUTTONS];
    //! Tracks which buttons were pressed during the previous frame.
    std::vector<CButton> _buttonsPressed;
    //! Tracks the analog values of all analogs on the controller.
    Sint16 _analogs[NUMCANALOGS];
    //! The analog value of the left trigger given as a value between 0 and 1.
    float _triggerLeft;
    //! The analog value of the right trigger given as a value between 0 and 1.
    float _triggerRight;
    //! Analog values of the left stick given as values between 0 and 1.
    std::pair<float, float> _stickLeft;
    //! Analog values of the right sitck given as values between 0 and 1.
    std::pair<float, float> _stickRight;
    //! Tracks which analogs are beyond the analog threshold (ANALOG_EPSILON).
    bool _activeAnalogs[NUMCANALOGS];
    friend Input;
  };
  static Sint32 GetInactiveController();
  static bool IsActiveControlller(Sint32 id);
  static void ActivateController(Sint32 id);
  static void DeactiveateController(Sint32 id);
  static Controller * GetController(Sint32 id);
private:
  Input();
  static Key ScancodeToKey(SDL_Scancode value);
  static MButton UnsignedToMButton(unsigned value);
  static CButton UnsignedToCButton(unsigned value);
  static CAnalog UnsignedToCAnalgo(unsigned sld_axis);
  static void Reset();
  static void OnKeyDownEvent(const SDL_Event & event);
  static void OnKeyUpEvent(const SDL_Event & event);
  static void OnMouseButtonDownEvent(const SDL_Event & event);
  static void OnMouseButtonUpEvent(const SDL_Event & event);
  static void OnMouseMotionEvent(const SDL_Event & event);
  static void OnMouseWheelEvent(const SDL_Event & event);
  static void OnControllerAdded(const SDL_Event & event);
  static void OnControllerRemoved(const SDL_Event & event);
  static void OnControllerDown(const SDL_Event & event);
  static void OnControllerUp(const SDL_Event & event);
  static void OnControllerAxis(const SDL_Event & event);
  //! Tracks which keys are down and which are not. True means that a key is
  // down.
  static bool _keysDown[NUMKEYS];
  //! Stores all of the keys that were pressed during the previous frame.
  // These values will be cleared and re-evaluated each time SDL events are
  // checked for.
  static std::vector<Key> _keysPressed;
  //! Tracks which mosue buttons are down. True means that the button is down.
  static bool _mouseButtonsDown[NUMMBUTTONS];
  //! This is the same as the _keyPressed vector, but is instead used for mouse
  // buttons.
  static std::vector<MButton> _mouseButtonsPressed;
  //! Stores the motion of the mouse during the previous frame.
  static std::pair<int, int> _mouseMotion;
  //! Stores the location of the mouse during the previous frame.
  static std::pair<int, int> _mouseLocation;
  //! Stores the motion undergone by the mouse wheel during the previous frame.
  static int _mouseWheelMotion;
  //! The ids of controllers that are available but have not yet been activate.
  static std::vector<Sint32> _inactiveController;
  //! The controllers that are active.
  static std::vector<Controller> _activeController;
  //! The value that an analog should be above in order to be considered 
  // active.
  static float _analogThreshold;
  //! When events are checked for, the CheckEvents function will first call the 
  //Reset function and then proceed to grab SDL events.
  friend SDLContext;
};

// S_INPUTERROR ///////////////////////////////////////////////////////////////

#include <string>

class InputError
{
public:
  enum Type
  {
    CONTROLLER_ERROR
  };
  Type GetType() const;
  const std::string & GetDescription() const;
private:
  InputError(Type error_type, const std::string & description);
  Type _type;
  std::string _description;
  friend Input;
};

#endif // !INPUT_H