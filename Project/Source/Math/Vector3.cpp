///////////////////////////////////////////////////////////////////////////////
///
/// \file Vector3.cpp
/// Implementation of the Vector 3 structure.
/// 
/// Authors: Joshua Davis, Benjamin Strukus
/// Copyright 2010-2012, DigiPen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////
#include "../Other/Precompiled.h"
#include "../Other/Debug.h"
#include "Vector3.h"

namespace Math
{

  const Vector3 Vector3::cZero(0.0f, 0.0f, 0.0f);
  const Vector3 Vector3::cXAxis(1.0f, 0.0f, 0.0f);
  const Vector3 Vector3::cYAxis(0.0f, 1.0f, 0.0f);
  const Vector3 Vector3::cZAxis(0.0f, 0.0f, 1.0f);

  float* Vector3::ToFloats()
  {
    return (float*)this;
  }

  float const *Vector3::ToFloats() const
  {
    return (float const*)this;
  }

  Vector3::Vector3(float xx, float yy, float zz)
  {
    x = xx;
    y = yy;
    z = zz;
  }

  Vector3::Vector3(float xyz)
  {
    x = xyz;
    y = xyz;
    z = xyz;
  }

  Vector3::Vector3(Vec2Param rhs, float zz)
  {
    x = rhs.x;
    y = rhs.y;
    z = zz;
  }

  Vector3::Vector3(ConstRealPointer data)
  {
    x = data[0];
    y = data[1];
    z = data[2];
  }

  float& Vector3::operator[](unsigned index)
  {
    // changed to assert - ctd
    ErrorIf(index > 2, "Math::Vector3 - Subscript out of range.");
    return ToFloats()[index];
  }

  float Vector3::operator[](unsigned index) const
  {
    // changed to assert - ctd
    ErrorIf(index > 2, "Math::Vector3 - Subscript out of range.");
    return ToFloats()[index];
  }

  //---------------------------------------------------- Binary Vector Comparisons
  bool Vector3::operator==(Vec3Param rhs) const
  {
    return Math::Equal(x, rhs.x) &&
      Math::Equal(y, rhs.y) &&
      Math::Equal(z, rhs.z);
  }

  bool Vector3::operator!=(Vec3Param rhs) const
  {
    return !(*this == rhs);
  }

  void Vector3::Set(float x_, float y_, float z_)
  {
    x = x_;
    y = y_;
    z = z_;
  }

  void Vector3::Splat(float xyz)
  {
    x = y = z = xyz;
  }

  void Vector3::ScaleByVector(Vec3Param rhs)
  {
    x *= rhs.x;
    y *= rhs.y;
    z *= rhs.z;
  }

  void Vector3::ZeroOut()
  {
    x = 0.0f;
    y = 0.0f;
    z = 0.0f;
  }

  void Vector3::AddScaledVector(Vec3Param vector, float scalar)
  {
    x += vector.x * scalar;
    y += vector.y * scalar;
    z += vector.z * scalar;
  }

  Vector3 Vector3::Reflect(Vec3Param rhs) const
  {
    Vector3 reflect = rhs;
    reflect *= (*this).Dot(rhs);
    reflect *= 2.0f;
    reflect -= *this;
    return reflect;
  }

  Vector3 Vector3::Project(Vec3Param axis)
  {
    float dot = Dot(axis);
    return axis * dot;
  }

  float Vector3::Length() const
  {
    return Sqrt(LengthSq());
  }

  float Vector3::LengthSq() const
  {
    return Dot(*this);
  }

  Vector3 Vector3::Normalized() const
  {
    Vector3 ret = *this;;
    ret /= Length();
    return ret;
  }

  float Vector3::Normalize()
  {
    float length = Length();
    *this /= length;
    return length;
  }

  void Vector3::Ceil()
  {
    x = Math::Ceil(x);
    y = Math::Ceil(y);
    z = Math::Ceil(z);
  }

  void Vector3::Floor()
  {
    x = Math::Floor(x);
    y = Math::Floor(y);
    z = Math::Floor(z);
  }

  void Vector3::Truncate()
  {
    x = float(int(x));
    y = float(int(y));
    z = float(int(z));
  }

  void Vector3::RoundToExtremes()
  {
    x = (x > 0) ? Math::Ceil(x) : Math::Floor(x);
    y = (y > 0) ? Math::Ceil(y) : Math::Floor(y);
    z = (z > 0) ? Math::Ceil(z) : Math::Floor(z);
  }

