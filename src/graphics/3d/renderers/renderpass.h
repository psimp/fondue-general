#ifndef RENDERPASS_H
#define RENDERPASS_H

#include <glad/glad.h>
#include <memory>
#include <utility>

#include "../../glwrappers/framebuffer.h"
#include "../../shader.h"
#include "../rasterizables/meshmanager.h"
#include "../batches/textured.h"
#include "../scenes/camera.h"
#include "../rasterizables/lightmanager.h"
#include "../../../utils/timer.h"
#include "../scenes/view.h"
#include "rprimitives.h"

namespace Fondue { namespace graphics {

//! Below are global texture bindings (0 - 31), which are used
//! exclusively for shader storage

#define NOISE_MAP                           1
#define POINT_MAP                           2
#define TID_MAP                             3
#define VOXEL_MAP                           4
#define INDEX_MAP                           5
#define SCENE_SDF                           6
#define SCENE_HITTRACKER                    7

#define TEX_ARRAY_C3                        8
#define TEX_ARRAY_C1                        9

#define BRDF_CONVOLUTION                    10

#define CONVOLUTER_SPECULAR_OUT             11
#define CONVOLUTER_DIFFUSE_OUT              12
#define CONVOLUTER_IN                       13

#define CUBER_OUT                           14
#define CUBER_IN                            15

#define FBO_LIGHTS1                         16
#define FBO_LIGHTS2                         17
#define FBO_LIGHTS3                         18

#define FBO_LIGHT_POS_BUFFER                19
#define FBO_LIGHT_NORM_BUFFER               20
#define FBO_LIGHT_FLUX_BUFFER               21
#define FBO_LIGHT_DEPTH_BUFFER              22

#define SHADOW_MAP                          23

    template <class T, class S>
    class RenderPass
    {

    protected:

        const uint SCENE_GRID_SIZE = 512;

        primitives rprims;

        std::unique_ptr<Shader<T, S>> mShader;

        void makeComputeShader(const char* compute)
        {
            mShader = std::make_unique<Shader<T, S>>(compute);
        }

        void makePipelineShader(const char* vertex, const char* fragment)
        {
            mShader = std::make_unique<Shader<T, S>>(vertex, fragment);
        }

    protected:
        RenderPass() { }
    };

} }

#endif // RENDERPASS_H
