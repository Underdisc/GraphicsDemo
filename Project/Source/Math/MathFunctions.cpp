///////////////////////////////////////////////////////////////////////////////
///
/// \file MathFunctions.cpp
/// Central location for all the math used by the Zero engine.
/// 
/// Authors: Benjamin Strukus
/// Copyright 2010-2012, DigiPen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////
#include "../Other/Precompiled.h"
#include "MathFunctions.h"
#include "EulerOrder.h"

namespace Math
{
  typedef Vector2    Vec2;
  typedef Vector3    Vec3;
  typedef Vector4    Vec4;
  typedef Matrix3    Mat3;
  typedef Matrix4    Mat4;
  typedef Quaternion Quat;

  namespace
  {
    const unsigned I = 0;
    const unsigned J = 1;
    const unsigned H = 2;

    void ClampAngle(float* angle)
    {
      while (*angle < -Math::cPi)
      {
        *angle += Math::cTwoPi;
      }
      while (*angle > Math::cPi)
      {
        *angle -= Math::cTwoPi;
      }
    }

  } // namespace

  ///Creates a skew symmetric matrix from the given 3D vector. Multiplying a 
  ///vector by this matrix is equivalent to the cross product using the input 
  ///vector.
  Matrix3 SkewSymmetric(Vec3Param vec3)
  {
    Matrix3 mtx;
    mtx.m22 = mtx.m11 = mtx.m00 = 0.0f;
    mtx.m01 = -vec3[2];
    mtx.m02 = vec3[1];
    mtx.m10 = vec3[2];
    mtx.m12 = -vec3[0];
    mtx.m20 = -vec3[1];
    mtx.m21 = vec3[0];
    return mtx;
  }

  Vector3 ProjectOut(Vec3Param vector, Vec3Param normal)
  {
    float projCoef = Math::Dot(vector, normal);
    return vector - normal * projCoef;
  }

  ///Converts a quaternion to an axis-angle pair (in radians). Axis is stored in 
  ///the Vector4's xyz and the angle is stored in the w.
  Vector4 ToAxisAngle(QuatParam quaternion, EulerOrders::Enum order)
  {
    Vector4 axisAngle;
    ToAxisAngle(quaternion, &axisAngle);
    return axisAngle;
  }

  void ToAxisAngle(QuatParam quaternion, Vec4Ptr axisAngle)
  {
    //ErrorIf(axisAngle == NULL, "Math - Null pointer passed for axis-angle pair.");
    Quat tempQuat(Normalized(quaternion));

    axisAngle->w = 2.0f * Math::ArcCos(tempQuat.w);
    float invSinAngle = Math::Sqrt(1.0f - tempQuat.w * tempQuat.w);

    if (Math::Abs(invSinAngle) < 0.0005f)
    {
      invSinAngle = 1.0f;
    }
    else
    {
      invSinAngle = 1.0f / invSinAngle;
    }
    axisAngle->x = tempQuat.x * invSinAngle;
    axisAngle->y = tempQuat.y * invSinAngle;
    axisAngle->z = tempQuat.z * invSinAngle;
  }

  ///Converts a quaternion to an axis-angle pair (in radians).
  void ToAxisAngle(QuatParam quaternion, Vec3Ptr axis, float* radians)
  {
    //ErrorIf(axis == NULL, "Math - Null pointer passed for axis.");
    //ErrorIf(radians == NULL, "Math - Null pointer passed for radians.");
    Quat tempQuat(Normalized(quaternion));

    *radians = 2.0f * Math::ArcCos(tempQuat.w);
    float invSinAngle = Math::Sqrt(1.0f - tempQuat.w * tempQuat.w);

    if (Math::Abs(invSinAngle) < 0.0005f)
    {
      invSinAngle = 1.0f;
    }
    else
    {
      invSinAngle = 1.0f / invSinAngle;
    }
    axis->x = tempQuat.x * invSinAngle;
    axis->y = tempQuat.y * invSinAngle;
    axis->z = tempQuat.z * invSinAngle;
  }

  ///Convert a 3x3 matrix to a set of Euler angles (in radians). The desired order
  ///of the rotations is expected to be in the given Euler angle structure.
  EulerAngles ToEulerAngles(Mat3Param matrix, EulerOrders::Enum order)
  {
    EulerAngles eulerAngles(order);
    ToEulerAngles(matrix, &eulerAngles);
    return eulerAngles;
  }