  void Vector3::Round()
  {
    x = Math::Round(x);
    y = Math::Round(y);
    z = Math::Round(z);
  }

  float Vector3::AttemptNormalize()
  {
    float lengthSq = LengthSq();

    //Although the squared length may not be zero, the sqrt of a small number
    //may be truncated to zero, causing a divide by zero crash.  This is why
    //we check to make sure that it is larger than our epsilon squared.
    if (lengthSq >= Epsilon() * Epsilon())
    {
      lengthSq = Sqrt(lengthSq);
      *this /= lengthSq;
    }
    return lengthSq;
  }

  Vec3Ref Vector3::Negate()
  {
    (*this) *= -1.0f;
    return *this;
  }

  bool Vector3::Valid() const
  {
    return IsValid(x) && IsValid(y) && IsValid(z);
  }

  Vector3 Vector3::Cross(Vec3Param v) const
  {
    const float _x = y * v.z - z * v.y;
    const float _y = z * v.x - x * v.z;
    const float _z = x * v.y - y * v.x;

    return Vector3(_x, _y, _z);
  }

  void Vector3::InvertComponents()
  {
    x = 1.0f / x;
    y = 1.0f / y;
    z = 1.0f / z;
  }

  //------------------------------------------------------------- Global Functions
  Vector3 operator*(float lhs, Vec3Param rhs)
  {
    return rhs * lhs;
  }

  float Distance(Vec3Param lhs, Vec3Param rhs)
  {
    return Length(rhs - lhs);
  }

  Vector3 ScaledByVector(Vec3Param lhs, Vec3Param rhs)
  {
    return lhs * rhs;
  }

  Vector3 DividedByVector(Vec3Param lhs, Vec3Param rhs)
  {
    return lhs / rhs;
  }

  float Length(Vec3Param vect)
  {
    return vect.Length();
  }

  float LengthSq(Vec3Param vect)
  {
    return vect.LengthSq();
  }

  Vector3 Normalized(Vec3Param vect)
  {
    return vect.Normalized();
  }

  float Normalize(Vec3Ptr vect)
  {
    // changed to assert - ctd
    ErrorIf(vect == NULL, "Vector3 - Null pointer passed for vector.");
    return vect->Normalize();
  }

  float AttemptNormalize(Vec3Ptr vect)
  {
    // changed to assert - ctd
    ErrorIf(vect == NULL, "Vector3 - Null pointer passed for vector.");
    return vect->AttemptNormalize();
  }

  Vector3 Cross(Vec3Param lhs, Vec3Param rhs)
  {
    return lhs.Cross(rhs);
  }

  Vector3 Cross2d(Vec3Param lhs, Vec3Param rhs)
  {
    Vector3 result = Vector3::cZero;
    result.z = lhs.x * rhs.y - rhs.x * lhs.y;
    return result;
  }

  void ZeroOut(Vec3Ptr vec)
  {
    // changed to assert - ctd
    ErrorIf(vec == NULL, "Vector3 - Null pointer passed for vector.");
    vec->ZeroOut();
  }

  void Negate(Vec3Ptr vec)
  {
    // changed to assert - ctd
    ErrorIf(vec == NULL, "Vector3 - Null pointer passed for vector.");
    *vec *= -1.0f;
  }

  Vector3 Negated(Vec3Param vec)
  {
    return Vector3(-vec.x, -vec.y, -vec.z);
  }

  Vector3 Abs(Vec3Param vec)
  {
    return Vector3(Math::Abs(vec.x), Math::Abs(vec.y), Math::Abs(vec.z));
  }

  Vector3 Min(Vec3Param lhs, Vec3Param rhs)
  {
    return Vector3(Math::Min(lhs.x, rhs.x),
      Math::Min(lhs.y, rhs.y),
      Math::Min(lhs.z, rhs.z));
  }

  Vector3 Max(Vec3Param lhs, Vec3Param rhs)
  {
    return Vector3(Math::Max(lhs.x, rhs.x),
      Math::Max(lhs.y, rhs.y),
      Math::Max(lhs.z, rhs.z));
  }

  Vector3 Lerp(Vec3Param start, Vec3Param end, float tValue)
  {
    return Vector3(start[0] + tValue * (end[0] - start[0]),
      start[1] + tValue * (end[1] - start[1]),
      start[2] + tValue * (end[2] - start[2]));
  }

