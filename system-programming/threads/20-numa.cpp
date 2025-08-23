#include <numa.h>
#include <iostream>
#include <vector>

int main() {
    if (numa_available() == -1) {
        std::cout << "NUMA not available on this system.\n";
        return 1;
    }
    int nodes = numa_max_node() + 1;
    std::cout << "NUMA nodes: " << nodes << std::endl;
    std::vector<int*> mem(nodes);
    for (int i = 0; i < nodes; ++i) {
        mem[i] = (int*)numa_alloc_onnode(1024 * sizeof(int), i);
        mem[i][0] = i;
        std::cout << "Allocated memory on node " << i << " value: " << mem[i][0] << std::endl;
        numa_free(mem[i], 1024 * sizeof(int));
    }
    return 0;
}