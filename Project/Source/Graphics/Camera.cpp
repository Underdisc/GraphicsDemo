/* All content(c) 2017 DigiPen(USA) Corporation, all rights reserved. */
#include "../Math/Reals.h"

#include "Camera.h"

#define PI 3.14159265359f

Camera::Camera(const Math::Vector3 & global_up) : _globalUp(global_up),
  _position(0.0f, 0.0f, 0.0f), _front(0.0f, 0.0, -1.0f),
  _right(1.0f, 0.0f, 0.0f),  _up(0.0f, 1.0f, 0.0f), _back(0.0f, 0.0f, 1.0f), 
  _yaw(-PI / 2.0f),  _pitch(0.0f), _updatedFrontVector(true),
  _updatedOrientationVectors(true), _updatedViewMatrix(true),
  _view(1.0f, 0.0f, 0.0f, 0.0f,
  0.0f, 1.0f, 0.0f, 0.0f,
  0.0f, 0.0f, 1.0f, 0.0f,
  0.0f, 0.0f, 0.0f, 1.0f)
{}

void Camera::SetPosition(const Math::Vector3 & position)
{
  _position = position;
  _updatedViewMatrix = false;
}

void Camera::MoveRight(float delta)
{
  if(!_updatedOrientationVectors)
    UpdateOrientationVectors();
  _position += _right * delta;
  _updatedViewMatrix = false;
}

void Camera::MoveUp(float delta)
{
  if(!_updatedOrientationVectors)
    UpdateOrientationVectors();
  _position += _up * delta;
  _updatedViewMatrix = false;
}

void Camera::MoveBack(float delta)
{
  if(!_updatedOrientationVectors)
    UpdateOrientationVectors();
  _position += _back * delta;
  _updatedViewMatrix = false;
}

void Camera::MoveGlobalUp(float delta)
{
  _position += _globalUp * delta;
  _updatedViewMatrix = false;
}

// direction is assumed to be normalized
void Camera::MoveInDirection(const Math::Vector3 & direction, float delta)
{
  _position += direction * delta;
  _updatedViewMatrix = false;
}

void Camera::MoveYaw(float delta)
{
  _yaw += delta;
  _updatedFrontVector = false;
  _updatedOrientationVectors = false;
  _updatedViewMatrix = false;
}

void Camera::MovePitch(float delta)
{
  _pitch += delta;
  _updatedFrontVector = false;
  _updatedOrientationVectors = false;
  _updatedViewMatrix = false;
}

void Camera::LookIn(const Math::Vector3 & direction)
{
  _front = direction;
  _front.Normalize();
  _yaw = Math::ArcTan(_front.z / _front.x);
  float xz_length = Math::Sqrt(_front.x * _front.x + _front.z * _front.z);
  _pitch = Math::ArcTan(_front.y / xz_length);
  _updatedOrientationVectors = false;
  _updatedViewMatrix = false;
}

void Camera::LookAt(const Math::Vector3 & position)
{
  _front = position - _position;
  _front.Normalize();
  _yaw = Math::ArcTan(_front.z / _front.x);
  float xz_length = Math::Sqrt(_front.x * _front.x + _front.z * _front.z);
  _pitch = Math::ArcTan(_front.y / xz_length);
  _updatedOrientationVectors = false;
  _updatedViewMatrix = false;
}

const Math::Vector3 & Camera::GetPosition()
{
  return _position;
}

float Camera::GetYaw()
{
  return _yaw;
}

float Camera::GetPitch()
{
  return _pitch;
}

const Math::Matrix4 & Camera::ViewMatrix()
{
  // update view matrix if needed
  if(!_updatedViewMatrix)
    UpdateViewMatrix();
  return _view;
}

void Camera::UpdateFrontVector()
{
  _front.x = Math::Cos(_pitch) * Math::Cos(_yaw);
  _front.y = Math::Sin(_pitch);
  _front.z = Math::Cos(_pitch) * Math::Sin(_yaw);
  _front = _front.Normalized();
  _updatedFrontVector = true;
}

void Camera::UpdateOrientationVectors()
{
  if(!_updatedFrontVector)
    UpdateFrontVector();
  _right = Math::Cross(_front, _globalUp).Normalized();
  _up = Math::Cross(_right, _front).Normalized();
  _back = -_front;
  _updatedOrientationVectors = true;
}

void Camera::UpdateViewMatrix()
{
  // update orientation vectors
  if(!_updatedOrientationVectors)
    UpdateOrientationVectors();
  Math::Matrix4 linear_term(
    _right.x, _right.y, _right.z, 0.0f,
    _up.x,    _up.y,    _up.z,    0.0f,
    _back.x,  _back.y,  _back.z,  0.0f,
    0.0f,     0.0f,     0.0f,     1.0f);
  Math::Matrix4 translation_term(
    1.0f, 0.0f, 0.0f, -_position.x,
    0.0f, 1.0f, 0.0f, -_position.y,
    0.0f, 0.0f, 1.0f, -_position.z,
    0.0f, 0.0f, 0.0f, 1.0f);
  // find final matrix
  _view = linear_term * translation_term;
  _updatedViewMatrix = true;
}