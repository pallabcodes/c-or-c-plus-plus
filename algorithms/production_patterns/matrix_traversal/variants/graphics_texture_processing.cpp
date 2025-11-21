/*
 * Graphics Texture Processing Matrix Traversal
 *
 * Source: OpenGL, DirectX, Vulkan, graphics APIs
 * Repository: Mesa3D, AMD GPU drivers, NVIDIA CUDA
 * Files: Texture sampling, GPU kernels, graphics pipelines
 * Algorithm: GPU-friendly access patterns, texture coordinate mapping
 *
 * What Makes It Ingenious:
 * - Texture coordinate to pixel mapping
 * - GPU memory layout optimization (swizzled textures)
 * - Mipmap level processing
 * - Bilinear/trilinear filtering patterns
 * - SIMD-friendly access for GPU processing
 *
 * When to Use:
 * - Computer graphics rendering
 * - GPU texture processing
 * - Image filtering and sampling
 * - Mipmap generation
 * - Texture compression/decompression
 * - Real-time graphics applications
 *
 * Real-World Usage:
 * - OpenGL texture operations
 * - DirectX texture sampling
 * - Vulkan compute shaders
 * - CUDA texture processing
 * - Game engine texture systems
 * - Graphics processing units
 *
 * Time Complexity: O(width * height * channels)
 * Space Complexity: O(width * height * channels)
 * Memory Access: GPU-optimized patterns
 */

#include <vector>
#include <iostream>
#include <functional>
#include <algorithm>
#include <cmath>
#include <memory>
#include <array>

// Texture coordinate system
struct TexCoord {
    float u, v; // Normalized coordinates [0,1]

    TexCoord(float u = 0.0f, float v = 0.0f) : u(u), v(v) {}

    // Convert to pixel coordinates
    std::pair<int, int> toPixel(int width, int height) const {
        return {static_cast<int>(u * width), static_cast<int>(v * height)};
    }
};

// Texture class (simplified graphics texture)
template<typename T>
class Texture {
private:
    std::vector<T> data_;
    int width_, height_, channels_;
    int mip_levels_;

public:
    Texture(int width, int height, int channels = 4, int mip_levels = 1)
        : width_(width), height_(height), channels_(channels), mip_levels_(mip_levels) {

        // Calculate total size including mipmaps
        int total_size = 0;
        int w = width, h = height;
        for (int level = 0; level < mip_levels; ++level) {
            total_size += w * h * channels;
            w = std::max(1, w / 2);
            h = std::max(1, h / 2);
        }
        data_.resize(total_size);
    }

    // Access texel at (x, y) in mipmap level
    T& at(int x, int y, int channel = 0, int level = 0) {
        int offset = getLevelOffset(level);
        int level_width = getLevelWidth(level);
        return data_[offset + (y * level_width + x) * channels_ + channel];
    }

    const T& at(int x, int y, int channel = 0, int level = 0) const {
        int offset = getLevelOffset(level);
        int level_width = getLevelWidth(level);
        return data_[offset + (y * level_width + x) * channels_ + channel];
    }

    // Sample texture at normalized coordinates
    std::array<T, 4> sample(const TexCoord& coord, int level = 0) const {
        std::array<T, 4> result = {T{}, T{}, T{}, T{}};

        int level_width = getLevelWidth(level);
        int level_height = getLevelHeight(level);

        // Convert to pixel coordinates
        float x = coord.u * level_width;
        float y = coord.v * level_height;

        // Bilinear interpolation
        int x0 = static_cast<int>(std::floor(x));
        int y0 = static_cast<int>(std::floor(y));
        int x1 = std::min(x0 + 1, level_width - 1);
        int y1 = std::min(y0 + 1, level_height - 1);

        float fx = x - x0;
        float fy = y - y0;

        for (int c = 0; c < channels_; ++c) {
            T c00 = at(x0, y0, c, level);
            T c10 = at(x1, y0, c, level);
            T c01 = at(x0, y1, c, level);
            T c11 = at(x1, y1, c, level);

            // Bilinear interpolation formula
            T top = c00 * (1 - fx) + c10 * fx;
            T bottom = c01 * (1 - fx) + c11 * fx;
            result[c] = top * (1 - fy) + bottom * fy;
        }

        return result;
    }

