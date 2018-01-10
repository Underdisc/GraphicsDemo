/* All content(c) 2017 - 2018 DigiPen(USA) Corporation, all rights reserved. */
/*****************************************************************************/
/*!
\file Framer.h
\author Connor Deakin
\par E-mail: connor.deakin\@digipen.edu
\par Project: Graphics
\date 2017/10/23
\brief
  Interface for the Framer utility class.
*/
/*****************************************************************************/
#pragma once

#include <vector>

/*****************************************************************************/
/*!
\class Framer
\brief
  Static utility class that is used for frame locking and fps profiling.
*/
/*****************************************************************************/
class Framer
{
public:
  static void Start();
  static void End();
  static void Unlock();
  static void Lock(int fps);
  static float AverageFPS();
  static float AverageFrameUsage();
private:
  Framer() {}
  static void CalculateAverages();
  //! Identifies whether the frame rate is locked or not
  static bool _locked;
  //! The target frame time for a locked frame rate
  static float _targetFrameTime;
  //! The time at the start of a frame
  static float _startTime;
  //! The time since a FPS calculation was made
  static float _timeSinceLastAverageCalculation;
  //! The amount of time the Framer will wait to make an FPS calculation
  static float _waitTimeForAverageCalculation;
  //! The average FPS over the duration specified by 
  // _waitTimeForAverageCalculation
  static float _averageFPS;
  //! The average usage of a frame over the duration specified by
  // _waitTimeForAverageCalculation
  static float _averageFrameUsage;
  //! The amount of time passed for each frame over the duration specified
  // by _waitTimeForAverageCalculation
  static std::vector<float> _frameTimes;
  //! The percentage used of each frame over the duration specified by
  // _waitTimeForAverageCalculation
  static std::vector<float> _frameUsages;
};