/*
* This file is part of BRAT 
*
* BRAT is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* BRAT is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#include "new-gui/brat/stdafx.h"


#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#if !defined(Q_OS_WIN) && !defined(Q_OS_MAC)
#include <unistd.h>
#endif
#ifdef Q_OS_WIN
#include <io.h>
#include <fcntl.h>
#endif

#include "common/BratVersion.h"
#include "new-gui/brat/BratLogger.h"

#include "PSProcessing.h"


// data output mode: binary, ascii85, hex-ascii
// 
enum DATAMODE
{ 
	BINARY, 
	ASCII85, 
	ASCIIHEX 
};


struct imagedata
{
	FILE		*fp;				// file pointer for jpeg file	
	std::string	filename;			// name of image file		
	int      width;					// pixels per line		
	int      height;				// rows			
	int      components;			// number of color components	
	int      bits_per_component;	// bits per color component	
	float    dpi;					// image resolution in dots per inch
	DATAMODE mode;					// output mode: 8bit, ascii, ascii85
	long     startpos;				// offset to jpeg data	
	bool     landscape;				// rotate image to landscape mode?
	bool     adobe;					// image includes Adobe comment marker
};


#define	DPI_IGNORE (float) (-1.0)		// dummy value for imagedata.dpi
#define DPI_USE_FILE ((float) 0.0)		// dummy value for imagedata.dpi



typedef unsigned char byte;

static unsigned char buf[4];
static unsigned long power85[5] = { 1L, 85L, 85L*85, 85L*85*85, 85L*85*85*85};
static int outbytes;				// Number of characters in an output line

// read 0-4 Bytes. result: number of bytes read
// 
static int ReadSomeBytes(FILE * in)
{
	int count, i;
	for ( count = 0; count < 4; count++ )
	{
		if ((i = getc(in)) == EOF)
			break;
		else
			buf[count] = (byte) i;
	}
	return count;
}

// Two percent characters at the start of a line will cause trouble
// with some post-processing software. In order to avoid this, we
// simply insert a line break if we encounter a percent character
// at the start of the line. Of course, this rather simplistic
// algorithm may lead to a large line count in pathological cases,
// but the chance for hitting such a case is very small, and even
// so it's only a cosmetic flaw and not a functional restriction.
//
static void outbyte(byte c, FILE * out)    // output one byte
{
	if (fputc(c, out) == EOF) 
	{
		LOG_TRACE( "jpeg2ps: write error - exit!");
		return;
	}

	if (++outbytes > 63 ||				// line limit reached
		(outbytes == 1 && c == '%') )	// caution: percent character at start of line
	{
		fputc('\n', out);	// insert line feed
		outbytes = 0;
	}
}


int ASCII85Encode( FILE *in, FILE *out )
{
	int count;
	unsigned long word, v;

	outbytes = 0;

	// 4 bytes read ==> output 5 bytes
	//
	while ((count = ReadSomeBytes(in)) == 4) 
	{
		word = ((unsigned long)(((unsigned int)buf[0] << 8) + buf[1]) << 16) + (((unsigned int)buf[2] << 8) + buf[3]);
		if (word == 0)
			outbyte('z', out);       // shortcut for 0
		else
			// calculate 5 ASCII85 bytes and output them
			// 
			for (int i = 4; i >= 0; i--) {
				v = word / power85[i];
				outbyte((byte) (v + '!'), out);
				word -= v * power85[i];
			}
	}

	word = 0;

	if (count != 0)   // 1-3 bytes left
	{
		// accumulate bytes
		// 
		for (int i = count-1; i >= 0; i--)
			word += (unsigned long)buf[i] << 8 * (3-i);

		// encoding as above, but output only count+1 bytes
		// 
		for (int i = 4; i >= 4-count; i--) 
		{
			v = word / power85[i];
			outbyte((byte) (v + '!'), out);
			word -= v * power85[i];
		}
	}

	fputc('~', out);	// EOD marker
	fputc('>', out);
	return 0;
}


void ASCIIHexEncode(FILE *in, FILE * out) 
{
	static char buffer[512];
	static char BinToHex[] = "0123456789ABCDEF";

	size_t i, n;
	unsigned char *p;

	int CharsPerLine = 0;
	fputc('\n', out);

	while ((n = fread(buffer, 1, sizeof(buffer), in)) != 0)
		for (i = 0, p = (unsigned char *) buffer; i < n; i++, p++) 
		{
			fputc(BinToHex[*p>>4], out);           // first nibble
			fputc(BinToHex[*p & 0x0F], out);       // second nibble
			if ((CharsPerLine += 2) >= 64) {
				fputc('\n', out);
				CharsPerLine = 0;
			}
		}

	fputc('>', out);         // EOD marker for PostScript hex strings
}



// The following enum is stolen from the IJG JPEG library
// Comments added by tm
// This table contains far too many names since jpeg2ps
// is rather simple-minded about markers
//
//
enum JPEG_MARKER	// JPEG marker codes			
{		
	M_SOF0  = 0xc0,	// baseline DCT				
	M_SOF1  = 0xc1,	// extended sequential DCT		
	M_SOF2  = 0xc2,	// progressive DCT			
	M_SOF3  = 0xc3,	// lossless (sequential)		

	M_SOF5  = 0xc5,	// differential sequential DCT		
	M_SOF6  = 0xc6,	// differential progressive DCT		
	M_SOF7  = 0xc7,	// differential lossless		

	M_JPG   = 0xc8,	// JPEG extensions			
	M_SOF9  = 0xc9,	// extended sequential DCT		
	M_SOF10 = 0xca,	// progressive DCT			
	M_SOF11 = 0xcb,	// lossless (sequential)		

	M_SOF13 = 0xcd,	// differential sequential DCT		
	M_SOF14 = 0xce,	// differential progressive DCT		
	M_SOF15 = 0xcf,	// differential lossless		

	M_DHT   = 0xc4,	// define Huffman tables		

	M_DAC   = 0xcc,	// define arithmetic conditioning table	

	M_RST0  = 0xd0,	// restart				
	M_RST1  = 0xd1,	// restart				
	M_RST2  = 0xd2,	// restart				
	M_RST3  = 0xd3,	// restart				
	M_RST4  = 0xd4,	// restart				
	M_RST5  = 0xd5,	// restart				
	M_RST6  = 0xd6,	// restart				
	M_RST7  = 0xd7,	// restart				

	M_SOI   = 0xd8,	// start of image			
	M_EOI   = 0xd9,	// end of image				
	M_SOS   = 0xda,	// start of scan			
	M_DQT   = 0xdb,	// define quantization tables		
	M_DNL   = 0xdc,	// define number of lines		
	M_DRI   = 0xdd,	// define restart interval		
	M_DHP   = 0xde,	// define hierarchical progression	
	M_EXP   = 0xdf,	// expand reference image(s)		

	M_APP0  = 0xe0,	// application marker, used for JFIF	
	M_APP1  = 0xe1,	// application marker			
	M_APP2  = 0xe2,	// application marker			
	M_APP3  = 0xe3,	// application marker			
	M_APP4  = 0xe4,	// application marker			
	M_APP5  = 0xe5,	// application marker			
	M_APP6  = 0xe6,	// application marker			
	M_APP7  = 0xe7,	// application marker			
	M_APP8  = 0xe8,	// application marker			
	M_APP9  = 0xe9,	// application marker			
	M_APP10 = 0xea,	// application marker			
	M_APP11 = 0xeb,	// application marker			
	M_APP12 = 0xec,	// application marker			
	M_APP13 = 0xed,	// application marker			
	M_APP14 = 0xee,	// application marker, used by Adobe	
	M_APP15 = 0xef,	// application marker			

	M_JPG0  = 0xf0,	// reserved for JPEG extensions		
	M_JPG13 = 0xfd,	// reserved for JPEG extensions		
	M_COM   = 0xfe,	// comment				

	M_TEM   = 0x01,	// temporary use			

	M_ERROR = 0x100	// dummy marker, internal use only	
};

//
// The following routine used to be a macro in its first incarnation:
//  #define get_2bytes(fp) ((unsigned int) (getc(fp) << 8) + getc(fp))
// However, this is bad programming since C doesn't guarantee
// the evaluation order of the getc() calls! As suggested by
// Murphy's law, there are indeed compilers which produce the wrong
// order of the getc() calls, e.g. the Metrowerks C compilers for BeOS
// and Macintosh.
// Since there are only few calls we don't care about the performance 
// penalty and use a simplistic C function.
//
// read two byte parameter, MSB first
static unsigned int get_2bytes(FILE *fp)
{
	unsigned int val;
	val = (unsigned int) (getc(fp) << 8);
	val += (unsigned int) (getc(fp));
	return val;
}

static int next_marker(FILE *fp)
{ 
	// look for next JPEG Marker
	// 
	int c, nbytes = 0;

	if (feof(fp))
		return M_ERROR;                 // dummy marker

	do {
		do {                            // skip to FF
			nbytes++;
			c = getc(fp);
		} while (c != 0xFF);
		do {                            // skip repeated FFs
			c = getc(fp);
		} while (c == 0xFF);
	} while (c == 0);					// repeat if FF/00

	return c;
}

// analyze JPEG marker
// 
bool AnalyzeJPEG(imagedata *image) 
{
	int b, c, unit;
	unsigned long i, length = 0;
#define APP_MAX 255
	unsigned char appstring[APP_MAX];
	bool SOF_done = false;

	// Tommy's special trick for Macintosh JPEGs: simply skip some
	// hundred bytes at the beginning of the file!
	do {
		do {                            // skip if not FF
			c = getc(image->fp);
		} while (!feof(image->fp) && c != 0xFF);

		do {                            // skip repeated FFs
			c = getc(image->fp);
		} while (c == 0xFF);

		// remember start position
		// 
		if ((image->startpos = ftell(image->fp)) < 0L) {
			LOG_TRACE( "Error: internal error in ftell()!\n");
			return false;
		}
		image->startpos -= 2;           // subtract marker length

		if (c == M_SOI) {
			fseek(image->fp, image->startpos, SEEK_SET);
			break;
		}
	} while (!feof(image->fp));

	if (feof(image->fp)) {
		LOG_TRACE( "Error: SOI marker not found!\n");
		return false;
	}

	// process JPEG markers
	// 
	while (!SOF_done && (c = next_marker(image->fp)) != M_EOI) {
		switch (c) {
			case M_ERROR:
				LOG_TRACE( "Error: unexpected end of JPEG file!\n");
				return false;

				// The following are not officially supported in PostScript level 2 
			case M_SOF2:
			case M_SOF3:
			case M_SOF5:
			case M_SOF6:
			case M_SOF7:
			case M_SOF9:
			case M_SOF10:
			case M_SOF11:
			case M_SOF13:
			case M_SOF14:
			case M_SOF15:
				LOG_TRACEstd( "Warning: JPEG file uses compression method " + h2sx<std::string>(c) + " - proceeding anyway." );
				LOG_TRACE( "PostScript output does not work on all PS interpreters!" );
				
				// FALLTHROUGH

			case M_SOF0:
			case M_SOF1:
				length = get_2bytes(image->fp);    // read segment length

				image->bits_per_component = getc(image->fp);
				image->height             = (int) get_2bytes(image->fp);
				image->width              = (int) get_2bytes(image->fp);
				image->components         = getc(image->fp);

				SOF_done = true;
				break;

			case M_APP0:		// check for JFIF marker with resolution
				length = get_2bytes(image->fp);

				for (i = 0; i < length-2; i++) {	// get contents of marker
					b = getc(image->fp);
					if (i < APP_MAX)				// store marker in appstring
						appstring[i] = (unsigned char) b;
				}

				// Check for JFIF application marker and read density values
				// per JFIF spec version 1.02.
				// We only check X resolution, assuming X and Y resolution are equal.
				// Use values only if resolution not preset by user or to be ignored.
				//

#define ASPECT_RATIO	0	// JFIF unit byte: aspect ratio only
#define DOTS_PER_INCH	1	// JFIF unit byte: dots per inch
#define DOTS_PER_CM		2	// JFIF unit byte: dots per cm

				if (image->dpi == DPI_USE_FILE && length >= 14 &&
					!strncmp((const char *)appstring, "JFIF", 4)) {
					unit = appstring[7];		        // resolution unit
														// resolution value
					image->dpi = (float) ((appstring[8]<<8) + appstring[9]);

					if (image->dpi == 0.0) {
						image->dpi = DPI_USE_FILE;
						break;
					}

					switch (unit) {
						// tell the caller we didn't find a resolution value
						case ASPECT_RATIO:
							image->dpi = DPI_USE_FILE;
							break;

						case DOTS_PER_INCH:
							break;

						case DOTS_PER_CM:
							image->dpi *= (float) 2.54;
							break;

						default:				// unknown ==> ignore
							LOG_TRACE( "Warning: JPEG file contains unknown JFIF resolution unit - ignored!\n");
							image->dpi = DPI_IGNORE;
							break;
					}
				}
				break;

			case M_APP14:							// check for Adobe marker
				length = get_2bytes(image->fp);

				for (i = 0; i < length-2; i++) {	// get contents of marker
					b = getc(image->fp);
					if (i < APP_MAX)				// store marker in appstring
						appstring[i] = (unsigned char) b;
				}

				// Check for Adobe application marker. It is known (per Adobe's TN5116)
				// to contain the string "Adobe" at the start of the APP14 marker.
				//
				if (length >= 12 && !strncmp((const char *) appstring, "Adobe", 5))
					image->adobe = true;			// set Adobe flag

				break;

			case M_SOI:		// ignore markers without parameters
			case M_EOI:
			case M_TEM:
			case M_RST0:
			case M_RST1:
			case M_RST2:
			case M_RST3:
			case M_RST4:
			case M_RST5:
			case M_RST6:
			case M_RST7:
				break;

			default:			// skip variable length markers
				length = get_2bytes(image->fp);
				for (length -= 2; length > 0; length--)
					(void) getc(image->fp);
				break;
		}
	}

	// do some sanity checks with the parameters
	// 
	if (image->height <= 0 || image->width <= 0 || image->components <= 0) 
	{
		LOG_TRACE( "Error: DNL marker not supported in PostScript Level 2!\n");
		return false;
	}

	// some broken JPEG files have this but they print anyway...
	// 
	if (length != (unsigned int) (image->components * 3 + 8))
		LOG_TRACE( "Warning: SOF marker has incorrect length - ignored!");

	if (image->bits_per_component != 8) 
	{
		LOG_TRACE( "Error: % image->bits_per_component bits per color component " );
		LOG_TRACE( "not supported in PostScript level 2!");
		return false;
	}

	if (image->components!=1 && image->components!=3 && image->components!=4)
	{
		LOG_TRACE( "Error: unknown color space (% image->components components)!" );
		return false;
	}

	return true;
}



#if (defined(Q_OS_WIN) || defined (Q_OS_MAC))

#define JPEG2PS_READMODE  "rb"	// read JPEG files in binary mode
#define JPEG2PS_WRITEMODE "wb"	// write (some) PS files in binary mode
#else
#define JPEG2PS_READMODE  "r"
#define JPEG2PS_WRITEMODE "w"	// write (some) PS files in binary mode

#endif


static const int Margin = 20;	// safety margin


#define BUFFERSIZE 1024
static char buffer[BUFFERSIZE];
static const char *ColorSpaceNames[] = {"", "Gray", "", "RGB", "CMYK" };

// Array of known page sizes including name, width, and height

struct PageSize_s
{ 
	const char *name; 
	int width; 
	int height; 
};

PageSize_s PageSizes[] = 
{
	// ISO paper sizes
	// 
	{"a0",	2380, 3368},
	{"a1",	1684, 2380},
	{"a2",	1190, 1684},
	{"a3",	842, 1190},
	{"a4",	595, 842},
	{"a5",	421, 595},
	{"a6",	297, 421},
	{"b5",	501, 709},

	// US customary sizes
	// 
	{"letter",	612, 792},
	{"legal",	612, 1008},
	{"ledger",	1224, 792},
	{"p11x17",	792, 1224}
};


#define PAGESIZELIST	(sizeof(PageSizes)/sizeof(PageSizes[0]))


#ifdef A4
int PageWidth  = 595;           // page width A4
int PageHeight = 842;           // page height A4
#else
int PageWidth  = 612;           // page width letter
int PageHeight = 792;           // page height letter
#endif


bool JPEGtoPS( imagedata *JPEG, FILE *PSfile, const std::string &title, bool autorotate = false ) 
{
	int llx, lly, urx, ury;        // Bounding box coordinates
	size_t n;
	float scale, sx, sy;           // scale factors
	time_t t;
	int i;

	// read image parameters and fill JPEG struct
	// 
	if (!AnalyzeJPEG(JPEG)) 
	{
		LOG_TRACEstd( "Error: '" + JPEG->filename + "' is not a proper JPEG file!" );
		return false;
	}

	// "Use resolution from file" was requested, but we couldn't find any
	if ( JPEG->dpi == DPI_USE_FILE ) 
	{ 
		LOG_TRACE( "Note: no resolution values found in JPEG file - using standard scaling.");
		JPEG->dpi = DPI_IGNORE;
	}

	if (JPEG->dpi == DPI_IGNORE) 
	{
		if (JPEG->width > JPEG->height && autorotate) 	// switch to landscape if needed
		{
			JPEG->landscape = true;
			LOG_TRACE( "Note: image width exceeds height - producing landscape output!");
		}
		if (!JPEG->landscape) {			// calculate scaling factors
			sx = (float) (PageWidth - 2*Margin) / JPEG->width;
			sy = (float) (PageHeight - 2*Margin) / JPEG->height;
		}else {
			sx = (float) (PageHeight - 2*Margin) / JPEG->width;
			sy = (float) (PageWidth - 2*Margin) / JPEG->height;
		}
		scale = std::min(sx, sy);		// We use at least one edge of the page
	} else {
		//LOG_TRACE( "Note: Using resolution %d dpi.\n", (int) JPEG->dpi);
		scale = (float) (72.0 / JPEG->dpi);     // use given image resolution
	}

	if (JPEG->landscape) 
	{
		// landscape: move to (urx, lly)
		// 
		urx = PageWidth - Margin;
		lly = Margin;
		ury = (int) (Margin + scale*JPEG->width + 0.9);    // ceiling
		llx = (int) (urx - scale * JPEG->height);          // floor
	}
	else {
		// portrait: move to (llx, lly)
		// 
		llx = lly = Margin;
		urx = (int) (llx + scale * JPEG->width + 0.9);     // ceiling
		ury = (int) (lly + scale * JPEG->height + 0.9);    // ceiling
	}

	time(&t);

	// produce EPS header comments
	// 
	fprintf(PSfile, "%%!PS-Adobe-3.0 EPSF-3.0\n");
	fprintf(PSfile, "%%%%Creator: BRAT %s \n", BRAT_VERSION_STRING);
	fprintf(PSfile, "%%%%Title: %s\n", title.c_str());
	fprintf(PSfile, "%%%%CreationDate: %s", ctime(&t));
	fprintf(PSfile, "%%%%BoundingBox: %d %d %d %d\n", llx, lly, urx, ury);
	fprintf(PSfile, "%%%%DocumentData: %s\n", JPEG->mode == BINARY ? "Binary" : "Clean7Bit");
	fprintf(PSfile, "%%%%LanguageLevel: 2\n");
	fprintf(PSfile, "%%%%EndComments\n");
	fprintf(PSfile, "%%%%BeginProlog\n");
	fprintf(PSfile, "%%%%EndProlog\n");
	fprintf(PSfile, "%%%%Page: 1 1\n");

	fprintf(PSfile, "/languagelevel where {pop languagelevel 2 lt}");
	fprintf(PSfile, "{true} ifelse {\n");
	fprintf(PSfile, "  (JPEG file '%s' needs PostScript Level 2!",	JPEG->filename.c_str());
	fprintf(PSfile, "\\n) dup print flush\n");
	fprintf(PSfile, "  /Helvetica findfont 20 scalefont setfont ");
	fprintf(PSfile, "100 100 moveto show showpage stop\n");
	fprintf(PSfile, "} if\n");

	fprintf(PSfile, "save\n");
	fprintf(PSfile, "/RawData currentfile ");

	if (JPEG->mode == ASCIIHEX)            // hex representation...
		fprintf(PSfile, "/ASCIIHexDecode filter ");
	else if (JPEG->mode == ASCII85)        // ...or ASCII85
		fprintf(PSfile, "/ASCII85Decode filter ");
	// else binary mode: don't use any additional filter!

	fprintf(PSfile, "def\n");

	fprintf(PSfile, "/Data RawData << ");
	fprintf(PSfile, ">> /DCTDecode filter def\n");

	// translate to lower left corner of image
	// 
	fprintf(PSfile, "%d %d translate\n", (JPEG->landscape ? PageWidth - Margin : Margin), Margin);

	if (JPEG->landscape)                 // rotation for landscape
		fprintf(PSfile, "90 rotate\n");

	fprintf(PSfile, "%.2f %.2f scale\n", // scaling
		JPEG->width * scale, JPEG->height * scale);
	fprintf(PSfile, "/Device%s setcolorspace\n", 
		ColorSpaceNames[JPEG->components]);
	fprintf(PSfile, "{ << /ImageType 1\n");
	fprintf(PSfile, "     /Width %d\n", JPEG->width);
	fprintf(PSfile, "     /Height %d\n", JPEG->height);
	fprintf(PSfile, "     /ImageMatrix [ %d 0 0 %d 0 %d ]\n",
		JPEG->width, -JPEG->height, JPEG->height);
	fprintf(PSfile, "     /DataSource Data\n");
	fprintf(PSfile, "     /BitsPerComponent %d\n", 
		JPEG->bits_per_component);

	// workaround for color-inverted CMYK files produced by Adobe Photoshop:
	// compensate for the color inversion in the PostScript code
	//
	if (JPEG->adobe && JPEG->components == 4) 
	{
		LOG_TRACE( "Note: Adobe-conforming CMYK file - applying workaround for color inversion.");
		fprintf(PSfile, "     /Decode [1 0 1 0 1 0 1 0]\n");
	}
	else 
	{
		fprintf(PSfile, "     /Decode [0 1");
		for (i = 1; i < JPEG->components; i++) 
			fprintf(PSfile," 0 1");
		fprintf(PSfile, "]\n");
	}

	fprintf(PSfile, "  >> image\n");
	fprintf(PSfile, "  Data closefile\n");
	fprintf(PSfile, "  RawData flushfile\n");
	fprintf(PSfile, "  showpage\n");
	fprintf(PSfile, "  restore\n");
	fprintf(PSfile, "} exec");

	// seek to start position of JPEG data
	// 
	fseek(JPEG->fp, JPEG->startpos, SEEK_SET);

	switch (JPEG->mode) 
	{
		case BINARY:
			// important: ONE blank and NO newline
			// 
			fprintf(PSfile, " ");

#if defined(Q_OS_WIN)
			fflush(PSfile);         			// up to now we have CR/NL mapping
			setmode(fileno(PSfile), O_BINARY);	// continue in binary mode
#endif
												// copy data without change
			while ((n = fread(buffer, 1, sizeof(buffer), JPEG->fp)) != 0)
				fwrite(buffer, 1, n, PSfile);

#if defined(Q_OS_WIN)
			fflush(PSfile);						// binary yet
			setmode(fileno(PSfile), O_TEXT);	// text mode
#endif
			break;

		case ASCII85:
			fprintf(PSfile, "\n");

			// ASCII85 representation of image data
			// 
			if (ASCII85Encode(JPEG->fp, PSfile)) 
			{
				LOG_TRACE( "Error: internal problems with ASCII85Encode!");
				return false;
			}
			break;

		case ASCIIHEX:
			// hex representation of image data (useful for buggy dvips)
			// 
			ASCIIHexEncode(JPEG->fp, PSfile);
			break;
	}
	fprintf(PSfile, "\n%%%%EOF\n");

	return true;
}


bool Convert2PostScript( const std::string &in_path, const std::string &out_path, const std::string &title )
{
	assert__( IsFile( in_path ) );

	imagedata image;

	image.mode     = ASCII85;
	image.startpos = 0L;
	image.landscape= false;
	image.dpi      = DPI_IGNORE;
	image.adobe    = false;

	FILE *outfile = fopen( out_path.c_str(), "w" );
	if ( outfile == nullptr ) 
	{
		LOG_TRACEstd( "Error: cannot open output file '" + out_path + "'.\n" );
		return false;
	} 

	image.filename = in_path.c_str();

	if ((image.fp = fopen(image.filename.c_str(), JPEG2PS_READMODE)) == nullptr) 
	{
		LOG_TRACEstd( "Error: couldn't read input file '" + image.filename + "'!" );
		return false;
	}

	bool result = JPEGtoPS( &image, outfile, title );
	fclose(image.fp);
	fclose(outfile);

	return result;
}
