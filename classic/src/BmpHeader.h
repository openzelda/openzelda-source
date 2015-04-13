

#ifndef BMP_FILE
#define BMP_FILE

typedef struct _BMPFileHeader
{
	uint16_t   FileType;     /* File type, always 4D42h ("BM") */
	uint32_t  FileSize;     /* Size of the file in bytes */
	uint16_t   Reserved1;    /* Always 0 */
	uint16_t   Reserved2;    /* Always 0 */
	uint32_t  BitmapOffset; /* Starting position of image data in bytes */
} BMPFILEHEADER;

typedef struct _BitmapHeader
{
		uint32_t Size;            /* Size of this header in bytes */
		int32_t  Width;           /* Image width in pixels */
		int32_t  Height;          /* Image height in pixels */
		uint16_t  Planes;          /* Number of color planes */
		uint16_t  BitsPerPixel;    /* Number of bits per pixel */
		uint32_t Compression;     /* Compression methods used */
		uint32_t SizeOfBitmap;    /* Size of bitmap in bytes */
		int32_t  HorzResolution;  /* Horizontal resolution in pixels per meter */
		int32_t  VertResolution;  /* Vertical resolution in pixels per meter */
		uint32_t ColorsUsed;      /* Number of colors in the image */
		uint32_t ColorsImportant; /* Minimum number of important colors */
} BITMAPHEADER;

typedef struct _BitmapPalette
{
		uint8_t Blue;      /* Blue component */
		uint8_t Green;     /* Green component */
		uint8_t Red;       /* Red component */
		uint8_t Reserved;  /* Padding (always 0) */
} BITMAPPALETTE;



#endif

