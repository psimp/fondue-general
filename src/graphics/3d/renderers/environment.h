#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <string>

#include "cubemap.h"
#include "rendertotexture.h"

namespace Fondue { namespace graphics {

    class Environment
    {

        GLuint cubeMap, specMap, diffMap, brdf;

    private:

        auto load_brdf_tex(const std::string& binHandle)
        {
            GLuint mTID;

            FILE* fpi = fopen(("res/binaries/env/" + binHandle + ".binf").c_str(), "rb");
            size_t info2[3];
            fread(info2, 3 * sizeof(size_t), 1, fpi);
            fclose(fpi);

            auto mWidth = info2[0];
            auto mHeight = info2[1];
            auto size = info2[2];
            uint16_t* pixels = new uint16_t[size];

            FILE* fpd = fopen(("res/binaries/env/" + binHandle + ".bin").c_str(), "rb");
            fread(pixels, size * sizeof(uint16_t), 1, fpd);
            fclose(fpd);

            glGenTextures(1, &mTID);
            glBindTexture(GL_TEXTURE_2D, mTID);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, mWidth, mHeight, 0, GL_RG, GL_HALF_FLOAT, pixels);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            delete []pixels;

            return mTID;
        }

        auto load_env_map(const std::string& binHandle, bool mipMapSampler)
        {
            FILE* fpi = fopen(("res/binaries/env/" + binHandle + ".binf").c_str(), "rb");
            size_t info2[3];
            fread(info2, 3 * sizeof(size_t), 1, fpi);
            fclose(fpi);

            size_t memsize = info2[2];
            size_t res = info2[1];

            GLuint cubeMapOut;
            glGenTextures(1, &cubeMapOut);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapOut);
            for (unsigned int i = 0; i < 6; ++i)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
                             res, res, 0, GL_RGB, GL_HALF_FLOAT, nullptr);
            }
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            if (mipMapSampler)
            {
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

                for (uint mip = 0; mip < CUBEMAP_MIPS; mip++)
                {
                    int mipWidth  = res * pow(0.5, mip);
                    int mipHeight = res * pow(0.5, mip);
                    int memsizemip = 3 * mipWidth * mipHeight;
                    for (unsigned int i = 0; i < 6; ++i)
                    {
                        uint16_t* pixels = new uint16_t[memsizemip];
                        FILE* fpd = fopen(("res/binaries/env/" + binHandle + "_" + std::to_string(i) + "_" + std::to_string(mip) + ".bin").c_str(), "rb");
                        fread(pixels, memsizemip * sizeof(uint16_t), 1, fpd);
                        fclose(fpd);

                        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mip, GL_RGB16F,
                                     mipWidth, mipHeight, 0, GL_RGB, GL_HALF_FLOAT, pixels);

                        delete[] pixels;
                    }
                }
            }
            else
            {
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

                for (unsigned int i = 0; i < 6; ++i)
                {
                    uint16_t* pixels = new uint16_t[memsize];
                    FILE* fpd = fopen(("res/binaries/env/" + binHandle + "_" + std::to_string(i) + ".bin").c_str(), "rb");
                    fread(pixels, memsize * sizeof(uint16_t), 1, fpd);
                    fclose(fpd);

                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
                                 res, res, 0, GL_RGB, GL_HALF_FLOAT, pixels);

                    delete[] pixels;
                }
            }

            return cubeMapOut;
        }

    public:

        void generate_env_binaries(const GLuint envMapTID, const std::string& binHandle)
        {
            CubeMapper cuber{"src/shaders/basic.vs", "src/shaders/basic.fs", 1024, true, false};
            CubeMapper convoluter_diffuse{"src/shaders/convolute.vs", "src/shaders/convolute.fs", 64, false, false};
            CubeMapper convoluter_specular{"src/shaders/convolute.vs", "src/shaders/convolutePreFilter.fs", 512, false, true};
            TextureRenderer brdfConv{"src/shaders/convoluteBRDF.fs", 1024};

            glActiveTexture(GL_TEXTURE0 + CUBER_OUT);

            auto cubeMapOut = cuber.init(CUBER_IN);

            glActiveTexture(GL_TEXTURE0 + CUBER_IN);
            glBindTexture(GL_TEXTURE_2D, envMapTID);

            glActiveTexture(GL_TEXTURE0 + CUBER_OUT);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapOut);

            cuber.generate_maps(binHandle + "_cube");

            glActiveTexture(GL_TEXTURE0 + CONVOLUTER_DIFFUSE_OUT);

            GLuint irrMap = convoluter_diffuse.init(CONVOLUTER_IN);

            glActiveTexture(GL_TEXTURE0 + CONVOLUTER_IN);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapOut);

            glActiveTexture(GL_TEXTURE0 + CONVOLUTER_DIFFUSE_OUT);
            glBindTexture(GL_TEXTURE_CUBE_MAP, irrMap);

            convoluter_diffuse.generate_maps(binHandle + "_diff");

            glActiveTexture(GL_TEXTURE0 + CONVOLUTER_SPECULAR_OUT);

            GLuint specMap = convoluter_specular.init(CONVOLUTER_IN);

            glActiveTexture(GL_TEXTURE0 + CONVOLUTER_IN);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapOut);

            glActiveTexture(GL_TEXTURE0 + CONVOLUTER_SPECULAR_OUT);
            glBindTexture(GL_TEXTURE_CUBE_MAP, specMap);

            convoluter_specular.generate_maps(binHandle + "_spec");

            glActiveTexture(GL_TEXTURE0 + BRDF_CONVOLUTION);

            GLuint brdfC = brdfConv.init();

            glActiveTexture(GL_TEXTURE0 + BRDF_CONVOLUTION);
            glBindTexture(GL_TEXTURE_2D, brdfC);

            brdfConv.generate_tex(binHandle + "_brdf");
        }

        void load_from_bin(const std::string binHandle)
        {
            glActiveTexture(GL_TEXTURE0 + CUBER_OUT);
            auto c1 = load_env_map(binHandle + "_cube", false);
            glBindTexture(GL_TEXTURE_CUBE_MAP, c1);

            glActiveTexture(GL_TEXTURE0 + CONVOLUTER_DIFFUSE_OUT);
            auto c2 = load_env_map(binHandle + "_diff", false);
            glBindTexture(GL_TEXTURE_CUBE_MAP, c2);

            glActiveTexture(GL_TEXTURE0 + CONVOLUTER_SPECULAR_OUT);
            auto c3 = load_env_map(binHandle + "_spec", true);
            glBindTexture(GL_TEXTURE_CUBE_MAP, c3);

            glActiveTexture(GL_TEXTURE0 + BRDF_CONVOLUTION);
            auto brdf = load_brdf_tex(binHandle + "_brdf");
            glBindTexture(GL_TEXTURE_2D, brdf);
        }

    };

} }

#endif // ENVIRONMENT_H
