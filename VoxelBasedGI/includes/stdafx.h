/********** Standard Libraries **********/
#include <vector>
#include <memory>
#include <iostream>
#include <algorithm>
#include <cstdio>

/********** Application Header **********/
#include "Defs.h"
#include "VertexStructs.h"
#include "Mesh3D.h"
#include "HighResClock.h"
#include "WindowManager.h"
#include "GeometryBatch.h"
#include "VertexAttribPointer.h"
#include "RenderBatch.h"
#include "Material.h"
#include "MaterialUtilities.h"
#include "GeometryManager.h"
#include "Scene.h"
#include "Camera.h"
#include "DataHolder.h"
#include "LightManager.h"
#include "TextureManager.h"
#include "InputSignals.h"
#include "ApplicationEvents.h"
#include "Object.h"
#include "InputManager.h"

/********** Math Library **********/

#include <glm/matrix.hpp>
#include "glm/vec4.hpp"
#include "glm/vec3.hpp"
#include "glm/vec2.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/matrix_transform.hpp"

/********** OpenGL **********/
#include "glad/glad.h"
#include <GLFW/glfw3.h>
