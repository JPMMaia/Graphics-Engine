#pragma once

#ifdef GRAPHICS_ENGINE_EXPORT
#define GRAPHICS_ENGINE_API __declspec(dllexport)
#else
#define GRAPHICS_ENGINE_API
#endif