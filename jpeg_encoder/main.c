#include <time.h>
#include <stdio.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define MAX_WIDTH 1080
#define MAX_HEIGH 1980

void rgb2yuv(uint8_t *rgb, uint8_t *yuv_y, uint8_t *yuv_u, uint8_t *yuv_v, unsigned int width, unsigned int height, unsigned int channel);
void block_data_8x8(uint8_t *data, uint8_t blocks[][64], unsigned int width, unsigned int height,  unsigned int w_block_size, unsigned int h_block_size);
int main() {
    // [0] load image to RGB
    clock_t time_start = clock();
    unsigned int width, height, channel;
    unsigned char *data = stbi_load("./lenna.png", &width, &height, &channel, STBI_rgb);
    printf("width: %d, height: %d, channel: %d\n", width, height, channel);
    if (width > MAX_WIDTH || height > MAX_HEIGH) {
        printf("image too large\n");
    }
    clock_t time_end_load = clock();
    printf("KPI-load image: %f ms\n", (double)(time_end_load - time_start)/CLOCKS_PER_SEC * 1000);


    // [1] convert RGB to YUV444
    uint8_t yuv_y[width * height];
    uint8_t yuv_u[width * height];
    uint8_t yuv_v[width * height];
    rgb2yuv(data, yuv_y, yuv_u, yuv_v, width, height, channel);
    clock_t time_rgb_to_yuv = clock();
    printf("KPI-rgb to yuv: %f ms\n", (double)(time_rgb_to_yuv - time_end_load)/CLOCKS_PER_SEC * 1000);
    

    // // write YUV444 to file
    // // 用 https://github.com/IENT/YUViewa 查看结果
    // // 设置 width, height, YUV Format为 yuv444 planar, Color Components为 YCbCr
    // FILE *fp = fopen("./lenna_yuv444.yuv", "wb");
    // fwrite(yuv_y, 1, width * height, fp);
    // fwrite(yuv_u, 1, width * height, fp);
    // fwrite(yuv_v, 1, width * height, fp);
    // fclose(fp);


    // [2] sample
    // todo: sample YUV444 to YUV420


    // [3] 分块, 每块8x8大小
    int w_block_size = width / 8  + (width %8 == 0 ? 0: 1);
    int h_block_size = height / 8 + (height %8 == 0 ? 0: 1);
    int block_size = w_block_size * h_block_size;
    uint8_t y_blocks[block_size][64];
    uint8_t u_blocks[block_size][64];
    uint8_t v_blocks[block_size][64];
    block_data_8x8(yuv_y, y_blocks, width, height, w_block_size, h_block_size);
    block_data_8x8(yuv_u, u_blocks, width, height, w_block_size, h_block_size);
    block_data_8x8(yuv_v, v_blocks, width, height, w_block_size, h_block_size);
    clock_t time_end_split_blocks = clock();
    printf("KPI-split blocks: %f ms\n", (double)(time_end_split_blocks - time_rgb_to_yuv)/CLOCKS_PER_SEC * 1000);
}

void rgb2yuv(uint8_t *rgb, uint8_t *yuv_y, uint8_t *yuv_u, uint8_t *yuv_v, unsigned int width, unsigned int height, unsigned int channel) {
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            int index = i * height + j;
            uint8_t r = rgb[index * channel];
            uint8_t g = rgb[index * channel + 1];
            uint8_t b = rgb[index * channel + 2];

            float luma = 0.299f   * r + 0.587f    * g + 0.114f    * b;
            float cb   = -0.1687f * r - 0.3313f   * g + 0.5f      * b + 128;
            float cr   = 0.5f     * r - 0.4187f   * g - 0.0813f   * b + 128;

            // YUV444 sample
            yuv_y[index] = (uint8_t)(luma);
            yuv_u[index] = (uint8_t)(cb);
            yuv_v[index] = (uint8_t)(cr);
        }
    }
}

void block_data_8x8(uint8_t *data, uint8_t blocks[][64], unsigned int width, unsigned int height,  unsigned int w_block_size, unsigned int h_block_size)
{
    for (int i = 0; i < w_block_size; i++) {
        for (int j = 0; j < h_block_size; j++) {
            uint8_t * block = blocks[i * w_block_size + j];
            for (int m = 0; m < 8; m++) {
                for (int n = 0; n < 8; n++) {
                    int w_pos = i * 8 + m;
                    int h_pos = j * 8 + n;
                    if (w_pos >= width || h_pos >= height) {
                        block[m + n*8] = 0;
                    } else {
                        block[m + n * 8] = data[h_pos * width + w_pos];
                    }
                }
            }
        }
    }
}