  void ToEulerAngles(Mat3Param matrix, EulerAnglesPtr eulerAngles)
  {
    //ErrorIf(eulerAngles == NULL, "Math - Null pointer passed for Euler angles.");
    unsigned i, j, k, h, parity, repeated, frame;
    EulerOrder::GetOrder(eulerAngles->Order, i, j, k, h, parity, repeated, frame);
    if (EulerOrders::Yes == repeated)
    {
      float sy = Math::Sqrt(matrix(i, j) * matrix(i, j) +
        matrix(i, k) * matrix(i, k));
      if (sy > 16.0f * FLT_EPSILON)
      {
        (*eulerAngles)[cX] = Math::ArcTan2(matrix(i, j), matrix(i, k));
        (*eulerAngles)[cY] = Math::ArcTan2(sy, matrix(i, i));
        (*eulerAngles)[cZ] = Math::ArcTan2(matrix(j, i), -matrix(k, i));
      }
      else
      {
        (*eulerAngles)[cX] = Math::ArcTan2(-matrix(j, k), matrix(j, j));
        (*eulerAngles)[cY] = Math::ArcTan2(sy, matrix(i, i));
        (*eulerAngles)[cZ] = 0.0f;
      }
    }
    else
    {
      float cy = Math::Sqrt(matrix(i, i) * matrix(i, i) +
        matrix(j, i) * matrix(j, i));
      if (cy > 16.0f * FLT_EPSILON)
      {
        (*eulerAngles)[cX] = Math::ArcTan2(matrix(k, j), matrix(k, k));
        (*eulerAngles)[cY] = Math::ArcTan2(-matrix(k, i), cy);
        (*eulerAngles)[cZ] = Math::ArcTan2(matrix(j, i), matrix(i, i));
      }
      else
      {
        (*eulerAngles)[cX] = Math::ArcTan2(-matrix(j, k), matrix(j, j));
        (*eulerAngles)[cY] = Math::ArcTan2(-matrix(k, i), cy);
        (*eulerAngles)[cZ] = 0.0f;
      }
    }
    if (EulerOrders::Odd == parity)
    {
      (*eulerAngles)[cX] *= -1.0f;
      (*eulerAngles)[cY] *= -1.0f;
      (*eulerAngles)[cZ] *= -1.0f;
    }

    ClampAngle(&((*eulerAngles)[cX]));
    ClampAngle(&((*eulerAngles)[cY]));
    ClampAngle(&((*eulerAngles)[cZ]));

    if (EulerOrders::Rotated == frame)
    {
      Math::Swap((*eulerAngles)[cX], (*eulerAngles)[cZ]);
    }
  }

  ///Convert a 4x4 matrix to a set of Euler angles in radians. The desired order
  ///of the rotations is expected to be in the given Euler angle structure.
  EulerAngles ToEulerAngles(Mat4Param matrix, EulerOrders::Enum order)
  {
    EulerAngles eulerAngles(order);
    ToEulerAngles(matrix, &eulerAngles);
    return eulerAngles;
  }

