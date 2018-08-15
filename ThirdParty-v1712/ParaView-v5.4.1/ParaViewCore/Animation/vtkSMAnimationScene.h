/*=========================================================================

  Program:   ParaView
  Module:    $RCSfile$

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkSMAnimationScene
 * @brief   animation scene for ParaView.
 *
 * vtkSMAnimationScene extends vtkAnimationCue to add support for a scene in
 * ParaView.
 *
 * We don't use vtkAnimationScene since ParaView has more elaborate playback
 * requirements. To support that, this class delegates playback
 * responsibility to vtkAnimationPlayer and subclasses.
 *
 * vtkSMAnimationScene also is proxy-aware and hence can work with proxies
 * and views proxies for updating property values, rendering, etc.
 *
 * vtkSMAnimationScene forwards the vtkCommand::StartEvent and
 * vtkCommand::EndEvent from vtkCompositeAnimationPlayer to mark the start and
 * end of animation playback.
*/

#ifndef vtkSMAnimationScene_h
#define vtkSMAnimationScene_h

#include "vtkAnimationCue.h"
#include "vtkCommand.h"           // needed for vtkCommand::UserEvent
#include "vtkPVAnimationModule.h" //needed for exports

class vtkCompositeAnimationPlayer;
class vtkEventForwarderCommand;
class vtkSMProxy;
class vtkSMViewProxy;

class VTKPVANIMATION_EXPORT vtkSMAnimationScene : public vtkAnimationCue
{
public:
  static vtkSMAnimationScene* New();
  vtkTypeMacro(vtkSMAnimationScene, vtkAnimationCue);
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  //@{
  /**
   * Add/Remove an AnimationCue to/from the Scene.
   * It's an error to add a cue twice to the Scene.
   */
  void AddCue(vtkAnimationCue* cue);
  void RemoveCue(vtkAnimationCue* cue);
  void RemoveAllCues();
  int GetNumberOfCues();
  //@}

  //@{
  /**
   * Add view proxies that are involved in the animation generated by this
   * scene. When playing the animation, the scene will call StillRender() on
   * the view proxies it is aware of, also updating any caching parameters.
   */
  void AddViewProxy(vtkSMViewProxy* proxy);
  void RemoveViewProxy(vtkSMViewProxy* proxy);
  void RemoveAllViewProxies();
  //@}

  //@{
  /**
   * Access the view proxies.
   */
  unsigned int GetNumberOfViewProxies();
  vtkSMViewProxy* GetViewProxy(unsigned int cc);
  //@}

  //@{
  /**
   * Set the time keeper. Time keeper is used to obtain the information about
   * timesteps. This is required to play animation in "Snap To Timesteps" mode.
   */
  void SetTimeKeeper(vtkSMProxy*);
  vtkGetObjectMacro(TimeKeeper, vtkSMProxy);
  //@}

  //@{
  /**
   * Lock the start time. When locked, the StartTime won't be automatically
   * updated when data time changes.
   */
  vtkSetMacro(LockStartTime, bool);
  vtkGetMacro(LockStartTime, bool);
  vtkBooleanMacro(LockStartTime, bool);
  //@}

  //@{
  /**
   * Lock the end time. When locked, the EndTime won't be automatically updated
   * when the data time changes.
   */
  vtkSetMacro(LockEndTime, bool);
  vtkGetMacro(LockEndTime, bool);
  vtkBooleanMacro(LockEndTime, bool);
  //@}

  //@{
  /**
   * Sets the current animation time.
   */
  void SetSceneTime(double time)
  {
    if (this->InTick)
    {
      // Since this method can be called during a Tick() event handler.
      return;
    }
    this->Initialize();
    this->Tick(time, 0, time);
  }
  //@}

  // Get the time of the most recent tick.
  // The only difference between this and AnimationTime (or ClockTime) defined
  // in the superclass is that, unlike the latter this is defined even outside
  // AnimationCueTickEvent handlers.
  vtkGetMacro(SceneTime, double);

  //@{
  /**
   * Get/Set the Playback Window for this cue.
   * The Playback Window is use to mask out time that belong to a given cue
   * but that we don't want to play back.
   * This is particulary useful when we want to export a subset of an animation
   * without recomputing any start and end value relative to the cue and the
   * number of frame associated to it.
   * This is used by the Animation Player to only play a subset of the cue.
   * To disable it just make the lower bound bigger than the upper one.
   */
  vtkSetVector2Macro(PlaybackTimeWindow, double);
  vtkGetVector2Macro(PlaybackTimeWindow, double);
  //@}

  //@{
  /**
   * Forwarded to vtkCompositeAnimationPlayer.
   */
  void SetLoop(int val);
  int GetLoop();
  void Play();
  void Stop();
  void GoToNext();
  void GoToPrevious();
  void GoToFirst();
  void GoToLast();
  void SetPlayMode(int val);
  int GetPlayMode();
  void SetNumberOfFrames(int val);
  void SetDuration(int val);
  void SetFramesPerTimestep(int val);
  //@}

  enum
  {
    // Fired whenever the vtkAnimationScene wants to request the
    // vtkSMAnimationSceneProxy to update the start/end times.
    // The calldata is a vtkVector2d with the suggested time-range.
    UpdateStartEndTimesEvent = vtkCommand::UserEvent
  };

  //@{
  /**
   * Set to true to force caching to be disabled. When false (default), caching
   * is determined based on the value from
   * vtkPVGeneralSettings::GetInstance()->GetCacheGeometryForAnimation().
   */
  vtkSetMacro(ForceDisableCaching, bool);
  vtkGetMacro(ForceDisableCaching, bool);
  //@}

protected:
  vtkSMAnimationScene();
  ~vtkSMAnimationScene();

  //@{
  /**
   * Overridden to ensure that caching parameters are passed to the view
   * correctly.
   */
  virtual void StartCueInternal() VTK_OVERRIDE;
  virtual void TickInternal(double currenttime, double deltatime, double clocktime) VTK_OVERRIDE;
  virtual void EndCueInternal() VTK_OVERRIDE;
  //@}

  //@{
  /**
   * Called when the timekeeper's time range changes.
   */
  void TimeKeeperTimeRangeChanged();
  void TimeKeeperTimestepsChanged();
  //@}

  bool LockStartTime;
  bool LockEndTime;
  bool InTick;
  double SceneTime;
  double PlaybackTimeWindow[2];
  bool ForceDisableCaching;
  vtkSMProxy* TimeKeeper;
  vtkCompositeAnimationPlayer* AnimationPlayer;
  vtkEventForwarderCommand* Forwarder;

  friend class vtkSMAnimationSceneImageWriter;
  bool OverrideStillRender;
  vtkSetMacro(OverrideStillRender, bool);

private:
  vtkSMAnimationScene(const vtkSMAnimationScene&) VTK_DELETE_FUNCTION;
  void operator=(const vtkSMAnimationScene&) VTK_DELETE_FUNCTION;

  class vtkInternals;
  vtkInternals* Internals;
  unsigned long TimeRangeObserverID;
  unsigned long TimestepValuesObserverID;
};

#endif
