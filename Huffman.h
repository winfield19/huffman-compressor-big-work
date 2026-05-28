#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

// Windows兼容性处理
#ifdef _WIN32
// 某些旧版Windows编译器可能不支持snprintf
#if defined(_MSC_VER) && _MSC_VER < 1900
#define snprintf _snprintf
#endif
// 定义64位整数打印格式
#define PRIu64 "llu"
#else
#include <inttypes.h>
#endif

#define MAX_TREE_HT 256
#define BYTE_SIZE 256
#define MAX_PATH_LEN 260
#define MAX_FILES 1000

// 魔数和版本
#define HUFFMAN_MAGIC 0x48554646  // "HUFF"
#define HUFFMAN_VERSION 1

// 霍夫曼树节点结构
typedef struct HuffmanNode {
	uint8_t data;
	uint32_t freq;
	struct HuffmanNode *left, *right;
} HuffmanNode;

// 最小堆结构
typedef struct MinHeap {
	uint16_t size;
	uint16_t capacity;
	HuffmanNode **array;
} MinHeap;

// 编码表项结构
typedef struct CodeTable {
	uint8_t code[MAX_TREE_HT];
	uint8_t length;
} CodeTable;

// 文件头结构（单文件压缩）
typedef struct {
	uint32_t magic;          // 魔数 "HUFF"
	uint16_t version;        // 版本号
	uint64_t originalSize;   // 原始文件大小
	uint64_t compressedSize; // 压缩后数据大小（不含头）
	uint16_t uniqueCount;    // 不同字节数
	uint32_t crc32;          // CRC32校验
} FileHeader;

// 多文件归档头结构
typedef struct {
	uint32_t magic;          // 魔数 "HUFF"
	uint16_t version;        // 版本号
	uint32_t fileCount;      // 文件数量
	uint32_t crc32;          // 整个归档的CRC32
} ArchiveHeader;

// 文件条目头（多文件归档中的每个文件）
typedef struct {
	uint16_t pathLen;        // 文件路径长度
	uint64_t originalSize;   // 原始大小
	uint64_t compressedSize; // 压缩后大小
	uint16_t uniqueCount;    // 不同字节数
	uint32_t crc32;          // 文件CRC32
} FileEntryHeader;

// 频率表项（用于存储到文件）
typedef struct {
	uint8_t byte;
	uint32_t freq;
} FreqEntry;

// 节点操作
HuffmanNode* createNode(uint8_t data, uint32_t freq);
void freeTree(HuffmanNode *root);

// 最小堆操作
MinHeap* createMinHeap(uint16_t capacity);
void swapNode(HuffmanNode **a, HuffmanNode **b);
void minHeapify(MinHeap *minHeap, uint16_t idx);
bool isSizeOne(MinHeap *minHeap);
HuffmanNode* extractMin(MinHeap *minHeap);
void insertMinHeap(MinHeap *minHeap, HuffmanNode *node);
void buildMinHeap(MinHeap *minHeap);
bool isLeaf(HuffmanNode *root);

// 霍夫曼树构建
MinHeap* createAndBuildMinHeap(uint32_t freq[], uint16_t *uniqueCount);
HuffmanNode* buildHuffmanTree(uint32_t freq[], uint16_t *uniqueCount);
HuffmanNode* rebuildHuffmanTree(FreqEntry entries[], uint16_t uniqueCount);

// 编码生成
void generateCodes(HuffmanNode *root, CodeTable codeTable[], uint8_t code[], uint8_t top);

// 统计频率
void countFrequency(const char *filename, uint32_t freq[], uint64_t *fileSize);

// 单文件压缩/解压
bool compressFile(const char *inputFile, const char *outputFile);
bool decompressFile(const char *inputFile, const char *outputFile);

// 多文件压缩/解压
bool compressFiles(const char **inputFiles, int fileCount, const char *outputFile);
bool decompressArchive(const char *inputFile, const char *outputDir);
void listArchive(const char *inputFile);

// 辅助函数
uint32_t calculateCRC32(const uint8_t *data, size_t length);
void writeBit(FILE *fp, uint8_t bit, uint8_t *buffer, uint8_t *bitCount);

// 通配符展开
int expandWildcard(const char *pattern, char **results, int maxResults);

// 目录操作
void createDirectories(const char *path);
bool compressDirectory(const char *dirPath, const char *outputFile);

#endif

