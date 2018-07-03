#pragma once

#include "raymarch.h"
#include "voxelizer.h"
#include "rendertoscreen.h"

namespace Fondue { namespace graphics {

    class Raytracer
	{

	private:

        RayMarch rm;
        Voxelizer sceneBBoxer{"src/shaders/voxelizer.fs"};

	public:

        Raytracer() { }

        ~Raytracer() { }

        void init_GeometryDependant()
        {
            sceneBBoxer.genSceneBBoxSdf();
            rm.init();
        }

        void draw(int cFrame)
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            rm.draw(cFrame);
        }

	};

} }
