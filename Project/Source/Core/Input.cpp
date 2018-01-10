/* All content(c) 2017 DigiPen(USA) Corporation, all rights reserved. */
/*****************************************************************************/
/*!
\file Input.cpp
\author Connor Deakin
\par E-mail: connor.deakin\@digipen.edu
\par Project: Onyx
\date 02/10/2017
\brief
  Implementation of the Input class.
*/
/*****************************************************************************/
// S_INPUT ////////////////////////////////////////////////////////////////////

#include "Input.h"

// static initializations
bool Input::_keysDown[NUMKEYS] = { false };
std::vector<Key> Input::_keysPressed;
bool Input::_mouseButtonsDown[NUMMBUTTONS] = { false };
std::vector<MButton> Input::_mouseButtonsPressed;
std::pair<int, int> Input::_mouseMotion(0, 0);
std::pair<int, int> Input::_mouseLocation(0, 0);
int Input::_mouseWheelMotion = 0;
std::vector<Sint32> Input::_inactiveController;
std::vector<Input::Controller> Input::_activeController;
float Input::_analogThreshold = 0.1f;


/*****************************************************************************/
/*!
\brief
Used to find out if a key was down during the previous frame.

\param key
The key that is being checked.

\return If the key was down, true.
*/
/*****************************************************************************/
bool Input::KeyDown(Key key)
{
  return _keysDown[key];
}

/*****************************************************************************/
/*!
\brief
Used to find out if a key was pressed during the previous frame.

\param key
The key that is being checked.

\return If the key was pressed, true.
*/
/*****************************************************************************/
bool Input::KeyPressed(Key key)
{
  size_t size = _keysPressed.size();
  for (unsigned i = 0; i < size; ++i) {
    if (_keysPressed[i] == key)
      return true;
  }
  return false;
}

/*****************************************************************************/
/*!
\brief
Used to find out if any key on the keyboard was pressed during the previous
frame.

\return If a key was pressed, true.
*/
/*****************************************************************************/
bool Input::AnyKeyPressed()
{
  if (_keysPressed.size() > 0)
    return true;
  return false;
}

/*****************************************************************************/
/*!
\brief
Used to find out if a mouse button was down during the previous frame.

\param mouse_button
The mouse button that is being checked.

\return If the mouse button was down, true.
*/
/*****************************************************************************/
bool Input::MouseButtonDown(MButton mouse_button)
{
  return _mouseButtonsDown[mouse_button];
}

/*****************************************************************************/
/*!
\brief
Used to find out if a mouse button was pressed during the previous frame.

\param mouse_button
The mouse button that is being checked.

\return If the mouse button was pressed, true.
*/
/*****************************************************************************/
bool Input::MouseButtonPressed(MButton mouse_button)
{
  size_t size = _mouseButtonsPressed.size();
  for (unsigned i = 0; i < size; ++i) {
    if (_mouseButtonsPressed[i] == mouse_button)
      return true;
  }
  return false;
}

/*****************************************************************************/
/*!
\brief
Used to check if any mouse button was pressed during the previous frame.

\return If a mouse button was pressed, true.
*/
/*****************************************************************************/
bool Input::AnyMouseButtonPressed()
{
  if (_mouseButtonsPressed.size() > 0)
    return true;
  return false;
}

/*****************************************************************************/
/*!
\brief
Returns the motion undergone by the mouse during the previous frame. The
first value in the pair is the X motion and the second value is the Y motion.

\return A pair containing the motion of the mouse during the previous frame.
*/
/*****************************************************************************/
const std::pair<int, int> & Input::MouseMotion()
{
  return _mouseMotion;
}

/*****************************************************************************/
/*!
\brief
Returns the location of the mouse during the previous frame. The first value
in the pair is the X location and the second value is the Y location.

\return A pair containing the location of the mouse during the previous frame.
*/
/*****************************************************************************/
const std::pair<int, int> & Input::MouseLocation()
{
  return _mouseLocation;
}

/*****************************************************************************/
/*!
\brief
Returns the motion undergone by the mouse wheel during the previous frame.
Motion away from the user will result in a positive one and motion towards
the user will result in a negative one.

\return Described in the brief.
*/
/*****************************************************************************/
int Input::MouseWheelMotion()
{
  return _mouseWheelMotion;
}

