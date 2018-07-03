#include "../src/protoengine.h"

using namespace Fondue;
using namespace graphics;

class Rasterizer : public Engine<Rasterizer>
{

private:

    TexturedBatch mesh{1};
    TextureArray textureBatch_3C{1024 * 4, 1024 * 4, GL_RGB8, GL_BGR, TEX_ARRAY_C3};
    TextureArray textureBatch_1C{1024 * 4, 1024 * 4, GL_R8, GL_BGR, TEX_ARRAY_C1};

    size_t emw, emh;
    Texture envRect{"res/cubemaps/cubeMap2.hdr", CUBER_IN};

    Camera camera{maths::vec3(-209.0f, 0.0f, 0.0), maths::mat4::perspective(45.0f, SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 10000.0f)};

    Renderer renderer{envRect};

    mesh_s CB0, CB1;
    light_s L0, L1, L2;

    bool GUIEnabled = false;

public:

    Rasterizer() { }

    ~Rasterizer() { }

    void init()
    {
        model_importer::load_model("res/models/cerberus/Cerberus_LP.obj", &textureBatch_3C, &textureBatch_1C, &mesh, nullptr);
        CB0 = mesh.add_instance();
        entity_manager::place_entity(CB0, 0,0,0, 10.0f);

        textureBatch_3C.generateMipMaps();
        textureBatch_1C.generateMipMaps();

        mWindow.captureCursor();
	}

    void init_GeometryDependant()
    {

    }

    void tick()
    {
        std::cout << getFPS() << std::endl;
    }

    void update()
    {
        camera.updateView();

        entity_manager::abs_rotate(CB0, 0.09f, 0, 1, 0);

        if (mWindow.isKeyPressed(GLFW_KEY_W))
            camera.translate(FORWARD, getFrameTime());
        if (mWindow.isKeyPressed(GLFW_KEY_S))
            camera.translate(BACKWARD, getFrameTime());
        if (mWindow.isKeyPressed(GLFW_KEY_A))
            camera.translate(LEFT, getFrameTime());
        if (mWindow.isKeyPressed(GLFW_KEY_D))
            camera.translate(RIGHT, getFrameTime());
        if (mWindow.isKeyPressed(GLFW_KEY_Z))
            camera.translate(UP, getFrameTime());
        if (mWindow.isKeyPressed(GLFW_KEY_X))
            camera.translate(DOWN, getFrameTime());

        if (!GUIEnabled) camera.rotate(mWindow.getMouseMovement());

        if (mWindow.isKeyTyped(GLFW_KEY_G))
        {
            mWindow.toggleCursor();
            GUIEnabled = !GUIEnabled;
        }

        mesh.update_positions();
        dynamic_light_manager::update_buffer();
	}

    void render()
    {
        renderer.draw();
	}

};


int main()
{
    Rasterizer i;
    i.start();
    return 0;
}