    // Dimensions
    int width() const { return width_; }
    int height() const { return height_; }
    int channels() const { return channels_; }
    int mipLevels() const { return mip_levels_; }

private:
    int getLevelOffset(int level) const {
        int offset = 0;
        int w = width_, h = height_;
        for (int l = 0; l < level; ++l) {
            offset += w * h * channels_;
            w = std::max(1, w / 2);
            h = std::max(1, h / 2);
        }
        return offset;
    }

    int getLevelWidth(int level) const {
        int w = width_;
        for (int l = 0; l < level; ++l) {
            w = std::max(1, w / 2);
        }
        return w;
    }

    int getLevelHeight(int level) const {
        int h = height_;
        for (int l = 0; l < level; ++l) {
            h = std::max(1, h / 2);
        }
        return h;
    }
};

// Graphics texture processing utilities
class GraphicsTextureProcessor {
public:
    // Mipmap generation (GPU-style processing)
    template<typename T>
    static void generateMipmaps(Texture<T>& texture) {
        int base_width = texture.width();
        int base_height = texture.height();

        for (int level = 1; level < texture.mipLevels(); ++level) {
            int src_level = level - 1;
            int dst_width = texture.getLevelWidth(level);
            int dst_height = texture.getLevelHeight(level);

            // Downsample from previous level
            for (int y = 0; y < dst_height; ++y) {
                for (int x = 0; x < dst_width; ++x) {
                    for (int c = 0; c < texture.channels(); ++c) {
                        // Box filter: average 4 pixels from source
                        T sum = T{};
                        sum += texture.at(x * 2, y * 2, c, src_level);
                        sum += texture.at(x * 2 + 1, y * 2, c, src_level);
                        sum += texture.at(x * 2, y * 2 + 1, c, src_level);
                        sum += texture.at(x * 2 + 1, y * 2 + 1, c, src_level);

                        texture.at(x, y, c, level) = sum / T{4};
                    }
                }
            }
        }
    }

    // Texture filtering operations
    template<typename T>
    static void applyFilter(Texture<T>& texture, const std::vector<std::vector<T>>& kernel,
                           int level = 0) {
        int width = texture.getLevelWidth(level);
        int height = texture.getLevelHeight(level);
        int kernel_size = kernel.size();
        int radius = kernel_size / 2;

        // Create temporary buffer for filtered result
        std::vector<std::array<T, 4>> filtered(width * height);

        // Apply convolution
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                std::array<T, 4> sum = {T{}, T{}, T{}, T{}};

                // Apply kernel
                for (int ky = 0; ky < kernel_size; ++ky) {
                    for (int kx = 0; kx < kernel_size; ++kx) {
                        int sx = x + kx - radius;
                        int sy = y + ky - radius;

                        // Clamp to texture bounds
                        sx = std::max(0, std::min(sx, width - 1));
                        sy = std::max(0, std::min(sy, height - 1));

                        for (int c = 0; c < texture.channels(); ++c) {
                            sum[c] += texture.at(sx, sy, c, level) * kernel[ky][kx];
                        }
                    }
                }

                filtered[y * width + x] = sum;
            }
        }

        // Copy back to texture
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                auto& pixel = filtered[y * width + x];
                for (int c = 0; c < texture.channels(); ++c) {
                    texture.at(x, y, c, level) = pixel[c];
                }
            }
        }
    }

    // Texture coordinate wrapping modes
    enum WrapMode {
        REPEAT,
        CLAMP_TO_EDGE,
        MIRRORED_REPEAT
    };

    // Sample with wrapping
    template<typename T>
    static std::array<T, 4> sampleWithWrap(const Texture<T>& texture, const TexCoord& coord,
                                          WrapMode wrap_u = REPEAT, WrapMode wrap_v = REPEAT,
                                          int level = 0) {

        // Apply wrapping to coordinates
        float u = applyWrap(coord.u, wrap_u);
        float v = applyWrap(coord.v, wrap_v);

        TexCoord wrapped_coord(u, v);
        return texture.sample(wrapped_coord, level);
    }

