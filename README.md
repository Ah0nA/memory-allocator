# SimpleAllocator (My first cpp project)

## Overview

The `SimpleAllocator` class provides two implementations of a basic memory allocator. The first implementation uses a linked list to manage free and allocated memory blocks, and the second implementation uses a simple offset-based approach within fixed-size blocks.

## Features

- **Dynamic Memory Allocation**: Allocates memory dynamically from fixed-size blocks.
- **Block Management**: Manages multiple blocks to handle memory requests that exceed the current block's capacity.
- **Simple Interface**: Provides an easy-to-use interface for memory allocation and deallocation.
- **Linked List Management**: The first implementation uses a linked list to manage memory blocks, allowing for splitting and coalescing of free blocks.

## Implementations

### Linked List Implementation

#### How It Works

- **Blocks**: Memory is divided into blocks (`BLOCK_SIZE` = 4096 bytes). Each block is a contiguous array of bytes.
- **Nodes**: Each block is managed by a `Node` structure that keeps track of the block's size, free status, and a pointer to the next block.
- **Alignment**: Ensures that the allocated memory is properly aligned according to the size of a pointer.
- **Splitting**: If a block has more space than needed, it is split into two blocks.

#### Code Explanation

##### Class Members

```cpp
class SimpleAllocator {
private:
    struct Node {
        size_t size;
        bool isFree;
        Node *next;
    };
    static const size_t BLOCK_SIZE = 4096;
    std::vector<char *> blocks;
    Node *head = nullptr;

public:
    SimpleAllocator();
    ~SimpleAllocator();
    void* allocator(size_t size);
    // void deallocator(size_t size);
};
```

##### Constructor

```cpp
SimpleAllocator() {}
```

##### Destructor

```cpp
~SimpleAllocator() {
    for (char* block : blocks) {
        delete[] block;
    }
}
```

##### Memory Allocation Function

```cpp
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
```

##### Usage Example

```cpp
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

    return 0;
}
```

### Offset-Based Implementation

#### How It Works

- **Blocks**: Memory is divided into blocks (`BLOCK_SIZE` = 4096 bytes). Each block is a contiguous array of bytes.
- **Offset Tracking**: An offset is used to keep track of the next available memory location within the current block.
- **Alignment**: Ensures that the allocated memory is properly aligned according to the size of a pointer.

#### Code Explanation

##### Class Members

```cpp
class SimpleAllocator {
private:
    static const size_t BLOCK_SIZE = 4096;
    std::vector<char*> blocks;
    size_t offset = 0;

public:
    SimpleAllocator();
    ~SimpleAllocator();
    void* allocator(size_t size);
    void deallocateAll();
};
```

##### Constructor

```cpp
SimpleAllocator() : offset(0) {}
```

##### Destructor

```cpp
~SimpleAllocator() {
    for (char* block : blocks) {
        delete[] block;
    }
}
```

##### Memory Allocation Function

```cpp
void* allocator(size_t size) {
    const size_t alignment = sizeof(void*);
    const size_t alignedOffset = (alignment + offset - 1) & ~(alignment - 1);

    if (blocks.empty() || (alignedOffset + size) > BLOCK_SIZE) {
        blocks.push_back(new char[BLOCK_SIZE]);
        offset = 0;
        std::cout << "Allocated new block at address: " << reinterpret_cast<void*>(blocks.back()) << std::endl;
    }

    void* ptr = reinterpret_cast<void*>(blocks.back() + alignedOffset);
    offset = alignedOffset + size;
    std::cout << "Allocated memory at address: " << ptr << std::endl;
    return ptr;
}
```

##### Deallocate All Function

```cpp
void deallocateAll() {
    for (char* block : blocks) {
        delete[] block;
        std::cout << "Deallocated block at address: " << reinterpret_cast<void*>(block) << std::endl;
    }

    blocks.clear();
    offset = 0;
}
```

##### Usage Example

```cpp
int main() {
    SimpleAllocator allocator;

    int* intPtr = static_cast<int*>(allocator.allocator(sizeof(int)));
    double* doublePtr = static_cast<double*>(allocator.allocator(sizeof(double)));

    std::cout << "Enter an integer value: " << std::endl;
    std::cin >> *intPtr;

    std::cout << "Enter a double value: " << std::endl;
    std::cin >> *doublePtr;

    std::cout << "Int Value: " << *intPtr << std::endl;
    std::cout << "Double Value: " << *doublePtr << std::endl;

    allocator.deallocateAll();
    intPtr = nullptr;
    doublePtr = nullptr;

    return 0;
}
```

### Steps to Run

1. **Compile the Program**:
    ```sh
    g++ -o SimpleAllocator SimpleAllocator.cpp
    ```

2. **Run the Executable**:
    ```sh
    ./SimpleAllocator
    ```

3. **Follow the Prompts**:
    - Enter an integer value.
    - Enter a double value.
    - The program will output the entered values.

## Notes

- The allocator uses two different strategies for managing memory: a linked list approach and an offset-based approach.
- Proper alignment is ensured for the allocated memory in both implementations.
- The `deallocateAll` function in the offset-based implementation deallocates all memory blocks, making the allocator ready for reuse.
- The linked list implementation provides a more advanced memory management approach, allowing for splitting and merging of blocks. The allocator uses a simple first-fit strategy for finding free memory blocks.


I was inspired to do a cpp version of this project by this article: https://arjunsreedharan.org/post/148675821737/memory-allocators-101-write-a-simple-memory by : https://github.com/arjun024/memalloc
