#include "../src/protoengine.h"

using namespace Fondue;
using namespace graphics;

class Rays : public Engine<Rays>
{

private:

    TexturedBatch mesh{2};

    Voxelizer voxelizer{"src/shaders/gensdf.fs"};
    Texture noise{"res/noise.jpg", NOISE_MAP};

    Camera camera{maths::vec3(0.0f, 1.0f, 0.0f), maths::mat4::perspective(45.0f, SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 10000.0f)};

    Raytracer renderer;

    bool GUIEnabled = false;

    mesh_s s0, s1;

public:

    Rays()
    { }

    ~Rays()
	{

	}

    void init()
    {
        View::init_viewspace_bbox_ubos();

        model_importer::load_model("res/models/tiger/tiger.obj", nullptr, nullptr, &mesh, &voxelizer);
        s0 = mesh.add_instance();
        entity_manager::place_entity(s0, 0,0,0, 1.0f);
        s1 = mesh.add_instance();
        entity_manager::place_entity(s1, 10,125,0, 1.0f);

        mWindow.captureCursor();
        View::updateViewSpace();
	}

    void init_GeometryDependant()
    {
        renderer.init_GeometryDependant();
    }

    void tick()
    {
        std::cout << getFPS() << std::endl;
    }

    void update()
    {
        //entity_manager::abs_rotate(s0, 0.1f, 0, 1, 0);

        camera.updateView();

        if (mWindow.isKeyPressed(GLFW_KEY_W))
            camera.translate(FORWARD, getFrameTime());
        if (mWindow.isKeyPressed(GLFW_KEY_S))
            camera.translate(BACKWARD, getFrameTime());
        if (mWindow.isKeyPressed(GLFW_KEY_A))
            camera.translate(RIGHT, getFrameTime());
        if (mWindow.isKeyPressed(GLFW_KEY_D))
            camera.translate(LEFT, getFrameTime());
        if (mWindow.isKeyPressed(GLFW_KEY_Z))
            camera.translate(UP, getFrameTime());
        if (mWindow.isKeyPressed(GLFW_KEY_X))
            camera.translate(DOWN, getFrameTime());

        auto mouseMovement = mWindow.getMouseMovement();
        if (!GUIEnabled && mouseMovement.first != 0 && mouseMovement.second != 0.0f)
            camera.rotate(-mouseMovement.first, -mouseMovement.second);

        if (mWindow.isKeyTyped(GLFW_KEY_G))
        {
            mWindow.toggleCursor();
            GUIEnabled = !GUIEnabled;
        }

        View::updateViewSpace();
	}

    void render()
    {
        renderer.draw(getCurrentFrame());
	}

    Raytracer getRenderer() const;
    void setRenderer(const Raytracer &value);
};

//int main()
//{
//    Rays i;
//    i.start();
//    return 0;
//}
