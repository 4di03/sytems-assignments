
typedef struct dynBlock{

int* arr;
size_t size;

}dynBlock;


dynBlock* allocDynBlock();

void storeMem2Blk(int* arr, size_t size, dynBlock* block);


void printDynBlock(dynBlock* block);

int charToInt(char digitChar);

dynBlock** loadData(char* dataPath);
