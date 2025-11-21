/*
 * Huffman Coding Compression Algorithm
 *
 * Source: Data compression libraries (zlib, gzip, JPEG)
 * Algorithm: Greedy frequency-based optimal prefix coding
 * Paper: "A Method for the Construction of Minimum-Redundancy Codes" by Huffman (1952)
 *
 * What Makes It Ingenious:
 * - Frequency analysis: Count symbol occurrences
 * - Greedy tree construction: Always combine least frequent symbols
 * - Prefix-free codes: No code is prefix of another
 * - Optimal compression: Mathematically proven minimal average code length
 * - Used in all major compression formats (ZIP, GZIP, JPEG, MP3)
 *
 * When to Use:
 * - Lossless data compression
 * - Frequency-based data (text, images, audio)
 * - Entropy coding in multimedia formats
 * - Protocol compression (HTTP/2, WebP)
 * - Archive formats (ZIP, 7z)
 *
 * Real-World Usage:
 * - ZIP and GZIP compression
 * - JPEG image entropy coding
 * - MP3 audio compression
 * - PNG image compression
 * - WebP image format
 * - Protocol buffers compression
 * - Database compression
 *
 * Time Complexity: O(n log n) for n distinct symbols
 * Space Complexity: O(n) for tree, O(n) for codes
 */

#include <vector>
#include <queue>
#include <unordered_map>
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <bitset>

// Huffman tree node
struct HuffmanNode {
    char symbol;
    int frequency;
    std::shared_ptr<HuffmanNode> left;
    std::shared_ptr<HuffmanNode> right;

    HuffmanNode(char sym, int freq)
        : symbol(sym), frequency(freq), left(nullptr), right(nullptr) {}

    HuffmanNode(std::shared_ptr<HuffmanNode> l, std::shared_ptr<HuffmanNode> r)
        : symbol('\0'), frequency(l->frequency + r->frequency), left(l), right(r) {}

    bool is_leaf() const {
        return left == nullptr && right == nullptr;
    }
};

// Custom comparator for priority queue
struct CompareNodes {
    bool operator()(const std::shared_ptr<HuffmanNode>& a,
                   const std::shared_ptr<HuffmanNode>& b) const {
        return a->frequency > b->frequency; // Min-heap
    }
};

// Huffman coding implementation
class HuffmanCoder {
private:
    std::shared_ptr<HuffmanNode> root_;
    std::unordered_map<char, std::string> codes_;
    std::unordered_map<std::string, char> decode_map_;

    // Build Huffman tree using greedy algorithm
    void build_tree(const std::unordered_map<char, int>& frequencies) {
        // Priority queue for greedy selection
        std::priority_queue<std::shared_ptr<HuffmanNode>,
                          std::vector<std::shared_ptr<HuffmanNode>>,
                          CompareNodes> pq;

        // Create leaf nodes for each symbol
        for (const auto& pair : frequencies) {
            pq.push(std::make_shared<HuffmanNode>(pair.first, pair.second));
        }

        // Build tree by repeatedly combining least frequent nodes
        while (pq.size() > 1) {
            auto left = pq.top(); pq.pop();
            auto right = pq.top(); pq.pop();

            // Create internal node
            auto internal = std::make_shared<HuffmanNode>(left, right);
            pq.push(internal);
        }

        root_ = pq.top();
        build_codes(root_, "");
    }

    // Recursively build codes from tree
    void build_codes(std::shared_ptr<HuffmanNode> node, std::string code) {
        if (!node) return;

        if (node->is_leaf()) {
            codes_[node->symbol] = code;
            decode_map_[code] = node->symbol;
            return;
        }

        build_codes(node->left, code + "0");
        build_codes(node->right, code + "1");
    }

    // Calculate frequency of each symbol in text
    std::unordered_map<char, int> calculate_frequencies(const std::string& text) {
        std::unordered_map<char, int> freq;
        for (char c : text) {
            freq[c]++;
        }
        return freq;
    }

    // Convert string of bits to bytes
    std::vector<uint8_t> bits_to_bytes(const std::string& bits) {
        std::vector<uint8_t> bytes;

        for (size_t i = 0; i < bits.size(); i += 8) {
            std::string byte_str = bits.substr(i, 8);
            if (byte_str.size() < 8) {
                // Pad with zeros
                byte_str += std::string(8 - byte_str.size(), '0');
            }

            uint8_t byte = 0;
            for (size_t j = 0; j < 8; ++j) {
                if (byte_str[j] == '1') {
                    byte |= (1 << (7 - j));
                }
            }
            bytes.push_back(byte);
        }

        return bytes;
    }

