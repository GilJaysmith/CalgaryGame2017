#pragma once

#include <cstddef>
#include <list>
#include <map>
#include <string>
#include <vector>

#include <Windows.h>
#undef CreateWindow

#define GLEW_STATIC
#include "../sdks/glew/include/GL/glew.h"

#include "../sdks/glfw/include/GLFW/glfw3.h"
#include "../sdks/soil/include/SOIL.h"

#include "../sdks/glm/glm.hpp"
#include "../sdks/glm/gtc/matrix_transform.hpp"
#include "../sdks/glm/gtc/type_ptr.hpp"
