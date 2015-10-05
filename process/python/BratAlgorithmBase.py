#!/usr/bin/python -tt
#==============================================================================
#                       <BratAlgorithmBase.py>
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
  This python module contains the algorithm base class with abstract methods. 
  The user-defined algorithms shall be subclasses of this base class. 
'''

import abc


class PyBratAlgoBase(metaclass = abc.ABCMeta):

    Py_UNDEF         = 0
    Py_INT           = 1
    Py_LONG          = 2
    Py_FLOAT         = 4
    Py_BYTE          = 5
    Py_STRING        = 6
    
    @abc.abstractmethod
    def Run(self, PyAlgoParams):
        """Runs the algorithm
        \param[in]  PyAlgoParams  A list with the input parameters (type: List).
        \return     result        The result of the execution (type: Float).
        """

    @abc.abstractmethod
    def GetName(self):
        """Returns the name of the algorithm (type: String)."""

    @abc.abstractmethod
    def GetDescription(self):
        """Returns the description of the algorithm (type: String)."""

    @abc.abstractmethod
    def GetNumInputParam(self):
        """Returns the number of input parameters to pass to the 'Run' function 
        (Type: Integer)."""

    @abc.abstractmethod
    def GetInputParamDesc(self, indexParam):
        """Returns the description of an input parameter.
        \param[in]    indexParam   The parameter index. First parameter index is 
                                   0, last one is 'number of parameters - 1'
                                   (Type: Int).
        \return       description  Parameter description (Type: String)."""
    
    
    @abc.abstractmethod
    def GetInputParamFormat(self, indexParam):
        """ Gets the format of an input parameter:
        Py_INT           for integer;
        Py_LONG          for long;
        Py_FLOAT         for float;
        Py_BYTE          for byte;
        Py_STRING        for string;
        \param[in]   indexParam   Parameter index. First parameter index is 0, 
                                  last one is 'number of parameters - 1' 
                                  (Type: Int).
        \return      ParamFormat  Parameter format value (Type: Int).   """
    
    @abc.abstractmethod
    def GetInputParamUnit(self, indexParam):
        """Returns the unit of an input parameter:
        \param[in]   indexParam   Parameter index. First parameter index is 0, 
                                  last one is 'number of parameters - 1' 
                                  (Type: Int).
        \return      ParamUnit    Parameter unit (Type: String)."""
    
    @abc.abstractmethod
    def GetOutputUnit(self):
        """Returns the unit of an output value returned by the 'Run' function
         (Type: String)."""
    
    #------------------------------#
    #    End of abstract methods   #
    #------------------------------#
    def CheckInputParams(self, PyAlgoParams):
        """
        Checks the number of input parameters.
        """
        if len(PyAlgoParams) != self.GetNumInputParam():
            raise Exception ("Error while running algorithm %s:"%self.GetName()+
                             " number of input " +
                             "parameters (%d) are not the "%len(PyAlgoParams) +
                             "expected number (%d)."%self.GetNumInputParam()
                            )


