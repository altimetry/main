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
  This python module contains an example of an algorithm that uses the Numpy module.
  It creates and prints a random matrix.
  (Python v.3.3)
'''

### Importing Brat Algorithm base ###
from BratAlgorithmBase import PyBratAlgoBase

### Extending list of search path for Python modules ###
# In this way, all Python modules installed in the user system can be imported 
# into this algorithm.
import sys
sys.path.extend( [
                  #'/usr/lib/python3.2', 
                  #'/usr/lib/python3.2/plat-linux2', 
                  #'/usr/lib/python3.2/lib-dynload', 
                  #'/usr/lib/python3/dist-packages',
                  '/usr/local/lib/python3.2/dist-packages' 
                  ]
                )

### Algorithm imports ####
import numpy as np

class Example_AlgoUsingNumpy(PyBratAlgoBase):
    
    def __init__(self):
        self.m1_printMatrix = float()


    def Run(self, PyAlgoParams):
        self.SetParamValues(PyAlgoParams)
        self.Dump()
        
        ## Creating a Numpy 3x5 matrix ######
        a = np.random.randint (10, size=(3, 5))
        
        if ( self.m1_printMatrix > 0 ) :     
            
            ##  NUMPY TEST ########################
            print ("======== MY ARRAY IS : ========")
            print (a)
            print ("===============================")
            #######################################
            return a[0][0]
        else:
            return float('NaN')
        
    
    def GetName(self):
        return "Example_AlgoUsingNumpy"
    
    def GetDescription(self):
        return ("Example of an algorithm that uses the Numpy Python module. " +
                "It creates and prints a random matrix.")

    def GetNumInputParam(self):
        return 1

    def GetInputParamName(self, indexParam):
        Param_dict = {0       : "Print_Matrix",
                      }
        value = Param_dict.get(indexParam)
        return value
    
    def GetInputParamDesc(self, indexParam):
        Param_dict = {0       : "If value is bigger than 0, the matrix is printed on Brat log and returns the first element.",
                      }
        value = Param_dict.get(indexParam)
        return value
    
    def GetInputParamFormat(self, indexParam):
        Param_dict = {0       : PyBratAlgoBase.Py_FLOAT,
                      }
        value = Param_dict.get(indexParam)
        return value
    
    def GetInputParamUnit(self, indexParam):
        Param_dict = {0       : "count",
                      }
        value = Param_dict.get(indexParam)
        return value

    def GetOutputUnit(self):
        return "count"

    #-----------------------------------------------#
    #   Used by Example_AlgoUsingNumpy.Run()        #
    #-----------------------------------------------#
    def SetParamValues(self, PyAlgoParams):
        self.CheckInputParams(PyAlgoParams)

        self.m1_printMatrix = float(PyAlgoParams[0])
    

    def Dump(self):
        fOut = ""
        fOut += ("\n==> Dump a Example_AlgoUsingNumpy Object at " + str(hex(id(self))) +
                 "\n")

        fOut += ("m1_printMatrix: " + str(self.m1_printMatrix) + "\n")
        
        fOut += ("==> END Dump a Example_AlgoUsingNumpy Object at " + str(hex(id(self)))
                 + "\n")
        print (fOut)

 

def main():   
    """ xxx xxx"""
    
    print ('Subclass:', issubclass(Example_AlgoUsingNumpy,   PyBratAlgoBase) )
    print ('Instance:', isinstance(Example_AlgoUsingNumpy(), PyBratAlgoBase) )
    
    algo = Example_AlgoUsingNumpy()
    print (" ")
    print ("Name:               " + str( algo.GetName() ))
    print ("Description:        " + str( algo.GetDescription() ))
    print ("Nb of Input:        " + str( algo.GetNumInputParam() ))
    print ("Par 1 description:  " + str( algo.GetInputParamDesc(0) ))
    
    print ("RESULT:      " + str( algo.Run([1]) )
                           + " " + algo.GetOutputUnit()            )
    
    print ('\nSubclasses of PyAlgoBase: ')
    for sc in PyBratAlgoBase.__subclasses__():
        print (sc.__name__)
      


# This is the standard boilerplate that calls the main() function.
if __name__ == '__main__':
    main()
