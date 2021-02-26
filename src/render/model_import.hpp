#pragma once

#include "group.hpp"
#include "render.hpp"
#include <string>

namespace Render {

Group importModel(std::string filename, Render&);

TextureHandle importSkybox(std::string filename, Render&);

}