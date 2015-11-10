#include "vtkInteractorStyle3DWPlot.h"

#include "vtkCamera.h"
#include "vtkCommand.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"


//----------------------------------------------------------------------------
static inline void clipMin(double &x, double min)
{
  if (x < min)
    {
    x = min;
    }
}

//----------------------------------------------------------------------------
static inline void clipMax(double &x, double max)
{
  if (x > max)
    {
    x = max;
    }
}

//----------------------------------------------------------------------------
static inline void clipWrap(double &x, double min, double max)
{
  x = fmod(x - min, max - min);

  if (x < 0)
    {
    x += max;
    }
  else
    {
    x += min;
    }
 }

//----------------------------------------------------------------------------
void vtkInteractorStyle3DWPlot::latlon2position(double latitude, double longitude, double position[3])
{

  double theta = vtkMath::RadiansFromDegrees(latitude);         //latitude * vtkMath::DoubleDegreesToRadians();
  double phi   = vtkMath::RadiansFromDegrees(longitude);        //longitude * vtkMath::DoubleDegreesToRadians();

  // Convert spherical coordinates to cartesian coordinates
  position[0] = cos(phi) * cos(theta);
  position[1] = sin(phi) * cos(theta);
  position[2] =            sin(theta);
}

vtkStandardNewMacro(vtkInteractorStyle3DWPlot);
//----------------------------------------------------------------------------
vtkInteractorStyle3DWPlot::vtkInteractorStyle3DWPlot()
{
  // Start in trackball mode
  this->UseTimers = 0;

  this->MotionSpeed = 1.5;

  this->FactorRoll =  50.0;
  //KAVOK: set default Zoom
  this->SetDefaultViewParameters(0.0, 0.0, 0.0, 2.5);
  this->SetDefaultView();

}

//----------------------------------------------------------------------------
vtkInteractorStyle3DWPlot::~vtkInteractorStyle3DWPlot()
{
}

//----------------------------------------------------------------------------
void vtkInteractorStyle3DWPlot::OnChar()
{
  // Overrule the default key behavior of vtkInteractorStyle
  char keycode = this->Interactor->GetKeyCode();

  switch (keycode)
  {
    case 'T':
    case 't':
      // Switch to trackball mode
      this->UseTimers = 0;
      break;

    case 'J':
    case 'j':
      // switch to joystick mode
      this->UseTimers = 1;
      break;

    case 'R':
    case 'r':
      // Only allow resetting when we are not interacting
      if (this->State == VTKIS_START)
        {
        this->SetDefaultView();
        }

        //to re-render stuff (ugly..)
        Pan();
        EndPan();
        this->InvokeEvent(vtkCommand::InteractionEvent, NULL);
      break;

    case 'Q':
    case 'q':
    case 'U':
    case 'u':
    case 'W':
    case 'w':
    case 'S':
    case 's':
      // Use default behavior defined in parent class
      vtkInteractorStyle::OnChar();
      break;

    default:
      break;
  }
}

//----------------------------------------------------------------------------
void vtkInteractorStyle3DWPlot::OnMouseMove()
{
  int x = this->Interactor->GetEventPosition()[0];
  int y = this->Interactor->GetEventPosition()[1];

  switch (this->State)
    {
    case VTKIS_PAN:
      this->FindPokedRenderer(x, y);
      this->Pan();
      this->InvokeEvent(vtkCommand::InteractionEvent, NULL);
      break;

    case VTKIS_DOLLY:
      this->FindPokedRenderer(x, y);
      this->Dolly();
      this->InvokeEvent(vtkCommand::InteractionEvent, NULL);
      break;

    case VTKIS_ROTATE:
      this->FindPokedRenderer(x, y);
      this->Rotate();
      this->InvokeEvent(vtkCommand::InteractionEvent, NULL);
      break;
    }
}

 //----------------------------------------------------------------------------
