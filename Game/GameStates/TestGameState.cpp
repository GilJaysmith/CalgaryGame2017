#include "Game/Pch.h"

#include "Game/GameStates/TestGameState.h"

TestGameState::TestGameState()
{
}

TestGameState::~TestGameState()
{

}

void TestGameState::OnEnter()
{
	Mesh* mesh = MemNew(MemoryPool::Rendering, Mesh);
	mesh->LoadFromYaml("cube");

	shader_program = mesh->GetShaderProgram();

	renderable = MemNew(MemoryPool::Rendering, Renderable)(mesh);
	renderable2 = MemNew(MemoryPool::Rendering, Renderable)(mesh);

	Renderer::RegisterRenderable(renderable);
	Renderer::RegisterRenderable(renderable2);

	glUseProgram(shader_program);

	glm::mat4 view = glm::lookAt(
		glm::vec3(1.2f, 1.2f, 1.2f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f)
	);
	GLint uniView = glGetUniformLocation(shader_program, "view");
	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 1.0f, 10.0f);
	GLint uniProj = glGetUniformLocation(shader_program, "proj");
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

	time = 0.0f;
	rot = 0;

	update_time = false;
	update_rotation = false;
}

void TestGameState::OnUpdate(const Time& frame_time)
{
	GameState::OnUpdate(frame_time);

	GLint uniTime = glGetUniformLocation(shader_program, "time");
	if (update_time)
	{
		time += 0.01f;
		glUniform1f(uniTime, time);
	}

	if (update_rotation)
	{
		rot = (rot + 1) % 360;
	}

	glm::mat4 world_transform;
	world_transform = glm::rotate(world_transform, glm::radians(float(rot)), glm::vec3(0.0f, 0.0f, 1.0f));

	glm::mat4 world_transform2;
	world_transform2 = glm::translate(world_transform2, glm::vec3(0.0f, 1.0f, 0.0f));
	world_transform2 = glm::rotate(world_transform2, glm::radians(float(-rot)), glm::vec3(0.0f, 0.0f, 1.0f));

	renderable->SetTransform(world_transform);
	renderable2->SetTransform(world_transform2);

	if (Input::GetKeyEvent(GLFW_KEY_T) == Input::PRESSED)
	{
		update_time = !update_time;
	}

	if (Input::GetKeyEvent(GLFW_KEY_R) == Input::PRESSED)
	{
		update_rotation = !update_rotation;
	}
}

void TestGameState::OnExit()
{

}
