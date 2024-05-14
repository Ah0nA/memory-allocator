#include <memory>
#include <iostream>
#include <vector>

class SimpleAllocator
{
private:
    struct Node
    {
        size_t size;
        bool isFree;
        Node *next;
    };
    static const size_t BLOCK_SIZE = 4096;
    std::vector<char *> blocks;
    Node *head = nullptr;

public:
    SimpleAllocator(){};
    ~SimpleAllocator()
    {
        for (char *block : blocks)
        {
            delete[] block;
        }
    }
void* allocator(size_t size) {
        const size_t alignment = sizeof(void*);
        const size_t alignedSize = (size + alignment - 1) & ~(alignment - 1);

        if (!head) {
            std::cout << "Initializing first block." << std::endl;
            blocks.push_back(new char[BLOCK_SIZE]);
            head = reinterpret_cast<Node*>(blocks.back());
            head->size = BLOCK_SIZE - sizeof(Node);
            head->isFree = true;
            head->next = nullptr;
        }

        Node* current = head;

        while (current) {
            if (current->isFree && current->size >= alignedSize) {
                std::cout << "Found suitable block: " << current << " with size: " << current->size << std::endl;
                current->isFree = false;
                if (current->size > alignedSize + sizeof(Node)) {
                    Node* splitBlock = reinterpret_cast<Node*>(reinterpret_cast<char*>(current) + sizeof(Node) + alignedSize);
                    splitBlock->size = current->size - alignedSize - sizeof(Node);
                    splitBlock->isFree = true;
                    splitBlock->next = current->next;
                    current->size = alignedSize;
                    current->next = splitBlock;
                    std::cout << "Splitting block. New block: " << splitBlock << " with size: " << splitBlock->size << std::endl;
                }
                return reinterpret_cast<void*>(current + 1);
            }
            current = current->next;
        }

        std::cout << "No suitable block found. Allocating new block." << std::endl;
        blocks.push_back(new char[BLOCK_SIZE]);
        Node* newBlock = reinterpret_cast<Node*>(blocks.back());
        newBlock->size = BLOCK_SIZE - sizeof(Node);
        newBlock->isFree = true;
        newBlock->next = head;
        head = newBlock;

        return allocator(size);
    }



    // void deallocator(size_t size)
    // {
    //     for (char *block : blocks)
    //     {
    //         delete[] block;
    //     }
    //     blocks.clear();
    //     head = nullptr;
    // }
};

int main() {
    SimpleAllocator allocate;

    std::unique_ptr<int> intPtr(static_cast<int*>(allocate.allocator(sizeof(int))));
    

    std::unique_ptr<double> doublePtr(static_cast<double*>(allocate.allocator(sizeof(double))));
    

    std::cout << "Enter an integer value: " << std::endl;
    std::cin >> *intPtr;

    std::cout << "Enter a double value: " << std::endl;
    std::cin >> *doublePtr;

    std::cout << "Int Value: " << *intPtr << std::endl;
    std::cout << "Double Value: " << *doublePtr << std::endl;

    //allocate.deallocator();

    return 0;
}