    // Convert bytes to string of bits
    std::string bytes_to_bits(const std::vector<uint8_t>& bytes) {
        std::string bits;
        for (uint8_t byte : bytes) {
            for (int i = 7; i >= 0; --i) {
                bits += (byte & (1 << i)) ? '1' : '0';
            }
        }
        return bits;
    }

public:
    // Build Huffman codes from text
    void build_from_text(const std::string& text) {
        auto frequencies = calculate_frequencies(text);
        if (frequencies.empty()) return;

        build_tree(frequencies);
    }

    // Build Huffman codes from frequency map
    void build_from_frequencies(const std::unordered_map<char, int>& frequencies) {
        if (frequencies.empty()) return;
        build_tree(frequencies);
    }

    // Encode text to compressed bit string
    std::string encode(const std::string& text) const {
        std::string encoded;
        for (char c : text) {
            auto it = codes_.find(c);
            if (it != codes_.end()) {
                encoded += it->second;
            } else {
                // Symbol not in codes (shouldn't happen with proper build)
                encoded += "00000000"; // 8-bit fallback
            }
        }
        return encoded;
    }

    // Decode bit string back to text
    std::string decode(const std::string& encoded_bits) const {
        std::string decoded;
        std::string current_code;

        for (char bit : encoded_bits) {
            current_code += bit;

            auto it = decode_map_.find(current_code);
            if (it != decode_map_.end()) {
                decoded += it->second;
                current_code.clear();
            }
        }

        return decoded;
    }

    // Compress text to bytes
    std::vector<uint8_t> compress(const std::string& text) {
        std::string encoded = encode(text);
        return bits_to_bytes(encoded);
    }

    // Decompress bytes to text
    std::string decompress(const std::vector<uint8_t>& compressed) {
        std::string bits = bytes_to_bits(compressed);
        return decode(bits);
    }

    // Get Huffman codes
    const std::unordered_map<char, std::string>& get_codes() const {
        return codes_;
    }

    // Print Huffman codes
    void print_codes() const {
        std::cout << "Huffman Codes:" << std::endl;
        for (const auto& pair : codes_) {
            std::cout << "'" << (pair.first == '\n' ? "\\n" :
                                pair.first == '\t' ? "\\t" :
                                pair.first == ' ' ? "space" :
                                std::string(1, pair.first))
                      << "' : " << pair.second << std::endl;
        }
    }

    // Calculate compression statistics
    void analyze_compression(const std::string& original) const {
        size_t original_bits = original.size() * 8;
        size_t compressed_bits = 0;

        for (char c : original) {
            auto it = codes_.find(c);
            if (it != codes_.end()) {
                compressed_bits += it->second.size();
            }
        }

        double compression_ratio = static_cast<double>(compressed_bits) / original_bits;

        std::cout << "Compression Analysis:" << std::endl;
        std::cout << "  Original size: " << original_bits << " bits" << std::endl;
        std::cout << "  Compressed size: " << compressed_bits << " bits" << std::endl;
        std::cout << "  Compression ratio: " << compression_ratio << std::endl;
        std::cout << "  Space saved: " << (1.0 - compression_ratio) * 100 << "%" << std::endl;

        // Calculate entropy (theoretical minimum)
        auto frequencies = calculate_frequencies(original);
        double entropy = 0.0;
        double total_chars = original.size();

        for (const auto& pair : frequencies) {
            double prob = pair.second / total_chars;
            entropy -= prob * log2(prob);
        }

        std::cout << "  Theoretical entropy: " << entropy << " bits/symbol" << std::endl;
        std::cout << "  Huffman efficiency: " << (entropy / (compressed_bits / total_chars)) * 100 << "%" << std::endl;
    }

private:
    // Calculate frequencies (moved to public in full implementation)
    std::unordered_map<char, int> calculate_frequencies(const std::string& text) const {
        std::unordered_map<char, int> freq;
        for (char c : text) {
            freq[c]++;
        }
        return freq;
    }
};

// Adaptive Huffman coding (updates tree as it encodes)
class AdaptiveHuffmanCoder {
private:
    struct AdaptiveNode {
        char symbol;
        int weight;
        int number;  // Node number for NYT (Not Yet Transmitted) handling
        std::shared_ptr<AdaptiveNode> parent;
        std::shared_ptr<AdaptiveNode> left;
        std::shared_ptr<AdaptiveNode> right;

