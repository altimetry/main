#ifndef VTKVELOCITYGLYPHFILTER_H_INCLUDED
#define VTKVELOCITYGLYPHFILTER_H_INCLUDED

#include "vtkPolyDataSource.h"
#include "vtkAppendPolyData.h"
#include "vtkGlyphSource2D.h"
#include "vtkGlyph2D.h"
#include "vtkMaskPoints.h"
#include "vtkConeSource.h"
//#include "vtkSphereSource.h"
#include "vtkGeoMapFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkPointData.h"

class vtkCellArray;
class vtkDoubleArray;
class vtkPoints;
class vtkPolyData;
class vtkShortArray;

class VTK_EXPORT vtkVelocityGlyphFilter : public vtkGeoMapFilter
{

    public:

        static vtkVelocityGlyphFilter *New();

        virtual void SetValuesNorth(vtkDoubleArray*);
        vtkGetObjectMacro(ValuesNorth,vtkDoubleArray);

        virtual void SetValuesEast(vtkDoubleArray*);
        vtkGetObjectMacro(ValuesEast,vtkDoubleArray);

        vtkSetMacro(IsGlyph,bool);
        vtkGetMacro(IsGlyph,bool);

        vtkSetMacro(MaxVE, double);
        vtkSetMacro(MaxVN, double);
        vtkSetMacro(MinVE, double);
        vtkSetMacro(MinVN, double);
        vtkSetMacro(MaxV, double);

        vtkSetClampMacro(GroupRatio, int, 0, 10);

        vtkSetClampMacro(InitLat, double, -90, 90);
        vtkSetClampMacro(EndLat, double, -90, 90);
        vtkSetClampMacro(InitLon, double, -180, 180);
        vtkSetClampMacro(EndLon, double, -180, 180);

        vtkSetClampMacro(MidLat, double, -90, 90);
        vtkSetClampMacro(MidLon, double, -180, 180);

    protected:

        vtkVelocityGlyphFilter();
        virtual ~vtkVelocityGlyphFilter();

        virtual void Execute();

    private:

        vtkDoubleArray*       ValuesNorth;
        vtkDoubleArray*       ValuesEast;

        double MaxVN, MinVN;
        double MaxVE, MinVE;
        double MaxV;

        double InitLat, EndLat, InitLon, EndLon;
        double MidLat, MidLon;

        bool IsGlyph;

        int GroupRatio;
};


#endif // VTKVELOCITYGLYPHFILTER_H_INCLUDED
