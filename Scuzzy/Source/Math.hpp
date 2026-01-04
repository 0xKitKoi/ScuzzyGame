#pragma once

#include <iostream>


struct Camera { // render space to world space fucking sucks
    float x, y;           // Camera position in world space
    int width, height;    // Viewport size ( the game is set to fullscreen only.)
    
    int mapWidth, mapHeight;
    
void centerOn(float targetX, float targetY) { // this centers the camera on a target position (the player)
    x = targetX - width / 2.0f;
    y = targetY - height / 2.0f;
    //printf("Before clamp: (%.1f, %.1f)\n", x, y); 
    clamp();
    //printf("After clamp: (%.1f, %.1f)\n", x, y);
}
    
    void clamp() {
        // For maps smaller than screen
        if (mapWidth < width) {
            x = -(width - mapWidth) / 2.0f; // Center it
        } else {
            // For maps larger than screen
            if (x < 0) x = 0;
            if (x > mapWidth - width) x = mapWidth - width;
        }
        
        if (mapHeight < height) {
            y = -(height - mapHeight) / 2.0f; // Center it
        } else {
            if (y < 0) y = 0;
            if (y > mapHeight - height) y = mapHeight - height;
        }
    }
    
    // Convert world position to screen position
    SDL_Rect worldToScreen(SDL_Rect worldRect) {
        return {
            (int)(worldRect.x - x),
            (int)(worldRect.y - y),
            worldRect.w,
            worldRect.h
        };
    }
};


struct Vector2f
{
	float x, y;
	Vector2f()
		:x(0.0f), y(0.0f)
	{}

	Vector2f(float p_x, float p_y)
		:x(p_x), y(p_y)
	{}

	void print()
	{
		std::cout << x << ", " << y << std::endl;
	}

	// Vector subtraction
    Vector2f operator-(const Vector2f& rhs) const {
        return Vector2f(x - rhs.x, y - rhs.y);
    }

    // Vector addition
    Vector2f operator+(const Vector2f& rhs) const {
        return Vector2f(x + rhs.x, y + rhs.y);
    }

    // Scalar multiplication
    Vector2f operator*(float scalar) const {
        return Vector2f(x * scalar, y * scalar);
    }

    // Compound operators
    Vector2f& operator+=(const Vector2f& rhs) {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    // Length (magnitude)
    float Length() const {
        return std::sqrt(x * x + y * y);
    }

    // Normalized vector
    Vector2f Normalized() const {
        float len = Length();
        if (len == 0.0f)
            return Vector2f(0.0f, 0.0f);
        return Vector2f(x / len, y / len);
    }

	
};

inline Vector2f lerp(const Vector2f& a, const Vector2f& b, float t) {
    return a * (1.0f - t) + b * t;
}


void draw_circle(SDL_Renderer* renderer, int x, int y, int radius, SDL_Color color);