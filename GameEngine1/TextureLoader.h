#ifndef __TEXTURE_LOADER_INCLUDED__
#define __TEXTURE_LOADER_INCLUDED__

#include <string>
#include <memory>
#include <fstream>
#include "Texture.h"
#include <vector>
#include <list>

class TextureLoader
{
private:

	//TGA

	struct TGAHeader
	{
		uint8_t idSize;
		uint8_t colourMap;
		uint8_t image;

		//Colour map
		uint16_t colourIdx;
		uint16_t entries;
		uint8_t entryBpp;

		//Image
		uint16_t xPos;
		uint16_t yPos;
		uint16_t width;
		uint16_t height;
		uint8_t bpp;
		uint8_t alpha;
	};

	struct TGA
	{
		TGAHeader header;
		std::unique_ptr<char[]> id;
		std::unique_ptr<uint8_t[]> colourMap;
		std::unique_ptr<uint8_t[]> image;
	};

	static std::unique_ptr<TGA> ReadTGA(std::ifstream&);

	//PNG
	struct PNGChunk
	{
		uint32_t length;
		char type[4];
		std::unique_ptr<uint8_t[]> data;
		uint32_t crc;
	};

	struct PNGChunkIHDR
	{
		uint32_t width;
		uint32_t height;
		uint8_t depth;
		uint8_t colour;
		uint8_t compression;
		uint8_t filter;
		uint8_t interlace;
	};

	struct PNGChunkPLTE
	{
		
	};

	struct PNGChunkIDAT
	{
		
	};

	struct PNGChunkIEND
	{
		
	};

	const static uint64_t pngSignature = 0x89504e470d0a1a0a;

	static std::list<PNGChunk> ReadPNG(std::ifstream&);

public:
	static Texture::Texture* LoadTGA(std::ifstream&);

	static Texture::Texture* LoadPNG(std::ifstream&);

	static Texture::Texture* LoadTexture(const std::string&);
};

#endif