  void ToEulerAngles(Mat4Param matrix, EulerAnglesPtr eulerAngles)
  {
    //ErrorIf(eulerAngles == NULL, "Math - Null pointer passed for Euler angles.");

    unsigned i, j, k, h, parity, repeated, frame;
    EulerOrder::GetOrder(eulerAngles->Order, i, j, k, h, parity, repeated, frame);
    if (EulerOrders::Yes == repeated)
    {
      float sy = Math::Sqrt(matrix(i, j) * matrix(i, j) +
        matrix(i, k) * matrix(i, k));
      if (sy > 16.0f * FLT_EPSILON)
      {
        (*eulerAngles)[cX] = Math::ArcTan2(matrix(i, j), matrix(i, k));
        (*eulerAngles)[cY] = Math::ArcTan2(sy, matrix(i, i));
        (*eulerAngles)[cZ] = Math::ArcTan2(matrix(j, i), -matrix(k, i));
      }
      else
      {
        (*eulerAngles)[cX] = Math::ArcTan2(-matrix(j, k), matrix(j, j));
        (*eulerAngles)[cY] = Math::ArcTan2(sy, matrix(i, i));
        (*eulerAngles)[cZ] = 0.0f;
      }
    }
    else
    {
      float cy = Math::Sqrt(matrix(i, i) * matrix(i, i) +
        matrix(j, i) * matrix(j, i));
      if (cy > 16.0f * FLT_EPSILON)
      {
        (*eulerAngles)[cX] = Math::ArcTan2(matrix(k, j), matrix(k, k));
        (*eulerAngles)[cY] = Math::ArcTan2(-matrix(k, i), cy);
        (*eulerAngles)[cZ] = Math::ArcTan2(matrix(j, i), matrix(i, i));
      }
      else
      {
        (*eulerAngles)[cX] = Math::ArcTan2(-matrix(j, k), matrix(j, j));
        (*eulerAngles)[cY] = Math::ArcTan2(-matrix(k, i), cy);
        (*eulerAngles)[cZ] = 0.0f;
      }
    }
    if (EulerOrders::Odd == parity)
    {
      (*eulerAngles)[cX] *= -1.0f;
      (*eulerAngles)[cY] *= -1.0f;
      (*eulerAngles)[cZ] *= -1.0f;
    }

    ClampAngle(&((*eulerAngles)[cX]));
    ClampAngle(&((*eulerAngles)[cY]));
    ClampAngle(&((*eulerAngles)[cZ]));

    if (EulerOrders::Rotated == frame)
    {
      Math::Swap((*eulerAngles)[cX], (*eulerAngles)[cZ]);
    }
  }

  ///Convert a quaternion to a set of Euler angles (in radians). The desired order
  ///of the rotations is expected to be in the given Euler angle structure.
  EulerAngles ToEulerAngles(QuatParam quaternion, EulerOrders::Enum order)
  {
    EulerAngles eulerAngles(order);
    ToEulerAngles(quaternion, &eulerAngles);
    return eulerAngles;
  }

  void ToEulerAngles(QuatParam quaternion, EulerAnglesPtr eulerAngles)
  {
    //ErrorIf(eulerAngles == NULL, "Math - Null pointer passed for Euler angles.");

    Matrix3 matrix;
    ToMatrix3(quaternion, &matrix);
    ToEulerAngles(matrix, eulerAngles);
  }

  /// Converts from Vector3 to Vector2, removing the z component of the Vector3.
  Vector2 ToVector2(Vec3Param v3)
  {
    return Vector2(v3.x, v3.y);
  }

  /// Converts from Vector2 to Vector3, adding the given z component.
  Vector3 ToVector3(Vec2Param v2, float z)
  {
    return Vector3(v2.x, v2.y, z);
  }

  /// Turns the 3d transformation matrix into 2d.
  Matrix3 ProjectXY(Mat4Param mat4)
  {
    Matrix3 mat3;
    mat3.m00 = mat4.m00;
    mat3.m01 = mat4.m01;
    mat3.m02 = mat4.m03; // Pull over the x translation

    mat3.m10 = mat4.m10;
    mat3.m11 = mat4.m11;
    mat3.m12 = mat4.m13; // Pull over the y translation

    mat3.m20 = 0;
    mat3.m21 = 0;
    mat3.m22 = 1;

    return mat3;
  }

  ///Converts an axis-angle pair to a 3x3 (in radians). Axis is stored in the
  ///Vector4's xyz and the angle is stored in the w. Axis is assumed to be 
  ///normalized.
  Matrix3 ToMatrix3(Vec4Param axisAngle)
  {
    Matrix3 matrix;
    ToMatrix3(axisAngle, &matrix);
    return matrix;
  }

  void ToMatrix3(Vec4Param axisAngle, Mat3Ptr matrix)
  {
    // convert matrix to rotation matrix using arbitrary axis rotation given an
    // axis and an angle
    // TODO: can be done by student if desired
  }

  ///Converts an axis-angle pair to a 3x3 matrix (in radians). Axis is assumed to
  ///be normalized.
  Matrix3 ToMatrix3(Vec3Param axis, float radians)
  {
    Matrix3 matrix;
    ToMatrix3(axis, radians, &matrix);
    return matrix;
  }

  void ToMatrix3(Vec3Param axis, float radians, Mat3Ptr matrix)
  {
    // convert matrix to rotation matrix using arbitrary axis rotation given an
    // axis and an angle
    // TODO: can be done by student if desired
  }

