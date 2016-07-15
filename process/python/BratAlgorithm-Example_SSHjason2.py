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
  This python module contains an example of an algorithm that calculates the
  Sea Surface Height from Jason2 data. It uses different fields and correction 
  parameteres to calculate SSH value.
  (Python v.3.3)
'''

from BratAlgorithmBase import PyBratAlgoBase

class Example_SSHjason2(PyBratAlgoBase):
    
    def __init__(self):
        self.m1_alt = float()
        self.m2_range_ku = float()
        self.m3_model_dry_tropo_corr = float()
        self.m4_hf_fluctuations_corr = float()
        self.m5_inv_bar_corr = float()
        self.m6_ocean_tide_sol1 = float()
        self.m7_solid_earth_tide = float()
        self.m8_pole_tide = float()
        self.m9_sea_state_bias_ku = float()
        self.m10_iono_corr_alt_ku = float()
        self.m11_rad_wet_tropo_corr = float()


    def Run(self, PyAlgoParams):
        self.SetParamValues(PyAlgoParams)
        self.Dump()
        
        # Plot only values over sea 
        if ( (self.m1_alt - self.m2_range_ku) > -130  and
             (self.m1_alt - self.m2_range_ku) < 100      ) :       
            return ( self.m1_alt - self.m2_range_ku - self.m3_model_dry_tropo_corr
                 - (self.m4_hf_fluctuations_corr + self.m5_inv_bar_corr)
                 - self.m6_ocean_tide_sol1 - self.m7_solid_earth_tide 
                 - self.m8_pole_tide - self.m9_sea_state_bias_ku 
                 - self.m10_iono_corr_alt_ku - self.m11_rad_wet_tropo_corr )
        else:
            return float('NaN')
        
    
    def GetName(self):
        return "Example_SSHjason2"
    
    def GetDescription(self):
        return ("Example of an algorithm that calculates the "
                + "Sea Surface Height from Jason2 data. "
                + "It uses different fields and correction parameteres to calculate SSH value.")

    def GetNumInputParam(self):
        return 11

    def GetInputParamName(self, indexParam):
        Param_dict = {0       : "%{alt}",
                      1       : "%{range_ku}",
                      2       : "model_dry_tropo_corr",
                      3       : "hf_fluctuations_corr",
                      4       : "inv_bar_corr",
                      5       : "ocean_tide_sol1",
                      6       : "solid_earth_tide",
                      7       : "pole_tide",
                      8       : "sea_state_bias_ku",
                      9       : "iono_corr_alt_ku",
                     10       : "rad_wet_tropo_corr",
                      }
        value = Param_dict.get(indexParam)
        return value
    
    def GetInputParamDesc(self, indexParam):
        Param_dict = {0       : "alt",
                      1       : "range_ku",
                      2       : "model_dry_tropo_corr",
                      3       : "hf_fluctuations_corr",
                      4       : "inv_bar_corr",
                      5       : "ocean_tide_sol1",
                      6       : "solid_earth_tide",
                      7       : "pole_tide",
                      8       : "sea_state_bias_ku",
                      9       : "iono_corr_alt_ku",
                     10       : "rad_wet_tropo_corr",
                      }
        value = Param_dict.get(indexParam)
        return value
    
    def GetInputParamFormat(self, indexParam):
        Param_dict = {0       : PyBratAlgoBase.Py_FLOAT,
                      1       : PyBratAlgoBase.Py_FLOAT,
                      2       : PyBratAlgoBase.Py_FLOAT,
                      3       : PyBratAlgoBase.Py_FLOAT,
                      4       : PyBratAlgoBase.Py_FLOAT,
                      5       : PyBratAlgoBase.Py_FLOAT,
                      6       : PyBratAlgoBase.Py_FLOAT,
                      7       : PyBratAlgoBase.Py_FLOAT,
                      8       : PyBratAlgoBase.Py_FLOAT,
                      9       : PyBratAlgoBase.Py_FLOAT,
                     10       : PyBratAlgoBase.Py_FLOAT,
                      }
        value = Param_dict.get(indexParam)
        return value
    
    def GetInputParamUnit(self, indexParam):
        Param_dict = {0       : "m",
                      1       : "m",
                      2       : "m",
                      3       : "m",
                      4       : "m",
                      5       : "m",
                      6       : "m",
                      7       : "m",
                      8       : "m",
                      9       : "m",
                     10       : "m",
                      }
        value = Param_dict.get(indexParam)
        return value

    def GetOutputUnit(self):
        return "m"

    #------------------------------------------#
    #   Used by Example_SSHjason2.Run()        #
    #------------------------------------------#
    def SetParamValues(self, PyAlgoParams):
        self.CheckInputParams(PyAlgoParams)

        self.m1_alt = float(PyAlgoParams[0])
        self.m2_range_ku = float(PyAlgoParams[1])
        self.m3_model_dry_tropo_corr = float(PyAlgoParams[2])
        self.m4_hf_fluctuations_corr = float(PyAlgoParams[3])
        self.m5_inv_bar_corr = float(PyAlgoParams[4])
        self.m6_ocean_tide_sol1 = float(PyAlgoParams[5])
        self.m7_solid_earth_tide = float(PyAlgoParams[6])
        self.m8_pole_tide = float(PyAlgoParams[7])
        self.m9_sea_state_bias_ku = float(PyAlgoParams[8])
        self.m10_iono_corr_alt_ku = float(PyAlgoParams[9])
        self.m11_rad_wet_tropo_corr = float(PyAlgoParams[10])
    

    def Dump(self):
        fOut = ""
        fOut += ("\n==> Dump a Example_SSHjason2 Object at " + str(hex(id(self))) +
                 "\n")

        # fOut += ("m1_alt: " + str(self.m1_alt) + "\n")
        
        fOut += ("==> END Dump a Example_SSHjason2 Object at " + str(hex(id(self)))
                 + "\n")
        print (fOut)

 

def main():   
    """ xxx xxx"""
    
    print ('Subclass:', issubclass(Example_SSHjason2,   PyBratAlgoBase) )
    print ('Instance:', isinstance(Example_SSHjason2(), PyBratAlgoBase) )
    
    algo = Example_SSHjason2()
    print (" ")
    print ("Name:        " + str( algo.GetName() ))
    print ("Description: " + str( algo.GetDescription() ))
    print ("Nb of Input: " + str( algo.GetNumInputParam() ))
    print ("Par 1 name:  " + str( algo.GetInputParamDesc(1) ))
    
    print ("RESULT:      " + str( algo.Run([60, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10]) )
                           + " " + algo.GetOutputUnit()            )
    
    print ('\nSubclasses of PyAlgoBase: ')
    for sc in PyBratAlgoBase.__subclasses__():
        print (sc.__name__)
      


# This is the standard boilerplate that calls the main() function.
if __name__ == '__main__':
    main()
