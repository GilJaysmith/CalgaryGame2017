#include <Windows.h>

#define GLEW_STATIC
#include "sdks/glew/include/GL/glew.h"

#include "sdks/glfw/include/GLFW/glfw3.h"
#include "sdks/soil/include/SOIL.h"

#include "sdks/glm/glm.hpp"
#include "sdks/glm/gtc/matrix_transform.hpp"
#include "sdks/glm/gtc/type_ptr.hpp"

#include "Engine/Memory/Memory.h"
#include "Engine/Rendering/Mesh.h"
#include "Engine/Physics/Physics.h"
#include "Engine/Rendering/Renderable.h"
#include "Engine/Rendering/Renderer.h"
#include "Engine/Rendering/ShaderManager.h"
#include "Engine/Rendering/TextureManager.h"
#include "Engine/Input/Input.h"
