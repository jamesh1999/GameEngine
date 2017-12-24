#include "TextureLoader.h"

using namespace GameEngine::Resources;

/*
 * Targa Images .tga
 */

//Read a .tga from the stream into the struct
std::unique_ptr<TextureLoader::TGA> TextureLoader::ReadTGA(std::ifstream& stream)
{
	std::unique_ptr<TGA> img = std::make_unique<TGA>();
	stream.read(reinterpret_cast<char*>(&img->header), sizeof(TGAHeader));

	img->id = std::make_unique<char[]>(img->header.idSize);
	stream.read(img->id.get(), img->header.idSize);

	if (img->header.colourMap == 1)
	{
		unsigned colourSize = img->header.entries * (img->header.entryBpp >> 3);
		img->colourMap = std::make_unique<uint8_t[]>(colourSize);
		stream.read(reinterpret_cast<char*>(img->colourMap.get()), colourSize);
	}

	unsigned imgSize = img->header.width * img->header.height * (img->header.bpp >> 3);
	img->image = std::make_unique<uint8_t[]>(imgSize);
	stream.read(reinterpret_cast<char*>(img->image.get()), imgSize);

	return img;
}

std::list<TextureLoader::PNGChunk> TextureLoader::ReadPNG(std::ifstream& stream)
{
	std::list<PNGChunk> chunks;

	//Verify signature bytes
	uint64_t signature;
	stream.read(reinterpret_cast<char*>(&signature), 8);
	if (signature != 0x89504e470d0a1a0a) return chunks;

	while(true)
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
Texture::Texture* TextureLoader::LoadTGA(std::ifstream& stream)
{
	std::unique_ptr<TGA> img = ReadTGA(stream);

	Texture::Texture* tex = new Texture::Texture;
	tex->data = new float[img->header.width * img->header.height * 4];

	bool useColourMap = img->header.colourMap == 1;
	int colourBpp = useColourMap ? img->header.entryBpp : img->header.bpp;

	for(int y = 0; y < img->header.height; ++y)
		for(int x = 0; x < img->header.width; ++x)
		{
			uint8_t* pixelPtr = img->image.get() + (img->header.width * y + x) * (img->header.bpp >> 3);
			if (useColourMap)
			{
				switch(img->header.bpp)
				{
				case 8:
					pixelPtr = img->colourMap.get() + *pixelPtr;
					break;
				case 16:
					pixelPtr = img->colourMap.get() + *reinterpret_cast<uint16_t*>(pixelPtr);
					break;
				}
			}

			switch(colourBpp)
			{
			case 16:
				{
					uint16_t colour = *reinterpret_cast<uint16_t*>(pixelPtr);
					tex->data[(y * img->header.width + x) * 4 + 0] = (colour >> 15 & 31) / 31.0f;
					tex->data[(y * img->header.width + x) * 4 + 1] = (colour >> 10 & 31) / 31.0f;
					tex->data[(y * img->header.width + x) * 4 + 2] = (colour >> 5 & 31) / 31.0f;
					tex->data[(y * img->header.width + x) * 4 + 3] = (colour >> 15 & 1) / 1.0f;
				}
				break;
			case 24:
				tex->data[(y * img->header.width + x) * 4 + 0] = *(pixelPtr + 2) / 255.0f;
				tex->data[(y * img->header.width + x) * 4 + 1] = *(pixelPtr + 1) / 255.0f;
				tex->data[(y * img->header.width + x) * 4 + 2] = *(pixelPtr + 0) / 255.0f;
				tex->data[(y * img->header.width + x) * 4 + 3] = 1.0f;
				break;
			case 32:
				tex->data[(y * img->header.width + x) * 4 + 0] = *(pixelPtr + 2) / 255.0f;
				tex->data[(y * img->header.width + x) * 4 + 1] = *(pixelPtr + 1) / 255.0f;
				tex->data[(y * img->header.width + x) * 4 + 2] = *(pixelPtr + 0) / 255.0f;
				tex->data[(y * img->header.width + x) * 4 + 3] = *(pixelPtr + 3) / 255.0f;
				break;
			}
		}

	return tex;
}

Texture::Texture* TextureLoader::LoadPNG(std::ifstream& stream)
{
	//std::list<PNGChunk> img = ReadPNG(stream);

	Texture::Texture* tex = new Texture::Texture;
	return tex;
}

//Identify the type of texture (from extension) and load to memory
Texture::Texture* TextureLoader::LoadTexture(const std::string& filename)
{
	int idx = filename.find_last_of('.');
	std::string extension = filename.substr(idx + 1, filename.size() - idx - 1);

	std::ifstream stream(filename, std::ios::binary);

	if (extension == "tga")
		return LoadTGA(stream);
	
	return nullptr;
}
