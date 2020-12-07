#ifndef UNTITLED_RENDERER_HPP
#define UNTITLED_RENDERER_HPP

#include "Quad.hpp"
#include "Buffer.hpp"
#include <unordered_set>

class Renderer {
public:
    void render(const std::vector<Quad>& quads, const Buffer& vb);
};

#endif //UNTITLED_RENDERER_HPP
