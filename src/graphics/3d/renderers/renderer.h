#pragma once

#include "environment.h"
#include "skybox.h"
#include "cullpass.h"
#include "forwardpass.h"

namespace Fondue { namespace graphics {

    class Renderer
	{

	private:

        Environment env;
        SkyBoxRenderer skybox;
        CullPass cullPass;
        ForwardPass fp;

	public:

        Renderer(const Texture& envMap)
        {
            env.generate_env_binaries(envMap.getTID(), "testenv");

            skybox.init(CUBER_OUT);

            glActiveTexture(GL_TEXTURE0);

            // cullPass.init();

            fp.init();
        }

        ~Renderer() { }

        void init_GeometryDependant()
        {

        }

        void draw()
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            skybox.draw();

            // cullPass.draw();

            fp.draw();

        }

	};

} }