  ///Convert a set of Euler angles to a 3x3 matrix (in radians).
  Matrix3 ToMatrix3(EulerAnglesParam eulerAngles)
  {
    Matrix3 matrix;
    ToMatrix3(eulerAngles, &matrix);
    return matrix;
  }

  void ToMatrix3(EulerAnglesParam eulerAngles, Mat3Ptr matrix)
  {
    //Only implemented for XYZs
    Math::Vector3 x_axis(1.0f, 0.0f, 0.0f);
    Math::Vector3 y_axis(0.0f, 1.0f, 0.0f);
    Math::Vector3 z_axis(0.0f, 0.0f, 1.0f);

    

  }

  ///Convert a 4x4 matrix to a 3x3 matrix. Simply copies the 4x4 matrix's upper 
  ///3x3 matrix (rotation & scale) to the 3x3 matrix.
  Matrix3 ToMatrix3(Mat4Param matrix)
  {
    Matrix3 mat3;
    ToMatrix3(matrix, &mat3);
    return mat3;
  }

  void ToMatrix3(Mat4Param mat4, Mat3Ptr mat3)
  {
    //ErrorIf(mat3 == NULL, "Math - Null pointer passed for matrix.");

    //First "cross" components
    mat3->m00 = mat4.m00;
    mat3->m01 = mat4.m01;
    mat3->m02 = mat4.m02;

    //Second "cross" components
    mat3->m10 = mat4.m10;
    mat3->m11 = mat4.m11;
    mat3->m12 = mat4.m12;

    //Third "cross" components
    mat3->m20 = mat4.m20;
    mat3->m21 = mat4.m21;
    mat3->m22 = mat4.m22;
  }

  ///Converts a quaternion to a 3x3 rotation matrix (in radians).
  Matrix3 ToMatrix3(QuatParam quaternion)
  {
    Matrix3 matrix;
    ToMatrix3(quaternion, &matrix);
    return matrix;
  }

  void ToMatrix3(QuatParam quaternion, Mat3Ptr matrix)
  {
    // TODO: student can implement quaternion->mat3 if desired
  }

  ///Convert a set of Euler angles to a 4x4 matrix (in radians).
  Matrix4 ToMatrix4(EulerAnglesParam eulerAngles)
  {
    Matrix4 matrix;
    ToMatrix4(eulerAngles, &matrix);
    return matrix;
  }

  void ToMatrix4(EulerAnglesParam eulerAngles, Mat4Ptr matrix)
  {
    // only implemented for XYZs
    // rotation vectors
    Math::Vector3 x_axis(1.0f, 0.0f, 0.0f);
    Math::Vector3 y_axis(0.0f, 1.0f, 0.0f);
    Math::Vector3 z_axis(0.0f, 0.0f, 1.0f);
    // rotation matrices
    Math::Matrix4 x_rot;
    Math::Matrix4 y_rot;
    Math::Matrix4 z_rot;
    // calculate final rotation matrix
    x_rot.Rotate(x_axis, eulerAngles.Angles.x);
    y_rot.Rotate(y_axis, eulerAngles.Angles.y);
    z_rot.Rotate(z_axis, eulerAngles.Angles.z);
    *matrix = z_rot * y_rot * x_rot;
  }

  ///Convert a 3x3 matrix to a 4x4 matrix. Simply copies the 3x3 matrix's values
  ///into the rotational part of the 4x4 matrix.
  Matrix4 ToMatrix4(Mat3Param matrix)
  {
    Matrix4 matrix4;
    ToMatrix4(matrix, &matrix4);
    return matrix4;
  }

  void ToMatrix4(Mat3Param mat3, Mat4Ptr mat4)
  {
    //ErrorIf(mat4 == NULL, "Math - Null pointer passed for matrix.");

    //First "cross" components
    mat4->m00 = mat3.m00;
    mat4->m01 = mat3.m01;
    mat4->m02 = mat3.m02;
    mat4->m03 = 0.0f;

    //Second "cross" components
    mat4->m10 = mat3.m10;
    mat4->m11 = mat3.m11;
    mat4->m12 = mat3.m12;
    mat4->m13 = 0.0f;

    //Third "cross" components
    mat4->m20 = mat3.m20;
    mat4->m21 = mat3.m21;
    mat4->m22 = mat3.m22;
    mat4->m23 = 0.0f;

    //Fourth "cross" components
    mat4->m30 = 0.0f;
    mat4->m31 = 0.0f;
    mat4->m32 = 0.0f;
    mat4->m33 = 1.0f;
  }

