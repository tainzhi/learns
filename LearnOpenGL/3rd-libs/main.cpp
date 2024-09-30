#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <filesystem.h>
#include <model.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


int main()
{    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);
    Model ourModel(FileSystem::getPath("resources/objects/backpack/backpack.obj"));
}