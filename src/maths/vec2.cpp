//#include "vec2.h"

//namespace protoengine {	namespace maths {

//	vec2::vec2()
//	{
//		x = 0.0f;
//		y = 0.0f;
//	}

//	vec2::vec2(const float& x, const float &y)
//	{
//		this->y = y;
//		this->x = x;
//	}

//	vec2& vec2::add(const vec2& other)
//	{
//		x += other.x;
//		y += other.y;

//		return *this;
//	}

//	vec2& vec2::subtract(const vec2& other)
//	{
//		x -= other.x;
//		y -= other.y;

//		return *this;
//	}

//	vec2 vec2::operator+(const vec2& v) {
//		return vec2(x + v.x, y + v.y);
//	}

//	vec2 vec2::operator-(const vec2& v) {
//		return vec2(x - v.x, y - v.y);
//	}

//	vec2& vec2::operator+=(const vec2& other)
//	{
//		return add(other);
//	}

//	vec2& vec2::operator-=(const vec2& other)
//	{
//		return subtract(other);
//	}

//	bool vec2::operator==(const vec2& other)
//	{
//		return x == other.x && y == other.y;
//	}

//	bool vec2::operator!=(const vec2& other)
//	{
//		return !(*this == other);
//	}

//	std::ostream& operator<<(std::ostream& stream, const vec2& vector)
//	{
//		stream << "(" << vector.x << ", " << vector.y << ")";
//		return stream;
//	}
//}}
