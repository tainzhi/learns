#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "dct.h"
#include "bitstr.h"
#include <time.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>

#define YUV420  // 注释掉该行，则YUV444采样
// 1 打印较详细的运行信息，0 不打印
const int DEBUG = 1;

#define DCT_SIZE 8

const uint8_t STD_QUANT_LUMIN_TABLE[64] = {
    16, 11, 10, 16, 24, 40, 51, 61,
    12, 12, 14, 19, 26, 58, 60, 55,
    14, 13, 16, 24, 40, 57, 69, 56,
    14, 17, 22, 29, 51, 87, 80, 62,
    18, 22, 37, 56, 68, 109,103,77,
    24, 35, 55, 64, 81, 104,113,92,
    49, 64, 78, 87, 103,121,120,101,
    72, 92, 95, 98, 112,100,103,99,
};

const uint8_t STD_QUANT_CHROM_TABLE[64] = {
    17, 18, 24, 47, 99, 99, 99, 99,
    18, 21, 26, 66, 99, 99, 99, 99,
    24, 26, 56, 99, 99, 99, 99, 99,
    47, 66, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
};

const int ZIGZAG_TABLE[64] = {
    0,  1,  8, 16,  9,  2,  3, 10,
    17, 24, 32, 25, 18, 11,  4,  5,
    12, 19, 26, 33, 40, 48, 41, 34,
    27, 20, 13,  6,  7, 14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36,
    29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46,
    53, 60, 61, 54, 47, 55, 62, 63,
};

const uint8_t STD_HUFFMAN_LUMIN_AC[] = {
    0x00, 0x02, 0x01, 0x03, 0x03, 0x02, 0x04, 0x03, 0x05, 0x05, 0x04, 0x04, 0x00, 0x00, 0x01, 0x7d,
    0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12, 0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07,
    0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08, 0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0,
    0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28,
    0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
    0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
    0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
    0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
    0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5,
    0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2,
    0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
    0xf9, 0xfa,
};
const uint8_t STD_HUFFMAN_LUMIN_DC[] = {
    0x00, 0x01, 0x05, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
};
const uint8_t STD_HUFFMAN_CHROM_AC[] = {
    0x00, 0x02, 0x01, 0x02, 0x04, 0x04, 0x03, 0x04, 0x07, 0x05, 0x04, 0x04, 0x00, 0x01, 0x02, 0x77,
    0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21, 0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71,
    0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91, 0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33, 0x52, 0xf0,
    0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34, 0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26,
    0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
    0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
    0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5,
    0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3,
    0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda,
    0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
    0xf9, 0xfa
};
const uint8_t STD_HUFFMAN_CHROM_DC[] = {
    0x00, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
};

typedef struct {
    unsigned num_0_before_code; // 在code之前出现的0的次数
    unsigned code_bit_len; // code的二进制长度
    unsigned code; // code数值本身
} RELEITEM;

typedef struct {
    int symbol;// unused
    int freq;  // unused
    int group; // unused
    int depth; // 该code在huffman编码树上的节点的深度
    int code;
} HUFCODEITEM;
const int MAX_HUFFMAN_CODE_LENGTH = 16;
HUFCODEITEM lumin_dc_huffman_code_tree[256];
HUFCODEITEM lumin_ac_huffman_code_tree[256];
HUFCODEITEM chrom_dc_huffman_code_tree[256];
HUFCODEITEM chrom_ac_huffman_code_tree[256];

void rgb2yuv(uint8_t* rgb, uint8_t* yuv_y, uint8_t* yuv_u, uint8_t* yuv_v, const int width, const int height, const int channel);
void yuv420_sample(uint8_t *yuv_y, uint8_t * yuv_u, uint8_t *yuv_v, int * y_width, int * y_height, int *uv_width, int * uv_height);
void block_data_8x8(const uint8_t* data, const unsigned int width, const unsigned int height, uint8_t ***blocks, size_t * w_blocks_size, size_t * h_blocks_size);
void save_yuv_to_file(const uint8_t* yuv_y, const uint8_t* yuv_u, const uint8_t* yuv_v, const unsigned int width, const unsigned int height, const char* filename);
void save_yuv_blocks_to_file(const uint8_t ** y_blocks, const uint8_t ** u_blocks, const uint8_t ** v_blocks, const size_t y_w_blocks_size, const size_t y_h_blocks_size, const size_t uv_w_blocks_size, const size_t uv_h_blocks_size, const char* filename);
void block_dct(uint8_t* in, int* out);
void quant_encode(int block[64], const uint8_t table[64]);
void quant_decode(int block[64], const uint8_t table[64]);
void zigzag_encode(int block[64], const int table[64]);
void zigzag_decode(int block[64], const int table[64]);
void build_huffman_tree();
void dc_ac_huffman_encode(const int block[64], int * dc, const HUFCODEITEM * dc_huffman_code_tree, const HUFCODEITEM * ac_huffman_code_tree, void * bs);


