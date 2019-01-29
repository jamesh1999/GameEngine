#include "TextureLoader.h"
#include <algorithm>
#include <cctype>

using namespace GameEngine::Resources;

/*
 * Targa Images .tga
 */

// Read a .tga header from the stream into TGAHeader struct
TextureLoader::TGAHeader TextureLoader::ReadHeaderTGA(std::ifstream& stream)
{
	TGAHeader header = {};

	stream.read(reinterpret_cast<char*>(&header.idSize), 1);
	stream.read(reinterpret_cast<char*>(&header.colourMap), 1);
	stream.read(reinterpret_cast<char*>(&header.image), 1);

	stream.read(reinterpret_cast<char*>(&header.colourIdx), 2);
	stream.read(reinterpret_cast<char*>(&header.entries), 2);
	stream.read(reinterpret_cast<char*>(&header.entryBpp), 1);

	stream.read(reinterpret_cast<char*>(&header.xPos), 2);
	stream.read(reinterpret_cast<char*>(&header.yPos), 2);
	stream.read(reinterpret_cast<char*>(&header.width), 2);
	stream.read(reinterpret_cast<char*>(&header.height), 2);
	stream.read(reinterpret_cast<char*>(&header.bpp), 1);
	stream.read(reinterpret_cast<char*>(&header.alpha), 1);

	return header;
}

// Read in a TGA colourmap assuming one present
std::unique_ptr<uint8_t[]> TextureLoader::ReadColourMapTGA(const TGAHeader& header, std::ifstream& stream)
{
	unsigned colourSize = header.entries * (header.entryBpp >> 3);
	auto colourMap = std::make_unique<uint8_t[]>(colourSize);
	stream.read(reinterpret_cast<char*>(colourMap.get()), colourSize);

	return colourMap;
}

// Read in / Decompress an RLE encoded TGA image
std::unique_ptr<uint8_t[]> TextureLoader::ReadRLEImageTGA(const TGAHeader& header, std::ifstream& stream)
{
	unsigned imgSize = header.width * header.height;
	unsigned bytesPerPixel = (header.bpp + 1) >> 3; // Allow for 15bpp
	auto image = std::make_unique<uint8_t[]>(imgSize * bytesPerPixel);

	unsigned curPixel = 0;
	while (curPixel < imgSize) // Loop over packets
	{
		uint8_t packet;
		stream.read(reinterpret_cast<char*>(&packet), 1);

		// RLE packet
		if (packet & 0x80)
		{
			uint8_t buffer[4]; // Keep buffer on stack
			stream.read(reinterpret_cast<char*>(buffer), bytesPerPixel);

			for (int i = 0; i <= (packet & 0x7f); ++i)
				memcpy(image.get() + (curPixel + i) * bytesPerPixel, buffer, bytesPerPixel);
		}
			// Normal packet
		else
			stream.read(reinterpret_cast<char*>(image.get() + curPixel * bytesPerPixel), (1 + packet) * bytesPerPixel);

		curPixel += 1 + packet & 0x7f;
	}

	return image;
}

// Read in an uncompressed TGA image
std::unique_ptr<uint8_t[]> TextureLoader::ReadImageTGA(const TGAHeader& header, std::ifstream& stream)
{
	unsigned bytesPerPixel = (header.bpp + 1) >> 3; // Allow for 15bpp
	unsigned imgSize = header.width * header.height;
	auto image = std::make_unique<uint8_t[]>(imgSize * bytesPerPixel);
	stream.read(reinterpret_cast<char*>(image.get()), imgSize * bytesPerPixel);

	return image;
}

// Read an entire .tga from the stream into the TGA struct
std::unique_ptr<TextureLoader::TGA> TextureLoader::ReadTGA(std::ifstream& stream)
{
	std::unique_ptr<TGA> img = std::make_unique<TGA>();

	img->header = ReadHeaderTGA(stream);

	img->id = std::make_unique<char[]>(img->header.idSize);
	stream.read(img->id.get(), img->header.idSize);

	if (img->header.colourMap == 1)
		img->colourMap = ReadColourMapTGA(img->header, stream);

	// Colour map is used not redundant
	if (img->header.image != 1 && img->header.image != 9)
		img->header.colourMap = 0;

	if (img->header.image & 0x08)
	{
		img->image = ReadRLEImageTGA(img->header, stream);
		img->header.image &= 0xf7;
	}
	else
		img->image = ReadImageTGA(img->header, stream);

	return img;
}

