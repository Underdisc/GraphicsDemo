/* All content(c) 2017 - 2018 DigiPen(USA) Corporation, all rights reserved. */
#pragma once

/*****************************************************************************/
/*!
\class Color
\brief
Used for storing rgb color values.
*/
/*****************************************************************************/
struct Color
{
  Color();
  Color(float x, float y, float z);
  Color(const Color & other);
  // The rgb value of the color
  union {
    float _values[3];
    struct {
      float _x, _y, _z;
    };
    struct {
      float _r, _g, _b;
    };
  };
};