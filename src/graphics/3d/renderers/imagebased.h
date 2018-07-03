#pragma once

#include "raymarch.h"
#include "voxelizer.h"
#include "rendertoscreen.h"

namespace Fondue { namespace graphics {

    class Renderer
	{

	private:

//        Environment env;
//        SkyBoxRenderer skybox;
//        CullPass cullPass;
//        ForwardPass fp;

        RayMarch rm;
        Voxelizer sceneBBoxer{"src/shaders/voxelizer.fs"};

	public:

        Renderer()
        {
//            env.generate_env_binaries(envMap.getTID(), "testenv");

//            skybox.init(CUBER_OUT);

//            glActiveTexture(GL_TEXTURE0);

//            cullPass.init();
        }

        ~Renderer() { }

        void init_GeometryDependant()
        {
            sceneBBoxer.genSceneBBoxSdf();
            rm.init();
        }

        void draw(int cFrame)
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            rm.draw(cFrame);


//            skybox.draw();

//            cullPass.draw();
//            fp.draw(textures3C, textures1C);
        }

	};

} }
