#include "Texture.h"
#include <SDL_image.h>

Texture::Texture() : mTexture(nullptr), mWidth(0), mHeight(0)
{

}

Texture::~Texture()
{
	Free();
}

bool Texture::LoadFromFile(const std::string& path, SDL_Renderer* renderer)
{
	SDL_Texture* texture = nullptr;

	SDL_Surface* tempSurface = IMG_Load(path.c_str());
	if (tempSurface == nullptr)
	{
		printf("Failed to load image at %s. Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
		if (texture == nullptr)
		{
			printf("Failed to create texture from image %s. Error: %s\n", path.c_str(), SDL_GetError());
		}
		else
		{
			mWidth = tempSurface->w;
			mHeight = tempSurface->h;
		}

		SDL_FreeSurface(tempSurface);
	}

	mTexture = texture;
	return mTexture != nullptr;
}

bool Texture::LoadFromFile(const std::string& path, SDL_Renderer* renderer, Uint8 r, Uint8 g, Uint8 b)
{
	SDL_Texture* texture = nullptr;

	SDL_Surface* tempSurface = IMG_Load(path.c_str());
	if (tempSurface == nullptr)
	{
		printf("Failed to load image at %s. Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		SDL_SetColorKey(tempSurface, SDL_TRUE, SDL_MapRGB(tempSurface->format, r, g, b));
		texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
		if (texture == nullptr)
		{
			printf("Failed to create texture from image %s. Error: %s\n", path.c_str(), SDL_GetError());
		}
		else
		{
			mWidth = tempSurface->w;
			mHeight = tempSurface->h;
		}

		SDL_FreeSurface(tempSurface);
	}

	mTexture = texture;
	return mTexture != nullptr;
}

void Texture::Free()
{
	if (mTexture != nullptr)
	{
		SDL_DestroyTexture(mTexture);
		mTexture = nullptr;
		mWidth = 0;
		mHeight = 0;
	}
}

void Texture::Render(SDL_Renderer* renderer, int x, int y, SDL_Rect* clip)
{
	SDL_Rect rect = { x,y,mWidth,mHeight };

	if (clip != nullptr)
	{
		rect.w = clip->w;
		rect.h = clip->h;
	}

	SDL_RenderCopy(renderer, mTexture, clip, &rect);
}

void Texture::Render(SDL_Renderer* renderer, int x, int y, int w, int h, SDL_Rect* clip)
{
	SDL_Rect rect = { x,y,w,h };
	SDL_RenderCopy(renderer, mTexture, clip, &rect);
}

void Texture::SetColourMod(Uint8 r, Uint8 g, Uint8 b)
{
	SDL_SetTextureColorMod(mTexture, r, g, b);
}

void Texture::SetBlendMode(SDL_BlendMode blendMode)
{
	SDL_SetTextureBlendMode(mTexture, blendMode);
}

void Texture::SetAlphaMod(Uint8 a)
{
	SDL_SetTextureAlphaMod(mTexture, a);
}