#include "camera.h"

namespace Fondue {	namespace graphics {

    Camera::Camera(maths::vec3 position, maths::mat4 projection, maths::vec3 up, float yaw, float pitch)
        : projection(projection), mFront(maths::vec3(0.0f, 0.0f, -1.0f))
	{
		mPosition = position;
		mWorldUp = up;
		mUp = up;
		mRight = maths::cross(mUp, mFront);
		mYaw = yaw;
		mPitch = pitch;

        current_camera = this;
	}

    void Camera::translate(Camera_Movement direction, float deltaTime)
	{
        float velocity = mMovementSpeed * deltaTime;
        if (direction == FORWARD)
            mPosition += mFront * velocity;
        if (direction == BACKWARD)
            mPosition -= mFront * velocity;
        if (direction == LEFT)
            mPosition -= mRight * velocity;
        if (direction == RIGHT)
            mPosition += mRight * velocity;
        if (direction == UP)
            mPosition += mUp * velocity;
        if (direction == DOWN)
            mPosition -= mUp * velocity;

        hasMoved = true;
	}

    void Camera::rotate(float xoffset, float yoffset, bool constrainPitch)
	{
        mYaw += xoffset * mMouseSensitivity;
        mPitch += yoffset * mMouseSensitivity;

        if (constrainPitch)
        {
            if (mPitch > 89.0f)
                mPitch = 89.0f;
            if (mPitch < -89.0f)
                mPitch = -89.0f;
        }

        hasMoved = true;
	}

    void Camera::rotate(std::pair<float, float> xy, bool constrainPitch)
    {
        rotate(xy.first, xy.second, constrainPitch);
    }

    void Camera::updateView()
	{
        float yaw = maths::toRad(mYaw);
        float pitch = maths::toRad(mPitch);

        maths::vec3 front;
        front.x = cos(yaw) * cos(pitch);
        front.y = sin(pitch);
        front.z = sin(yaw) * cos(pitch);
        mFront = front;
        mRight = cross(mFront, mWorldUp);
        mUp    = cross(mRight, mFront);

        viewMatrix = maths::mat4::lookTowards(mUp, mPosition + mFront, mPosition);
        projectionView = viewMatrix * projection;

        hasMoved = false;
	}

    const maths::mat4& Camera::getViewMatrix()
    {
        return viewMatrix;
    }

    const maths::mat4& Camera::getProjectionMatrix()
    {
        return projection;
    }

    const maths::mat4& Camera::getProjectionViewMatrix()
    {
        return projectionView;
    }

} }
