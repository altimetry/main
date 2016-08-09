BratAlgorithmBase.py
Must be imported by every user written Python algorithm to be loaded in brat.

BratAlgorithmExample1.py is used by the command line executable RunPythonAlgorithm,
a prototype for embedding Python.

BratAlgorithm-PyAlgoExample.py
and
BratAlgorithm-PyAlgoExampleAnother.py

are examples of user written Python algorithms that can be loaded by brat. To test
them, they must be copied first, with BratAlgorithmBase.py, to the sub-directory 
"python" of the "Default Data Directory", which you can specify in the brat 
application settigs dialog. 
At start-up, brat will always search this directory for python algorithms to load.



### HOW TO INSTALL SOFTWARE PACKAGES IN PYTHON ( INCLUDED WITH BRAT ) ######

 => DEBIAN:

    "Pip" is not only the recommended tool as the easiest way to install Python packages.
    The Python version included in BRAT installation has not "pip" installed. So, the user has 2 options: 
    
    1. Install "pip-3.2" (and also python3, in the case it's not installed ) in your system. 
       >> sudo apt-get install python3-pip
    
    2. Follow the steps described in the link below for installing "pip-3.2" in the Python included in BRAT installation.
       https://pip.pypa.io/en/stable/installing/#do-i-need-to-install-pip
    
    Then, use "pip" command line interface for installing Python software packages, where [BRAT_INSTALL_DIR] is the
    BRAT installation directory and [PACKAGE_NAME] the package name.
​        >> pip-3.2 install --target=[BRAT_INSTALL_DIR]/bin/Python/lib/python3.2 [PACKAGE_NAME]
