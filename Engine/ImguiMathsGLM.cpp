#include "ImguiMathsGLM.h"

glm::vec2 ImguiMathsGLM::Vec2(const ImVec2& v)
{
    return glm::vec2(v.x, v.y);
}

ImVec2 ImguiMathsGLM::Vec2(glm::vec2 v)
{
    return ImVec2(v.x, v.y);
}

glm::vec4 ImguiMathsGLM::Vec4(const ImVec4& v)
{
    return glm::vec4(v.x, v.y, v.z, v.w);
}

ImVec4 ImguiMathsGLM::Vec4(glm::vec4 v)
{
    return ImVec4(v.x, v.y, v.z, v.w);
}