/*****************************************************************************/
/*!
\brief
Returns the lowest inactive controller. For example, if controllers 1, 2,
and 3 are inactive, controller 1 will be returned. If there are no inactive
controllers, -1 will be returned.

\return The id of the inactive controller.
*/
/*****************************************************************************/
Sint32 Input::GetInactiveController()
{
  // making sure there are inactive controllers.
  if (_inactiveController.size() == 0)
    return -1;
  const Sint32 max_controllers = 4;
  Sint32 id = max_controllers;
  for (Sint32 c_id : _inactiveController) {
    if (c_id < id)
      id = c_id;
  }
  return id;
}

/*****************************************************************************/
/*!
\brief
Find out whether a controller is active or not through an id. This id
will be a number from 0 to 3.

\param id The id of the controller.

\return If the controller is active, true, otherwise, false.
*/
/*****************************************************************************/
bool Input::IsActiveControlller(Sint32 id)
{
  for (const Controller & controller : _activeController) {
    if (controller._id == id)
      return true;
  }
  return false;
}

/*****************************************************************************/
/*!
\brief
Activates then specified controller. Throws an error if the controller
id did not exist among the current inactive controllers.

\param id The id that was given by GetInactiveController.
*/
/*****************************************************************************/
void Input::ActivateController(Sint32 id)
{
  std::vector<Sint32>::iterator it = _inactiveController.begin();
  std::vector<Sint32>::iterator it_e = _inactiveController.end();
  for (; it != it_e; ++it) {
    if (*it == id) {
      SDL_GameController * controller = SDL_GameControllerOpen(id);
      _inactiveController.erase(it);
      _activeController.push_back(Controller(id, controller));
      return;
    }
  }
  InputError error(InputError::CONTROLLER_ERROR,
    "The controller ID was not present among the inactive controller IDS.");
  throw(error);
}

/*****************************************************************************/
/*!
\brief
Deactivates a specified controller. An error is thrown if the given id is
not found among the active controllers.

\param id The id of the active controller that is to be deactivated.
*/
/*****************************************************************************/
void Input::DeactiveateController(Sint32 id)
{
  std::vector<Controller>::iterator it = _activeController.begin();
  std::vector<Controller>::iterator it_e = _activeController.end();
  for (; it != it_e; ++it) {
    if (id == (*it)._id) {
      SDL_GameControllerClose((*it)._sdlController);
      _activeController.erase(it);
      _inactiveController.push_back(id);
      return;
    }
  }
  InputError error(InputError::CONTROLLER_ERROR,
    "The controller ID was not present among the active controller IDs.");
  throw(error);
}


/*****************************************************************************/
/*!
\brief
Returns the Controller instance of a controller given a specific id for that
controller. An error will be thrown if the id does not exist among the active
controllers.

\param id The id of the controller to be fetched.

\return The controller instance.
*/
/*****************************************************************************/
Input::Controller * Input::GetController(Sint32 id)
{
  for (Controller & controller : _activeController) {
    if (id == controller._id)
      return &controller;
  }
  InputError error(InputError::CONTROLLER_ERROR,
    "The requested controller ID was not found among active controllers");
  throw(error);
}

/*****************************************************************************/
/*!
\brief
Identfies whether a button was down during the previous frame or not.

\param button
The button being evaluated.

\return If the button was down, true.
*/
/*****************************************************************************/
bool Input::Controller::ButtonDown(CButton button) const
{
  return _buttonsDown[button];
}

/*****************************************************************************/
/*!
\brief
Identifies whether a button was pressed during the previous frame.

\param button
The button being evaluated.

\return If the button was pressed. true
*/
/*****************************************************************************/
bool Input::Controller::ButtonPressed(CButton button) const
{
  for (CButton pressed_button : _buttonsPressed) {
    if (pressed_button == button)
      return true;
  }
  return false;
}

/*****************************************************************************/
/*!
\brief
Gets the raw analog value of an analog during the previous frame.

\param analog The analog whose value will be returned.

\return The analog's raw value.
*/
/*****************************************************************************/
Sint16 Input::Controller::AnalogValue(CAnalog analog) const
{
  return _analogs[analog];
}

