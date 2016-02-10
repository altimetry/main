/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkInteractorStyleWPlot.h
  Language:  C++
  Date:      -
  Version:   S&T

    *** THIS FILE WAS MODIFIED  ***
    *** THIS FILE WAS CREATED BY SCIENCE & TECHNOLOGY BV ***

  Copyright (C) 2002-2005 Science & Technology BV, The Netherlands.

  This file is part of VISAN.

  VISAN is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  VISAN is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with VISAN; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


=========================================================================*/
#ifndef __vtkInteractorStyleWPlot_h
#define __vtkInteractorStyleWPlot_h

#include "vtkInteractorStyle.h"
#include "PlotData/vtkList.h"

#include "brathl.h"

 
/*! Creates a type name for std::map vtkObject base class */ 
typedef std::map<std::string, vtkObject*> mapvtkobject; 


class vtkIndent;
class vtkOutlineSource;
class vtkRenderer;
class vtkTransform;
class vtkTransformCollection;
class vtkActor2D;
class vtkInteractorStyleWPlot;
//-------------------------------------------------------------
//------------------- vtkWPlotState class --------------------
//-------------------------------------------------------------
class VTK_EXPORT vtkWPlotState : public vtkObject
{
public:
  static vtkWPlotState *New();
  vtkTypeMacro(vtkWPlotState, vtkObject);
  void PrintSelf(std::ostream& os, vtkIndent indent);

  vtkSetVector2Macro(Size, double);
  vtkGetVector2Macro(Size, double);

  vtkSetVector2Macro(MidPoint, double);
  vtkGetVector2Macro(MidPoint, double);

  vtkSetMacro(ZoomScale, double);  //KAVOK
  vtkGetMacro(ZoomScale, double);

protected:

  vtkWPlotState();
  virtual ~vtkWPlotState();

protected:
  double MidPoint[2];
  double Size[2];
  double ZoomScale;  //KAVOK

};

//-------------------------------------------------------------
//------------------- vtkWPlotProjectionState class --------------------
//-------------------------------------------------------------
class VTK_EXPORT vtkWPlotProjectionState : public vtkObject
{
public:
  static vtkWPlotProjectionState *New();
  vtkTypeMacro(vtkWPlotProjectionState, vtkObject);
  void PrintSelf(std::ostream& os, vtkIndent indent);

  vtkSetMacro(Projection, int32_t);
  vtkGetMacro(Projection, int32_t);

  void SaveState(vtkInteractorStyleWPlot*);
  void RestoreState(vtkInteractorStyleWPlot*);
  void RestoreState(vtkInteractorStyleWPlot*, int32_t index);
  void ClearStates();
  bool HasState() {return CountState() > 0;};
  uint32_t CountState() {return this->States.size();};

  vtkWPlotState* GetState(int32_t index);

protected:


  vtkWPlotProjectionState();
  virtual ~vtkWPlotProjectionState();

  vtkObArray States;
  int32_t CurrentState;

protected:
  int32_t Projection;

};


//-------------------------------------------------------------
//------------------- vtkInteractorStyleWPlot class --------------------
//-------------------------------------------------------------

// This interactorstyle only uses VTKIS_NONE, VTKIS_ZOOM, VTKIS_PAN

// Define our own VTKIS types
#define VTKIS_OUTLINEZOOM        100




//-------------------------------------------------------------
//------------------- vtkObMap class --------------------
//-------------------------------------------------------------
/** 
  a set of object management classes.

 
 \version 1.0
*/


class vtkObMap : public mapvtkobject
{
public:
   /// vtkObMap ctor
   vtkObMap(bool bDelete = true);
  
   /// vtkObMap dtor
   virtual ~vtkObMap();

/////////////
// Methods
/////////////
public:

   
   /** Inserts a vtkObject object
   * \param key : vtkObject name (std::map key)
   * \param ob : vtkObject value 
   * \param withExcept : true for std::exception handling, flse otherwise
   * \return vtkObject object or NULL if error */
   vtkObject* Insert(const std::string& key, vtkObject* ob, bool withExcept = true);

   /** Inserts a vtkObMap 
   * \param obMap : vtkObMap to insert
   * \param withExcept : true for std::exception handling, flse otherwise
    */
   //void Insert(const vtkObMap& obMap, bool withExcept = true);

   
   /** Tests if an element identify by 'key' already exists
   * \return a vtkObject pointer if exists, otherwise NULL */
   vtkObject* Exists(const std::string& key);