  ///Converts a quaternion to a 4x4 rotation matrix (in radians).
  Matrix4 ToMatrix4(QuatParam quaternion)
  {
    Matrix4 matrix;
    ToMatrix4(quaternion, &matrix);
    return matrix;
  }

  void ToMatrix4(QuatParam quaternion, Mat4Ptr matrix)
  {
    // TODO: student can implement quaternion->mat3 if desired
  }

  ///Converts a 3D vector to a quaternion.
  Quat ToQuaternion(Vec3Param vector)
  {
    Quat quaternion;
    ToQuaternion(vector, &quaternion);
    return quaternion;
  }

  void ToQuaternion(Vec3Param vector, QuatPtr quaternion)
  {
    //ErrorIf(quaternion == NULL, "Math - Null pointer passed for quaternion.");

    quaternion->Set(vector.x, vector.y, vector.z, 0.0f);
  }

  ///Converts an axis-angle pair to a quaternion (in radians). Axis is stored in
  ///the Vector4's xyz and the angle is stored in the w. Axis is assumed to be 
  ///normalized.
  Quat ToQuaternion(Vec4Param axisAngle)
  {
    Quat quaternion;
    ToQuaternion(axisAngle, &quaternion);
    return quaternion;
  }

  void ToQuaternion(Vec4Param axisAngle, QuatPtr quaternion)
  {
    //ErrorIf(quaternion == NULL, "Math - Null pointer passed for quaternion.");

    float alpha = axisAngle.w * 0.5f;
    float sinAlpha = Math::Sin(alpha);

    quaternion->x = axisAngle.x * sinAlpha;
    quaternion->y = axisAngle.y * sinAlpha;
    quaternion->z = axisAngle.z * sinAlpha;
    quaternion->w = Math::Cos(alpha);
  }

  ///Converts an axis-angle pair to a quaternion (in radians). Axis is assumed to
  ///be normalized.
  Quaternion ToQuaternion(Vec3Param axis, float radians)
  {
    Quat quaternion;
    ToQuaternion(axis, radians, &quaternion);
    return quaternion;
  }

  void ToQuaternion(Vec3Param axis, float radians, QuatPtr quaternion)
  {
    //ErrorIf(quaternion == NULL, "Math - Null pointer passed for quaternion.");

    float alpha = radians * 0.5f;
    float sinAlpha = Math::Sin(alpha);

    quaternion->x = axis.x * sinAlpha;
    quaternion->y = axis.y * sinAlpha;
    quaternion->z = axis.z * sinAlpha;
    quaternion->w = Math::Cos(alpha);
  }

  ///Convert a set of Euler angles to a quaternion (in radians).
  Quat ToQuaternion(EulerAnglesParam eulerAngles)
  {
    Quat quaternion;
    ToQuaternion(eulerAngles, &quaternion);
    return quaternion;
  }

  void ToQuaternion(EulerAnglesParam eulerAngles, QuatPtr quaternion)
  {
    //ErrorIf(quaternion == NULL, "Math - Null pointer passed for quaternion.");

    float angles[3] = { eulerAngles[cX], eulerAngles[cY], eulerAngles[cZ] };
    unsigned i, j, k, h, parity, repeated, frame;
    EulerOrder::GetOrder(eulerAngles.Order, i, j, k, h, parity, repeated, frame);
    if (EulerOrders::Rotated == frame)
    {
      Math::Swap(angles[cX], angles[cZ]);
    }

    if (EulerOrders::Odd == parity)
    {
      angles[cY] *= -1.0f;
    }

    float t[3], c[3], s[3];
    t[I] = angles[cX] * 0.5f; c[I] = Math::Cos(t[I]); s[I] = Math::Sin(t[I]);
    t[J] = angles[cY] * 0.5f; c[J] = Math::Cos(t[J]); s[J] = Math::Sin(t[J]);
    t[H] = angles[cZ] * 0.5f; c[H] = Math::Cos(t[H]); s[H] = Math::Sin(t[H]);

    const float cc = c[I] * c[H];
    const float cs = c[I] * s[H];
    const float sc = s[I] * c[H];
    const float ss = s[I] * s[H];
    if (EulerOrders::Yes == repeated)
    {
      angles[i] = c[J] * (cs + sc);
      angles[j] = s[J] * (cc + ss);
      angles[k] = s[J] * (cs - sc);
      quaternion->w = c[J] * (cc - ss);
    }
    else
    {
      angles[i] = c[J] * sc - s[J] * cs;
      angles[j] = c[J] * ss + s[J] * cc;
      angles[k] = c[J] * cs - s[J] * sc;
      quaternion->w = c[J] * cc + s[J] * ss;
    }
    if (EulerOrders::Odd == parity)
    {
      angles[j] *= -1.0f;
    }
    quaternion->x = angles[cX];
    quaternion->y = angles[cY];
    quaternion->z = angles[cZ];
  }