/*****************************************************************************/
/*!
\brief
Identifies whether an anlog is beyond the analog threshold, therefore
making the analog a valid input.

\param analog The analog being checked for validity.

\return If the analog value is above the analog threshold, true.
*/
/*****************************************************************************/
bool Input::Controller::AnalogActive(CAnalog analog) const
{
  return _activeAnalogs[analog];
}

/*****************************************************************************/
/*!
\brief
The analog value of the left trigger. Will be given as a number from 0 to 1.

\return That floating analog value of the left trigger.
*/
/*****************************************************************************/
float Input::Controller::TriggerLeft() const
{
  return _triggerLeft;
}

/*****************************************************************************/
/*!
\brief
The analog value of the right trigger. Will be given as a number from 0 to 1.

\return That floating analog value of the right trigger.
*/
/*****************************************************************************/
float Input::Controller::TriggerRight() const
{
  return _triggerRight;
}

/*****************************************************************************/
/*!
\brief
Returns the floating point values associated with the left stick. These
values will be between 0 and 1.

\return The floating analog values. The first value is x, the second is y.
*/
/*****************************************************************************/
const std::pair<float, float> & Input::Controller::StickLeft() const
{
  return _stickLeft;
}

/*****************************************************************************/
/*!
\brief
Returns the floating point values associated with the right stick. These
values will be between 0 and 1.

\return The floating analog values. The first value is x, the second is y.
*/
/*****************************************************************************/
const std::pair<float, float> & Input::Controller::StickRight() const
{
  return _stickRight;
}



/*!
\brief Controller instance constuctor.
\param id The SDL id of the controller.
\param controller The SDL_GameController value.
*/
Input::Controller::Controller(Sint32 id, SDL_GameController * controller) :
  _id(id), _sdlController(controller), _buttonsDown{ false }, _analogs{ 0 },
  _triggerLeft(0.0f), _triggerRight(0.0f), _stickLeft(0.0f, 0.0f),
  _stickRight(0.0f, 0.0f), _activeAnalogs{ false }
{}

/*!
\brief Updates an analog value of the controller.
\param analog The analog being updated.
\param value The analog's new value.
*/
inline void Input::Controller::UpdateAnalog(CAnalog analog, Sint16 value)
{
  const float max_analog = 32767.0f;
  float norm_value = (float)value / max_analog;
  _analogs[analog] = value;
  switch (analog)
  {
  case TRIGGERLEFT: _triggerLeft = norm_value; break;
  case TRIGGERRIGHT: _triggerRight = norm_value; break;
  case STICKLEFTX: _stickLeft.first = norm_value; break;
  case STICKLEFTY: _stickLeft.second = -norm_value; break;
  case STICKRIGHTX: _stickRight.first = norm_value; break;
  case STICKRIGHTY: _stickRight.second = -norm_value; break;
  default: break;
  }
  if (norm_value > _analogThreshold || -norm_value > _analogThreshold)
    _activeAnalogs[analog] = true;
  else
    _activeAnalogs[analog] = false;
}