// Copies the contents of a single TGA pixel into the Texture
void TextureLoader::GetPixelTGA(float* out, int x, int y, TGA* img)
{
	uint8_t* pixelPtr;
	// Vertical flip if necessary
	unsigned bytesPerPixel = (img->header.bpp + 1) >> 3; // Allow for 15bpp
	if (img->header.alpha & 0x20)
		pixelPtr = img->image.get() + (img->header.width * y + x) * bytesPerPixel;
	else
		pixelPtr = img->image.get() + (img->header.width * (img->header.height - y - 1) + x) * bytesPerPixel;

	// Grey-scale images
	if (img->header.image == 3)
	{
		*(out + 0) = *pixelPtr / 255.0f;
		*(out + 1) = *pixelPtr / 255.0f;
		*(out + 2) = *pixelPtr / 255.0f;
		*(out + 3) = *pixelPtr / 255.0f;
		return;
	}

	// Default to opaque
	*(out + 3) = 1.0f;

	// Repoint to colour map if necessary
	if (img->header.colourMap == 1)
	{
		switch (img->header.bpp)
		{
		case 8:
			pixelPtr = img->colourMap.get() + *pixelPtr;
			break;
		case 15:
			// Read/clear alpha flag then carry on as in 16 bit
			if (*(pixelPtr + 1) & 0x80) *(pixelPtr + 1) &= 0x7f;
			else *(out + 3) = 0.0f;
		case 16:
			{
				// Little endian read
				uint16_t mapIdx = *pixelPtr + (static_cast<uint16_t>(*(pixelPtr + 1)) << 8);
				pixelPtr = img->colourMap.get() + mapIdx;
			}
			break;
		}
	}

	// Convert colour representation to normalized floats
	int bpp = img->header.colourMap == 1
		          ? img->header.entryBpp
		          : img->header.bpp;
	switch (bpp)
	{
	case 16:
		*(out + 0) = ((*(pixelPtr + 1) & 0x7c) >> 2) / 31.0f;
		*(out + 1) = (((*(pixelPtr + 1) & 0x03) << 3) | ((*(pixelPtr + 0) & 0xe0) >> 5)) / 31.0f;
		*(out + 2) = (*(pixelPtr + 0) & 0x1f) / 31.0f;
		*(out + 3) = (*(pixelPtr + 1) & 0x80) / 128.0f;
		break;
	case 24:
		*(out + 0) = *(pixelPtr + 2) / 255.0f;
		*(out + 1) = *(pixelPtr + 1) / 255.0f;
		*(out + 2) = *(pixelPtr + 0) / 255.0f;
		break;
	case 32:
		*(out + 0) = *(pixelPtr + 2) / 255.0f;
		*(out + 1) = *(pixelPtr + 1) / 255.0f;
		*(out + 2) = *(pixelPtr + 0) / 255.0f;
		*(out + 3) = *(pixelPtr + 3) / 255.0f;
		break;
	}
}

std::list<TextureLoader::PNGChunk> TextureLoader::ReadPNG(std::ifstream& stream)
{
	std::list<PNGChunk> chunks;

	//Verify signature bytes
	uint64_t signature;
	stream.read(reinterpret_cast<char*>(&signature), 8);
	if (signature != 0x89504e470d0a1a0a) return chunks;

	while (true)
	{
		PNGChunk chunk;

		stream.read(reinterpret_cast<char*>(&chunk.length), 4);
		stream.read(chunk.type, 4);
		chunk.data = std::make_unique<uint8_t[]>(chunk.length);
		stream.read(reinterpret_cast<char*>(chunk.data.get()), chunk.length);
		stream.read(reinterpret_cast<char*>(&chunk.crc), 4);

		//chunks.push_back(chunk);

		if (strncmp(chunk.type, "IEND", 4))
			return chunks;
	}
}

//Load a .tga from stream to memory
Texture* TextureLoader::LoadTGA(std::ifstream& stream)
{
	std::unique_ptr<TGA> img = ReadTGA(stream);

	// Set up Texture resource
	Texture* tex = new Texture;
	tex->m_h = img->header.height;
	tex->m_w = img->header.width;
	tex->m_data = new float[img->header.width * img->header.height * 4];

	for (int y = 0; y < img->header.height; ++y)
		for (int x = 0; x < img->header.width; ++x)
		{
			GetPixelTGA(tex->m_data + 4 * (y * img->header.width + x), x, y, img.get());

			// Texture opacity flag
			if (tex->m_data[(y * img->header.width + x) * 4 + 3] < 1.0f) tex->m_opaque = false;
		}

	return tex;
}

Texture* TextureLoader::LoadPNG(std::ifstream& stream)
{
	//std::list<PNGChunk> img = ReadPNG(stream);

	Texture* tex = new Texture;
	return tex;
}

//Identify the type of texture (from extension) and load to memory
Texture* TextureLoader::Load(const std::string& filename)
{
	if (filename == "") return new Texture;

	int idx = filename.find_last_of('.');
	std::string extension = filename.substr(idx + 1, filename.size() - idx - 1);
	transform(extension.begin(), extension.end(), extension.begin(), tolower);

	std::ifstream stream(filename, std::ios::binary);

	if (extension == "tga")
		return LoadTGA(stream);

	return nullptr;
}