int main() {
    // [0] load image to RGB
    clock_t time_start = clock();
    int width, height, channel;
    unsigned char* data = stbi_load("./lenna.png", &width, &height, &channel, STBI_rgb);
    if (DEBUG == 1)
        printf("width: %d, height: %d, channel: %d\n", width, height, channel);
    clock_t time_end_load = clock();
    printf("KPI-load image: %f ms\n", (double)(time_end_load - time_start) / CLOCKS_PER_SEC * 1000);


    // [1] convert RGB to YUV
    // image像素坐标从左上角(0,0)到右下角(height, width)
    // height垂直方向是row索引从0~height
    // width水平方向是column索引从0~width
    uint8_t yuv_y[width * height];
    uint8_t yuv_u[width * height];
    uint8_t yuv_v[width * height];
    rgb2yuv(data, yuv_y, yuv_u, yuv_v, width, height, channel);
    clock_t time_rgb_to_yuv = clock();
    printf("KPI-rgb to yuv: %f ms\n", (double)(time_rgb_to_yuv - time_end_load) / CLOCKS_PER_SEC * 1000);
    if (DEBUG == 1) {
        // write YUV to file
        // 用 https://github.com/IENT/YUViewa 查看结果
        // 设置 width, height, YUV Format为 yuv planar, Color Components为 YCbCr
        save_yuv_to_file(yuv_y, yuv_u, yuv_v, width, height, "./lenna_yuv.yuv");
    }


    // [2] sample
    int y_width = width, y_height = height;
    int uv_width = width, uv_height = height;
    #ifdef YUV420
    yuv420_sample(yuv_y, yuv_u, yuv_v, &y_width, &y_height, &uv_width, &uv_height);
    if (DEBUG == 1)
        printf("yuv420 sample, uv width:%d, uv height:%d\n", uv_width, uv_height);
    #endif


    // [3] 分块, 每块8x8大小
    // y分量width被分成的块数
    size_t y_w_blocks_size = 0;
    size_t y_h_blocks_size = 0;
    // uv分量width方向被分成的块数
    size_t uv_w_blocks_size = 0;
    size_t uv_h_blocks_size = 0;
    // 都是二维数组，大小是 [y_block_size][64]
    // 二维数组即二重指针
    // 每一行都是固定的64列
    uint8_t **y_blocks = NULL;
    uint8_t **u_blocks = NULL;
    uint8_t **v_blocks = NULL;
    // 要修改 y_blocks 和 u_blocks, v_blocks 的大小, 故传入地址
    block_data_8x8(yuv_y, y_width, y_height, &y_blocks, &y_w_blocks_size, &y_h_blocks_size);
    block_data_8x8(yuv_u, uv_width, uv_height, &u_blocks, &uv_w_blocks_size, &uv_h_blocks_size);
    block_data_8x8(yuv_v, uv_width, uv_height, &v_blocks, &uv_w_blocks_size,&uv_h_blocks_size);
    size_t y_blocks_size = y_w_blocks_size * y_h_blocks_size;
    size_t uv_blocks_size = uv_w_blocks_size * uv_h_blocks_size;
    if (DEBUG == 1)
        printf("y blocks size: %zu, uv blocks size: %zu\n", y_blocks_size, uv_blocks_size);
    clock_t time_end_split_blocks = clock();
    printf("KPI-split blocks: %f ms\n", (double)(time_end_split_blocks - time_rgb_to_yuv) / CLOCKS_PER_SEC * 1000);
    if (DEBUG == 1)
        save_yuv_blocks_to_file((const uint8_t **)y_blocks, (const uint8_t **)u_blocks, (const uint8_t **)v_blocks, y_w_blocks_size, y_h_blocks_size, uv_w_blocks_size, uv_h_blocks_size, "./lenna_blocks_yuv.yuv");


    // [4] 离散余弦变换 DCT
    // fix: 不能在栈上分配内存，会爆掉
    // float y_blocks_dct[block_size][64];
    // float u_blocks_dct[block_size][64];
    // float v_blocks_dct[block_size][64];
    // 在堆上分配内存
    // 二维数组，每一行都是固定的64列
    // 二重指针，存储的是y_blocks_size大小的 int *指针
    int ** y_blocks_dct = (int **)malloc(sizeof(int*) * y_blocks_size);
    // 上面申请的int *指针，又申请64个int内存，赋值给该指针
    for (int i = 0; i < y_blocks_size; i++) y_blocks_dct[i] = (int*)malloc(sizeof(int) * 64);
    int ** u_blocks_dct = (int **)malloc(sizeof(int*) * uv_blocks_size);
    for (int i = 0; i < uv_blocks_size; i++) u_blocks_dct[i] = (int*)malloc(sizeof(int) * 64);
    int ** v_blocks_dct = (int **)malloc(sizeof(int*) * uv_blocks_size);
    for (int i = 0; i < uv_blocks_size; i++) v_blocks_dct[i] = (int*)malloc(sizeof(int) * 64);
    // 最原始版本的 DCT 很慢，处理 512*512 的原图要花费 12.29s
    // for (int i = 0; i < block_size; i++) {
    //     block_dct(y_blocks[i], y_blocks_dct[i]);
    //     block_dct(u_blocks[i], u_blocks_dct[i]);
    //     block_dct(v_blocks[i], v_blocks_dct[i]);
    // }
    // 参考 https://github.com/binglingziyu/audio-video-blog-demos/blob/master/15-rgb-to-jpeg/rgb-to-jpeg.c 
    // 快速傅里叶变换的使得复杂度从 O(N^2) 降低到 N/log_2N
    // 处理512*512 的原图只花费 11ms, 复杂度降低了1000倍
    init_dct_module();
    for (int i = 0; i < y_blocks_size; i++) {
        // 二重指针通过数组索引访问，获取一重指针，即一行数据的地址
        uint8_t* y_block = y_blocks[i];
        int* y_block_dct = y_blocks_dct[i];
        // level shift, 把值域从[0,255]移动到[-128,127]
        for (int j = 0; j < 64; j++) y_block_dct[j] = y_block[j] - 128;
        for (int j = 0; j < 64; j++) y_block_dct[j] = y_block_dct[j] << 2;
        fdct2d8x8(y_block_dct, NULL);
    #ifdef YUV420
    }
    for (int i = 0; i < uv_blocks_size; i++) {
    #endif
        uint8_t* u_block = u_blocks[i];
        int* u_block_dct = u_blocks_dct[i];
        // level shift, 把值域从[0,255]移动到[-128,127]
        for (int j = 0; j < 64; j++) u_block_dct[j] = u_block[j] - 128;
        for (int j = 0; j < 64; j++) u_block_dct[j] = u_block_dct[j] << 2;
        fdct2d8x8(u_block_dct, NULL);

        uint8_t* v_block = v_blocks[i];
        int* v_block_dct = v_blocks_dct[i];
        // level shift, 把值域从[0,255]移动到[-128,127]
        for (int j = 0; j < 64; j++) v_block_dct[j] = v_block[j] - 128;
        for (int j = 0; j < 64; j++) v_block_dct[j] = v_block_dct[j] << 2;
        fdct2d8x8(v_block_dct, NULL);
    }
    clock_t time_end_dct = clock();
    printf("KPI-dct: %f ms\n", (double)(time_end_dct - time_end_split_blocks) / CLOCKS_PER_SEC * 1000);


    // [量化] jpeg指定压缩质量1~99
    for (int i = 0; i < y_blocks_size; i++) {
        // y - lumain table
        quant_encode(y_blocks_dct[i], STD_QUANT_LUMIN_TABLE);
    #ifdef YUV420
    }
    for (int i = 0; i < uv_blocks_size; i++) {
    #endif
        // u/v - chrom table
        quant_encode(u_blocks_dct[i], STD_QUANT_CHROM_TABLE);
        quant_encode(v_blocks_dct[i], STD_QUANT_CHROM_TABLE);
    }
    clock_t time_quant = clock();
    printf("KPI-quant: %f ms\n", (double)(time_quant - time_end_dct) / CLOCKS_PER_SEC * 1000);


    // [Zigzag扫描]
    for (int i = 0; i < y_blocks_size; i++) {
        zigzag_encode(y_blocks_dct[i], ZIGZAG_TABLE);
    #ifdef YUV420
    }
    for (int i = 0; i < uv_blocks_size; i++) {
    #endif
        zigzag_encode(u_blocks_dct[i], ZIGZAG_TABLE);
        zigzag_encode(v_blocks_dct[i], ZIGZAG_TABLE);
    }
    clock_t time_zigzag_scan = clock();
    printf("KPI-zigzag scan: %f ms\n", (double)(time_zigzag_scan - time_quant)/ CLOCKS_PER_SEC * 1000);

    // [DC系数的差分脉冲调制解码]
    // [DC系数的中间格式计算]
    // [AC系数的游程长度编码]
    // [AC系数的中间格式计算]
    // [熵编码-huffman编码]
    build_huffman_tree();
    int dc[3] = {0};
    char *buffer = (char *)malloc(width * height * 2);
    int buffer_max_width = width * height * 2;
    void *bs = bitstr_open(BITSTR_MEM, buffer, &buffer_max_width);
    #ifndef YUV420
    // YUV444 sample, yuv分块存储
    // yuv分块交叉存储，而不是连续存储yuv各自的所有分块，优点是在解码时可以加载一个y分块，u分块，v分块后解析出该块8x8的图片像素数据，再加载解析下一个分块
    // 若是yuv连续存储，那么会需要全部加载所有的分块到内存后，再依次解析各分块信息，这堆内存挑战很大
    for (int i = 0; i < y_blocks_size; i++) {
        dc_ac_huffman_encode(y_blocks_dct[i], dc + 0, lumin_dc_huffman_code_tree, lumin_ac_huffman_code_tree, bs);
        dc_ac_huffman_encode(u_blocks_dct[i], dc + 1, chrom_dc_huffman_code_tree, chrom_ac_huffman_code_tree, bs);
        dc_ac_huffman_encode(v_blocks_dct[i], dc + 2, chrom_dc_huffman_code_tree, chrom_ac_huffman_code_tree, bs);
    }
    #else
    // yuv420 sample, 存储2x2的y分块，再存储1个u分块，1个v分块
    // 依次存储4x8x8 y分量，8x8 u分量，8x8 v 分量
    for (int i = 0; i < y_h_blocks_size; i += 2) {
        for (int j = 0; j < y_w_blocks_size; j += 2) {
            // 编码并存储相邻的2x2 y 分块
            dc_ac_huffman_encode(y_blocks_dct[i * y_w_blocks_size + j], dc + 0, lumin_dc_huffman_code_tree, lumin_ac_huffman_code_tree, bs);
            dc_ac_huffman_encode(y_blocks_dct[i * y_w_blocks_size + j + 1], dc + 0, lumin_dc_huffman_code_tree, lumin_ac_huffman_code_tree, bs);
            dc_ac_huffman_encode(y_blocks_dct[(i + 1) * y_w_blocks_size + j], dc + 0, lumin_dc_huffman_code_tree, lumin_ac_huffman_code_tree, bs);
            dc_ac_huffman_encode(y_blocks_dct[(i + 1) * y_w_blocks_size + j + 1], dc + 0, lumin_dc_huffman_code_tree, lumin_ac_huffman_code_tree, bs);

            // 编码并存储对应的u分块
            dc_ac_huffman_encode(u_blocks_dct[i / 2  * y_w_blocks_size/2 + j/2], dc + 1, chrom_dc_huffman_code_tree, chrom_ac_huffman_code_tree, bs);
            // 编码并存储对应的v分块
            dc_ac_huffman_encode(v_blocks_dct[i / 2 * y_w_blocks_size/2 + j/2], dc + 2, chrom_dc_huffman_code_tree, chrom_ac_huffman_code_tree, bs);
        }
    }
    #endif
    long data_length = bitstr_tell(bs);
    if (DEBUG == 1)
    {
        printf("data length: %ld\n", data_length);
    }
    clock_t time_huffman_encode = clock();
    printf("KPI-huffman encode: %f ms\n", (double)(time_huffman_encode - time_zigzag_scan)/ CLOCKS_PER_SEC * 1000);

    
    // [write to file]
    FILE *fp = fopen("./lenna.jpeg", "wb");
    // SOI (start of image) 图像开始标记
    fputc(0xFF, fp);
    fputc(0xD8, fp);
    // 参考： https://www.cnblogs.com/sddai/p/5666924.html
    // APP0 Markder, 0xE0
        // APP0 长度 2byte
        // identifier 5byte
        // version 2byte
        // ...
    // APPn Markers, 0xE1 - 0xEF
        // ...
    fputc(0xFF, fp);
        
    // 多个 DQT(difine quantization table), 0xDB
    // DQT
    const uint8_t *qtab[2] = {STD_QUANT_LUMIN_TABLE, STD_QUANT_CHROM_TABLE};
    for (int i = 0; i < 2; i++) {
        fputc(0xFF, fp);
        fputc(0xDB, fp);
        size_t len = 2 + 1 + 64;
        fputc(len >> 8, fp);
        fputc(len >> 0, fp);
        // dqt table id 分别是0， 1, 即0x00, 0x01
        fputc(i, fp);
        for (int j = 0; j < 64; j++) {
            // 反 zigzag scan
            fputc(qtab[i][ZIGZAG_TABLE[j]], fp);
        }
    }
    // SOF0 (start of frame), 0xCO
    fputc(0xFF, fp);
    fputc(0xC0, fp);
    // start of frame length
    int SOF0_len = 2 + 1 + 2 + 2 + 1 + 3*3;
    fputc(SOF0_len >> 8, fp); // 写入 SOF0长度的高字节
    fputc(SOF0_len >> 0, fp); // 写入 S0F0长度的低字节
    // precision 8bit
    fputc(8, fp);
    fputc(height >> 8, fp);
    fputc(height >> 0, fp);
    fputc(width >> 8, fp);
    fputc(width >> 0, fp);
    // number of color components
    fputc(3, fp);
    // vertical sample factor, horizontal sample factor, quantizaton table
    // Y U V 的 id 分别是0x01, 0x02, 0x03
    // 若是 YUV420采样，则三者采样因子是 0x22, 0x11, 0x11
    // 若是 YUV422采样，则三者采样因子时 0x21, 0x11, 0x11
    // 这里是YUV444采样，三者采样因子都是 0x11；
    // 最后一列则是 quantization table id
    // y-0x01 使用 STD_QUANT_LUMIN_TABLE 0x00
    // u-0x02 和 v-0x03 使用 STD_QUANT_CHROM_TABLE 0x01
    #ifndef YUV420
    // YUV444采样，三者采样因子都是 0x11；
    fputc(0x01, fp); fputc(0x11, fp); fputc(0x00, fp);
    fputc(0x02, fp); fputc(0x11, fp); fputc(0x01, fp);
    fputc(0x03, fp); fputc(0x11, fp); fputc(0x01, fp);
    #else
    // YUV420采样，三者采样因子是 0x22, 0x11, 0x11
    fputc(0x01, fp); fputc(0x22, fp); fputc(0x00, fp);
    fputc(0x02, fp); fputc(0x11, fp); fputc(0x01, fp);
    fputc(0x03, fp); fputc(0x11, fp); fputc(0x01, fp);
    #endif

    // DHT AC(Definie huffman table), 0xC4
    // lumin ac id 0x10, chrom ac id 0x11
    const uint8_t *huffman_ac_tables[] = { STD_HUFFMAN_LUMIN_AC, STD_HUFFMAN_CHROM_AC};
    for (int i = 0; i < 2; i++) {
        fputc(0xFF, fp);
        fputc(0xC4, fp);
        size_t len = 2 + 1 + 16;
        for (int j = 0; j < 16; j++) {
            len += huffman_ac_tables[i][j];
        }
        fputc(len >> 8, fp);
        fputc(len >> 0, fp);
        // dht ac id, 0x10和0x11
        fputc(i + 0x10, fp);
        fwrite(huffman_ac_tables[i], 1,  len - 3, fp);
    }
    // DHT DC, lumin dc id 0x00, chrom dc id 0x01
    const uint8_t *huffman_dc_tables[] = {STD_HUFFMAN_LUMIN_DC, STD_HUFFMAN_CHROM_DC};
    for (int i = 0; i < 2; i++) {
        fputc(0xFF, fp);
        fputc(0xC4, fp);
        size_t len = 2 + 1 + 16;
        for (int j = 0; j < 16; j++) {
            len += huffman_dc_tables[i][j];
        }
        fputc(len >> 8, fp);
        fputc(len >> 0, fp);
        // dht dc id, 0x00 和 0x01
        fputc(i + 0x00, fp);
        fwrite(huffman_dc_tables[i], 1,  len - 3, fp);
    }

    // SOS (start of scan, 扫描开始)
    fputc(0xFF, fp);
    fputc(0xDA, fp);
    // 扫描开始长度
    int SOS_len = 2 + 1 + 2 * 3 + 3;
    fputc(SOS_len >> 8, fp);
    fputc(SOS_len >> 0 ,fp);
    // number of color components 颜色分量数
    fputc(0x03, fp);
    // 高4位dc huffman table id, 低4位ac huffman table id
    // y - 0x01, 
    fputc(0x01, fp); fputc(0x00, fp);
    fputc(0x02, fp); fputc(0x11, fp);
    fputc(0x03, fp); fputc(0x11, fp);
    // 谱选择开始，固定值0x00
    fputc(0x00, fp);
    // 谱选择结束，固定值0x3F
    fputc(0x3F, fp);
    // 谱选择，0x00
    fputc(0x00, fp);

    //data
    fwrite(buffer, 1, data_length, fp);

    // EOI (End of image), 图像结束
    fputc(0xFF, fp);
    fputc(0xD9, fp);

    fflush(fp);
    fclose(fp);
    clock_t time_save_jpeg = clock();
    printf("KPI-save jpeg: %f ms\n", (double)(time_save_jpeg - time_huffman_encode)/ CLOCKS_PER_SEC * 1000);

    free(buffer);
    // free memory
    for (int i = 0; i < y_blocks_size; i++) {
        free(y_blocks[i]);
        free(y_blocks_dct[i]);
    }
    free(y_blocks);
    free(y_blocks_dct);
    for (int i = 0; i < uv_blocks_size; i++) {
        free(u_blocks[i]);
        free(u_blocks_dct[i]);
        free(v_blocks[i]);
        free(v_blocks_dct[i]);
    }
    free(u_blocks);
    free(u_blocks_dct);
    free(v_blocks);
    free(v_blocks_dct);
}