        AdaptiveNode(char sym = '\0', int num = -1)
            : symbol(sym), weight(0), number(num),
              parent(nullptr), left(nullptr), right(nullptr) {}

        bool is_leaf() const { return left == nullptr && right == nullptr; }
        bool is_nyt() const { return symbol == '\0' && number != -1; }
    };

    std::shared_ptr<AdaptiveNode> root_;
    std::unordered_map<char, std::shared_ptr<AdaptiveNode>> symbol_nodes_;
    std::shared_ptr<AdaptiveNode> nyt_node_;
    int next_number_;

    void update_tree(std::shared_ptr<AdaptiveNode> node) {
        while (node) {
            // Find highest numbered node with same weight
            std::shared_ptr<AdaptiveNode> swap_node = find_swap_node(node);
            if (swap_node && swap_node != node) {
                swap_nodes(node, swap_node);
                node = swap_node; // Continue with swapped node
            }

            node->weight++;
            node = node->parent;
        }
    }

    std::shared_ptr<AdaptiveNode> find_swap_node(std::shared_ptr<AdaptiveNode> node) {
        // Find the highest numbered node with same weight (simplified)
        return nullptr; // Placeholder for full implementation
    }

    void swap_nodes(std::shared_ptr<AdaptiveNode> a, std::shared_ptr<AdaptiveNode> b) {
        // Swap node positions in tree (complex operation)
        // Full implementation would swap parent/child pointers
    }

public:
    AdaptiveHuffmanCoder() : next_number_(0) {
        nyt_node_ = std::make_shared<AdaptiveNode>('\0', next_number_++);
        root_ = nyt_node_;
    }

    // Adaptive encoding (tree updates as we encode)
    std::string encode_symbol(char symbol) {
        std::string code;

        if (symbol_nodes_.find(symbol) != symbol_nodes_.end()) {
            // Symbol exists, encode path
            code = get_code(symbol_nodes_[symbol]);
        } else {
            // Symbol is NYT, encode NYT path + symbol bits
            code = get_code(nyt_node_) + std::bitset<8>(symbol).to_string();
            // Add new node for symbol
            add_new_symbol(symbol);
        }

        update_tree(symbol_nodes_[symbol]);
        return code;
    }

    std::string get_code(std::shared_ptr<AdaptiveNode> node) {
        std::string code;
        auto current = node;
        while (current->parent) {
            if (current == current->parent->left) {
                code = "0" + code;
            } else {
                code = "1" + code;
            }
            current = current->parent;
        }
        return code;
    }

    void add_new_symbol(char symbol) {
        // Create new internal node and leaf node
        // This is a simplified version
        auto new_leaf = std::make_shared<AdaptiveNode>(symbol, next_number_++);
        symbol_nodes_[symbol] = new_leaf;
    }
};

// Example usage
int main() {
    std::cout << "Huffman Coding Compression Demonstration:" << std::endl;

    // Test text with varying frequencies
    std::string text = "this is an example of a huffman tree for compression. "
                      "huffman coding uses a greedy algorithm to build optimal prefix codes. "
                      "the algorithm works by repeatedly combining the two least frequent symbols.";

    std::cout << "Original text (" << text.size() << " characters):" << std::endl;
    std::cout << text.substr(0, 100) << "..." << std::endl;

    // Build Huffman codes
    HuffmanCoder coder;
    coder.build_from_text(text);

    // Print codes
    coder.print_codes();

    // Encode and decode
    std::string encoded = coder.encode(text);
    std::string decoded = coder.decode(encoded);

    std::cout << "\nEncoding/Decoding test:" << std::endl;
    std::cout << "Original length: " << text.size() << " characters" << std::endl;
    std::cout << "Encoded length: " << encoded.size() << " bits" << std::endl;
    std::cout << "Compression ratio: " << (encoded.size() / 8.0) / text.size() << std::endl;

    // Verify correctness
    bool correct = (text == decoded);
    std::cout << "Decoding correct: " << (correct ? "YES" : "NO") << std::endl;

    // Analyze compression
    coder.analyze_compression(text);

    std::cout << "\nHuffman coding demonstrates:" << std::endl;
    std::cout << "- Greedy algorithm for optimal prefix codes" << std::endl;
    std::cout << "- Frequency analysis and priority queue usage" << std::endl;
    std::cout << "- Mathematical optimality proof" << std::endl;
    std::cout << "- Used in all major compression formats" << std::endl;
    std::cout << "- Prefix-free property prevents ambiguity" << std::endl;

    return 0;
}