  void Clamp(Vec3Ptr vec, float min, float max)
  {
    //ErrorIf(vec == NULL, "Vector3 - Null pointer passed for vector.");
    vec->x = Math::Clamp(vec->x, min, max);
    vec->y = Math::Clamp(vec->y, min, max);
    vec->z = Math::Clamp(vec->z, min, max);
  }

  Vector3 Clamped(Vec3Param vec, float min, float max)
  {
    Vector3 results;
    results[0] = Math::Clamp(vec[0], min, max);
    results[1] = Math::Clamp(vec[1], min, max);
    results[2] = Math::Clamp(vec[2], min, max);
    return results;
  }

  Vector3 DebugClamp(Vec3Param vec, float min, float max, bool& wasClamped)
  {
    Vector3 result;
    bool xClamped, yClamped, zClamped;
    result.x = DebugClamp(vec.x, min, max, xClamped);
    result.y = DebugClamp(vec.y, min, max, yClamped);
    result.z = DebugClamp(vec.z, min, max, zClamped);
    wasClamped = xClamped || yClamped || zClamped;
    return result;
  }

  bool AllLess(Vec3Param lhs, Vec3Param rhs)
  {
    return (lhs.x < rhs.x && lhs.y < rhs.y && lhs.z < rhs.z);
  }

  bool AnyLess(Vec3Param lhs, Vec3Param rhs)
  {
    return (lhs.x < rhs.x || lhs.y < rhs.y || lhs.z < rhs.z);
  }

  bool AllGreater(Vec3Param lhs, Vec3Param rhs)
  {
    return (lhs.x > rhs.x && lhs.y > rhs.y && lhs.z > rhs.z);
  }

  bool AnyGreater(Vec3Param lhs, Vec3Param rhs)
  {
    return (lhs.x > rhs.x || lhs.y > rhs.y || lhs.z > rhs.z);
  }


  //-------------------------------------------------------------- Unary Operators
  Vector3 Vector3::operator-() const
  {
    return Vector3(-x, -y, -z);
  }

  //------------------------------------------ Binary Assignment Operators (reals)
  void Vector3::operator*=(float rhs)
  {
    x *= rhs;
    y *= rhs;
    z *= rhs;
  }

  void Vector3::operator/=(float rhs)
  {
    //ErrorIf(rhs == 0.0f, "Math::Vector3 - Division by zero.");
    x /= rhs;
    y /= rhs;
    z /= rhs;
  }

  //----------------------------------------------------- Binary Operators (reals)

  Vector3 Vector3::operator*(float rhs) const
  {
    Vector3 ret = *this;
    ret *= rhs;
    return ret;
  }

  Vector3 Vector3::operator/(float rhs) const
  {
    //ErrorIf(Math::IsZero(rhs), "Math::Vector3 - Division by zero.");
    Vector3 ret = *this;
    ret /= rhs;
    return ret;
  }

  //----------------------------------------- Binary Assignment Operator (Vectors)

  void Vector3::operator+=(Vec3Param rhs)
  {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
  }

  void Vector3::operator-=(Vec3Param rhs)
  {
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
  }

  //--------------------------------------------------- Binary Operators (Vectors)
  Vector3 Vector3::operator+(Vec3Param rhs) const
  {
    Vector3 ret = *this;
    ret += rhs;
    return ret;
  }

  Vector3 Vector3::operator-(Vec3Param rhs) const
  {
    Vector3 ret = *this;
    ret -= rhs;
    return ret;
  }

  Vector3 Vector3::operator*(Vec3Param rhs) const
  {
    return Vector3(x * rhs.x, y * rhs.y, z * rhs.z);
  }

  Vector3 Vector3::operator/(Vec3Param rhs) const
  {
   // ErrorIf(rhs.x == 0.0f || rhs.y == 0.0f || rhs.z == 0.0f,
      //"Vector3 - Division by zero.");
    return Vector3(x / rhs.x, y / rhs.y, z / rhs.z);
  }

  void Vector3::operator*=(Vec3Param rhs)
  {
    x *= rhs.x;
    y *= rhs.y;
    z *= rhs.z;
  }

  void Vector3::operator/=(Vec3Param rhs)
  {
    x /= rhs.x;
    y /= rhs.y;
    z /= rhs.z;
  }

  float Vector3::Dot(Vec3Param v) const
  {
    return x * v.x + y * v.y + z * v.z;
  }

  float Dot(Vec3Param lhs, Vec3Param rhs)
  {
    return lhs.Dot(rhs);
  }


}// namespace Math
