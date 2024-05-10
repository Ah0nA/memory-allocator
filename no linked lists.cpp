#include <iostream>
#include <vector>

class SimpleAllocator{
    private: 
    static const size_t BLOCK_SIZE = 4096;
    std::vector<char*> blocks;
    size_t offset = 0;

    public:
    SimpleAllocator() : offset(0) {};
    ~SimpleAllocator(){
        for(char * block : blocks){
            delete[] block;
        }
    }

    void * allocator(size_t size){
        const size_t alignment = sizeof(void*);
        const size_t alignedOffset = (alignment + offset - 1) & ~(alignment - 1);

        if(blocks.empty() || (alignedOffset + size) > BLOCK_SIZE){
            blocks.push_back(new char[BLOCK_SIZE]);
            offset = 0;
            std::cout << "Allocated new block at address: " << reinterpret_cast<void*>(blocks.back()) << std::endl;
     }

     void* ptr = reinterpret_cast<void*>(blocks.back() + alignedOffset);
        offset = alignedOffset + size;
        std::cout<<"Allocated a new block at address: "<< ptr <<std::endl;
        return ptr;
    }

    void deallocateAll(){
        for(char * block : blocks){
            delete [] block;
             std::cout << "Deallocated block at address: " << reinterpret_cast<void*>(block) << std::endl;
        }

        blocks.clear();
        offset = 0;
    }


};

int main(){
    SimpleAllocator allocate;

    int* intPtr = static_cast<int*>(allocate.allocator(sizeof(int)));
    //*intPtr = 43;

    double* doublePtr = static_cast<double*>(allocate.allocator(sizeof(double)));
    //*doublePtr = 3.142;

    std::cout<<"Enter an integer value: "<<std::endl;
    std::cin>>*intPtr;

    std::cout<<"Enter a double value: "<<std::endl;
    std::cin>>*doublePtr;

    std::cout<<"Int Value: "<<*intPtr<<std::endl;
    std::cout<<"Double Value: "<<*doublePtr<<std::endl;

    allocate.deallocateAll();
    intPtr = nullptr;
    doublePtr = nullptr;

    return 0;
}