void vtkInteractorStyle3DWPlot::OnLeftButtonDown()
{
  this->FindPokedRenderer(this->Interactor->GetEventPosition()[0],
                          this->Interactor->GetEventPosition()[1]);
  if (this->CurrentRenderer == NULL)
    {
    return;
    }

  if (this->Interactor->GetShiftKey())
    {
    if (this->Interactor->GetControlKey())
      {
      this->StartRotate();
      }
    else
      {
      this->StartDolly();
      }
    }
  else
    {
        this->StartPan();
    }
}

//----------------------------------------------------------------------------
void vtkInteractorStyle3DWPlot::OnLeftButtonUp()
{
  switch (this->State)
    {
    case VTKIS_DOLLY:
      this->EndDolly();
      break;
    case VTKIS_PAN:
      this->EndPan();
      break;
    case VTKIS_ROTATE:
      this->EndRotate();
      break;
    }
}

//----------------------------------------------------------------------------
void vtkInteractorStyle3DWPlot::OnMiddleButtonDown()
{
  this->FindPokedRenderer(this->Interactor->GetEventPosition()[0],
                          this->Interactor->GetEventPosition()[1]);
  if (this->CurrentRenderer == NULL)
    {
    return;
    }

  this->StartRotate();
}

//----------------------------------------------------------------------------
void vtkInteractorStyle3DWPlot::OnMiddleButtonUp()
{
  switch (this->State)
    {
    case VTKIS_ROTATE:
      this->EndRotate();
      break;
    }
}

//----------------------------------------------------------------------------
void vtkInteractorStyle3DWPlot::OnRightButtonDown()
{
  this->FindPokedRenderer(this->Interactor->GetEventPosition()[0],
                          this->Interactor->GetEventPosition()[1]);
  if (this->CurrentRenderer == NULL)
    {
    return;
    }

  this->StartDolly();
}

//----------------------------------------------------------------------------
void vtkInteractorStyle3DWPlot::OnRightButtonUp()
{
  switch (this->State)
    {
    case VTKIS_DOLLY:
      this->EndDolly();
      break;
    }
}

//----------------------------------------------------------------------------
void vtkInteractorStyle3DWPlot::OnMouseWheelForward()
{
      //this->MotionSpeed *= 1.05;
      // Added the zooming effect
      this->Zoom *= 1.05;

      SetView();
}

//----------------------------------------------------------------------------
void vtkInteractorStyle3DWPlot::OnMouseWheelBackward()
{
      //this->MotionSpeed /= 1.05;
      // Added the zooming effect
      this->Zoom /= 1.05;
      SetView();
}

void vtkInteractorStyle3DWPlot::getMouseMotion(double &dx, double &dy)
{
  if (this->CurrentRenderer == NULL)
    {
    return;
    }

  vtkRenderWindowInteractor *rwi = this->Interactor;

  if (this->GetUseTimers() == 1)
    {
    // When in joystick mode, use motion relative to window center
    double *center = this->CurrentRenderer->GetCenter();

    dx = (double)(rwi->GetEventPosition()[0] - center[0]) / 10.0;
    dy = (double)(rwi->GetEventPosition()[1] - center[1]) / 10.0;
    }
  else
    {
    // When in trackball mode, use motion relative to last position
    dx = (double)(rwi->GetEventPosition()[0] - rwi->GetLastEventPosition()[0]);
    dy = (double)(rwi->GetEventPosition()[1] - rwi->GetLastEventPosition()[1]);
    }
}