void save_yuv_to_file(const uint8_t* yuv_y, const uint8_t* yuv_u, const uint8_t* yuv_v, const unsigned int width, const unsigned int height, const char* filename) {
    FILE* fp = fopen(filename, "wb");
    fwrite(yuv_y, 1, width * height, fp);
    fwrite(yuv_u, 1, width * height, fp);
    fwrite(yuv_v, 1, width * height, fp);
    // fwrite(yuv_y, width * height, 1,  fp);
    // fwrite(yuv_u, width * height, 1, fp);
    // fwrite(yuv_v, width * height, 1, fp);
    fclose(fp);
}

void save_yuv_blocks_to_file(const uint8_t ** y_blocks, const uint8_t ** u_blocks, const uint8_t ** v_blocks, const size_t y_w_blocks_size, const size_t y_h_blocks_size, const size_t uv_w_blocks_size, const size_t uv_h_blocks_size, const char* filename) {
    if (DEBUG == 1)
        printf("y_w_blocks_size: %d, y_h_blocks_size: %d, uv_w_blocks_size: %d, uv_h_blocks_size: %d\n", y_w_blocks_size, y_h_blocks_size, uv_w_blocks_size, uv_h_blocks_size);
    FILE* fp_block = fopen(filename, "wb");
    for (int i = 0; i < y_h_blocks_size; i++) {
        for (int k = 0; k < 8; k++) {
            for (int j = 0; j < y_w_blocks_size; j++) {
                fwrite(y_blocks[i * y_w_blocks_size + j] + k * 8, 1, 8, fp_block);
            }
        }
    }
    for (int i = 0; i < uv_h_blocks_size; i++) {
        for (int k = 0; k < 8; k++) {
            for (int j = 0; j < uv_w_blocks_size; j++) {
                fwrite(u_blocks[i * uv_w_blocks_size + j] + k * 8, 1, 8, fp_block);
            }
        }
    }
    for (int i = 0; i < uv_h_blocks_size; i++) {
        for (int k = 0; k < 8; k++) {
            for (int j = 0; j < uv_w_blocks_size; j++) {
                fwrite(v_blocks[i * uv_w_blocks_size + j] + k * 8, 1, 8, fp_block);
            }
        }
    }
    fclose(fp_block);
}