/*!
\brief Converts an SDL scancode to a Key.
\param value The SDL_Scancode value.
\return The SDL_Scancode's corresponding Key value.
*/
Key Input::ScancodeToKey(SDL_Scancode value)
{
  // not proud of this, but I also don't want to type
  // SDL_SCANCODE_ again.
  switch (value)
  {
  case SDL_SCANCODE_ESCAPE: return ESCAPE;
  case SDL_SCANCODE_F1: return F1;
  case SDL_SCANCODE_F2: return F2;
  case SDL_SCANCODE_F3: return F3;
  case SDL_SCANCODE_F4: return F4;
  case SDL_SCANCODE_F5: return F5;
  case SDL_SCANCODE_F6: return F6;
  case SDL_SCANCODE_F7: return F7;
  case SDL_SCANCODE_F8: return F8;
  case SDL_SCANCODE_F9: return F9;
  case SDL_SCANCODE_F10: return F10;
  case SDL_SCANCODE_F11: return F11;
  case SDL_SCANCODE_F12: return F12;
  case SDL_SCANCODE_GRAVE: return TILDE;
  case SDL_SCANCODE_1: return N1;
  case SDL_SCANCODE_2: return N2;
  case SDL_SCANCODE_3: return N3;
  case SDL_SCANCODE_4: return N4;
  case SDL_SCANCODE_5: return N5;
  case SDL_SCANCODE_6: return N6;
  case SDL_SCANCODE_7: return N7;
  case SDL_SCANCODE_8: return N8;
  case SDL_SCANCODE_9: return N9;
  case SDL_SCANCODE_0: return N0;
  case SDL_SCANCODE_MINUS: return MINUS;
  case SDL_SCANCODE_EQUALS: return EQUAL;
  case SDL_SCANCODE_BACKSPACE: return BACKSPACE;
  case SDL_SCANCODE_TAB: return TAB;
  case SDL_SCANCODE_Q: return Q;
  case SDL_SCANCODE_W: return W;
  case SDL_SCANCODE_E: return E;
  case SDL_SCANCODE_R: return R;
  case SDL_SCANCODE_T: return T;
  case SDL_SCANCODE_Y: return Y;
  case SDL_SCANCODE_U: return U;
  case SDL_SCANCODE_I: return I;
  case SDL_SCANCODE_O: return O;
  case SDL_SCANCODE_P: return P;
  case SDL_SCANCODE_LEFTBRACKET: return BRACKETLEFT;
  case SDL_SCANCODE_RIGHTBRACKET: return BRACKETRIGHT;
  case SDL_SCANCODE_BACKSLASH: return BACKSLASH;
  case SDL_SCANCODE_CAPSLOCK: return CAPSLOCK;
  case SDL_SCANCODE_A: return A;
  case SDL_SCANCODE_S: return S;
  case SDL_SCANCODE_D: return D;
  case SDL_SCANCODE_F: return F;
  case SDL_SCANCODE_G: return G;
  case SDL_SCANCODE_H: return H;
  case SDL_SCANCODE_J: return J;
  case SDL_SCANCODE_K: return K;
  case SDL_SCANCODE_L: return L;
  case SDL_SCANCODE_SEMICOLON: return SEMICOLON;
  case SDL_SCANCODE_APOSTROPHE: return APOSTROPHE;
  case SDL_SCANCODE_RETURN: return RETURN;
  case SDL_SCANCODE_LSHIFT: return SHIFTLEFT;
  case SDL_SCANCODE_Z: return Z;
  case SDL_SCANCODE_X: return X;
  case SDL_SCANCODE_C: return C;
  case SDL_SCANCODE_V: return V;
  case SDL_SCANCODE_B: return B;
  case SDL_SCANCODE_N: return N;
  case SDL_SCANCODE_M: return M;
  case SDL_SCANCODE_COMMA: return COMMA;
  case SDL_SCANCODE_PERIOD: return PERIOD;
  case SDL_SCANCODE_SLASH: return FOWARDSLASH;
  case SDL_SCANCODE_RSHIFT: return SHIFTRIGHT;
  case SDL_SCANCODE_LCTRL: return CTRLLEFT;
  case SDL_SCANCODE_LGUI: return GUILEFT;
  case SDL_SCANCODE_LALT: return ALTLEFT;
  case SDL_SCANCODE_SPACE: return SPACE;
  case SDL_SCANCODE_RALT: return ALTRIGHT;
  case SDL_SCANCODE_SELECT: return SELECT;
  case SDL_SCANCODE_RCTRL: return CTRLRIGHT;
  case SDL_SCANCODE_PRINTSCREEN: return PRINTSCREEN;
  case SDL_SCANCODE_SCROLLLOCK: return SCROLLLOCK;
  case SDL_SCANCODE_PAUSE: return PAUSE;
  case SDL_SCANCODE_INSERT: return INSERT;
  case SDL_SCANCODE_HOME: return HOME;
  case SDL_SCANCODE_PAGEUP: return PAGEUP;
  case SDL_SCANCODE_DELETE: return Key::DEL;
  case SDL_SCANCODE_END: return END;
  case SDL_SCANCODE_PAGEDOWN: return PAGEDOWN;
  case SDL_SCANCODE_UP: return ARROWUP;
  case SDL_SCANCODE_DOWN: return ARROWDOWN;
  case SDL_SCANCODE_LEFT: return ARROWLEFT;
  case SDL_SCANCODE_RIGHT: return ARROWRIGHT;
  default: return Key::OTHERKEY;
  }
}

