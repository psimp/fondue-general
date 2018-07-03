#pragma once

#include "../../../maths/maths.h"

namespace Fondue {	namespace graphics {

	enum Camera_Movement {
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT,
		UP,
		DOWN,
		ROLLLEFT,
		ROLLRIGHT
    };

	class Camera
	{

    public:

        mutable bool hasMoved = false;

        static inline Camera* current_camera;

        Camera(maths::vec3 position, maths::mat4 projection, maths::vec3 up = {0, 1, 0}, float yaw = 0, float pitch = 0);

        void translate(Camera_Movement direction, float deltaTime);
        void rotate(float xoffset, float yoffset, bool constrainPitch = true);
        void rotate(std::pair<float, float> xy, bool constrainPitch = true);
        void updateView();

        const maths::mat4 &getViewMatrix();
        const maths::mat4 &getProjectionMatrix();
        const maths::mat4 &getProjectionViewMatrix();
        maths::vec3 getPosition() const { return mPosition; }

	private:

        maths::mat4 viewMatrix, projection, projectionView;

		// Camera Attributes
        maths::vec3 mPosition;
        maths::vec3 mFront;
        maths::vec3 mUp;
        maths::vec3 mRight;
        maths::vec3 mWorldUp;
        maths::vec3 mFocus;
		float mVelocityX = 0, mVelocityY = 0;
		// Euler Angles
        float mYaw, mPitch;
		// Orientation Quaternions
        maths::quat mPrevOrientation;
        maths::quat mOrientation;
		// Camera options
        float mMovementSpeed = 190.0f, mMouseSensitivity = 0.05f;
		float T = 0;

	};

} }
