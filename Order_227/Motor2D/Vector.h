// ----------------------------------------------------
// Vector class    -----------
// ----------------------------------------------------

#ifndef VECTOR_H
#define VECTOR_H

#include <cmath>

#define PI 3.14159f

// 2D Vector
template<class TYPE>
class Vector2
{
public:

	TYPE x, y;

	Vector2()
	{}

	Vector2(const Vector2<TYPE>& vec) : x(vec.x), y(vec.y)
	{}

	Vector2(const TYPE& x, const TYPE& y) : x(x), y(y)
	{}

	Vector2& create(const TYPE& x, const TYPE& y)
	{
		this->x = x;
		this->y = y;

		return(*this);
	}

	// Math ------------------------------------------------
	Vector2 operator -(const Vector2 &vec) const
	{
		Vector2<TYPE> r;

		r.x = x - vec.x;
		r.y = y - vec.y;

		return(r);
	}

	Vector2 operator -(const TYPE &num) const
	{
		Vector2<TYPE> r;

		r.x = x - num;
		r.y = y - num;

		return(r);
	}

	Vector2 operator +(const Vector2 &vec) const
	{
		Vector2<TYPE> r;

		r.x = x + vec.x;
		r.y = y + vec.y;

		return(r);
	}

	Vector2 operator +(const TYPE &num) const
	{
		Vector2<TYPE> r;

		r.x = x + num;
		r.y = y + num;

		return(r);
	}

	Vector2 operator *(const Vector2 &vec) const
	{
		Vector2<TYPE> r;

		r.x = x * vec.x;
		r.y = y * vec.y;

		return(r);
	}

	Vector2 operator *(const TYPE &num) const
	{
		Vector2<TYPE> r;

		r.x = x * num;
		r.y = y * num;

		return(r);
	}

	const Vector2& operator -=(const Vector2 &vec)
	{
		x -= vec.x;
		y -= vec.y;

		return(*this);
	}

	const Vector2& operator -=(const TYPE &num)
	{
		x -= num;
		y -= num;

		return(*this);
	}

	const Vector2& operator +=(const Vector2 &vec)
	{
		x += vec.x;
		y += vec.y;

		return(*this);
	}

	const Vector2& operator +=(const TYPE &num)
	{
		x += num;
		y += num;

		return(*this);
	}

	const Vector2& operator *=(const Vector2 &vec)
	{
		x *= vec.x;
		y *= vec.y;

		return(*this);
	}

	const Vector2& operator *=(const TYPE &num)
	{
		x *= num;
		y *= num;

		return(*this);
	}

	bool operator ==(const Vector2& vec) const
	{
		return (x == vec.x && y == vec.y);
	}

	bool operator !=(const Vector2& vec) const
	{
		return (x != vec.x || y != vec.y);
	}

	// Utils ------------------------------------------------
	bool IsZero() const
	{
		return (x == 0 && y == 0);
	}

	Vector2& SetToZero()
	{
		x = y = (TYPE)0;
		return(*this);
	}

	Vector2& Reverse()
	{
		x = -x;
		y = -y;

		return(*this);
	}

	// Vector Operations
	TYPE GetMagnitude()
	{
		return sqrt(pow(x, 2) + pow(y, 2));
	}

	Vector2 GetUnitVector()
	{
		TYPE magnitude = GetMagnitude();
		return { x / magnitude, y / magnitude };
	}

	float GetAngle(Vector2<TYPE> vec)	// All calculations are done in RADIANS and angle calculation goes COUNTER-CLOCKWISE
	{
		float dot = x * vec.x + y * vec.y;
		float det = x * vec.y - y * vec.x;
		float angle = atan2(det, dot);

		if (angle < 0.0f) {
			angle = 2 * PI + angle;
		}

		return angle;
	}
};

typedef Vector2<short> sVec2;
typedef Vector2<int> iVec2;
typedef Vector2<unsigned int> uVec2;
typedef Vector2<float> fVec2;
typedef Vector2<double> dVec2;