/*!
\brief Converts an SDL unsigned value to an MButton.
\param value The unsigned int SDL uses to refer to a mouse button.
\return The corresponding MButton value.
*/
MButton Input::UnsignedToMButton(unsigned value)
{
  switch (value)
  {
  case SDL_BUTTON_LEFT: return LEFT;
  case SDL_BUTTON_MIDDLE: return MIDDLE;
  case SDL_BUTTON_RIGHT: return RIGHT;
  default: return MButton::OTHERMBUTTON;
  }
}

/*!
\brief Convert an SDL unsigned value to a CButton.
\param value The SDL unsigned value.
\return The corresponding CButton.
*/
CButton Input::UnsignedToCButton(unsigned value)
{
  switch (value)
  {
  case SDL_CONTROLLER_BUTTON_A: return EX;  break;
  case SDL_CONTROLLER_BUTTON_B: return CIRCLE; break;
  case SDL_CONTROLLER_BUTTON_X: return SQUARE; break;
  case SDL_CONTROLLER_BUTTON_Y: return TRIANGLE; break;
  case SDL_CONTROLLER_BUTTON_DPAD_UP: return DPADUP; break;
  case SDL_CONTROLLER_BUTTON_DPAD_DOWN: return DPADDOWN; break;
  case SDL_CONTROLLER_BUTTON_DPAD_LEFT: return DPADLEFT; break;
  case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: return DPADRIGHT; break;
  case SDL_CONTROLLER_BUTTON_LEFTSHOULDER: return BUMPERLEFT; break;
  case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: return BUMPERRIGHT; break;
  case SDL_CONTROLLER_BUTTON_LEFTSTICK: return STICKLEFT; break;
  case SDL_CONTROLLER_BUTTON_RIGHTSTICK: return STICKRIGHT; break;
  case SDL_CONTROLLER_BUTTON_BACK: return RESET; break;
  case SDL_CONTROLLER_BUTTON_START: return START; break;
  default: return OTHERCBUTTON; break;
  }
  return CButton::EX;
}

/*!
\brief Convert an SDL unsigned value to a CAnalog.
\param value The SDL unsigned value.
\return The corresponding CAnalog.
*/
CAnalog Input::UnsignedToCAnalgo(unsigned analog)
{
  switch (analog)
  {
  case SDL_CONTROLLER_AXIS_TRIGGERLEFT: return TRIGGERLEFT; break;
  case SDL_CONTROLLER_AXIS_TRIGGERRIGHT: return TRIGGERRIGHT; break;
  case SDL_CONTROLLER_AXIS_LEFTX: return STICKLEFTX; break;
  case SDL_CONTROLLER_AXIS_LEFTY: return STICKLEFTY; break;
  case SDL_CONTROLLER_AXIS_RIGHTX: return STICKRIGHTX; break;
  case SDL_CONTROLLER_AXIS_RIGHTY: return STICKRIGHTY; break;
  default: return OTHERCANALOG; break;
  }
}

/*****************************************************************************/
/*!
\brief
Resets values that should only be present for a single frame. This is called
but SDLContext::CheckEvents before events are checked for.
*/
/*****************************************************************************/
void Input::Reset()
{
  _keysPressed.clear();
  _mouseButtonsPressed.clear();
  _mouseMotion.first = 0;
  _mouseMotion.second = 0;
  _mouseWheelMotion = 0;
  for (Controller & controller : _activeController) {
    controller._buttonsPressed.clear();
  }
}


/*!
\brief Updates Input values for a corresponding SDL_KEYDOWN event.
\param event The SDL event.
*/
void Input::OnKeyDownEvent(const SDL_Event & event)
{
  if (event.key.repeat)
    return;
  Key value = ScancodeToKey(event.key.keysym.scancode);
  _keysDown[value] = true;
  _keysPressed.push_back(value);
}


