#include <sstream>

class Vector4
{
public:

	Vector4()
	{
		x = 0;
		y = 0;
		z = 0;
		w = 0;
	}

	Vector4(float f_x, float f_y, float f_z, float f_w)
	{
		x = f_x;
		y = f_y;
		z = f_z;
		w = f_w;
	}

	Vector4(const float* f_n)
	{
		x = f_n[0];
		y = f_n[1];
		z = f_n[2];
		w = f_n[3];
	}

	__inline bool is_valid() const // not used in this project but just in case I ever need it.
	{
		return std::isfinite(x) && std::isfinite(y) && std::isfinite(z) && std::isfinite(w);
	}

	float length() const
	{
		return sqrt(x * x + y * y + z * z + w * w);
	}

	float length_sqr(void) const
	{
		return (x * x + y * y + z * z + w * w);
	}

	float dist_to_sqr(const Vector4& v4_n) const
	{
		Vector4 delta;

		delta.x = x - v4_n.x;
		delta.y = y - v4_n.y;
		delta.z = z - v4_n.z;
		delta.w = z - v4_n.w;

		return delta.length_sqr();
	}

	void normalize()
	{
		float f_length = this->length();
		if (f_length) {
			x *= 1 / f_length;
			y *= 1 / f_length;
			z *= 1 / f_length;
			w *= 1 / f_length;
		}
	}

	Vector4& operator=(const Vector4& v4_n)
	{
		x = v4_n.x;
		y = v4_n.y;
		z = v4_n.z;
		w = v4_n.w;
		return *this;
	}

	Vector4 operator-(void) const
	{
		return Vector4(-x, -y, -z, -w);
	}

	Vector4 operator+(const Vector4& v4_n) const
	{
		return Vector4(x + v4_n.x, y + v4_n.y, z + v4_n.z, w + v4_n.w);
	}

	Vector4 operator+(float f_n) const
	{
		return Vector4(x + f_n, y + f_n, z + f_n, w + f_n);
	}

	Vector4 operator-(const Vector4& v4_n) const
	{
		return Vector4(x - v4_n.x, y - v4_n.y, z - v4_n.z, w - v4_n.w);
	}

	Vector4 operator-(float f_n) const
	{
		return Vector4(x - f_n, y - f_n, z - f_n, w - f_n);
	}

	Vector4 operator*(float f_n) const
	{
		return Vector4(x * f_n, y * f_n, z * f_n, w * f_n);
	}

	Vector4 operator*(const Vector4& v4_n) const
	{
		return Vector4(x * v4_n.x, y * v4_n.y, z * v4_n.z, w * v4_n.w);
	}

	Vector4 operator/(float f_n) const
	{
		return Vector4(x / f_n, y / f_n, z / f_n, w / f_n);
	}

	Vector4 operator/(const Vector4& v4_n) const
	{
		return Vector4(x / v4_n.x, y / v4_n.y, z / v4_n.z, w / v4_n.w);
	}

	bool operator==(const Vector4& v4_n) const
	{
		return (v4_n.x == x) && (v4_n.y == y) && (v4_n.z == z) && (v4_n.w == w);
	}

	bool operator!=(const Vector4& v4_n) const
	{
		return (v4_n.x != x) || (v4_n.y != y) || (v4_n.z != z) || (v4_n.w != w);
	}

	Vector4& operator+=(const Vector4& v4_n)
	{
		x += v4_n.x;
		y += v4_n.y;
		z += v4_n.z;
		w += v4_n.w;
		return *this;
	}

	Vector4& operator-=(const Vector4& v4_n)
	{
		x -= v4_n.x;
		y -= v4_n.y;
		z -= v4_n.z;
		w -= v4_n.w;
		return *this;
	}

	Vector4& operator*=(float f_n)
	{
		x *= f_n;
		y *= f_n;
		z *= f_n;
		w *= f_n;
		return *this;
	}

	Vector4& operator*=(const Vector4& v4_n)
	{
		x *= v4_n.x;
		y *= v4_n.y;
		z *= v4_n.z;
		w *= v4_n.w;
		return *this;
	}

	Vector4& operator/=(const Vector4& v4_n)
	{
		x /= v4_n.x;
		y /= v4_n.y;
		z /= v4_n.z;
		w /= v4_n.w;
		return *this;
	}

	Vector4& operator+=(float f_n)
	{
		x += f_n;
		y += f_n;
		z += f_n;
		w += f_n;
		return *this;
	}

	Vector4& operator/=(float f_n)
	{
		x /= f_n;
		y /= f_n;
		z /= f_n;
		w /= f_n;
		return *this;
	}

	Vector4& operator-=(float f_n)
	{
		x -= f_n;
		y -= f_n;
		z -= f_n;
		w -= f_n;
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

	float x, y, z, w;
};