//----------------------------------------------------------------------------
void vtkInteractorStyle3DWPlot::Pan()
{
  if (this->CurrentRenderer == NULL)
    {
    return;
    }

  // Retrieve mouse motion
  double dx, dy = 0.0;
  getMouseMotion(dx, dy);

  // Determine pan speed
  int *size = this->CurrentRenderer->GetRenderWindow()->GetSize();

  // Panning acts as though the cursor is moving the earth as if it was a trackball (ie.
  // based on the tangent at the surface). The scale is defined by the window height and the
  // camera projection scheme. This is because the FoV is defiend in the vertical plane.

  // map a pixel to a tangent scale on the earth in earth-radius units.
  double speed = 2.0 * 3.0 / (this->Zoom * (double)size[1]);

  // Set the camera view
  if (this->GetUseTimers() == 1)
    {
    // Use reversed panning in joystick mode!
    // This behavior may seem counterintuitive when zoomed out, but it is very intuitive when zoomed in.
    this->Latitude  += 57.29577951 * dy * speed;
    this->Longitude += 57.29577951 * dx * speed;
    }
  else
    {
    // this SHOULD be 180/pi atan(dy*speed), but consider atan(x) ~= x
    this->Latitude  -= 57.29577951 * dy * speed;
    this->Longitude -= 57.29577951 * dx * speed;
    }
  this->SetView();
}

//----------------------------------------------------------------------------
void vtkInteractorStyle3DWPlot::Rotate()
{
  if (this->CurrentRenderer == NULL)
    {
    return;
    }

  // Retrieve mouse motion
  double dx, dy = 0.0;
  getMouseMotion(dx, dy);

  // Determine roll speed
  double *center = this->CurrentRenderer->GetCenter();
  double rollSpeed = this->FactorRoll * this->MotionSpeed / (double)center[1];

  // Set the camera view
  this->Roll -= (dy * rollSpeed);
  this->SetView();
}

//----------------------------------------------------------------------------
void vtkInteractorStyle3DWPlot::Dolly()
{
  if (this->CurrentRenderer == NULL)
    {
    return;
    }

  // Retrieve mouse motion
  double dx, dy = 0.0;
  getMouseMotion(dx, dy);

  // Determine zoom speed
  double *center = this->CurrentRenderer->GetCenter();
  double zoomSpeed = this->MotionSpeed * this->Zoom / (double)center[1];

  // Set the camera view - this is not actually a Dolly, but this hijacks the
  // Dolly interation scheme. This is done to prevent the camera from being
  // moved inside the earth.
  this->Zoom += (dy * zoomSpeed);
  this->SetView();
}

//----------------------------------------------------------------------------
void vtkInteractorStyle3DWPlot::Modified()
{
  vtkInteractorStyle::Modified();

  this->SetView();
}

//----------------------------------------------------------------------------
void vtkInteractorStyle3DWPlot::SetDefaultViewParameters(double latitude, double longitude, double roll, double zoom)
{
  this->DefaultLatitude  = latitude;
  this->DefaultLongitude = longitude;
  this->DefaultRoll      = roll;
  this->DefaultZoom      = zoom;
}

//----------------------------------------------------------------------------
void vtkInteractorStyle3DWPlot::GetDefaultViewParameters(double &latitude, double &longitude, double &roll, double &zoom)
{
  latitude  = this->DefaultLatitude;
  longitude = this->DefaultLongitude;
  roll      = this->DefaultRoll;
  zoom      = this->DefaultZoom;
}

//----------------------------------------------------------------------------
void vtkInteractorStyle3DWPlot::SetDefaultView()
{
  this->Latitude  = this->DefaultLatitude;
  this->Longitude = this->DefaultLongitude;
  this->Roll      = this->DefaultRoll;
  this->Zoom      = this->DefaultZoom;

  this->SetView();
}

//----------------------------------------------------------------------------
void vtkInteractorStyle3DWPlot::SetViewParameters(double latitude, double longitude, double roll, double zoom)
{
  this->Latitude  = latitude;
  this->Longitude = longitude;
  this->Roll      = roll;
  this->Zoom      = zoom;

  this->SetView();
}

//----------------------------------------------------------------------------
void vtkInteractorStyle3DWPlot::GetViewParameters(double &latitude, double &longitude, double &roll, double &zoom)
{
  latitude  = this->Latitude;
  longitude = this->Longitude;
  roll      = this->Roll;
  zoom      = this->Zoom;
}