   /** Delete an element referenced by it
   * \return true if no error, otherwise false */
   bool Erase(vtkObMap::iterator it);
   
   /** Delete an element by its key
   * \return true if no error, otherwise false */
   bool Erase(const std::string& key);

   /** Remove all elements and clear the std::map*/
   void RemoveAll();

   /** operator[] redefinition. Searches a vtkObject object identifiy by 'key'.
     \param key : vtkObject keyword 
     \return a pointer to the vtkObject object if found, NULL  if not found */
   vtkObject* operator[](const std::string& key);

   bool GetDelete() {return m_bDelete;};
   void SetDelete(bool value) {m_bDelete = value;};

  /// Dump fonction
   virtual void Dump(std::ostream& fOut = std::cerr);


protected:

  bool m_bDelete;

 
};


class VTK_EXPORT vtkInteractorStyleWPlot : public vtkInteractorStyle
{
public:

  enum UserEventIds {
      EndZoomEvent = vtkCommand::UserEvent + 1,
      EndPanEvent,
      EndRestoreStateEvent
  };

  static vtkInteractorStyleWPlot *New();
  vtkTypeMacro(vtkInteractorStyleWPlot, vtkInteractorStyle);
  void PrintSelf(std::ostream& os, vtkIndent indent);

  // Description:
  // Event bindings controlling the effects of pressing mouse buttons
  // or moving the mouse.
  virtual void OnRightButtonDown();
  virtual void OnRightButtonUp();
  virtual void OnMiddleButtonDown();
  virtual void OnMiddleButtonUp();
  virtual void OnLeftButtonDown();
  virtual void OnLeftButtonUp();
  virtual void OnChar();
  virtual void OnTimer();

  void SetTransformCollection(vtkTransformCollection* collection);
  vtkTransformCollection* GetTransformCollection();
  void SetViewportSize(int width, int height);
  void SetRatio(double xRatio, double yRatio);

  /*
  vtkSetVector2Macro(StartPos, int);
  vtkGetVector2Macro(StartPos, int);
*/

  vtkSetVector2Macro(Size, double);
  vtkGetVector2Macro(Size, double);

  vtkSetVector2Macro(RatioVector, double);
  vtkGetVector2Macro(RatioVector, double);

  vtkGetMacro(ZoomScale, double);
  void SetZoomScale(double);

  void SaveState(int32_t projection);
  void RestoreState(int32_t projection);
  void RestoreState(int32_t projection, int32_t index);
  void ClearStates(int32_t projection);
  bool HasState(int32_t projection);
  uint32_t CountState(int32_t projection);

  void ZoomTo(int x1, int y1, int x2, int y2);
  void SetViewMidPoint(double x, double y);
  void GetViewMidPoint(double &x, double &y);
  double minX();
  double maxX();
  double minY();
  double maxY();


protected:
  vtkInteractorStyleWPlot();
  virtual ~vtkInteractorStyleWPlot();

  void SetTransformation();

  void Pan();
  void OutlineZoom();
  void OutlineZoom(int x, int y);
  void Zoom();
  void Zoom(int x, int y);


  void StartPan();
  void StartZoom();
  void StartZoom(int x, int y);
  void StartOutlineZoom();
  void StartOutlineZoom(int x, int y);
  void EndOutlineZoom();
  void EndZoom();
  void EndPan();

  void MapPointToMidPointTranslateVector(double x, double y);


  vtkWPlotProjectionState* GetProjState(int32_t projection);
  vtkWPlotState* GetState(int32_t projection, int32_t index);


protected:

  vtkTransformCollection*	TransformCollection;
  int StartPos[2];
  int PrevPos[2];
  double MidPoint[2];
  double Size[2];
  double ZoomTranslateX;
  double ZoomTranslateY;
  double ZoomScale;  //KAVOK
  int ViewportSize[2];
  double RatioVector[2];

  vtkOutlineSource* 	OutlineSource;
  vtkActor2D* 		OutlineActor;

  vtkObMap ProjStates;

private:
  vtkInteractorStyleWPlot(const vtkInteractorStyleWPlot&);  // Not implemented.
  void operator=(const vtkInteractorStyleWPlot&);  // Not implemented.
};

#endif