// Y = 0.299*R + 0.587*G + 0.114*B
// U = Cb = -0.169*R - 0.331*G + 0.500*B + 128
// V = Cr = 0.500*R - 0.419*G - 0.081*B + 128
// R/G/B  [0 ~ 255]
// Y/Cb/Cr[0 ~ 255]
void rgb2yuv(uint8_t* rgb, uint8_t* yuv_y, uint8_t* yuv_u, uint8_t* yuv_v, const int width, const int height, const int channel) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int index = i * width + j;
            uint8_t r = rgb[index * channel];
            uint8_t g = rgb[index * channel + 1];
            uint8_t b = rgb[index * channel + 2];

            float luma = 0.29900f * r + 0.58700f * g + 0.11400f * b;
            float cb = -0.16874f * r - 0.33126f * g + 0.50000f * b + 128;
            float cr = 0.50000f * r - 0.41869f * g - 0.08131f * b + 128;
            if (luma < 0 || cb < 0 || cr < 0) {
                printf("luma: %f, cb: %f, cr: %f", luma, cb, cr);
            }

            // YUV444 sample
            yuv_y[index] = (uint8_t)(luma);
            yuv_u[index] = (uint8_t)(cb);
            yuv_v[index] = (uint8_t)(cr);
        }
    }
}


