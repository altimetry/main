#if defined( __cplusplus )
extern "C" {
#endif  /* __cplusplus */



#include <stdio.h>
#include <stdlib.h>


#include "brathl.h"
#include "brathl_error.h"
#include "brathlc.h"


int main(int argc, char **argv)
{
  char		*Names[10];
  int32_t	ReturnCode;
  double	*Data[2]	= {NULL,NULL};
  int32_t	Sizes[2]	= {-1, -1};
  char		*Expressions[2];
  char		*Units[2];
  int32_t	ActualSize;
  int32_t i = 0;
  int32_t j = 0;
  double *vector1 = NULL;
  double *vector2 = NULL;

  // Register Brat algorithms
  brathl_RegisterAlgorithms();
  
  // Load aliases dictionnary  
  brathl_LoadAliasesDictionary();

  Names[0]	= "C:\\BratData\\Jason1\\JA1_SDR_2PBP113_001.NASA";
  Names[1]	= "C:\\BratData\\Jason1\\JA1_SDR_2PbP113_085.NASA";
  Names[2]	= "C:\\BratData\\Jason1\\JA1_SDR_2PbP232_085.CNES";

  Expressions[0]	= "longitude";
  Units[0]		= "count";
  Expressions[1]	= "swh_ku";
  Units[1]		= "count";

  // Here is an expression using the  alias 'swh': %{sig_wave_height} -->  'swh' field in tha aliases dictionary (Jason-1)
  //Expressions[1]	= "%{sig_wave_height}";
  //Units[1]		= "count";

  // Here is an expression calling the 'BratAlgoFilterMedianAtp' algorithm
  //Expressions[1] = "exec(\"BratAlgoFilterMedianAtp\", %{sig_wave_height}, 7, 1, 0)";
  //Units[1]		= "count";


  ReturnCode	= brathl_ReadData(2, Names,
				  "data",
          //				  "latitude > 20 && latitude < 30",
				  NULL,
				  2,
				  Expressions,
				  Units,
				  Data,
				  Sizes,
				  &ActualSize,
				  0,
				  0,
				  18446744073709551616.0);
  printf("Return code          : %d\n", ReturnCode);
  printf("Actual number of data: %d\n", ActualSize);

  vector1 = Data[0];
  vector2 = Data[1];
  
  for (i = 0 ; i < ActualSize ; i++)
  {    
    printf("%d %s=%f, %s=%f\n", i, Expressions[0], vector1[i], Expressions[1], vector2[i]);
  }
 
  /* 
  for (i = 0 ; i < 2 ; i++)
  {
    for (j = 0 ; j < ActualSize ; j++)
    {    
      printf("%s=%f\n", Expressions[i], Data[i][j]);
    }
  }
  */
  return 0;
}

#if defined( __cplusplus )
}
#endif  /* __cplusplus */
