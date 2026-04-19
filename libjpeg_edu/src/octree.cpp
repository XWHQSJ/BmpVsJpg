#include "libjpeg_edu/octree.h"

#include <algorithm>
#include <cstdlib>
#include <memory>
#include <stdexcept>

namespace libjpeg_edu {

namespace {

struct Node {
    bool is_leaf = false;
    unsigned pixel_count = 0;
    unsigned red_sum = 0;
    unsigned green_sum = 0;
    unsigned blue_sum = 0;
    Node* child[8] = {};
    Node* next = nullptr;
};

// Pool allocator to avoid per-node heap calls
struct NodePool {
    std::vector<std::unique_ptr<Node[]>> blocks;
    size_t block_size = 4096;
    size_t offset = 0;

    Node* alloc() {
        if (blocks.empty() || offset >= block_size) {
            blocks.push_back(std::make_unique<Node[]>(block_size));
            offset = 0;
        }
        return &blocks.back()[offset++];
    }
};

static const uint8_t kMask[8] = {0x80, 0x40, 0x20, 0x10,
                                  0x08, 0x04, 0x02, 0x01};

Node* create_node(unsigned level, unsigned color_bits,
                  unsigned& leaf_count, Node** reducible,
                  NodePool& pool) {
    Node* n = pool.alloc();
    n->is_leaf = (level == color_bits);
    if (n->is_leaf) {
        ++leaf_count;
    } else {
        n->next = reducible[level];
        reducible[level] = n;
    }
    return n;
}

void add_color(Node** pp, uint8_t r, uint8_t g, uint8_t b,
               unsigned color_bits, unsigned level,
               unsigned& leaf_count, Node** reducible,
               NodePool& pool) {
    if (*pp == nullptr) {
        *pp = create_node(level, color_bits, leaf_count, reducible, pool);
    }
    if ((*pp)->is_leaf) {
        (*pp)->pixel_count++;
        (*pp)->red_sum += r;
        (*pp)->green_sum += g;
        (*pp)->blue_sum += b;
        return;
    }
    int shift = 7 - static_cast<int>(level);
    int idx = (((r & kMask[level]) >> shift) << 2) |
              (((g & kMask[level]) >> shift) << 1) |
              ((b & kMask[level]) >> shift);
    add_color(&((*pp)->child[idx]), r, g, b,
              color_bits, level + 1, leaf_count, reducible, pool);
}

void reduce_tree(unsigned color_bits, unsigned& leaf_count,
                 Node** reducible) {
    int i = static_cast<int>(color_bits) - 1;
    while (i > 0 && reducible[i] == nullptr) --i;

    Node* node = reducible[i];
    reducible[i] = node->next;

    unsigned children = 0;
    unsigned rs = 0, gs = 0, bs = 0;
    for (int c = 0; c < 8; ++c) {
        if (node->child[c]) {
            rs += node->child[c]->red_sum;
            gs += node->child[c]->green_sum;
            bs += node->child[c]->blue_sum;
            node->pixel_count += node->child[c]->pixel_count;
            node->child[c] = nullptr; // pool owns memory
            ++children;
        }
    }
    node->is_leaf = true;
    node->red_sum = rs;
    node->green_sum = gs;
    node->blue_sum = bs;
    leaf_count -= (children - 1);
}

void collect_colors(const Node* tree, std::vector<uint32_t>& palette) {
    if (tree == nullptr) return;
    if (tree->is_leaf) {
        uint8_t r = static_cast<uint8_t>(tree->red_sum / tree->pixel_count);
        uint8_t g = static_cast<uint8_t>(tree->green_sum / tree->pixel_count);
        uint8_t b = static_cast<uint8_t>(tree->blue_sum / tree->pixel_count);
        palette.push_back((static_cast<uint32_t>(r) << 16) |
                          (static_cast<uint32_t>(g) << 8) |
                          static_cast<uint32_t>(b));
        return;
    }
    for (int i = 0; i < 8; ++i) {
        collect_colors(tree->child[i], palette);
    }
}

} // namespace

std::vector<uint32_t> octree_palette(const ImageBuffer& img,
                                     unsigned max_colors,
                                     unsigned color_bits) {
    if (img.empty() || img.channels != 3) {
        throw std::runtime_error("octree_palette requires a non-empty RGB image");
    }
    if (color_bits > 8) color_bits = 8;
    if (max_colors < 2) max_colors = 2;

    NodePool pool;
    Node* tree = nullptr;
    unsigned leaf_count = 0;
    Node* reducible[9] = {};

    const uint8_t* px = img.data.data();
    size_t npx = img.pixel_count();
    for (size_t i = 0; i < npx; ++i) {
        uint8_t r = px[i * 3 + 0];
        uint8_t g = px[i * 3 + 1];
        uint8_t b = px[i * 3 + 2];
        add_color(&tree, r, g, b, color_bits, 0, leaf_count, reducible, pool);
        while (leaf_count > max_colors) {
            reduce_tree(color_bits, leaf_count, reducible);
        }
    }

    std::vector<uint32_t> palette;
    palette.reserve(leaf_count);
    collect_colors(tree, palette);
    return palette;
}

ImageBuffer apply_palette(const ImageBuffer& img,
                          const std::vector<uint32_t>& palette) {
    if (img.empty() || img.channels != 3 || palette.empty()) {
        throw std::runtime_error("apply_palette: invalid input");
    }

    ImageBuffer out;
    out.width = img.width;
    out.height = img.height;
    out.channels = 3;
    out.data.resize(img.data.size());

    size_t npx = img.pixel_count();
    for (size_t i = 0; i < npx; ++i) {
        uint8_t r = img.data[i * 3 + 0];
        uint8_t g = img.data[i * 3 + 1];
        uint8_t b = img.data[i * 3 + 2];

        int best_dist = INT32_MAX;
        uint32_t best = palette[0];
        for (uint32_t c : palette) {
            int dr = r - static_cast<int>((c >> 16) & 0xFF);
            int dg = g - static_cast<int>((c >> 8) & 0xFF);
            int db = b - static_cast<int>(c & 0xFF);
            int dist = dr * dr + dg * dg + db * db;
            if (dist < best_dist) {
                best_dist = dist;
                best = c;
            }
        }
        out.data[i * 3 + 0] = static_cast<uint8_t>((best >> 16) & 0xFF);
        out.data[i * 3 + 1] = static_cast<uint8_t>((best >> 8) & 0xFF);
        out.data[i * 3 + 2] = static_cast<uint8_t>(best & 0xFF);
    }
    return out;
}

} // namespace libjpeg_edu