void yuv420_sample(uint8_t *yuv_y, uint8_t * yuv_u, uint8_t *yuv_v, int * y_width, int * y_height, int *uv_width, int * uv_height)
{
    // 420采样 y分量不做处理
    int sampled_width = (*uv_width) / 2;
    int sampled_height = (*uv_height) / 2;
    for (int i = 0; i < *uv_height; i += 2) {
        for (int j = 0; j < *uv_width; j += 2) {
            yuv_u[i / 2 * sampled_width + j / 2] = (
                yuv_u[i * (*uv_width) + j]
                + yuv_u[i * (*uv_width) + j + 1]
                + yuv_u[(i + 1) * (*uv_width) + j]
                + yuv_u[(i + 1) * (*uv_width) + j + 1]) / 4;
            yuv_v[i / 2 * sampled_width + j / 2] = (
                yuv_v[i * (*uv_width) + j]
                + yuv_v[i * (*uv_width) + j + 1]
                + yuv_v[(i + 1) * (*uv_width) + j]
                + yuv_v[(i + 1) * (*uv_width) + j + 1]) / 4;
        }
    }
    *uv_width = sampled_width;
    *uv_height = sampled_height;
}

/**
 * @param ***blocks 要创建一个二维数组，那么需要在该function中修改一个二重指针，故传参三重指针
 * @param *w_blocks_size  width方向被分割的块数
 * @param *h_blocks_size  height方向被分割的块数
 */
