#pragma once

#include "model.hpp"
#include "render.hpp"
#include <string>

namespace Render {

Model importModel(std::string filename, Render&);

TextureHandle importSkybox(std::string filename, Render&);

}