// 3D Vector
template<class TYPE>
class Vector3
{
public:

	TYPE x, y, z;

	Vector3()
	{}

	Vector3(const Vector3<TYPE>& vec) : x(vec.x), y(vec.y), z(vec.z)
	{}

	Vector3(const TYPE& x, const TYPE& y, const TYPE& z) : x(x), y(y), z(z)
	{}

	Vector3& create(const TYPE& x, const TYPE& y, const TYPE& z)
	{
		this->x = x;
		this->y = y;
		this->z = z;

		return(*this);
	}

	// Math ------------------------------------------------
	Vector3 operator -(const Vector3 &vec) const
	{
		Vector3<TYPE> r;

		r.x = x - vec.x;
		r.y = y - vec.y;
		r.z = z - vec.z;

		return(r);
	}

	Vector3 operator -(const TYPE &num) const
	{
		Vector3<TYPE> r;

		r.x = x - num;
		r.y = y - num;
		r.z = z - num;

		return(r);
	}

	Vector3 operator +(const Vector3 &vec) const
	{
		Vector3<TYPE> r;

		r.x = x + vec.x;
		r.y = y + vec.y;
		r.z = z + vec.z;

		return(r);
	}

	Vector3 operator +(const TYPE &num) const
	{
		Vector3<TYPE> r;

		r.x = x + num;
		r.y = y + num;
		r.z = z + num;

		return(r);
	}

	Vector3 operator *(const Vector3 &vec) const
	{
		Vector3<TYPE> r;

		r.x = x * vec.x;
		r.y = y * vec.y;
		r.z = z * vec.z;

		return(r);
	}

	Vector3 operator *(const TYPE &num) const
	{
		Vector3<TYPE> r;

		r.x = x * num;
		r.y = y * num;
		r.z = z * num;

		return(r);
	}

	const Vector3& operator -=(const Vector3 &vec)
	{
		x -= vec.x;
		y -= vec.y;
		z -= vec.z;

		return(*this);
	}

	const Vector3& operator -=(const TYPE &num)
	{
		x -= num;
		y -= num;
		z -= num;

		return(*this);
	}

	const Vector3& operator +=(const Vector3 &vec)
	{
		x += vec.x;
		y += vec.y;
		z += vec.z;

		return(*this);
	}

	const Vector3& operator +=(const TYPE &num)
	{
		x += num;
		y += num;
		z += num;

		return(*this);
	}

	const Vector3& operator *=(const Vector3 &vec)
	{
		x *= vec.x;
		y *= vec.y;
		z *= vec.z;

		return(*this);
	}

	const Vector3& operator *=(const TYPE &num)
	{
		x *= num;
		y *= num;
		z *= num;

		return(*this);
	}

	bool operator ==(const Vector3& vec) const
	{
		return (x == vec.x && y == vec.y && z == vec.z);
	}

	bool operator !=(const Vector3& vec) const
	{
		return (x != vec.x || y != vec.y || z != vec.z);
	}

	// Utils ------------------------------------------------
	bool IsZero() const
	{
		return (x == 0 && y == 0 && z == 0);
	}

	Vector3& SetToZero()
	{
		x = y = z = (TYPE)0;
		return(*this);
	}

	Vector3& Reverse()
	{
		x = -x;
		y = -y;
		z = -z;

		return(*this);
	}

	//Vector Operations
	TYPE GetMagnitude()
	{
		return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
	}

	Vector3 GetUnitVector()
	{
		TYPE magnitude = GetMagnitude();
		return { x / magnitude, y / magnitude, z / magnitude };
	}
};

typedef Vector3<short> sVec3;
typedef Vector3<int> iVec3;
typedef Vector3<unsigned int> uVec3;
typedef Vector3<float> fVec3;
typedef Vector3<double> dVec3;

#endif	//VECTOR_H