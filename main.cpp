#include <gl\glew.h>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>
#include <vector>

#include "Engine.h"
#include "Render.h"
#include "utils.h"

glm::vec2 SCREEN_SIZE(1024, 768);

struct Scene
{
	void AddCube(glm::vec3 pos, GLuint glProgram)
	{
		Render::RenderObject* ro = CreateCubeRO(1.0f, glProgram);
		ro->frame.modelMatrix = glm::translate(glm::mat4(1.0f), pos);
		AddRenderObject(ro);
	}

	void AddRenderObject(Render::RenderObject* ro)
	{
		objects.push_back(ro);
	}

	std::vector<Render::RenderObject*>& renderObjects() { return objects; }

private:
	std::vector<Render::RenderObject*> objects;
};

int main(int argc, char* args[])
{
	Engine eng("Engine", SCREEN_SIZE.x, SCREEN_SIZE.y);

	Scene scene;
	Camera cam;

	const float aspectRatio = SCREEN_SIZE.x / SCREEN_SIZE.y;
	cam.projMat = glm::perspective(45.0f, aspectRatio, 1.0f, 10.0f);
	cam.viewMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -7.0f));
	
	GLuint glTexture = CreateTexture("texture.tga");
	GLuint glProgram = CreateProgram();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, glGetUniformLocation(glProgram, "colorTexture"));

	scene.AddCube(glm::vec3(-2.0f, 0.0f, 0.0f), glProgram);
	scene.AddCube(glm::vec3(2.0f, 0.0f, 0.0f), glProgram);

	while (eng.Run())
	{
		Render::Clear(Render::ClearFlags::Color | Render::ClearFlags::Depth);

		Render::RenderAll(cam, scene.renderObjects());

		eng.SwapBuffers();
	}

	return 0;
}