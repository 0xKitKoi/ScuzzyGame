#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

extern SDL_Renderer* gRenderer;
extern TTF_Font* gFont;

//Texture wrapper class
class LTexture
{
public:
	//Initializes variables
	LTexture();
	//LTexture(std::string texturepath, int w, int h);

	//Deallocates memory
	~LTexture();

	//Loads image at specified path
	bool loadFromFile(std::string path);

	//Creates image from font string
	bool loadFromRenderedText(std::string textureText, SDL_Color textColor);

	//Deallocates texture
	void free();

	//Set color modulation
	void setColor(Uint8 red, Uint8 green, Uint8 blue);

	//Set blending
	void setBlendMode(SDL_BlendMode blending);

	//Set alpha modulation
	void setAlpha(Uint8 alpha);

	//Renders texture at given point
	void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE, float scale = 1.0f);

	SDL_Texture* getTexture() const
	{
		return mTexture;
	}
	//Gets image dimensions
	int getWidth();
	int getHeight();

private:
	//The actual hardware texture
	SDL_Texture* mTexture;

	//Image dimensions
	int mWidth;
	int mHeight;
};