
namespace protoengine { namespace maths {

//	mat4::mat4()
//	{
//        for (float& f : elements)
//            f = 0.0f;
//	}

//	mat4::mat4(float diagonal)
//	{
//        for (float& f : elements)
//            f = 0.0f;

//        elements[0]  = diagonal;
//        elements[5]  = diagonal;
//        elements[10] = diagonal;
//        elements[15] = diagonal;
//	}

//	mat4 mat4::identity()
//	{
//		return mat4(1.0f);
//	}

//	void mat4::Mat4xMat4SSE(mat4& out, const mat4& A, const mat4& B)
//	{
//		__m128 row1 = B.SSErows[0];
//		__m128 row2 = B.SSErows[1];
//		__m128 row3 = B.SSErows[2];
//		__m128 row4 = B.SSErows[3];
//		for (int i = 0; i < 4; i++) {
//			__m128 brod1 = _mm_set1_ps(A.elements[4 * i + 0]);
//			__m128 brod2 = _mm_set1_ps(A.elements[4 * i + 1]);
//			__m128 brod3 = _mm_set1_ps(A.elements[4 * i + 2]);
//			__m128 brod4 = _mm_set1_ps(A.elements[4 * i + 3]);
//			__m128 row = _mm_add_ps(
//				_mm_add_ps(
//					_mm_mul_ps(brod1, row1),
//					_mm_mul_ps(brod2, row2)),
//				_mm_add_ps(
//					_mm_mul_ps(brod3, row3),
//					_mm_mul_ps(brod4, row4)));
//			_mm_store_ps(&out.elements[4 * i], row);
//		}
//	}

//	void mat4::Mat4xVec4SSE(vec4& out, const mat4& M, const vec4& V)
//	{
//		__m128 vec_x = _mm_permute_ps(V.SSEVec4, 0x00);
//		__m128 vec_y = _mm_permute_ps(V.SSEVec4, 0x55);
//		__m128 vec_z = _mm_permute_ps(V.SSEVec4, 0xAA);
//		__m128 vec_w = _mm_permute_ps(V.SSEVec4, 0xFF);

//		__m128 acc = _mm_setzero_ps();
//		acc = _mm_add_ps(acc, _mm_mul_ps(vec_x, M.SSErows[0]));
//		acc = _mm_add_ps(acc, _mm_mul_ps(vec_y, M.SSErows[1]));
//		acc = _mm_add_ps(acc, _mm_mul_ps(vec_z, M.SSErows[2]));
//		acc = _mm_add_ps(acc, _mm_mul_ps(vec_w, M.SSErows[3]));

//		out.SSEVec4 = acc;
//	}

//    vec3 mat4::Mat4xVec3SSE(const mat4& M, const float& vx, const float& vy, const float& vz)
//	{
//        __m128 vec_x = _mm_set1_ps(vx);
//        __m128 vec_y = _mm_set1_ps(vy);
//        __m128 vec_z = _mm_set1_ps(vz);
//        __m128 vec_w = _mm_set1_ps(1.0f);

//        __m128 acc = _mm_setzero_ps();
//        acc = _mm_add_ps(acc, _mm_mul_ps(vec_x, M.SSErows[0]));
//        acc = _mm_add_ps(acc, _mm_mul_ps(vec_y, M.SSErows[1]));
//        acc = _mm_add_ps(acc, _mm_mul_ps(vec_z, M.SSErows[2]));
//        acc = _mm_add_ps(acc, _mm_mul_ps(vec_w, M.SSErows[3]));

//        vec3 rv;
//        _mm_storeu_ps(rv.elements, acc);
//	}

//    void mat4::Mat4x4TransposeSSE(mat4& out, const mat4& in)
//	{
//		__m128 row1 = in.SSErows[0];
//		__m128 row2 = in.SSErows[1];
//		__m128 row3 = in.SSErows[2];
//		__m128 row4 = in.SSErows[3];
//		_MM_TRANSPOSE4_PS(row1, row2, row3, row4);
//		out.SSErows[0] = row1;
//		out.SSErows[1] = row2;
//		out.SSErows[2] = row3;
//		out.SSErows[3] = row4;
//	}

//    void mat4::M44TransformInverseSSE(mat4& out, const mat4& inM)
//    {
//        __m128 t0 = XYXY(inM.SSErows[0], inM.SSErows[1]);
//        __m128 t1 = YZYZ(inM.SSErows[0], inM.SSErows[1]);
//        out.SSErows[0] = SWIZZLE(t0, inM.SSErows[2], 0,2,0,3);
//        out.SSErows[1] = SWIZZLE(t0, inM.SSErows[2], 1,3,1,3);
//        out.SSErows[2] = SWIZZLE(t1, inM.SSErows[2], 0,2,2,3);

//        __m128 sizeSqr;
//        sizeSqr =                     _mm_mul_ps(out.SSErows[0], out.SSErows[0]);
//        sizeSqr = _mm_add_ps(sizeSqr, _mm_mul_ps(out.SSErows[1], out.SSErows[1]));
//        sizeSqr = _mm_add_ps(sizeSqr, _mm_mul_ps(out.SSErows[2], out.SSErows[2]));

//        __m128 one = _mm_set1_ps(1.f);
//        __m128 rSizeSqr = _mm_blendv_ps(
//            _mm_div_ps(one, sizeSqr),
//            one,
//            _mm_cmplt_ps(sizeSqr, _mm_set1_ps(EPSILON))
//            );

//        out.SSErows[0] = _mm_mul_ps(out.SSErows[0], rSizeSqr);
//        out.SSErows[1] = _mm_mul_ps(out.SSErows[1], rSizeSqr);
//        out.SSErows[2] = _mm_mul_ps(out.SSErows[2], rSizeSqr);

//        out.SSErows[3] =                            _mm_mul_ps(out.SSErows[0], SWIZZLE(inM.SSErows[3], 0));
//        out.SSErows[3] = _mm_add_ps(out.SSErows[3], _mm_mul_ps(out.SSErows[1], SWIZZLE(inM.SSErows[3], 1)));
//        out.SSErows[3] = _mm_add_ps(out.SSErows[3], _mm_mul_ps(out.SSErows[2], SWIZZLE(inM.SSErows[3], 2)));
//        out.SSErows[3] = _mm_sub_ps(_mm_setr_ps(0.f, 0.f, 0.f, 1.f), out.SSErows[3]);
//    }

//    mat4 operator*(const mat4& left, const mat4& right)
//	{
//        mat4 out;
//        maths::mat4::Mat4xMat4SSE(out, left, right);
//        return out;
//	}

//    mat4& operator*=(mat4& left, const mat4& right)
//    {
//        maths::mat4::Mat4xMat4SSE(left, left, right);
//        return left;
//    }

//	vec4 operator*(const mat4& left, const vec4& right)
//	{
//        vec4 out;
//        maths::mat4::Mat4xVec4SSE(out, left, right);
//        return out;
//	}

//    vec4& operator*=(vec4& left, const mat4& right)
//    {
//        maths::mat4::Mat4xVec4SSE(left, right, left);
//        return left;
//    }

//	mat4 mat4::orthographic(float left, float right, float bottom, float top, float near, float far)
//	{
//		mat4 result(1.0f);

//		result.elements[0 + 0 * 4] = 2.0f / (right - left);
//		result.elements[1 + 1 * 4] = 2.0f / (top - bottom);
//		result.elements[2 + 2 * 4] = 2.0f / (near - far);
//		result.elements[3 + 3 * 4] = 1.0f;

//		result.elements[0 + 3 * 4] = (left + right) / (left - right);
//		result.elements[1 + 3 * 4] = (bottom + top) / (bottom - top);
//		result.elements[2 + 3 * 4] = (far + near) / (near - far);

//		return result;
//	}

//	mat4 mat4::perspective(float fov, float aspectRatio, float near, float far)
//	{
//		mat4 result(1.0f);

//		float q = 1.0f / tan(toRad(0.5f * fov));
//		float a = q / aspectRatio;
//		float b = (near + far) / (near - far);
//		float c = (2.0f * near * far) / (near - far);

//		result.elements[0 + 0 * 4] = a;
//		result.elements[1 + 1 * 4] = q;
//		result.elements[2 + 2 * 4] = b;
//		result.elements[3 + 2 * 4] = -1.0f;
//		result.elements[2 + 3 * 4] = c;
//		result.elements[3 + 3 * 4] = 0.0f;

//		return result;
//	}

//	mat4 mat4::translation(const vec3& translation)
//	{
//		mat4 result(1.0f);

//		result.elements[0 + 3 * 4] = translation.x;
//		result.elements[1 + 3 * 4] = translation.y;
//		result.elements[2 + 3 * 4] = translation.z;

//		return result;
//	}

//	mat4 mat4::rotation(float angle, const vec3& axis)
//	{
//		mat4 result(1.0f);

//		float r = toRad(angle);
//		float c = cos(r);
//		float s = sin(r);
//		float omc = 1.0f - c;

//		float x = axis.x;
//		float y = axis.y;
//		float z = axis.z;

//		result.elements[0 + 0 * 4] = x * x * omc + c;
//		result.elements[1 + 0 * 4] = y * x * omc + z * s;
//		result.elements[2 + 0 * 4] = z * x * omc - y * s;

//		result.elements[0 + 1 * 4] = x * y * omc - z * s;
//		result.elements[1 + 1 * 4] = y * y * omc + c;
//		result.elements[2 + 1 * 4] = z * y * omc + x * s;

//		result.elements[0 + 2 * 4] = x * z * omc + y * s;
//		result.elements[1 + 2 * 4] = y * z * omc - x * s;
//		result.elements[2 + 2 * 4] = z * z * omc + c;

//		return result;
//	}

//	mat4 mat4::scale(const vec3& scale)
//	{
//		mat4 result(1.0f);

//		result.elements[0 + 0 * 4] = scale.x;
//		result.elements[1 + 1 * 4] = scale.y;
//		result.elements[2 + 2 * 4] = scale.z;

//		return result;
//	}

//    mat4 mat4::lookTowards(const vec3& _up, const vec3& focusPosition, const vec3& eyePosition)
//    {
//        vec3 direction = normalize(focusPosition - eyePosition);
//        vec3 right = normalize(cross(direction, _up));
//        vec3 up = normalize(cross(direction, right));

//		mat4 viewSpace(1.0f);

//		viewSpace.elements[0 + 0 * 4] = right.x;
//		viewSpace.elements[1 + 0 * 4] = up.x;
//		viewSpace.elements[2 + 0 * 4] = direction.x;

//		viewSpace.elements[0 + 1 * 4] = right.y;
//		viewSpace.elements[1 + 1 * 4] = up.y;
//		viewSpace.elements[2 + 1 * 4] = direction.y;

//		viewSpace.elements[0 + 2 * 4] = right.z;
//		viewSpace.elements[1 + 2 * 4] = up.z;
//		viewSpace.elements[2 + 2 * 4] = direction.z;

//        maths::mat4::Mat4xMat4SSE(viewSpace, mat4::translation(eyePosition), viewSpace);

//		return viewSpace;
//	}

//	std::ostream& operator<<(std::ostream& stream, const mat4& matrix)
//	{
//		stream << "{" << matrix.rows[0] << std::endl << ", " << matrix.rows[1] << std::endl << ", " << matrix.rows[2] << std::endl << ", " << matrix.rows[3] << std::endl << "}";
//		return stream;
//	}

}}