//----------------------------------------------------------------------------
double vtkInteractorStyle3DWPlot::GetViewCenterLatitude()
{
  return this->Latitude;
}

//----------------------------------------------------------------------------
double vtkInteractorStyle3DWPlot::GetViewCenterLongitude()
{
  return this->Longitude;
}

//----------------------------------------------------------------------------
double vtkInteractorStyle3DWPlot::GetViewZoom()
{
  return this->Zoom;
}

//----------------------------------------------------------------------------
void vtkInteractorStyle3DWPlot::SetView()
{
  vtkRenderer *renderer = this->CurrentRenderer;
  if (renderer == NULL)
    {
    return;
    }

  //Restrict input values to proper range
  clipWrap(this->Longitude, -180.0, +180.0);    // longitude in [-180.0, +180.0)
  clipMin(this->Latitude, -90.0);               // latitude  in [-90.0, +90.0]
  clipMax(this->Latitude, +90.0);
  clipMin(this->Roll, -180.0);                  // roll      in [-180.0, +180.0)
  clipMax(this->Roll, +180.0);
  clipMin(this->Zoom, 1.0);                     // zoom      in [+1.0, -->)
  clipMax(this->Zoom, 1e6 );

  // Set default camera viewUp
  const double viewUp[3] = {0.0, 0.0, 1.0};

  // Determine the (x, y, z) position on the surface from (latitude, longitude)
  double positionSurface[3];
  latlon2position(this->Latitude, this->Longitude, positionSurface);

  // Position the camera so that it hovers above the unit sphere
  // The distance chosen makes the entire sphere visible
  double positionCamera[3];
  for (int i = 0; i < 3; i++)
    {
    positionCamera[i] = positionSurface[i] * 4.0;
    }

  // Reposition the camera so it hovers above the unit sphere and looks at the origin
  vtkCamera *camera = renderer->GetActiveCamera();
  camera->SetViewUp(viewUp);
  camera->SetFocalPoint(positionSurface);
  camera->SetPosition(positionCamera);
  camera->Roll(this->Roll);
  //camera->Dolly(1 / this->Zoom);

  double angle = 57.29577951 * 2.0 * atan(0.75 / this->Zoom); // approx 73 degrees at Zoom = 1.0

  camera->SetViewAngle(angle);

  // Adjust camera parameters
  if (this->AutoAdjustCameraClippingRange)
    {
    renderer->ResetCameraClippingRange();
    }

  vtkRenderWindowInteractor *renderWindowInteractor = this->Interactor;
  if (renderWindowInteractor == NULL)
    {
    return;
    }

  if (renderWindowInteractor->GetLightFollowCamera())
    {
    renderer->UpdateLightsGeometryToFollowCamera();
    }

  // Render
  renderWindowInteractor->Render();

  // Post and event to the renderWindowInteractor
  this->InvokeEvent("WorldViewChanged", &Zoom);
};

//----------------------------------------------------------------------------
void vtkInteractorStyle3DWPlot::PrintSelf(std::ostream& os, vtkIndent indent)
{
  vtkInteractorStyle::PrintSelf(os,indent);
  vtkInteractorStyle::PrintSelf(os,indent);

  os << indent << "Latitude: "  << this->Latitude  << std::endl
     << indent << "Longitude: " << this->Longitude << std::endl
     << indent << "Roll: "      << this->Roll      << std::endl
     << indent << "Zoom: "      << this->Zoom      << std::endl;

  os << indent << "DefaultLatitude: "  << this->DefaultLatitude  << std::endl
     << indent << "DefaultLongitude: " << this->DefaultLongitude << std::endl
     << indent << "DefaultRoll: "      << this->DefaultRoll      << std::endl
     << indent << "DefaultZoom: "      << this->DefaultZoom      << std::endl;

  os << indent << "MotionSpeed: " << this->MotionSpeed << std::endl
     << indent << "FactorRoll: "  << this->FactorRoll  << std::endl;
}
