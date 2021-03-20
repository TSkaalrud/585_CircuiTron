#pragma once

#include "model.hpp"

namespace Render {
using Group [[deprecated("Use Model")]] = Model;
using GroupInstance [[deprecated("Use ModelInstance")]] = ModelInstance;
} // namespace Render