/*!
\brief Updates Input values for a corresponding SDL_KEYUP event.
\param event The SDL event.
*/
void Input::OnKeyUpEvent(const SDL_Event & event)
{
  Key value = ScancodeToKey(event.key.keysym.scancode);
  _keysDown[value] = false;
}

/*!
\brief Updates Input values for a corresponding SDL_MOUSEBUTTONDOWN event.
\param event The SDL event.
*/
void Input::OnMouseButtonDownEvent(const SDL_Event & event)
{
  MButton value = UnsignedToMButton(event.button.button);
  _mouseButtonsDown[value] = true;
  _mouseButtonsPressed.push_back(value);
}

/*!
\brief Updates Input values for a corresponding SDL_MOUSEBUTTONUP event.
\param event The SDL event.
*/
void Input::OnMouseButtonUpEvent(const SDL_Event & event)
{
  MButton value = UnsignedToMButton(event.button.button);
  _mouseButtonsDown[value] = false;
}

/*!
\brief Updates Input values for a corresponding SDL_MOUSEMOTION event.
\param event The SDL event.
*/
void Input::OnMouseMotionEvent(const SDL_Event & event)
{
  _mouseMotion.first = event.motion.xrel;
  _mouseMotion.second = event.motion.yrel;
  _mouseLocation.first = event.motion.x;
  _mouseLocation.second = event.motion.y;
}

/*!
\brief Updates Input values for a corresponding SDL_MOUSEWHEEL event.
\param event The SDL event.
*/
void Input::OnMouseWheelEvent(const SDL_Event & event)
{
  _mouseWheelMotion = static_cast<int>(event.wheel.y);
}

/*!
\brief Adds a controller to the inactive controllers vector when it is added.
\param The SDL event.
*/
void Input::OnControllerAdded(const SDL_Event & event)
{
  _inactiveController.push_back(event.cdevice.which);
}

/*!
\brief Removes a controller from the input class on a remove controller event.
\param event The SDL event.
*/
void Input::OnControllerRemoved(const SDL_Event & event)
{
  if (IsActiveControlller(event.cdevice.which))
    DeactiveateController(event.cdevice.which);
  std::vector<Sint32>::iterator it = _inactiveController.begin();
  std::vector<Sint32>::iterator it_e = _inactiveController.end();
  for (; it != it_e; ++it) {
    if (*it == event.cdevice.which) {
      _inactiveController.erase(it);
      return;
    }
  }
}

/*!
\brief Updates a controllers instance's button states on a button down event.
\param event The SDL event.
*/
void Input::OnControllerDown(const SDL_Event & event)
{
  CButton button = UnsignedToCButton(event.cbutton.button);
  for (Controller & controller : _activeController) {
    if (event.cbutton.which == controller._id) {
      controller._buttonsDown[button] = true;
      controller._buttonsPressed.push_back(button);
      return;
    }
  }
}

/*!
\brief Called when a controller button is lifted.
\param event The SDL event.
*/
void Input::OnControllerUp(const SDL_Event & event)
{
  CButton button = UnsignedToCButton(event.cbutton.button);
  for (Controller & controller : _activeController) {
    if (event.cbutton.which == controller._id) {
      controller._buttonsDown[button] = false;
      return;
    }
  }
}

/*!
\brief Updates a changed controller analog.
\param event The SDL event.
*/
void Input::OnControllerAxis(const SDL_Event & event)
{
  CAnalog analog = UnsignedToCAnalgo(event.caxis.axis);
  for (Controller & controller : _activeController) {
    if (event.caxis.which == controller._id) {
      CAnalog analog = UnsignedToCAnalgo(event.caxis.axis);
      controller.UpdateAnalog(analog, event.caxis.value);
      return;
    }
  }
}

// S_INPUTERROR ///////////////////////////////////////////////////////////////

/*!
\brief Gets the InputError::Type of the error.
\return The type of the error.
*/
InputError::Type InputError::GetType() const
{
  return _type;
}

/*!
\brief Gets a description of the error.
\return The error's description.
*/
const std::string & InputError::GetDescription() const
{
  return _description;
}

/*!
\brief InputError Constructor.
\param type The type of the error being created.
\param description A description of the error.
*/
InputError::InputError(Type type, const std::string & description) :
  _type(type), _description(description)
{}