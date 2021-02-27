#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef _WIN32
	#include <io.h>
#else
	#include <unistd.h>
#endif

#include "image.h"

#ifndef SEEK_SET
	#define SEEK_SET 0
#endif

#ifdef _WIN32
	#define OPEN_FLAGS O_BINARY | O_RDONLY
#else
	#define OPEN_FLAGS O_RDONLY
#endif

#define MIN_IMAGE	8
#define	MAX_IMAGE	2048

////////////////////////////////////////////////////////////////////////////////////////
//
//	cg_image Implementation
//
////////////////////////////////////////////////////////////////////////////////////////

cg_image::cg_image ()
{
	Palette = data = NULL;
	glTex = sizeX = sizeY = BPP = 0;

	minFilter = GL_NEAREST;
	magFilter = GL_NEAREST;
	wrapMode = GL_REPEAT;
	envMode = GL_REPLACE;

	hasMipMap = false;
}

cg_image::~cg_image ()
{
	free();
}

bool cg_image::load ( const char * fileName, bool generateMipmaps )
{
	// Datei in Zwischenspeicher lesen
	if ( !loadBMP ( fileName, false ) )
		if ( !loadTGA ( fileName, false ) )
			if ( !loadRGB ( fileName, false ) )
			{
				free();
				printf( "Unsupported image format!\n" );
				return false;
			}

	// OpenGL-Textur erzeugen
	GLuint tex;
	// 1 Texturelement wird in GL erzeugt
	glGenTextures( 1, &tex );

	// ... und gebunden
	glBindTexture( GL_TEXTURE_2D, tex );

	// MipMap JA/NEIN
	if (generateMipmaps) {
		gluBuild2DMipmaps(GL_TEXTURE_2D,
			BPP >> 3,						// Textur enthaelt 3 Komponenten pro Texel
			sizeX, sizeY,					// Groesse der geladenen Textur
			(BPP == 24) ? GL_RGB : GL_RGBA,	// die Komponenten beschreiben RGB-Werte
			GL_UNSIGNED_BYTE,				// deren Typ ist UNSIGNED BYTE (0..255)
			data);							// das Feld von Texeln */
		hasMipMap = true;
	}
	else {
		glTexImage2D(GL_TEXTURE_2D, 0,
			BPP >> 3,
			sizeX, sizeY,
			0,
			(BPP == 24) ? GL_RGB : GL_RGBA,
			GL_UNSIGNED_BYTE,
			data);
		hasMipMap = false;
	}

	// Zwischenspeicher freigeben
	free();
	glTex = tex;

	return true;
}

void cg_image::setMagFilter(GLint filter) 
{
	if (GL_NEAREST == filter || GL_LINEAR == filter)
		this->magFilter = filter;
}
void cg_image::setMinFilter(GLint filter)
{
	if (GL_NEAREST == filter || GL_LINEAR == filter ||
		(this->hasMipMap && GL_LINEAR_MIPMAP_LINEAR == filter) ||
		(this->hasMipMap && GL_LINEAR_MIPMAP_NEAREST == filter) ||
		(this->hasMipMap && GL_NEAREST_MIPMAP_LINEAR == filter) ||
		(this->hasMipMap && GL_NEAREST_MIPMAP_NEAREST == filter)
		)
	this->minFilter = filter;
}
void cg_image::setWrapMode(GLint mode)
{
	if (GL_CLAMP == mode || GL_REPEAT == mode)
		this->wrapMode = mode;
}
void cg_image::setEnvMode(GLint mode)
{
	if (GL_REPLACE == mode || GL_DECAL == mode || GL_MODULATE == mode || GL_BLEND == mode)
		this->envMode = mode;
}

GLint cg_image::getMagFilter(void) {
	return this->magFilter;
}

GLint cg_image::getMinFilter(void) {
	return this->minFilter;
}

GLint cg_image::getWrapMode(void) {
	return this->wrapMode;
}

GLint cg_image::getEnvMode(void) {
	return this->envMode;
}

void cg_image::bind ()
{
	// bind the texture
	glBindTexture( GL_TEXTURE_2D, glTex );

	// set wrap mode
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->wrapMode);

	// set mag / min filter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->magFilter);

	// set env mode
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, this->envMode);

	// bind the texture
	// glBindTexture( GL_TEXTURE_2D, glTex );
}