void block_data_8x8(const uint8_t* data, const unsigned int width, const unsigned int height, uint8_t ***blocks, size_t * w_blocks_size, size_t * h_blocks_size)
{
    *h_blocks_size = height / 8 + (height % 8 == 0 ? 0 : 1);
    *w_blocks_size = width / 8 + (width % 8 == 0 ? 0 : 1);
    int blocks_size = (*h_blocks_size) * (*w_blocks_size);
    // 先申请一维数组，存储所有的8x8块的地址指针
    // *blocks 是三重指针的解引用，结果是二重指针
    *blocks = (uint8_t **)malloc(blocks_size * sizeof(uint8_t *));
    // 左上角坐标（0,0), 右下角坐标(height, width)
    // row in [0, height], column in [0, width]
    for (int row = 0; row < (*h_blocks_size); row++) {
        for (int col = 0; col < (*w_blocks_size); col++) {
            // 再申请8x8块
            uint8_t *block = (uint8_t *)malloc(sizeof(uint8_t) * 64);
            (*blocks)[row * (*w_blocks_size) + col] = block;
            for (int sub_row = 0; sub_row < 8; sub_row++) {
                for (int sub_col = 0; sub_col < 8; sub_col++) {
                    int data_row = row * 8 + sub_row;
                    int data_col = col * 8 + sub_col;
                    int pos =  sub_row * 8 + sub_col;
                    if (data_row >= height || data_col >= width) {
                        block[pos] = 0;
                    }
                    else {
                        block[pos] = data[data_row * width + data_col];
                    }
                }
            }
        }
    }
}

