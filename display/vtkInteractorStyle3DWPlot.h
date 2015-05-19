#ifndef __vtkInteractorStyle3DWPlot
#define __vtkInteractorStyle3DWPlot

/* KAVOK: This comes from VISAN code */

#include "vtkInteractorStyle.h"

class VTK_EXPORT vtkInteractorStyle3DWPlot : public vtkInteractorStyle
{
public:

   // event ID for adaptive Lat/Lon
   enum UserEventIds {
       EndZoomEvent = 101
  };

  static vtkInteractorStyle3DWPlot *New();
  vtkTypeMacro(vtkInteractorStyle3DWPlot, vtkInteractorStyle);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Set and get the default view
  void SetDefaultViewParameters(double  latitude, double  longitude, double  roll, double  zoom);
  void GetDefaultViewParameters(double &latitude, double &longitude, double &roll, double &zoom);

  // Set the current view to the default view
  void SetDefaultView();

  // Set and get the current view
  void SetViewParameters(double  latitude, double  longitude, double  roll, double  zoom);
  void GetViewParameters(double &latitude, double &longitude, double &roll, double &zoom);
  double GetViewCenterLatitude();
  double GetViewCenterLongitude();
  double GetViewZoom();
  static void latlon2position(double latitude, double longitude, double position[3]);

  // Set and get for individual Default view parameters
  vtkSetMacro(DefaultLatitude, double);
  vtkGetMacro(DefaultLatitude, double);

  vtkSetMacro(DefaultLongitude, double);
  vtkGetMacro(DefaultLongitude, double);

  vtkSetMacro(DefaultRoll, double);
  vtkGetMacro(DefaultRoll, double);

  vtkSetMacro(DefaultZoom, double);
  vtkGetMacro(DefaultZoom, double);

  // Set and get for individual current view parameters
  vtkSetMacro(Latitude, double);
  vtkGetMacro(Latitude, double);

  vtkSetMacro(Longitude, double);
  vtkGetMacro(Longitude, double);

  vtkSetMacro(Roll, double);
  vtkGetMacro(Roll, double);

  vtkSetMacro(Zoom, double);
  vtkGetMacro(Zoom, double);

  // The relative speed of all interactor motions
  vtkSetClampMacro(MotionSpeed, double, 0.1, 10.0);
  vtkGetMacro(MotionSpeed, double);

  // Description:
  // Event bindings controlling the effects of pressing mouse buttons
  // or moving the mouse.
  virtual void OnChar();
  virtual void OnMouseMove();
  virtual void OnRightButtonDown();
  virtual void OnRightButtonUp();
  virtual void OnMiddleButtonDown();
  virtual void OnMiddleButtonUp();
  virtual void OnLeftButtonDown();
  virtual void OnLeftButtonUp();
  virtual void OnMouseWheelForward();
  virtual void OnMouseWheelBackward();

  // These methods for the different interactions in different modes
  // are overridden in subclasses to perform the correct motion. Since
  // they are called by OnTimer, they do not have mouse coord parameters
  // (use interactor's GetEventPosition and GetLastEventPosition)
  virtual void Pan();
  virtual void Dolly();
  virtual void Rotate();

protected:
  vtkInteractorStyle3DWPlot();
  ~vtkInteractorStyle3DWPlot();

  // Current view parameters
  double Latitude;
  double Longitude;
  double Roll;
  double Zoom;

  // Default view parameters
  double DefaultLatitude;
  double DefaultLongitude;
  double DefaultRoll;
  double DefaultZoom;

  // Speed of all motions
  double MotionSpeed;

  // Speed factor of individual motions
  double FactorRoll;

  // Retrieve mouse motion, both for trackball mode and joystick mode
  void getMouseMotion(double &dx, double &dy);

  // Called by macro setters to set current view
  virtual void Modified();

  // Set the current view
  void SetView();

private:
  vtkInteractorStyle3DWPlot(const vtkInteractorStyle3DWPlot&);  // Not implemented.
  void operator=(const vtkInteractorStyle3DWPlot&);                 // Not implemented.
};


#endif // __vtkInteractorStyle3DWPlot
