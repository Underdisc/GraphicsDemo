/* All content(c) 2017 DigiPen(USA) Corporation, all rights reserved. */
#pragma once

#include "../Math/Vector3.h"
#include "../Math/Matrix4.h"

class Camera
{
public:
  Camera(const Math::Vector3 & global_up);
  void SetPosition(const Math::Vector3 & position);
  void MoveRight(float delta);
  void MoveUp(float delta);
  void MoveBack(float delta);
  void MoveGlobalUp(float delta);
  void MoveInDirection(const Math::Vector3 & direction, float delta);
  void MoveYaw(float delta);
  void MovePitch(float delta);
  void LookIn(const Math::Vector3 & direction);
  void LookAt(const Math::Vector3 & position);
  const Math::Vector3 & GetPosition();
  float GetYaw();
  float GetPitch();
  const Math::Matrix4 & ViewMatrix();
private:
  void UpdateFrontVector();
  void UpdateOrientationVectors();
  void UpdateViewMatrix();
  Math::Vector3 _globalUp;
  Math::Vector3 _position;
  Math::Vector3 _front;
  Math::Vector3 _right;
  Math::Vector3 _up;
  Math::Vector3 _back;
  float _yaw;
  float _pitch;
  bool _updatedFrontVector;
  bool _updatedOrientationVectors;
  bool _updatedViewMatrix;
  Math::Matrix4 _view;
  
};