double ck(float k) {
    if (k == 0) return sqrt(1.0 / DCT_SIZE);
    else return sqrt(2.0 / DCT_SIZE);
}

void block_dct(uint8_t* in, int* out)
{
    double sum = 0;
    for (int u = 0; u < DCT_SIZE; u++) {
        for (int v = 0; v < DCT_SIZE; v++) {
            for (int i = 0; i < DCT_SIZE; i++) {
                for (int j = 0; j < DCT_SIZE; j++) {
                    // M_PI is included in <math.h>
                    // -128 是level shift, 这样才能把值域从０－255映射到－128－127
                    sum += (in[i * DCT_SIZE + j] - 128) * cos((2 * i + 1) * u * M_PI / (2 * DCT_SIZE)) * cos((2 * j + 1) * v * M_PI / (2 * DCT_SIZE));
                }
            }
            out[u * DCT_SIZE + v] = sum * ck(u) * ck(v);
            sum = 0.0;
        }
    }
}

void quant_encode(int block[64], const uint8_t table[64])
{
    for (int i = 0; i < 64; i++) block[i] /= table[i];
}

void quant_decode(int block[64], const uint8_t table[64])
{
    for (int i = 0; i < 64; i++) block[i] *= table[i];
}

void zigzag_encode(int block[64], const int table[64]) {
    int temp[64];
    for (int i = 0; i < 64; i++) temp[i] = block[table[i]];
    memcpy(block, temp, 64 * sizeof(int));
}