  ///Converts a 3x3 matrix to a quaternion (in radians).
  Quat ToQuaternion(Mat3Param matrix)
  {
    Quat quaternion;
    ToQuaternion(matrix, &quaternion);
    return quaternion;
  }

  void ToQuaternion(Mat3Param matrix, QuatPtr quaternion)
  {
    //ErrorIf(quaternion == NULL, "Math - Null pointer passed for quaternion.");

    if (matrix.m00 + matrix.m11 + matrix.m22 > 0.0f)
    {
      float t = matrix.m00 + matrix.m11 + matrix.m22 + 1.0f;
      float s = Math::Rsqrt(t) * 0.5f;

      (*quaternion)[3] = s * t;
      (*quaternion)[2] = (matrix.m10 - matrix.m01) * s;
      (*quaternion)[1] = (matrix.m02 - matrix.m20) * s;
      (*quaternion)[0] = (matrix.m21 - matrix.m12) * s;
    }
    else if (matrix.m00 > matrix.m11 && matrix.m00 > matrix.m22)
    {
      float t = matrix.m00 - matrix.m11 - matrix.m22 + 1.0f;
      float s = Math::Rsqrt(t) * 0.5f;

      (*quaternion)[0] = s * t;
      (*quaternion)[1] = (matrix.m10 + matrix.m01) * s;
      (*quaternion)[2] = (matrix.m02 + matrix.m20) * s;
      (*quaternion)[3] = (matrix.m21 - matrix.m12) * s;
    }
    else if (matrix.m11 > matrix.m22)
    {
      float t = -matrix.m00 + matrix.m11 - matrix.m22 + 1.0f;
      float s = Math::Rsqrt(t) * 0.5f;

      (*quaternion)[1] = s * t;
      (*quaternion)[0] = (matrix.m10 + matrix.m01) * s;
      (*quaternion)[3] = (matrix.m02 - matrix.m20) * s;
      (*quaternion)[2] = (matrix.m21 + matrix.m12) * s;
    }
    else
    {
      float t = -matrix.m00 - matrix.m11 + matrix.m22 + 1.0f;
      float s = Math::Rsqrt(t) * 0.5f;

      (*quaternion)[2] = s * t;
      (*quaternion)[3] = (matrix.m10 - matrix.m01) * s;
      (*quaternion)[0] = (matrix.m02 + matrix.m20) * s;
      (*quaternion)[1] = (matrix.m21 + matrix.m12) * s;
    }
  }

  ///Converts a 4x4 matrix to a quaternion (in radians).
  Quat ToQuaternion(Mat4Param matrix)
  {
    Quat quaternion;
    ToQuaternion(matrix, &quaternion);
    return quaternion;
  }