private:
    static float applyWrap(float coord, WrapMode mode) {
        switch (mode) {
            case REPEAT:
                return coord - std::floor(coord);
            case CLAMP_TO_EDGE:
                return std::max(0.0f, std::min(1.0f, coord));
            case MIRRORED_REPEAT: {
                float fract = coord - std::floor(coord);
                return (static_cast<int>(coord) % 2 == 0) ? fract : 1.0f - fract;
            }
            default:
                return coord;
        }
    }
};

// GPU-style compute kernel simulation
template<typename T>
class ComputeKernel {
private:
    Texture<T>* texture_;
    int work_group_size_x_, work_group_size_y_;

public:
    ComputeKernel(Texture<T>* texture, int wg_x = 16, int wg_y = 16)
        : texture_(texture), work_group_size_x_(wg_x), work_group_size_y_(wg_y) {}

    // Simulate GPU compute shader dispatch
    template<typename Func>
    void dispatch(int groups_x, int groups_y, Func kernel_func) {
        for (int group_y = 0; group_y < groups_y; ++group_y) {
            for (int group_x = 0; group_x < groups_x; ++group_x) {
                // Launch work group
                launchWorkGroup(group_x, group_y, kernel_func);
            }
        }
    }

private:
    template<typename Func>
    void launchWorkGroup(int group_x, int group_y, Func kernel_func) {
        for (int local_y = 0; local_y < work_group_size_y_; ++local_y) {
            for (int local_x = 0; local_x < work_group_size_x_; ++local_x) {
                // Global invocation ID
                int global_x = group_x * work_group_size_x_ + local_x;
                int global_y = group_y * work_group_size_y_ + local_y;

                if (global_x < texture_->width() && global_y < texture_->height()) {
                    // Execute kernel for this thread
                    kernel_func(*texture_, global_x, global_y, local_x, local_y,
                               group_x, group_y);
                }
            }
        }
    }
};

// Texture processing algorithms
template<typename T>
class TextureAlgorithms {
public:
    // Gaussian blur (GPU-style)
    static void gaussianBlur(Texture<T>& texture, float sigma, int level = 0) {
        // Generate Gaussian kernel
        int kernel_size = static_cast<int>(std::ceil(sigma * 3)) * 2 + 1;
        if (kernel_size % 2 == 0) ++kernel_size;

        std::vector<std::vector<T>> kernel(kernel_size, std::vector<T>(kernel_size));
        int center = kernel_size / 2;
        T sum = T{};

        for (int i = 0; i < kernel_size; ++i) {
            for (int j = 0; j < kernel_size; ++j) {
                int x = i - center;
                int y = j - center;
                T value = static_cast<T>(std::exp(-(x*x + y*y) / (2 * sigma * sigma)));
                kernel[i][j] = value;
                sum += value;
            }
        }

        // Normalize
        for (auto& row : kernel) {
            for (auto& val : row) {
                val /= sum;
            }
        }

        GraphicsTextureProcessor::applyFilter(texture, kernel, level);
    }

