#!/usr/bin/python -tt
#
#  This file is part of BRAT.
#
#    BRAT is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    BRAT is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with BRAT.  If not, see <http://www.gnu.org/licenses/>.
#

'''
  This python module contains an example of a dummy algorithm that
  categorizes the wind speed according with Beaufort wind force scale.
  (Python v.3.3)
'''

from BratAlgorithmBase import PyBratAlgoBase


class Example_J2WindSpeedCat(PyBratAlgoBase):
    
    def __init__(self):
        self.m1_latitute = float()
        self.m2_longitude = float()
        self.m3_wind_speed = float()
        self.m4_surface_type = int()

    def Run(self, PyAlgoParams):
        self.SetParamValues(PyAlgoParams)
        self.Dump()

        if self.m4_surface_type > 2 : return -1  # float('NaN')

        if self.m3_wind_speed < 0.3  :  return 0
        if self.m3_wind_speed < 1.5  :  return 1
        if self.m3_wind_speed < 3.3  :  return 2
        if self.m3_wind_speed < 5.5  :  return 3
        if self.m3_wind_speed < 7.9  :  return 4
        if self.m3_wind_speed < 10.7 :  return 5
        if self.m3_wind_speed < 13.8 :  return 6
        if self.m3_wind_speed < 17.1 :  return 7
        if self.m3_wind_speed < 20.7 :  return 8
        if self.m3_wind_speed < 24.4 :  return 9
        if self.m3_wind_speed < 28.4 :  return 10
        if self.m3_wind_speed < 32.6 :  return 11
        else:                           return 12
        
    
    def GetName(self):
        return "Example_J2WindSpeedCat"
    
    def GetDescription(self):
        return ("Example of a dummy algorithm that categorizes the "
                + "wind speed according with Beaufort wind force scale. "
                + "It uses the 'surface_type' flag to return only wind "
                + "values over land.")

    def GetNumInputParam(self):
        return 4

    def GetInputParamName(self, indexParam):
        Param_dict = {0       : "%{lat}",
                      1       : "%{lon}",
                      2       : "wind_speed_alt",
                      3       : "surface_type",
                      }
        value = Param_dict.get(indexParam)
        return value
    
    def GetInputParamDesc(self, indexParam):
        Param_dict = {0       : "Latitude field",
                      1       : "Longitude field",
                      2       : "Wind speed field",
                      3       : ("Surface type field, where:\n"
                                 + "0 = open oceans or semi-closed seas\n"
                                 + "1 = enclosed seas or lakes\n"
                                 + "2 = continental ice\n"
                                 + "3 = land."),
                      }
        value = Param_dict.get(indexParam)
        return value
    
    def GetInputParamFormat(self, indexParam):
        Param_dict = {0       : PyBratAlgoBase.Py_FLOAT,
                      1       : PyBratAlgoBase.Py_FLOAT,
                      2       : PyBratAlgoBase.Py_FLOAT,
                      3       : PyBratAlgoBase.Py_INT,
                      }
        value = Param_dict.get(indexParam)
        return value
    
    def GetInputParamUnit(self, indexParam):
        Param_dict = {0       : "degrees_north",
                      1       : "degrees_east",
                      2       : "m/s",
                      3       : "count",
                      }
        value = Param_dict.get(indexParam)
        return value

    def GetOutputUnit(self):
        return "Beaufort number or scale level"

    #------------------------------------------#
    #   Used by Example_J2WindSpeedCat.Run()   #
    #------------------------------------------#
    def SetParamValues(self, PyAlgoParams):
        self.CheckInputParams(PyAlgoParams)

        self.m1_latitute = float(PyAlgoParams[0])
        self.m2_longitude = float(PyAlgoParams[1])
        self.m3_wind_speed = float(PyAlgoParams[2])
        self.m4_surface_type = int(PyAlgoParams[3])
    

    def Dump(self):
        fOut = ""
        fOut += ("\n==> Dump a Example_J2WindSpeedCat Object at " + str(hex(id(self))) +
                 "\n")
  
        fOut += ("m1_latitute: " + str(self.m1_latitute) + "\n")
        fOut += ("m2_longitude: " + str(self.m2_longitude) + "\n")
        fOut += ("m3_wind_speed: " + str(self.m3_wind_speed) + "\n")
        fOut += ("m4_surface_type: " + str(self.m4_surface_type) + "\n")
        
        fOut += ("==> END Dump a Example_J2WindSpeedCat Object at " + str(hex(id(self)))
                 + "\n")
        print (fOut)

 

def main():   
    """ xxx xxx"""
    
    print ('Subclass:', issubclass(Example_J2WindSpeedCat,   PyBratAlgoBase) )
    print ('Instance:', isinstance(Example_J2WindSpeedCat(), PyBratAlgoBase) )
    
    algo = Example_J2WindSpeedCat()
    print (" ")
    print ("Name:        " + str( algo.GetName() ))
    print ("Description: " + str( algo.GetDescription() ))
    print ("Nb of Input: " + str( algo.GetNumInputParam() ))
    print ("Par 1 name:  " + str( algo.GetInputParamDesc(1) ))
    
    print ("RESULT:      " + str( algo.Run([10, 20, 1.5, 2]) )
                           + " " + algo.GetOutputUnit()            )
    
    print ('\nSubclasses of PyAlgoBase: ')
    for sc in PyBratAlgoBase.__subclasses__():
        print (sc.__name__)
      


# This is the standard boilerplate that calls the main() function.
if __name__ == '__main__':
    main()