void cg_image::free ()
{
	if ( Palette )
		delete [] Palette;
	if ( data )
		delete [] data;
	if ( glTex != 0 )
		glDeleteTextures( 1, &glTex );

	Palette = data = NULL;
	glTex = sizeX = sizeY = BPP = 0;

	minFilter = GL_NEAREST;
	magFilter = GL_NEAREST;
	wrapMode = GL_REPEAT;
	envMode = GL_REPLACE;

	hasMipMap = false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//	private Hilfsfunktionen
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int cg_image::getFileSize ( const char * fileName )
{
	struct stat file_stats;
	if ( stat ( fileName, &file_stats ) )
		return -1;
	return file_stats.st_size;
}

int cg_image::getFileSize ( int fileDes )
{
	struct stat file_stats;
	if ( fstat ( fileDes, &file_stats ) )
		return -1;
	return file_stats.st_size;
}

void cg_image::convert8bit2RGB ()
{
	if ( BPP != 8 )
		return;

	int SSize = sizeX * sizeY;
	int DSize = SSize * 3;
	int Pos = 0;
	unsigned char *NewData = new unsigned char [DSize];

	for ( int i = 0; i < SSize; i++ )
	{
		NewData[Pos++] = Palette[3 * data[i]];
		NewData[Pos++] = Palette[3 * data[i] + 1];
		NewData[Pos++] = Palette[3 * data[i] + 2];
	}
	BPP = 24;
	delete [] data;
	data = NewData;
	delete [] Palette;
	Palette = NULL;
}

void cg_image::convert8bit2RGBA ( int alpha )
{
	if ( BPP != 8 )
		return;

	int SSize = sizeX * sizeY;
	int DSize = SSize << 2;
	int Pos = 0;
	unsigned char *NewData = new unsigned char [DSize];

	for ( int i = 0; i < SSize; i++ )
	{
		NewData[Pos++] = Palette[3 * data[i]];
		NewData[Pos++] = Palette[3 * data[i] + 1];
		NewData[Pos++] = Palette[3 * data[i] + 2];
		NewData[Pos++] = alpha;
	}
	BPP = 32;
	delete [] data;
	data = NewData;
	delete [] Palette;
	Palette = NULL;
}

void cg_image::convertRGB2RGBA ( int alpha )
{
	if ( BPP != 24 )
		return;

	int SSize = sizeX * sizeY;
	int DSize = SSize << 2;
	int Pos = 0;
	int j = 0;
	unsigned char *NewData = new unsigned char [DSize];

	for ( int i = 0; i < SSize; i++ )
	{
		NewData[Pos++] = data[j++];
		NewData[Pos++] = data[j++];
		NewData[Pos++] = data[j++];
		NewData[Pos++] = alpha;
	}
	BPP = 32;
	delete [] data;
	data = NewData;
}

void cg_image::setAlpha ( int alpha )
{
	if ( BPP != 32 )
		return;

	int Size = sizeX * sizeY << 2;
	for ( int i = 3; i < Size; i += 4 )
		data[i] = alpha;
}

bool cg_image::horizontalFlip ()
{
	int i;
	int Bytes = BPP >> 3;
	int Line = 0;
	int LineLeft = 0;
	int ScanLine = sizeX * Bytes;
	int LineRight = ScanLine - Bytes;
	int HalfLine = ScanLine / Bytes >> 1;
	BYTE R, G, B;
	BYTE Pixel[4];

	if ( BPP != 24 && BPP != 32 && BPP != 8 )
	{
		printf ( "Unsupported operation for this color depth!\n" );
		return false;
	}
	for ( int j = 0; j < sizeY; j++ )
	{
		for ( i = 0; i < HalfLine; i++ )
		{
			switch ( BPP )
			{
			case 8:
				R = data[LineLeft];
				data[LineLeft] = data[LineRight];
				data[LineRight] = R;
				break;
			case 24:
				R = data[LineLeft];
				G = data[LineLeft + 1];
				B = data[LineLeft + 2];
				data[LineLeft]     = data[LineRight];
				data[LineLeft + 1] = data[LineRight + 1];
				data[LineLeft + 2] = data[LineRight + 2];
				data[LineRight]     = R;
				data[LineRight + 1] = G;
				data[LineRight + 2] = B;
				break;
			case 32:
				memmove ( Pixel, data + LineLeft, Bytes );
				memmove ( data + LineLeft, data + LineRight, Bytes );
				memmove ( data + LineRight, Pixel, Bytes );
				break;
			}
			LineLeft  += Bytes;
			LineRight -= Bytes;
		}
		Line += ScanLine;
		LineLeft  = Line;
		LineRight = Line + ScanLine - Bytes;
	}
	return true;
}

bool cg_image::verticalFlip ()
{
	int ScanLine = sizeX * ( BPP >> 3 );
	BYTE *Dummy = new BYTE [ScanLine];
	int ImageSize = ScanLine * sizeY;
	int LineTop = 0;
	int LineBottom = ImageSize - ScanLine;
	for ( int j = 0; j < sizeY; j++ )
	{
		memmove ( Dummy, data + LineBottom, ScanLine );
		memmove ( data + LineBottom, data + LineTop, ScanLine );
		memmove ( data + LineTop, Dummy, ScanLine );
	}
	delete []Dummy;
	return true;
}

bool cg_image::swapRB ()
{
	BYTE R;
	int Pos = 0;
	int Add = BPP >> 3;
	int i;

	if ( BPP != 24 && BPP != 32 )
		return false;
	for ( int j = 0; j < sizeY; j++ )
	{
		for ( i = 0; i < sizeX; i++ )
		{
			R = data[Pos];
			data[Pos] = data[Pos + 2];
			data[Pos + 2] = R;
			Pos += Add;
		}
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////
//	RGB Funktionen
////////////////////////////////////////////////////////////////////////////////
void cg_image::convertShort ( unsigned short *array, unsigned int length )
{
	unsigned short b1, b2;
	unsigned char *ptr;

	ptr = ( unsigned char * ) array;
	while ( length-- )
	{
		b1 = *ptr++;
		b2 = *ptr++;
		*array++ = ( b1 << 8 ) | ( b2 );
	}
}

void cg_image::convertUint ( GLuint *array, unsigned int length )
{
	unsigned int b1, b2, b3, b4;
	unsigned char *ptr;

	ptr = ( unsigned char * ) array;
	while ( length-- )
	{
		b1 = *ptr++;
		b2 = *ptr++;
		b3 = *ptr++;
		b4 = *ptr++;
		*array++ = ( b1 << 24 ) | ( b2 << 16 ) | ( b3 << 8 ) | ( b4 );
	}
}

rawImageRec * cg_image::rawImageOpen ( const char *fileName, bool verbose )
{
	union
	{
		int testWord;
		char testByte[4];
	} endianTest;

	rawImageRec *raw;
	int swapFlag;
	int x;

	endianTest.testWord = 1;
	swapFlag = ( endianTest.testByte[0] == 1 );
	raw = ( rawImageRec * ) malloc ( sizeof ( rawImageRec ) );
	memset ( raw, 0, sizeof ( rawImageRec ) );
	if ( raw == NULL )
	{
		fprintf ( stderr, "Out of memory!\n" );
		exit ( 1 );
	}
	if ( ( raw->file = fopen ( fileName, "rb" ) ) == NULL )
	{
		if ( verbose )
			printf ( "%s: Cannot open file for reading!\n", fileName );
		::free ( raw );
		return NULL;
	}
	if ( fread ( raw, 1, 12, raw->file ) != 12 )
	{
		if ( verbose )
			printf ( "%s: Could not read file correctly!\n", fileName );
		fclose ( raw -> file );
		::free ( raw );
		return NULL;
	}
	if ( swapFlag )
		convertShort ( &raw->imagic, 6 );

	if ( raw -> sizeX > MAX_IMAGE || raw -> sizeX < MIN_IMAGE || raw -> sizeY > MAX_IMAGE || raw -> sizeY < MIN_IMAGE )
	{
		fclose ( raw -> file );
		::free ( raw );
		return NULL;
	}
	raw->tmp  = ( unsigned char * ) malloc ( raw->sizeX * 256 );
	raw->tmpR = ( unsigned char * ) malloc ( raw->sizeX * 256 );
	raw->tmpG = ( unsigned char * ) malloc ( raw->sizeX * 256 );
	raw->tmpB = ( unsigned char * ) malloc ( raw->sizeX * 256 );
	if ( raw->tmp == NULL || raw->tmpR == NULL || raw->tmpG == NULL || raw->tmpB == NULL )
	{
		fprintf ( stderr, "Out of memory!\n" );
		exit ( 1 );
	}
	if ( ( raw->type & 0xFF00 ) == 0x0100 )
	{
		x = raw->sizeY * raw->sizeZ * ( int ) sizeof ( GLuint );
		raw->rowStart = ( GLuint * ) malloc ( x );
		raw->rowSize  = ( GLint * ) malloc ( x );
		if ( raw->rowStart == NULL || raw->rowSize == NULL )
		{
			fprintf ( stderr, "Out of memory!\n" );
			exit ( 1 );
		}
		raw->rleEnd = 512 + ( 2 * x );
		fseek ( raw->file, 512, SEEK_SET );
		if ( fread ( raw->rowStart, 1, x, raw->file ) != ( UINT ) x )
		{
			if ( verbose )
				printf ( "%s: Could not read file correctly!\n", fileName );
			rawImageClose ( raw );
			return NULL;
		}
		if ( fread ( raw->rowSize,  1, x, raw->file ) != ( UINT ) x )
		{
			if ( verbose )
				printf ( "%s: Could not read file correctly!\n", fileName );
			rawImageClose ( raw );
			return NULL;
		}
		if ( swapFlag )
		{
			convertUint ( raw->rowStart, ( unsigned int ) ( x / sizeof ( GLuint ) ) );
			convertUint ( ( GLuint * ) raw->rowSize, ( unsigned int ) ( x / sizeof ( GLint ) ) );
		}
	}
	return raw;
}

void cg_image::rawImageClose ( rawImageRec *raw )
{
	fclose ( raw->file );
	if ( raw -> tmp )
		::free ( raw->tmp );
	if ( raw -> tmpR )
		::free ( raw->tmpR );
	if ( raw -> tmpG )
		::free ( raw->tmpG );
	if ( raw -> tmpB )
		::free ( raw->tmpB );
	if ( raw )
		::free ( raw );
}

bool cg_image::rawImageGetRow ( rawImageRec *raw, unsigned char *buf, int y, int z )
{
	unsigned char *iPtr, *oPtr, pixel;
	int count;

	if ( ( raw->type & 0xFF00 ) == 0x0100 )
	{
		fseek ( raw->file, ( long ) raw->rowStart[y + z * raw->sizeY], SEEK_SET );
		if ( fread ( raw->tmp, 1, ( unsigned int ) raw->rowSize[y + z * raw->sizeY], raw->file ) !=
				( unsigned int ) raw->rowSize[y + z * raw->sizeY] )
			return false;

		iPtr = raw->tmp;
		oPtr = buf;
		for ( ;; )
		{
			pixel = *iPtr++;
			count = ( int ) ( pixel & 0x7F );
			if ( !count )
				break;
			if ( pixel & 0x80 )
			{
				while ( count-- )
					*oPtr++ = *iPtr++;
			}
			else
			{
				pixel = *iPtr++;
				while ( count-- )
					*oPtr++ = pixel;
			}
		}
	}
	else
	{
		fseek ( raw->file, 512 + ( y * raw->sizeX ) + ( z * raw->sizeX * raw->sizeY ), SEEK_SET );
		if ( fread ( buf, 1, raw->sizeX, raw->file ) != raw->sizeX )
			return false;
	}
	return true;
}

bool cg_image::rawImageGetData ( rawImageRec *raw, cg_image *final )
{
	bool Ret = true;
	unsigned char *ptr;
	int i, j;

	final -> data = new unsigned char [ ( raw -> sizeX + 1 ) * ( raw -> sizeY + 1 ) << 2];
	ptr = final->data;
	for ( i = 0; i < ( int ) ( raw->sizeY ); i++ )
	{
		if ( !rawImageGetRow ( raw, raw->tmpR, i, 0 ) )
		{
			Ret = false;
			break;
		}
		if ( !rawImageGetRow ( raw, raw->tmpG, i, 1 ) )
		{
			Ret = false;
			break;
		}
		if ( !rawImageGetRow ( raw, raw->tmpB, i, 2 ) )
		{
			Ret = false;
			break;
		}
		for ( j = 0; j < ( int ) ( raw->sizeX ); j++ )
		{
			*ptr++ = * ( raw->tmpR + j );
			*ptr++ = * ( raw->tmpG + j );
			*ptr++ = * ( raw->tmpB + j );
		}
	}
	return Ret;
}

bool cg_image::loadRGB ( const char *fileName, bool verbose )
{
	free();

	rawImageRec *raw;

	if ( ( raw = rawImageOpen ( fileName, verbose ) ) != NULL )
	{
		sizeX = raw->sizeX;
		sizeY = raw->sizeY;
		BPP   = 24;
		if ( !rawImageGetData ( raw, this ) )
		{
			if ( verbose )
				printf ( "%s: Could not read file!\n", fileName );
			return false;
		}
		rawImageClose ( raw );
	}
	return true;
}

/******************************************************************************/
//		BMP Funktionen
/******************************************************************************/
bool cg_image::loadBMP ( const char *fileName, bool verbose )
{
	free();

	unsigned char *bits;
	unsigned char *Pal = NULL;
	long bitsize;
	BITMAPFILEHEADER header;
	BITMAPINFO info;
	int fp;

	if ( ( fp = open ( fileName, OPEN_FLAGS ) ) == -1 )
	{
		if ( verbose )
			printf ( "Could not open file for reading\n" ) ;
	}
	else
	{
		read ( fp, &header, sizeof ( BITMAPFILEHEADER ) );
		if ( ( header.bfType != 0x424D ) && ( header.bfType != 0x4D42 ) )
		{
			close ( fp ) ;
			if ( verbose )
				printf ( "Not a bitmap file!\n" ) ;
		}
		else if ( read ( fp, &info, sizeof ( BITMAPINFOHEADER ) ) != sizeof ( BITMAPINFOHEADER ) )
		{
			close ( fp ) ;
			if ( verbose )
				printf ( "Could not read the bitmap header\n" ) ;
		}
		else
		{
			if ( info.bmiHeader.biBitCount != 8 && info.bmiHeader.biBitCount != 24 && info.bmiHeader.biBitCount != 32 )
			{
				if ( verbose )
					printf ( "Unsupported color depth!\n" );
				return false;
			}
			if ( ( bitsize = info.bmiHeader.biSizeImage ) == 0 )
				bitsize = info.bmiHeader.biWidth * ( info.bmiHeader.biBitCount >> 3 ) *
						  info.bmiHeader.biHeight;
			if ( info.bmiHeader.biBitCount == 8 )
			{
				RGBQUAD Quad[256];
				int NumColor = info.bmiHeader.biClrUsed;
				if ( read ( fp, Quad, NumColor * sizeof ( RGBQUAD ) ) != ( int ) ( NumColor * sizeof ( RGBQUAD ) ) )
				{
					close ( fp );
					if ( verbose )
						printf ( "Could not read bitmap correctly!\n" ) ;
					return false;
				}
				int j = 0;
				Pal = new unsigned char [768];
				memset ( Pal, 0, 768 );
				for ( int i = 0; i < NumColor; i++ )
				{
					Pal[j++] = Quad[i].rgbRed;
					Pal[j++] = Quad[i].rgbGreen;
					Pal[j++] = Quad[i].rgbBlue;
				}
			}
			bits = new unsigned char [bitsize];
			if ( read ( fp, bits, bitsize ) != bitsize )
			{
				if ( Pal )
					delete []Pal;
				delete []bits;
				close ( fp );
				if ( verbose )
					printf ( "Could not read bitmap correctly!\n" ) ;
			}
			else
			{
				close ( fp );
				sizeX = info.bmiHeader.biWidth;
				sizeY = info.bmiHeader.biHeight;
				data  = ( unsigned char * ) bits;
				BPP   = info.bmiHeader.biBitCount;
				Palette = Pal;
				if ( BPP == 8 )
					convert8bit2RGB ();
				else
					swapRB ();			// ???????????????
				return true;
			}
		}
	}
	return false;
}

/******************************************************************************/
//		TGA Funktionen
/******************************************************************************/
bool cg_image::loadTGA ( const char *File, bool verbose )
{
	free();

	bool Compress = false;
	BYTE *Pal = NULL;
	char Dummy[256];
	int Fd;
	int ImageSize;
	TGAHeader Head;

	if ( ( Fd = open ( File, OPEN_FLAGS ) ) == -1 )
	{
		if ( verbose )
			printf ( "%s: Cannot open file for reading!\n", File );
		return false;
	}
	if ( read ( Fd, &Head, sizeof ( TGAHeader ) ) != sizeof ( TGAHeader ) )
	{
		if ( verbose )
			printf ( "%s: Could not read file correctly!\n", File );
		close ( Fd );
		return false;
	}
	if ( Head.PixelDepth != 24 && Head.PixelDepth != 32 && Head.PixelDepth != 8 )
	{
		if ( verbose )
			printf ( "Unsupported color depth!\n" );
		close ( Fd );
		return false;
	}
	if ( Head.ImageType != 2 && Head.ImageType != 1 &&
			Head.ImageType != 9 && Head.ImageType != 10 )
	{
		if ( verbose )
			printf ( "Unsupported TGA type!\n" );
		close ( Fd );
		return false;
	}
	if ( Head.ImageType == 9 || Head.ImageType == 10 )
		Compress = true;
	// Read ID
	if ( Head.IdLength )
	{
		if ( read ( Fd, Dummy, Head.IdLength ) != Head.IdLength )
		{
			if ( verbose )
				printf ( "%s: Could not read file correctly!\n", File );
			close ( Fd );
			return false;
		}
	}
	if ( Head.CmapType == 1 )  	// Palette einlesen
	{
		int Len = 3 * Head.CmapLength;
		Pal = new BYTE [Len];
		if ( read ( Fd, Pal, Len ) != Len )
		{
			delete [] Pal;
			close ( Fd );
			if ( verbose )
				printf ( "%s: Could not read file correctly!\n", File );
			return false;
		}
	}

	sizeX   = Head.ImageWidth;
	sizeY   = Head.ImageHeight;
	BPP     = Head.PixelDepth;
	Palette = Pal;
	ImageSize = sizeX * ( BPP >> 3 ) * sizeY;

	data = new BYTE [ImageSize];
	if ( !Compress )
	{
		if ( read ( Fd, data, ImageSize ) != ImageSize )
		{
			if ( Pal )
				delete [] Pal;
			delete [] data;
			if ( verbose )
				printf ( "%s: Could not read file correctly!\n", File );
			return false;
		}
	}
	else
	{
		int ReadBytes = getFileSize ( Fd );
		unsigned char *Temp = new unsigned char [ReadBytes];
		int i = 0, j;
		int Pos = 0;
		int Rep;
		int R = 0, G = 0, B = 0, A = 0;
		ReadBytes = read ( Fd, Temp, ReadBytes );
		while ( 1 )
		{
			if ( Temp[i] & 0x80 )
			{
				Rep = ( Temp[i++] & 0x7F ) + 1;
				switch ( BPP )
				{
				case 8:
					R = Temp[i++];
					memset ( data + Pos, R, Rep );
					Pos += Rep;
					break;
				case 24:
					R = Temp[i++];
					G = Temp[i++];
					B = Temp[i++];
					break;
				case 32:
					R = Temp[i++];
					G = Temp[i++];
					B = Temp[i++];
					A = Temp[i++];
					break;
				}
				for ( j = 0; j < Rep; j++ )
				{
					if ( BPP == 8 )
						break;
					switch ( BPP )
					{
					case 24:
						data[Pos++] = R;
						data[Pos++] = G;
						data[Pos++] = B;
						break;
					case 32:
						data[Pos++] = R;
						data[Pos++] = G;
						data[Pos++] = B;
						data[Pos++] = A;
						break;
					}
				}
			}
			else
			{
				Rep = ( Temp[i++] + 1 ) * ( BPP >> 3 );
				memmove ( data + Pos, Temp + i, Rep );
				i += Rep;
				Pos += Rep;
			}
			if ( Pos >= ImageSize || i >= ReadBytes )
				break;
		}
		delete []Temp;
	}
	if ( Head.ImagDesc & 0x10 )
		horizontalFlip ();
	if ( !( Head.ImagDesc & 0x20 ) )
		verticalFlip ();
	if ( BPP == 8 )
		convert8bit2RGB ();
	swapRB ();			// ??????????????
	close ( Fd );
	return true;
}