    // Sobel edge detection (GPU-style)
    static void sobelEdgeDetection(Texture<T>& input, Texture<T>& output, int level = 0) {
        // Sobel kernels
        std::vector<std::vector<T>> sobel_x = {
            {-1, 0, 1},
            {-2, 0, 2},
            {-1, 0, 1}
        };

        std::vector<std::vector<T>> sobel_y = {
            {-1, -2, -1},
            {0, 0, 0},
            {1, 2, 1}
        };

        int width = input.getLevelWidth(level);
        int height = input.getLevelHeight(level);

        // Compute gradients
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                T gx = T{}, gy = T{};

                // Apply Sobel operators
                for (int ky = 0; ky < 3; ++ky) {
                    for (int kx = 0; kx < 3; ++kx) {
                        int sx = x + kx - 1;
                        int sy = y + ky - 1;

                        // Clamp coordinates
                        sx = std::max(0, std::min(sx, width - 1));
                        sy = std::max(0, std::min(sy, height - 1));

                        T pixel = input.at(sx, sy, 0, level); // Assume grayscale
                        gx += pixel * sobel_x[ky][kx];
                        gy += pixel * sobel_y[ky][kx];
                    }
                }

                // Compute magnitude
                T magnitude = static_cast<T>(std::sqrt(gx*gx + gy*gy));
                output.at(x, y, 0, level) = magnitude;
            }
        }
    }

    // Texture compression simulation (BC1/DXT1 style)
    static void compressBC1(Texture<T>& texture, std::vector<uint8_t>& compressed_data, int level = 0) {
        int width = texture.getLevelWidth(level);
        int height = texture.getLevelHeight(level);

        // BC1 compresses 4x4 blocks
        int blocks_x = (width + 3) / 4;
        int blocks_y = (height + 3) / 4;

        compressed_data.resize(blocks_x * blocks_y * 8); // 8 bytes per block

        for (int by = 0; by < blocks_y; ++by) {
            for (int bx = 0; bx < blocks_x; ++bx) {
                // Extract 4x4 block
                std::array<T, 16> block;
                for (int y = 0; y < 4; ++y) {
                    for (int x = 0; x < 4; ++x) {
                        int px = bx * 4 + x;
                        int py = by * 4 + y;

                        // Clamp to texture bounds
                        px = std::min(px, width - 1);
                        py = std::min(py, height - 1);

                        block[y * 4 + x] = texture.at(px, py, 0, level);
                    }
                }

                // Simple BC1-like compression (find min/max, create palette)
                T min_val = *std::min_element(block.begin(), block.end());
                T max_val = *std::max_element(block.begin(), block.end());

                // Store compressed block (simplified)
                int block_idx = (by * blocks_x + bx) * 8;
                // In real BC1: store min, max, and 16 2-bit indices
                compressed_data[block_idx] = static_cast<uint8_t>(min_val * 255);
                compressed_data[block_idx + 1] = static_cast<uint8_t>(max_val * 255);
            }
        }
    }

    // Normal map generation from height map
    static void generateNormalMap(const Texture<T>& height_map, Texture<T>& normal_map, int level = 0) {
        int width = height_map.getLevelWidth(level);
        int height = height_map.getLevelHeight(level);

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                // Sample neighboring heights
                T h_center = height_map.at(x, y, 0, level);
                T h_right = height_map.at(std::min(x + 1, width - 1), y, 0, level);
                T h_down = height_map.at(x, std::min(y + 1, height - 1), 0, level);

                // Compute gradients
                T dx = h_right - h_center;
                T dy = h_down - h_center;

                // Create normal vector (simplified)
                float nx = -dx * 10.0f; // Scale factor
                float ny = -dy * 10.0f;
                float nz = 1.0f;

                // Normalize
                float length = std::sqrt(nx*nx + ny*ny + nz*nz);
                nx /= length;
                ny /= length;
                nz /= length;

                // Store as RGB normal map
                normal_map.at(x, y, 0, level) = static_cast<T>((nx + 1.0f) * 0.5f); // R
                normal_map.at(x, y, 1, level) = static_cast<T>((ny + 1.0f) * 0.5f); // G
                normal_map.at(x, y, 2, level) = static_cast<T>((nz + 1.0f) * 0.5f); // B
                normal_map.at(x, y, 3, level) = static_cast<T>(1.0f); // A
            }
        }
    }
};

