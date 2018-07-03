#pragma once

#include "emscripten.h"
#include <functional>

#include "utils/timer.h"
#include "utils/system.h"
#include "graphics/window.h"

#include "graphics/3d/rasterizables/meshmanager.h"
#include "graphics/3d/rasterizables/model.h"
#include "graphics/3d/renderers/renderer.h"
#include "graphics/3d/renderers/raytracer.h"
#include "graphics/glwrappers/texturearray.h"
#include "graphics/glwrappers/texture.h"
#include "graphics/3d/scenes/view.h"

#include "maths/maths.h"

namespace Fondue {

    template <typename Interface>
    class Engine
	{

	private:

        int mFramesPerSecond, mUpdatesPerSecond, mCurrentFrame;
        float mCurrentFrameTime = 0, mDeltaFrameTime = 0, mLastFrameTime = 0;

	protected:

        graphics::Window mWindow{"Engine", (unsigned int) SCREEN_WIDTH, (unsigned int) SCREEN_HEIGHT};

        Engine() : mFramesPerSecond(0), mUpdatesPerSecond(0) { }

        ~Engine() { }

		// Runs once per init
        void init()
        { static_cast<Interface*>(this)->init();    }
        void init_GeometryDependant()
        { static_cast<Interface*>(this)->init_GeometryDependant(); }
		// Runs once per second
        void tick()
        { static_cast<Interface*>(this)->tick();    }
		// Runs 60 times per second
        void update()
        { static_cast<Interface*>(this)->update();  }
		// Runs as fast as possible
        void render()
        { static_cast<Interface*>(this)->render();  }

	public:

		void start()
		{
            globalTimer.reset();
            graphics::dynamic_light_manager::init();

			init();

            for (auto batch : graphics::TexturedBatch::context_tex_batches)
            {
                batch->init_main_vbo();
                batch->init_matrix_vbo();
                batch->init_ibo();
                batch->update_positions();
            }

            graphics::dynamic_light_manager::update_buffer();

            init_GeometryDependant();

			run();
		}

    protected:

        unsigned int getFPS() const   { return mFramesPerSecond;         }
        unsigned int getUPS() const   { return mUpdatesPerSecond;        }
        float getTimeElapsed() const  { return globalTimer.elapsed();    }
        float getFrameTime() const    { return mDeltaFrameTime;          }
        float getCurrentFrame() const { return mCurrentFrame;        }

	private:

        static void run(void* rm)
        {
            auto func = (std::function<void()>*)func;
            (*func)();
        }

		void run()
		{
			float secondsElapsed = 0.0f;
			float updateTimer = 0.0f;
			float updateTick = 1.0f / 60.0f;
			unsigned int frames = 0;
			unsigned int updates = 0;

            std::function<void()> main_loop = [&]()
            {
                if (getTimeElapsed() - updateTimer > updateTick)
                {
                    mCurrentFrameTime = getTimeElapsed();
                    mDeltaFrameTime = mCurrentFrameTime - mLastFrameTime;
                    mLastFrameTime = mCurrentFrameTime;

                    update();
                    updates++;
                    updateTimer += updateTick;
                }

                render();

                mWindow.update();
                frames++, mCurrentFrame++;
                if (globalTimer.elapsed() - secondsElapsed > 1.0f)
                {
                    tick();
                    mFramesPerSecond = frames;
                    mUpdatesPerSecond = updates;
                    frames = 0;
                    updates = 0;
                    secondsElapsed += 1.0f;
                }
            };

#ifndef FONDUE-WEB
            while (!mWindow.closed())
            {
                main_loop();
            }
#else
            emscripten_set_main_loop_arg(runfunc, main_loop, 0, 1);
#endif
        }

	};

}
