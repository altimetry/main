#!/usr/bin/python -tt
#==============================================================================
#                       <BratAlgorithmExample1.py>
#==============================================================================
# Project   : S3-ALTB
# Company   : Deimos Engenharia
# Component : BRAT
# Language  : Python (v.3.3)
#------------------------------------------------------------------------------
# Scope : (see the following docstring)
# Usage : ----
#==============================================================================
# $Revision:  $:
# $LastChangedBy:  $:
# $LastChangedDate:  $:
#==============================================================================

'''
  This python module contains the code used for defining a new algorithm, and
  it shall be taken as an example.
'''

from BratAlgorithmBase import PyBratAlgoBase


class PyAlgoExample1(PyBratAlgoBase):
    
    def __init__(self):
        self.m_1 = float()
        self.m_2 = int()
        self.m_3 = str()
        self.m_4 = int()
        self.m_5 = bytes()

    def Run(self, PyAlgoParams):
        self.SetParamValues(PyAlgoParams)
        self.Dump()
        
        return float(self.m_2) + float(self.m_4)
        
    
    def GetName(self):
        return "Example1"
    
    def GetDescription(self):
        return ("Example 1 of algorithm " +
                "- return (parameter 2 + parameter 4)")

    def GetNumInputParam(self):
        return 5
    
    def GetInputParamDesc(self, indexParam):
        Param_dict = {0       : "first parameter",
                      1       : "second parameter",
                      2       : "third parameter",
                      3       : "fourth parameter",
                      4       : "fifth parameter",
                      }
        value = Param_dict.get(indexParam)
        return value
    
    def GetInputParamFormat(self, indexParam):
        Param_dict = {0       : PyBratAlgoBase.Py_FLOAT,
                      1       : PyBratAlgoBase.Py_INT,
                      2       : PyBratAlgoBase.Py_STRING,
                      3       : PyBratAlgoBase.Py_LONG,
                      4       : PyBratAlgoBase.Py_BYTE,
                      }
        value = Param_dict.get(indexParam)
        return value
    
    def GetInputParamUnit(self, indexParam):
        Param_dict = {0       : "degrees_north",
                      1       : "m",
                      2       : "",
                      3       : "m", 
                      4       : ""   
                      }
        value = Param_dict.get(indexParam)
        return value

    def GetOutputUnit(self):
        return "m"

    #----------------------------------#
    #   Used by PyAlgoExample1.Run()   #
    #----------------------------------#
    def SetParamValues(self, PyAlgoParams):
        self.CheckInputParams(PyAlgoParams)
        
        self.m_1 = float(PyAlgoParams[0])
        self.m_2 =   int(PyAlgoParams[1])
        self.m_3 =   str(PyAlgoParams[2])
        self.m_4 =   int(PyAlgoParams[3])
        self.m_5 = bytes([PyAlgoParams[4]])
    
    def Dump(self):
        fOut = ""
        fOut += ("\n==> Dump a PyAlgoExample1 Object at " + str(hex(id(self))) + 
                 "\n")
  
        fOut += ("m_1: " + str(self.m_1) + "\n")
        fOut += ("m_2: " + str(self.m_2) + "\n")
        fOut += ("m_3: " + str(self.m_3) + "\n")
        fOut += ("m_4: " + str(self.m_4) + "\n")
        fOut += ("m_5: " + str(self.m_5) + "\n")
        
        fOut += ("==> END Dump a PyAlgoExample1 Object at " + str(hex(id(self)))
                 + "\n")
        print (fOut)

 

def main():   
    """ xxx xxx"""
    
    print ('Subclass:', issubclass(PyAlgoExample1,   PyBratAlgoBase) )
    print ('Instance:', isinstance(PyAlgoExample1(), PyBratAlgoBase) )
    
    algo = PyAlgoExample1()
    print (" ")
    print ("Name:        " + str( algo.GetName() ))
    print ("Description: " + str( algo.GetDescription() ))
    print ("Nb of Input: " + str( algo.GetNumInputParam() ))
    
    print ("RESULT:      " + str( algo.Run([10, 20, 'abcd', 40, 42]) )
                           + " " + algo.GetOutputUnit()            )
    
    print ('\nSubclasses of PyAlgoBase: ')
    for sc in PyBratAlgoBase.__subclasses__():
        print (sc.__name__)
      


# This is the standard boilerplate that calls the main() function.
if __name__ == '__main__':
    main()