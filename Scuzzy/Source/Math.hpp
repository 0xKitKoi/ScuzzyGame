#pragma once

#include <iostream>

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