void zigzag_decode(int block[64], const int table[64]) {
    int temp[64];
    for (int i = 0; i < 64; i++) temp[table[i]] = block[i];
    memcpy(block, temp, 64 * sizeof(int));
}

/**
 * @brief 计算编码的以二进制形式的长度，正数是其本身，负数是其补码
 * @param code 待编码的整数
 * @param code_bit_length 编码的二进制长度
 */
void category_encode(int * code, int * code_bit_length)
{
    unsigned int abs_code = abs(*code);
    unsigned int mask = (1<<15);
    int i = 15;
    if (abs_code == 0) {
        *code_bit_length = 0;
        return;
    }
    while (i && !(abs_code & mask)) {
        mask >>= 1;
        i--;
    }
    *code_bit_length = i + 1;
    // 负数，用其补码表示
    if (*code < 0) *code = (1 << *code_bit_length) - abs_code - 1;
}

void build_huffman_tree_from_std(const uint8_t * std_huf, HUFCODEITEM * huftree) {
    int code = 0x00, k = 0, tabsize = 0;
    uint8_t hufsize[256];
    int hufcode[256];
    for (int i = 0; i < MAX_HUFFMAN_CODE_LENGTH; i++) {
        for (int j = 0; j < std_huf[i]; j++ ) {
            hufsize[k] = i + 1;
            hufcode[k] = code;
            code++ ;
            k++;
        }
        code <<= 1;
    }
    tabsize = k;
    // 前 MAX_HUFFMAN_CODE_LENGTH 个符号，对应标准编码表的编码长度
    for (int i = 0; i < tabsize; i++) {
        int symbol = std_huf[MAX_HUFFMAN_CODE_LENGTH + i];
        huftree[symbol].depth = hufsize[i];
        huftree[symbol].code = hufcode[i];
    }

}
void build_huffman_tree() {
    build_huffman_tree_from_std(STD_HUFFMAN_LUMIN_DC, lumin_dc_huffman_code_tree);
    build_huffman_tree_from_std(STD_HUFFMAN_LUMIN_AC, lumin_ac_huffman_code_tree);
    build_huffman_tree_from_std(STD_HUFFMAN_CHROM_DC, chrom_dc_huffman_code_tree);
    build_huffman_tree_from_std(STD_HUFFMAN_CHROM_AC, chrom_ac_huffman_code_tree);
}

/**
 * huffman编码对应的分块，并存储到缓存bs中
 */
void dc_ac_huffman_encode(const int block[64], int * dc, const HUFCODEITEM * dc_huffman_code_tree, const HUFCODEITEM * ac_huffman_code_tree, void * bs)
{
    RELEITEM rlelist[63];
    int diff, code, code_bit_len;
    int i, j, num_0_before_code, eob;
    // 相邻块的首域值的差异
    diff = block[0] - *dc;
    *dc = block[0];
    code = diff;
    // 块中第0个值，是直流dc分量，也无需计算先导0
    category_encode(&code, &code_bit_len);
    // huffman encode for dc
    bitstr_put_bits(bs, dc_huffman_code_tree[code_bit_len].code, dc_huffman_code_tree[code_bit_len].depth);
    bitstr_put_bits(bs, code, code_bit_len);
    for (i = 1, j = 0, num_0_before_code = 0, eob = 0; i < 64 && j < 63; i++) {
        // 统计连续的0的个数
        if (block[i] == 0 && num_0_before_code < 15) {
            num_0_before_code++;
        } else {
            code = block[i];
            code_bit_len = 0;
            category_encode(&code, &code_bit_len);
            rlelist[j].num_0_before_code = num_0_before_code;
            rlelist[j].code_bit_len = code_bit_len;
            rlelist[j].code = code;
            num_0_before_code = 0;
            j++;
            if (code_bit_len != 0) {
                eob = j;
            }
        }
    }
    if (block[63] == 0) {
        rlelist[eob].num_0_before_code = 0;
        rlelist[eob].code_bit_len = 0;
        rlelist[eob].code = 0;
        j = eob + 1;
    }
    for (i = 0; i < j; i++) {
        // 高四位存储code之前的0的出现次数，低4位存储code本身的二进制长度
        uint8_t temp = (rlelist[i].num_0_before_code << 4) | (rlelist[i].code_bit_len << 0);
        // 组成的8bit数值以霍夫曼编码的形式存储
        bitstr_put_bits(bs, ac_huffman_code_tree[temp].code, ac_huffman_code_tree[temp].depth);
        bitstr_put_bits(bs, rlelist[i].code, rlelist[i].code_bit_len);
    }
}