
#define DEVICE_MEMORY	15

/* the following device codes should be unprintable (0-31)
   otherwise amend the selection validity check in DECODE.C */

#define UNKNOWN      31
#define U_TIFF       0
#define WHITE        0x00
#define BLACK        0xff
#define EOLFLAG      0x01
#define EOPFLAG      0x04
#define TAB          0x09
#define FF           0x0c
#define SPACE        0x20

/* a few magic numbers follow , from decode.h*/

#define BLACK       0xff
#define INIT        -1
#define DEINIT      -2
#define OUREOP      -3
#define BADRUN      3000
#define OUREOL      3001
#define OUREOL2     3002
#define HORZMODE    3003
#define VTMODE3N    3004
#define VTMODE2N    3005
#define VTMODE1N    3006
#define VTMODE0     3007
#define VTMODE1P    3008
#define VTMODE2P    3009
#define VTMODE3P    3010
#define PASSMODE    3011


/* TIFF IFD tags - not all the tags defined are actually used. */

#define NewSubfileType_tag             254
#define ImageWidth_tag                 256
#define ImageLength_tag                257
#define BitsPerSample_tag              258
#define Compression_tag                259
#define PhotometricInterpretation_tag  262
#define FillOrder_tag                  266
#define DocumentName_tag               269
#define StripOffsets_tag               273
#define Orientation_tag                274
#define SamplesPerPixel_tag            277
#define RowsPerStrip_tag               278
#define StripByteCounts_tag            279
#define XResolution_tag                282
#define YResolution_tag                283
#define T4Options_tag                  292
#define T6Options_tag                  293
#define ResolutionUnit_tag             296
#define PageNumber_tag                 297
#define Software_tag                   305
#define DateTime_tag                   306
#define BadFaxLines_tag                326
#define CleanFaxData_tag               327
#define ConsecutiveBadFaxLines_tag     328

/* TIFF IFD entry #defines and structure */

#define IFD_BYTE 1
#define IFD_ASCII 2
#define IFD_SHORT 3
#define IFD_LONG 4
#define IFD_RATIONAL 5

#define VIEWAS600	1
#define VIEWAS400	2
#define VIEWAS300	3
#define VIEWAS200	4
#define VIEWAS100	5
#define VIEWAS50	6
#define VIEWAUTOWIDTH	7
#define VIEWAUTOHEIGHT	8

typedef int NODE [][2];

typedef struct TiffHeader
{
	short int nOrderId;
	short int nTiffId;
	long lnFirstIfdOffset;
}TIFFHEADER;

typedef struct TiffEntry
{
   unsigned short int tag ;
   unsigned short int type ;
   unsigned long int count ;
   unsigned long int value ;
}TIFFENTRY;

typedef struct TiffMark
{
	unsigned short wTag;
	unsigned short int wType;
	unsigned long int dwCount;
	unsigned long int dwValue;
}TIFFMARK;

typedef struct TiffImage
{
   unsigned short int wWidth;
   unsigned short int wLength;

   unsigned short wBitOrder;	// from BYTE to WORD
   unsigned long int dwOffset;
   unsigned long int dwByteCount;
   unsigned short wOptions;
   unsigned short int wPage;

   unsigned long int dwIfd;	// the offset from file begin to point next IFD

   LPBYTE	lpImage;
}TIFFIMAGE;

typedef struct TiffStruct		// length 214
{
	unsigned short int ifd_count ;
	TIFFENTRY NewSubfileType ;
	TIFFENTRY ImageWidth ;
	TIFFENTRY ImageLength ;
	TIFFENTRY BitsPerSample ;
	TIFFENTRY Compression ;
	TIFFENTRY PhotometricInterpretation ;
	TIFFENTRY FillOrder ;
	TIFFENTRY StripOffsets ;
	TIFFENTRY SamplesPerPixel ;
	TIFFENTRY RowsPerStrip ;
	TIFFENTRY StripByteCounts ;
	TIFFENTRY Xresolution ;
	TIFFENTRY Yresolution ;
	TIFFENTRY T4Options ;
	TIFFENTRY ResolutionUnit ;
	TIFFENTRY PageNumber ;
	unsigned long int nextifd ;
	unsigned long int Xres[2] ;
	unsigned long int Yres[2] ;
}TIFF;

typedef struct OutputImage
{
	WORD wWidth;
	WORD wLength;
	
	LPBYTE lpImage;
	long int lnCount;

	long lnBufLen;	// nyg added
}OUTPUTIMAGE;

typedef struct DevParms
{
   unsigned char res ;        /* resolution 1=normal 3.85/mm 2=fine 7.7/mm */
   unsigned char id ;         /* output device coded as above */
   unsigned char color ;      /* color of run */
   unsigned int total_lines ; /* total lines/slice (lines per call to output */
   unsigned int this_line ;   /* line we have reached in the slice */
   long int dest_width ;      /* destination device width in dots */
   long int dest_height ;     /* destination device height in dots */
   long int source_width ;    /* source file width */
   long int source_height ;   /* source file height */
   char *cur_line ;           /* pointer to line being composed (for MR) */
   //FILE *tiffile ;            /* output file for device U_TIFF */
   HANDLE hFile;
   TIFF* ifd ;          /* pointer to IFD used when writing tiffile */
}DEVPARMS;

void reverse (void *pointer,int lnCountdown);