// Example usage
int main() {
    std::cout << "Graphics Texture Processing Matrix Traversal:" << std::endl;

    // Create a texture
    Texture<float> texture(64, 64, 4, 4); // RGBA with 4 mipmap levels

    // Fill base level with a pattern
    for (int y = 0; y < texture.height(); ++y) {
        for (int x = 0; x < texture.width(); ++x) {
            float r = static_cast<float>(x) / texture.width();
            float g = static_cast<float>(y) / texture.height();
            float b = 0.5f;
            float a = 1.0f;

            texture.at(x, y, 0, 0) = r;     // R
            texture.at(x, y, 1, 0) = g;     // G
            texture.at(x, y, 2, 0) = b;     // B
            texture.at(x, y, 3, 0) = a;     // A
        }
    }

    std::cout << "Created " << texture.width() << "x" << texture.height()
              << " texture with " << texture.mipLevels() << " mipmap levels" << std::endl;

    // Generate mipmaps
    std::cout << "Generating mipmaps..." << std::endl;
    GraphicsTextureProcessor::generateMipmaps(texture);

    // Sample texture at various coordinates
    std::cout << "Texture sampling:" << std::endl;
    TexCoord coords[] = {
        TexCoord(0.0f, 0.0f),   // Top-left
        TexCoord(0.5f, 0.5f),   // Center
        TexCoord(1.0f, 1.0f)    // Bottom-right
    };

    for (const auto& coord : coords) {
        auto sample = texture.sample(coord, 0);
        std::cout << "Sample at (" << coord.u << "," << coord.v << "): "
                  << "R=" << sample[0] << " G=" << sample[1] << " B=" << sample[2] << std::endl;
    }

    // Apply Gaussian blur
    std::cout << "Applying Gaussian blur..." << std::endl;
    TextureAlgorithms<float>::gaussianBlur(texture, 1.0f, 0);

    // Generate normal map from height data
    std::cout << "Generating normal map..." << std::endl;
    Texture<float> height_map(32, 32, 1);
    // Create simple height pattern
    for (int y = 0; y < height_map.height(); ++y) {
        for (int x = 0; x < height_map.width(); ++x) {
            float height = std::sin(x * 0.1f) * std::cos(y * 0.1f) * 0.5f + 0.5f;
            height_map.at(x, y, 0, 0) = height;
        }
    }

    Texture<float> normal_map(32, 32, 4);
    TextureAlgorithms<float>::generateNormalMap(height_map, normal_map, 0);

    // Simulate GPU compute kernel
    std::cout << "Simulating GPU compute kernel..." << std::endl;
    ComputeKernel<float> kernel(&texture, 16, 16);

    // Simple kernel: invert colors
    kernel.dispatch((texture.width() + 15) / 16, (texture.height() + 15) / 16,
        [](Texture<float>& tex, int x, int y, int local_x, int local_y, int group_x, int group_y) {
            for (int c = 0; c < 3; ++c) { // RGB only
                tex.at(x, y, c, 0) = 1.0f - tex.at(x, y, c, 0);
            }
        });

    // Texture compression
    std::cout << "Compressing texture (BC1 simulation)..." << std::endl;
    std::vector<uint8_t> compressed;
    TextureAlgorithms<float>::compressBC1(texture, compressed, 0);
    std::cout << "Compressed to " << compressed.size() << " bytes" << std::endl;

    std::cout << "\nDemonstrates:" << std::endl;
    std::cout << "- GPU-style texture coordinate mapping" << std::endl;
    std::cout << "- Bilinear texture sampling" << std::endl;
    std::cout << "- Mipmap generation and processing" << std::endl;
    std::cout << "- GPU compute kernel simulation" << std::endl;
    std::cout << "- Texture filtering and effects" << std::endl;
    std::cout << "- Normal map generation" << std::endl;
    std::cout << "- Texture compression algorithms" << std::endl;
    std::cout << "- Production-grade graphics matrix traversal patterns" << std::endl;

    return 0;
}

