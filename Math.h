#pragma once
#include <random>
#include <cmath>
#include "algorithm"

namespace Math {
    constexpr float Pi = 3.1415926535f;
	const float TwoPi = Pi * 2.0f;
	const float PiOver2 = Pi / 2.0f;

    inline float ToRadians(float degrees) {
        return degrees * Pi / 180.0f;
    }

    inline float Cos(float angle) {
        return std::cos(angle);
    }

    inline float Sin(float angle) {
        return std::sin(angle);
    }

    inline float ToDegrees(float radians) {
        return radians * 180.0f / Pi;
    }

	inline float DegToRad(float degrees)
	{
		return degrees * Pi / 180.0f;
	}

	inline float Sign(float val)
	{
		if (val > 0.0f) return 1.0f;
		if (val < 0.0f) return -1.0f;
		return 0.0f;
	}

	inline float Atan2(float y, float x)
	{
		return atan2f(y, x);
	}

    inline bool NearZero(float val, float epsilon = 0.001f)
    {
        if (fabs(val) <= epsilon)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

	static std::random_device rd;
	static std::mt19937 generator(rd());
	inline float Random(const float amount, const float RandomLength) {
		const float length = std::fabs(RandomLength);
		const float min_val = amount - length;
		const float max_val = amount + length;
		std::uniform_real_distribution<float> distribution(min_val, max_val);
		return distribution(generator);
	}

	inline float Sqrt(float value)
	{
		return sqrtf(value);
	}

	inline float Wrap(float value, float min, float max) {
		if (value < min) return max - 2.0f;
		if (value > max) return min + 2.0f;
		return value;
	}

	inline float Lerp1(float a, float b, float f)
	{
		return a + f * (b - a);
	}

	inline float Abs(float val) {
		return std::fabs(val);
	}

	inline float Min(float a, float b)
	{
		return std::min(a, b);
	}

	inline float Max(float a, float b)
	{
		return std::max(a, b);
	}

	inline float WrapPositive(float value, float max) {
		return fmodf(fmodf(value, max) + max, max);
	}

	inline float WrapAngle(float angle) {
		float f = fmodf(angle + Math::Pi, Math::TwoPi);
		return f - Math::Pi;
	}

	inline float Pow(float base, float exp) {
		return std::pow(base, exp);
	}

	inline float Approach(float current, float target, float delta)
	{
		if (current < target) {
			current += delta;
			if (current > target)
				current = target;
		}
		else {
			current -= delta;
			if (current < target)
				current = target;
		}
		return current;
	}

		inline float Clamp(float value, float min, float max)
	{
		if (value < min) return min;
		if (value > max) return max;
		return value;
	}
}

class Vector2
{
public:
	float x;
	float y;

	Vector2()
		:x(0.0f)
		, y(0.0f)
	{
	}

	explicit Vector2(float inX, float inY)
		:x(inX)
		, y(inY)
	{
	}

	void Set(float inX, float inY)
	{
		x = inX;
		y = inY;
	}

	friend Vector2 operator+(const Vector2& a, const Vector2& b)
	{
		return Vector2(a.x + b.x, a.y + b.y);
	}

	friend Vector2 operator-(const Vector2& a, const Vector2& b)
	{
		return Vector2(a.x - b.x, a.y - b.y);
	}

	friend Vector2 operator*(const Vector2& a, const Vector2& b)
	{
		return Vector2(a.x * b.x, a.y * b.y);
	}

	friend Vector2 operator*(const Vector2& vec, float scalar)
	{
		return Vector2(vec.x * scalar, vec.y * scalar);
	}

	friend Vector2 operator*(float scalar, const Vector2& vec)
	{
		return Vector2(vec.x * scalar, vec.y * scalar);
	}

	Vector2& operator*=(float scalar)
	{
		x *= scalar;
		y *= scalar;
		return *this;
	}

	Vector2& operator+=(const Vector2& right)
	{
		x += right.x;
		y += right.y;
		return *this;
	}

	Vector2& operator-=(const Vector2& right)
	{
		x -= right.x;
		y -= right.y;
		return *this;
	}

	float LengthSq() const
	{
		return (x * x + y * y);
	}

	float Length() const
	{
		return (Math::Sqrt(LengthSq()));
	}

	void Normalize()
	{
		float length = Length();
		x /= length;
		y /= length;
	}

	static Vector2 Normalize(const Vector2& vec)
	{
		Vector2 temp = vec;
		temp.Normalize();
		return temp;
	}


	static float Dot(const Vector2& a, const Vector2& b)
	{
		return (a.x * b.x + a.y * b.y);
	}

	static Vector2 Lerp(const Vector2& a, const Vector2& b, float f)
	{
		return Vector2(a + f * (b - a));
	}

	static Vector2 Reflect(const Vector2& v, const Vector2& n)
	{
		return v - 2.0f * Vector2::Dot(v, n) * n;
	}

	static Vector2 Transform(const Vector2& vec, const class Matrix3& mat, float w = 1.0f);

	static const Vector2 Zero;
	static const Vector2 UnitX;
	static const Vector2 UnitY;
	static const Vector2 NegUnitX;
	static const Vector2 NegUnitY;
};

class Vector3
{
public:
	float x;
	float y;
	float z;

	Vector3()
		: x(0.0f)
		, y(0.0f)
		, z(0.0f)
	{
	}

	explicit Vector3(float inX, float inY, float inZ)
		: x(inX)
		, y(inY)
		, z(inZ)
	{
	}
};

inline Vector2 RotateVector(const Vector2& vec, float radians)
{
	float cosA = cosf(radians);
	float sinA = sinf(radians);
	return Vector2(
		vec.x * cosA - vec.y * sinA,
		vec.x * sinA + vec.y * cosA
	);
}