  void ToQuaternion(Mat4Param matrix, QuatPtr quaternion)
  {
    //ErrorIf(quaternion == NULL, "Math - Null pointer passed for quaternion.");

    if (matrix.m00 + matrix.m11 + matrix.m22 > 0.0f)
    {
      float t = matrix.m00 + matrix.m11 + matrix.m22 + 1.0f;
      float s = Math::Rsqrt(t) * 0.5f;

      (*quaternion)[3] = s * t;
      (*quaternion)[2] = (matrix.m10 - matrix.m01) * s;
      (*quaternion)[1] = (matrix.m02 - matrix.m20) * s;
      (*quaternion)[0] = (matrix.m21 - matrix.m12) * s;
    }
    else if (matrix.m00 > matrix.m11 && matrix.m00 > matrix.m22)
    {
      float t = matrix.m00 - matrix.m11 - matrix.m22 + 1.0f;
      float s = Math::Rsqrt(t) * 0.5f;

      (*quaternion)[0] = s * t;
      (*quaternion)[1] = (matrix.m10 + matrix.m01) * s;
      (*quaternion)[2] = (matrix.m02 + matrix.m20) * s;
      (*quaternion)[3] = (matrix.m21 - matrix.m12) * s;
    }
    else if (matrix.m11 > matrix.m22)
    {
      float t = -matrix.m00 + matrix.m11 - matrix.m22 + 1.0f;
      float s = Math::Rsqrt(t) * 0.5f;

      (*quaternion)[1] = s * t;
      (*quaternion)[0] = (matrix.m10 + matrix.m01) * s;
      (*quaternion)[3] = (matrix.m02 - matrix.m20) * s;
      (*quaternion)[2] = (matrix.m21 + matrix.m12) * s;
    }
    else
    {
      float t = -matrix.m00 - matrix.m11 + matrix.m22 + 1.0f;
      float s = Math::Rsqrt(t) * 0.5f;

      (*quaternion)[2] = s * t;
      (*quaternion)[3] = (matrix.m10 - matrix.m01) * s;
      (*quaternion)[0] = (matrix.m02 + matrix.m20) * s;
      (*quaternion)[1] = (matrix.m21 + matrix.m12) * s;
    }
  }

  ///Generates a set of orthonormal vectors from the given vectors, modifying u 
  ///and v.
  void GenerateOrthonormalBasis(Vec3Param w, Vec3Ptr u, Vec3Ptr v)
  {
    //ErrorIf(u == NULL, "Math - Null pointer passed for vector U.");
    //ErrorIf(v == NULL, "Math - Null pointer passed for vector V.");

    if ((Math::Abs(w.x) >= Math::Abs(w.y)) && (Math::Abs(w.x) >= Math::Abs(w.z)))
    {
      u->x = -w.y;
      u->y = w.x;
      u->z = 0.0f;
    }
    else
    {
      u->x = 0.0f;
      u->y = w.z;
      u->z = -w.y;
    }
    Normalize(u);
    *v = Cross(w, *u);
    Normalize(v);
  }

  ///Generates a set of orthonormal vectors from the given vectors while using 
  ///debug checks, modifies u and v
  void DebugGenerateOrthonormalBasis(Vec3Param w, Vec3Ptr u, Vec3Ptr v)
  {
    //ErrorIf(u == NULL, "Math - Null pointer passed for vector U.");
    //ErrorIf(v == NULL, "Math - Null pointer passed for vector V.");

    if ((Math::Abs(w.x) >= Math::Abs(w.y)) && (Math::Abs(w.x) >= Math::Abs(w.z)))
    {
      u->x = -w.y;
      u->y = w.x;
      u->z = 0.0f;
    }
    else
    {
      u->x = 0.0f;
      u->y = w.z;
      u->z = -w.y;
    }
    AttemptNormalize(u);
    *v = Cross(w, *u);
    AttemptNormalize(v);
  }

  ///Perform the dot product with the specified basis vector of the 3x3 matrix and
  ///the given vector.
  float BasisNDot(Mat3Param matrix, unsigned basisIndex, Vec3Param vector)
  {
    return Dot(vector, matrix.Basis(basisIndex));
  }

  ///Perform the dot product with the specified basis vector of the 3x3 matrix and
  ///the given vector.
  float BasisNDot(Mat4Param matrix, unsigned basisIndex, Vec4Param vector)
  {
    return Dot(vector, matrix.Basis(basisIndex));
  }

  ///Perform the dot product with the basis vector of the 3x3 matrix that 
  ///represents the x-axis (if the matrix is pure rotation) and the given vector.
  float BasisXDot(Mat3Param matrix, Vec3Param vector)
  {
    return Dot(vector, matrix.BasisX());
  }

  ///Perform the dot product with the basis vector of the 3x3 matrix that 
  ///represents the y-axis (if the matrix is pure rotation) and the given vector.
  float BasisYDot(Mat3Param matrix, Vec3Param vector)
  {
    return Dot(vector, matrix.BasisY());
  }

  ///Perform the dot product with the basis vector of the 3x3 matrix that 
  ///represents the z-axis (if the matrix is pure rotation) and the given vector.
  float BasisZDot(Mat3Param matrix, Vec3Param vector)
  {
    return Dot(vector, matrix.BasisZ());
  }

