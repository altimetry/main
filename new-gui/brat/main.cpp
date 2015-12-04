#include "stdafx.h"

#include <QMainWindow>

#include <qgsapplication.h>

#if defined (RCCC_test)
/// RCCC test - to delete ////////////////////////////////////////
#include "new-gui/Common/tools/CoreTypes.h"
#include "new-gui/Common/+Utils.h"
/////////////////////////////////////////////////////////////////

#endif

int main(int argc, char *argv[])
{
    /// RCCC test - to delete ////////////////////////////////////////

    /// CStringArray Test: deleted operator= method //
    /*
    CStringArray s1, s2;

    s1.push_back("This is the first string");
    s1.push_back("This is the second string");

    // Using operator =
    s2 = s1;

    // Using operator ==
    if (s1 == s2)   { std::cout << "CStringArray s1 and s2 are equal!" << std::endl;}
    else            { std::cout << ">>> ERROR!!!!" << std::endl;                        }

    for (int i = 0; i < 2; ++i )
    {
      std::cout << "s1 [" << i << "] => " << s1[i] << std::endl;
      std::cout << "s2 [" << i << "] => " << s2[i] << std::endl;
    }
    */
    /// CInt64Array Test: deleted operator= method //
    /*
    CInt64Array s1, s2;

    s1.push_back(int64_t(1234567890));
    s1.push_back(int64_t(9876543210));

    // Using operator =
    s2 = s1;

    // Using operator ==
    if (s1 == s2)   { std::cout << "CInt64Array s1 and s2 are equal!" << std::endl;}
    else            { std::cout << ">>> ERROR!!!!" << std::endl;                   }

    for (int i = 0; i < 2; ++i )
    {
      std::cout << "s1 [" << i << "] => " << s1[i] << std::endl;
      std::cout << "s2 [" << i << "] => " << s2[i] << std::endl;
    }
    */
    /// CUIntArray Test: deleted operator= method //
    /*
    CUIntArray s1, s2;

    s1.push_back(uint32_t(1234567890));
    s1.push_back(uint32_t(4123456789));

    // Using operator =
    s2 = s1;

    // Using operator ==
    if ( s1 == s2 ) { std::cout << "CUIntArray s1 and s2 are equal!" << std::endl;}
    else            { std::cout << ">>> ERROR!!!!" << std::endl;                   }

    for (int i = 0; i < 2; ++i )
    {
      std::cout << "s1 [" << i << "] => " << s1[i] << std::endl;
      std::cout << "s2 [" << i << "] => " << s2[i] << std::endl;
    }
    */
    /// CUInt64Array Test: deleted operator= method //
    /*
    CUInt64Array s1, s2;

    s1.push_back(uint64_t(1234567890));
    s1.push_back(uint64_t(9876543210));

    // Using operator =
    s2 = s1;

    // Using operator ==
    if (s1 == s2) { std::cout << "CUInt64Array s1 and s2 are equal!" << std::endl;}
    else            { std::cout << ">>> ERROR!!!!" << std::endl;                   }

    for (int i = 0; i < 2; ++i )
    {
      std::cout << "s1 [" << i << "] => " << s1[i] << std::endl;
      std::cout << "s2 [" << i << "] => " << s2[i] << std::endl;
    }
    */
    /// CInt16Array Test: deleted operator= method //
    /*
    CInt16Array s1, s2;

    s1.push_back(int16_t(-32760));
    s1.push_back(int16_t(32767));

    // Using operator =
    s2 = s1;

    // Using operator ==
    if (s1 == s2) { std::cout << "CInt16Array s1 and s2 are equal!" << std::endl;}
    else            { std::cout << ">>> ERROR!!!!" << std::endl;                   }

    for (int i = 0; i < 2; ++i )
    {
      std::cout << "s1 [" << i << "] => " << s1[i] << std::endl;
      std::cout << "s2 [" << i << "] => " << s2[i] << std::endl;
    }
    */
    /// CUInt16Array Test: deleted operator= method //
    /*
    CUInt16Array s1, s2;

    s1.push_back(uint16_t(65534));
    s1.push_back(uint16_t(12345));

    // Using operator =
    s2 = s1;

    // Using operator ==
    if (s1 == s2) { std::cout << "CUInt16Array s1 and s2 are equal!" << std::endl;}
    else            { std::cout << ">>> ERROR!!!!" << std::endl;                   }

    for (int i = 0; i < 2; ++i )
    {
      std::cout << "s1 [" << i << "] => " << s1[i] << std::endl;
      std::cout << "s2 [" << i << "] => " << s2[i] << std::endl;
    }
    */
    /// CInt8Array Test: deleted operator= method //
    /*
    CInt8Array s1, s2;

    s1.push_back(int8_t(-127));
    s1.push_back(int8_t(127));

    // Using operator =
    s2 = s1;

    // Using operator ==
    if (s1 == s2) { std::cout << "CInt8Array s1 and s2 are equal!" << std::endl;}
    else          { std::cout << ">>> ERROR!!!!" << std::endl;                  }

    for (int i = 0; i < 2; ++i )
    {
      std::cout << "s1 [" << i << "] => " << int16_t(s1[i]) << std::endl;
      std::cout << "s2 [" << i << "] => " << int16_t(s2[i]) << std::endl;
    }
    */
    /// CUInt8Array Test: deleted operator= method //
    /*
    CUInt8Array s1, s2;

    s1.push_back(uint8_t(127));
    s1.push_back(uint8_t(250));

    // Using operator =
    s2 = s1;

    // Using operator ==
    if (s1 == s2) { std::cout << "CUInt8Array s1 and s2 are equal!" << std::endl;}
    else          { std::cout << ">>> ERROR!!!!" << std::endl;                   }

    for (int i = 0; i < 2; ++i )
    {
      std::cout << "s1 [" << i << "] => " << int16_t(s1[i]) << std::endl;
      std::cout << "s2 [" << i << "] => " << int16_t(s2[i]) << std::endl;
    }
    */
    /// CFloatArray Test: deleted operator= method //
    /*
    CFloatArray s1, s2;

    s1.push_back(float(1234567890));
    s1.push_back(float(0.01234567));

    // Using operator =
    s2 = s1;

    // Using operator ==
    if (s1 == s2) { std::cout << "CFloatArray s1 and s2 are equal!" << std::endl;}
    else          { std::cout << ">>> ERROR!!!!" << std::endl;                   }

    for (int i = 0; i < 2; ++i )
    {
      std::cout << "s1 [" << i << "] => " << s1[i] << std::endl;
      std::cout << "s2 [" << i << "] => " << s2[i] << std::endl;
    }
    */
    /// CDoubleArray Test: deleted operator= method //
    /*
    CDoubleArray s1, s2;

    s1.push_back(double(1234567890));
    s1.push_back(double(0.01234567));

    // Using operator =
    s2 = s1;

    // Using operator ==
    if (s1 == s2) { std::cout << "CDoubleArray s1 and s2 are equal!" << std::endl;}
    else          { std::cout << ">>> ERROR!!!!" << std::endl;                   }

    for (int i = 0; i < 2; ++i )
    {
      std::cout << "s1 [" << i << "] => " << s1[i] << std::endl;
      std::cout << "s2 [" << i << "] => " << s2[i] << std::endl;
    }
    */

#if defined (RCCC_test)
    /// StringReplace Test ///////////
    std::string str1 ("There is one bird in this bird cage.");
    std::string str2 ("bird");
    std::string replaceBy ("canary");
#endif

    // different member versions of find in the same order as above:
//    std::size_t found = str1.find(str2);
//    if (found!=std::string::npos)
//      std::cout << "first 'bird' found at: " << found << '\n';

//    found=str1.find("bird is small",found+1,4);
//    if (found!=std::string::npos)
//      std::cout << "second 'bird' found at: " << found << '\n';

    // let's replace the first bird:
//    str1.replace(str1.find(str2),str2.length(),replaceBy);
//    std::cout << str1 << '\n';

    // let's replace all "bird":
    /*
    std::string str1_rep = str1;

    std::size_t found = i_find(str1_rep, str2);
    std::cout << "Found at: " << found << "/" << str1_rep.length() << '\n';
    std::cout << str1_rep << '\n';


    while (found != str1_rep.length())
    {
        str1_rep.replace(found, str2.length(), replaceBy);
        found = i_find(str1_rep, str2, found + replaceBy.length());
        std::cout << "Found at: " << found << "/" << str1_rep.length() << '\n';
        std::cout << str1_rep << '\n';
    }
    */

    /*
    // Let's use StringReplace():
    std::string str1_rep = replace( str1, str2, replaceBy, false);
    std::cout << str1_rep << '\n';
    */

    /////////////////////////////////////////////////////////////////

    QgsApplication a(argc, argv, true);

    QMainWindow w;
    w.show();

    return a.exec();
}
