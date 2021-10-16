#include <sstream>

class Vector2
{
public:

	Vector2()
	{
		x = 0;
		y = 0;
	}

	Vector2(float f_x, float f_y)
	{
		x = f_x;
		y = f_y;
	}

	Vector2(const float* f_n)
	{
		x = f_n[0];
		y = f_n[1];
	}

	__inline bool is_valid() const // not used in this project but just in case I ever need it.
	{
		return std::isfinite(x) && std::isfinite(y);
	}

	float length() const
	{
		return sqrt(x * x + y * y);
	}

	float length_sqr(void) const
	{
		return (x * x + y * y);
	}

	float dist_to_sqr(const Vector2& Vector2_n) const
	{
		Vector2 delta;

		delta.x = x - Vector2_n.x;
		delta.y = y - Vector2_n.y;

		return delta.length_sqr();
	}

	void normalize()
	{
		float f_length = this->length();
		if (f_length) {
			x *= 1 / f_length;
			y *= 1 / f_length;
		}
	}

	Vector2& operator=(const Vector2& Vector2_n)
	{
		x = Vector2_n.x;
		y = Vector2_n.y;
		return *this;
	}

	Vector2 operator-(void) const
	{
		return Vector2(-x, -y);
	}

	Vector2 operator+(const Vector2& Vector2_n) const
	{
		return Vector2(x + Vector2_n.x, y + Vector2_n.y);
	}

	Vector2 operator+(float f_n) const
	{
		return Vector2(x + f_n, y + f_n);
	}

	Vector2 operator-(const Vector2& Vector2_n) const
	{
		return Vector2(x - Vector2_n.x, y - Vector2_n.y);
	}

	Vector2 operator-(float f_n) const
	{
		return Vector2(x - f_n, y - f_n);
	}

	Vector2 operator*(float f_n) const
	{
		return Vector2(x * f_n, y * f_n);
	}

	Vector2 operator*(const Vector2& Vector2_n) const
	{
		return Vector2(x * Vector2_n.x, y * Vector2_n.y);
	}

	Vector2 operator/(float f_n) const
	{
		return Vector2(x / f_n, y / f_n);
	}

	Vector2 operator/(const Vector2& Vector2_n) const
	{
		return Vector2(x / Vector2_n.x, y / Vector2_n.y);
	}

	bool operator==(const Vector2& Vector2_n) const
	{
		return (Vector2_n.x == x) && (Vector2_n.y == y);
	}

	bool operator!=(const Vector2& Vector2_n) const
	{
		return (Vector2_n.x != x) || (Vector2_n.y != y);
	}

	Vector2& operator+=(const Vector2& Vector2_n)
	{
		x += Vector2_n.x;
		y += Vector2_n.y;
		return *this;
	}

	Vector2& operator-=(const Vector2& Vector2_n)
	{
		x -= Vector2_n.x;
		y -= Vector2_n.y;
		return *this;
	}

	Vector2& operator*=(float f_n)
	{
		x *= f_n;
		y *= f_n;
		return *this;
	}

	Vector2& operator*=(const Vector2& Vector2_n)
	{
		x *= Vector2_n.x;
		y *= Vector2_n.y;
		return *this;
	}

	Vector2& operator/=(const Vector2& Vector2_n)
	{
		x /= Vector2_n.x;
		y /= Vector2_n.y;
		return *this;
	}

	Vector2& operator+=(float f_n)
	{
		x += f_n;
		y += f_n;
		return *this;
	}

	Vector2& operator/=(float f_n)
	{
		x /= f_n;
		y /= f_n;
		return *this;
	}

	Vector2& operator-=(float f_n)
	{
		x -= f_n;
		y -= f_n;
		return *this;
	}

	float& operator[](int i)
	{
		return ((float*)this)[i];
	}

	float operator[](int i) const
	{
		return ((float*)this)[i];
	}

	float x, y;
};