  ///Perform the dot product with the basis vector of the 4x4 matrix that 
  ///represents the x-axis (if the matrix is pure rotation) and the given vector.
  float BasisXDot(Mat4Param matrix, Vec4Param vector)
  {
    return Dot(vector, matrix.BasisX());
  }

  ///Perform the dot product with the basis vector of the 4x4 matrix that 
  ///represents the y-axis (if the matrix is pure rotation) and the given vector.
  float BasisYDot(Mat4Param matrix, Vec4Param vector)
  {
    return Dot(vector, matrix.BasisY());
  }

  ///Perform the dot product with the basis vector of the 4x4 matrix that 
  ///represents the z-axis (if the matrix is pure rotation) and the given vector.
  float BasisZDot(Mat4Param matrix, Vec4Param vector)
  {
    return Dot(vector, matrix.BasisZ());
  }

  //----------------------------------------------------------- Rotation Functions
  float Angle(Vec2Param a, Vec2Param b)
  {
    float dot = Dot(a, b);
    return Math::ArcCos(dot);
  }

  float Angle(Vec3Param a, Vec3Param b)
  {
    float dot = Dot(a, b);
    return Math::ArcCos(dot);
  }

  float Angle(QuatParam a, QuatParam b)
  {
    float dot = Dot(a, b);
    if (dot == 0.0f)
      return 0.0f;
    float angle = 2.0f * Math::ArcCos(dot);
    return angle;
  }

  Vector2 Slerp(Vec2Param start, Vec2Param end, float t)
  {
    float dot = Dot(start, end);
    float theta = Math::ArcCos(dot) * t;
    Vector2 relativeVec = end - start * dot;
    relativeVec.Normalize();
    return (start * Math::Cos(theta)) + (relativeVec * Math::Sin(theta));
  }

  Vector3 Slerp(Vec3Param start, Vec3Param end, float t)
  {
    float dot = Dot(start, end);
    float theta = Math::ArcCos(dot) * t;
    Vector3 relativeVec = end - start * dot;
    relativeVec.Normalize();
    return (start * Math::Cos(theta)) + (relativeVec * Math::Sin(theta));
  }

  template<typename type>
  type GenericTowards(const type& a, const type& b, float maxAngle)
  {
    const float cAngleEpsilon = 0.0000001f;

    float angle = Angle(a, b);

    if (angle > Math::cPi)
    {
      angle -= Math::cTwoPi;
    }

    angle = Math::Abs(angle);
    if (angle > cAngleEpsilon)
    {
      float t = maxAngle / angle;
      if (t > 1.0f)
      {
        t = 1.0f;
      }
      return Slerp(a, b, t);
    }
    else
    {
      return b;
    }
  }

  Vector2 RotateTowards(Vec2Param a, Vec2Param b, float maxAngle)
  {
    Vec2 an = a.Normalized();
    Vec2 bn = b.Normalized();
    return GenericTowards(an, bn, maxAngle);
  }

  Vector3 RotateTowards(Vec3Param a, Vec3Param b, float maxAngle)
  {
    Vec3 an = a.Normalized();
    Vec3 bn = b.Normalized();
    return GenericTowards(an, bn, maxAngle);
  }

  Quat RotateTowards(QuatParam a, QuatParam b, float maxAngle)
  {
    return GenericTowards(a, b, maxAngle);
  }

  // Get the rotation angle between two vectors (in radians)
  float SignedAngle(Vec3Param a, Vec3Param b, Vec3Param up)
  {
    // Get the right vector
    Vec3 right = Math::Cross(a, up);
    right.AttemptNormalize();

    // Get the forward and right dot products
    float forwardDot = Math::Clamp(Math::Dot(a, b), -1.0f, 1.0f);
    float rightDot = Math::Clamp(Math::Dot(right, b), -1.0f, 1.0f);

    // Get the actual angle from the forward dot product
    float finalAngle = Math::ArcCos(forwardDot);

    // If we're actually on the left side...
    if (rightDot > 0.0f)
    {
      // Compute the float final angle given the quadrant it's in (kinda like atan2)
      finalAngle = -finalAngle;
    }

    // Return the finally computed angle
    return finalAngle;
  }

}// namespace Math
