/* All content(c) 2017 - 2018 DigiPen(USA) Corporation, all rights reserved. */
#include "Color.h"

Color::Color() : _x(0.0f), _y(0.0f), _z(0.0f) 
{}

Color::Color(float x, float y, float z) : _x(x), _y(y), _z(z) 
{}

Color::Color(const Color & other) : _x(other._x), _y(other._y), _z(other._z) 
{}