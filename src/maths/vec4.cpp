//#include "vec4.h"

//namespace protoengine {	namespace maths {

//	vec4::vec4(const float& x, const float &y, const float &z, const float &w)
//	{
//		this->x = x;
//		this->y = y;
//		this->z = z;
//		this->w = w;
//	}

//	vec4& vec4::add(const vec4& other)
//	{
//		x += other.x;
//		y += other.y;
//		z += other.z;
//		w += other.w;

//		return *this;
//	}

//	vec4& vec4::subtract(const vec4& other)
//	{
//		x -= other.x;
//		y -= other.y;
//		z -= other.z;
//		w -= other.w;

//		return *this;
//	}

//	vec4 vec4::operator+(const vec4& v) {
//		return vec4(x + v.x, y + v.y, z + v.z, w + v.w);
//	}

//	vec4 vec4::operator-(const vec4& v) {
//		return vec4(x - v.x, y - v.y, z - v.x, w - v.w);
//	}

//	vec4& vec4::operator+=(const vec4& other)
//	{
//		return add(other);
//	}

//	vec4& vec4::operator-=(const vec4& other)
//	{
//		return subtract(other);
//	}

//	bool vec4::operator==(const vec4& other)
//	{
//		return x == other.x && y == other.y && z == other.z && w == other.w;
//	}

//	bool vec4::operator!=(const vec4& other)
//	{
//		return !(*this == other);
//	}

//	std::ostream& operator<<(std::ostream& stream, const vec4& vector)
//	{
//		stream << "(" << vector.x << ", " << vector.y << ", " << vector.z << ", " << vector.w << ")";
//		return stream;
//	}
//}}
