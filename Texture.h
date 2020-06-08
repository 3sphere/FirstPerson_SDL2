#pragma once
#include <string>
#include <SDL.h>

class Texture
{
public:
	Texture();
	~Texture();

	bool LoadFromFile(const std::string& path, SDL_Renderer* renderer);
	bool LoadFromFile(const std::string& path, SDL_Renderer* renderer, Uint8 r, Uint8 g, Uint8 b); // for colour keying
	void Free();
	void Render(SDL_Renderer* renderer, int x, int y, SDL_Rect* clip = nullptr);
	void Render(SDL_Renderer* renderer, int x, int y, int w, int h, SDL_Rect* clip = nullptr);
	void SetColourMod(Uint8 r, Uint8 g, Uint8 b);
	void SetBlendMode(SDL_BlendMode blendMode);
	void SetAlphaMod(Uint8 a);
	int GetWidth() const { return mWidth; }
	int GetHeight() const { return mHeight; }

private:
	SDL_Texture* mTexture;
	int mWidth;
	int mHeight;
};