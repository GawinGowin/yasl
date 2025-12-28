#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <stdbool.h>

// Base64デコード用のテーブル
static const unsigned char base64_table[256] = {
    ['A'] = 0,  ['B'] = 1,  ['C'] = 2,  ['D'] = 3,  ['E'] = 4,  ['F'] = 5,
    ['G'] = 6,  ['H'] = 7,  ['I'] = 8,  ['J'] = 9,  ['K'] = 10, ['L'] = 11,
    ['M'] = 12, ['N'] = 13, ['O'] = 14, ['P'] = 15, ['Q'] = 16, ['R'] = 17,
    ['S'] = 18, ['T'] = 19, ['U'] = 20, ['V'] = 21, ['W'] = 22, ['X'] = 23,
    ['Y'] = 24, ['Z'] = 25, ['a'] = 26, ['b'] = 27, ['c'] = 28, ['d'] = 29,
    ['e'] = 30, ['f'] = 31, ['g'] = 32, ['h'] = 33, ['i'] = 34, ['j'] = 35,
    ['k'] = 36, ['l'] = 37, ['m'] = 38, ['n'] = 39, ['o'] = 40, ['p'] = 41,
    ['q'] = 42, ['r'] = 43, ['s'] = 44, ['t'] = 45, ['u'] = 46, ['v'] = 47,
    ['w'] = 48, ['x'] = 49, ['y'] = 50, ['z'] = 51, ['0'] = 52, ['1'] = 53,
    ['2'] = 54, ['3'] = 55, ['4'] = 56, ['5'] = 57, ['6'] = 58, ['7'] = 59,
    ['8'] = 60, ['9'] = 61, ['+'] = 62, ['/'] = 63, ['='] = 64
};

// データ構造
typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} RGB;

typedef struct {
    size_t width;
    size_t height;
    double score;
    char description[64];
} SizeCandidate;

typedef struct {
    unsigned char *data;
    size_t data_len;
    size_t pixel_count;
    bool is_valid;
    char error_msg[256];
} DecodedImage;

// Base64デコード関数
unsigned char* base64_decode(const char* input, size_t* out_len) {
    size_t in_len = strlen(input);

    // 改行文字を除去したクリーンな入力を作成
    char* clean_input = malloc(in_len + 1);
    if (!clean_input) return NULL;

    size_t clean_len = 0;
    for (size_t i = 0; i < in_len; i++) {
        if (input[i] != '\n' && input[i] != '\r') {
            clean_input[clean_len++] = input[i];
        }
    }
    clean_input[clean_len] = '\0';

    // Base64は4文字単位でなければならない
    if (clean_len == 0 || clean_len % 4 != 0) {
        free(clean_input);
        return NULL;
    }

    *out_len = clean_len / 4 * 3;
    if (clean_input[clean_len - 1] == '=') (*out_len)--;
    if (clean_input[clean_len - 2] == '=') (*out_len)--;

    unsigned char* output = malloc(*out_len);
    if (!output) {
        free(clean_input);
        return NULL;
    }

    size_t i, j;
    for (i = 0, j = 0; i < clean_len;) {
        uint32_t a = clean_input[i] == '=' ? 0 : base64_table[(unsigned char)clean_input[i]];
        uint32_t b = clean_input[i+1] == '=' ? 0 : base64_table[(unsigned char)clean_input[i+1]];
        uint32_t c = clean_input[i+2] == '=' ? 0 : base64_table[(unsigned char)clean_input[i+2]];
        uint32_t d = clean_input[i+3] == '=' ? 0 : base64_table[(unsigned char)clean_input[i+3]];

        uint32_t triple = (a << 18) | (b << 12) | (c << 6) | d;

        if (j < *out_len) output[j++] = (triple >> 16) & 0xFF;
        if (j < *out_len) output[j++] = (triple >> 8) & 0xFF;
        if (j < *out_len) output[j++] = triple & 0xFF;

        i += 4;
    }

    free(clean_input);
    return output;
}

// ファイルから Base64 データを読み込む
char* read_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        return NULL;
    }

    // ファイルサイズを取得
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // メモリ確保
    char* buffer = malloc(file_size + 1);
    if (!buffer) {
        fclose(file);
        return NULL;
    }

    // ファイル読み込み
    size_t read_size = fread(buffer, 1, file_size, file);
    buffer[read_size] = '\0';

    fclose(file);
    return buffer;
}

// Base64データをデコードして DecodedImage を作成
DecodedImage decode_image(const char* base64_data) {
    DecodedImage img = {0};

    img.data = base64_decode(base64_data, &img.data_len);
    if (!img.data) {
        img.is_valid = false;
        snprintf(img.error_msg, sizeof(img.error_msg),
                 "Base64 decode failed (invalid format or padding)");
        return img;
    }

    // RGB データは3バイトの倍数でなければならない
    if (img.data_len % 3 != 0) {
        img.is_valid = false;
        snprintf(img.error_msg, sizeof(img.error_msg),
                 "Decoded size (%zu bytes) is not divisible by 3 (incomplete RGB data)",
                 img.data_len);
        return img;
    }

    img.pixel_count = img.data_len / 3;
    img.is_valid = true;
    return img;
}

// 2のべき乗かどうかを判定
bool is_power_of_2(size_t n) {
    return n > 0 && (n & (n - 1)) == 0;
}

// 因数分解してサイズ候補を見つける
int find_factor_pairs(size_t pixel_count, SizeCandidate candidates[], int max_candidates) {
    int count = 0;
    size_t sqrt_n = (size_t)sqrt((double)pixel_count);

    for (size_t i = 1; i <= sqrt_n && count < max_candidates; i++) {
        if (pixel_count % i == 0) {
            size_t width = pixel_count / i;
            size_t height = i;

            // 横長パターン追加
            candidates[count].width = width;
            candidates[count].height = height;
            candidates[count].score = 0.0;
            candidates[count].description[0] = '\0';
            count++;

            // 正方形でない場合は縦長パターンも追加
            if (width != height && count < max_candidates) {
                candidates[count].width = height;
                candidates[count].height = width;
                candidates[count].score = 0.0;
                candidates[count].description[0] = '\0';
                count++;
            }
        }
    }

    return count;
}

// スコア計算関数
double calculate_score(size_t width, size_t height) {
    double score = 0.0;
    double aspect = (double)width / (double)height;

    char desc[64] = "";
    bool first = true;

    // 正方形ボーナス
    if (width == height) {
        score += 100.0;
        strcat(desc, "(square");
        first = false;
    }

    // 2のべき乗ボーナス
    bool width_pow2 = is_power_of_2(width);
    bool height_pow2 = is_power_of_2(height);

    if (width_pow2 && height_pow2) {
        score += 50.0;
        if (first) {
            strcat(desc, "(power-of-2");
            first = false;
        } else {
            strcat(desc, ", power-of-2");
        }
    } else if (width_pow2 || height_pow2) {
        score += 25.0;
        if (first) {
            strcat(desc, "(power-of-2");
            first = false;
        } else {
            strcat(desc, ", power-of-2");
        }
    }

    // 一般的なアスペクト比ボーナス
    if (fabs(aspect - 1.0) < 0.01) {
        score += 80.0;  // 1:1 (正方形と重複するが追加)
    } else if (fabs(aspect - 4.0/3.0) < 0.01 || fabs(aspect - 3.0/4.0) < 0.01) {
        score += 60.0;  // 4:3
        if (first) {
            strcat(desc, "(4:3");
            first = false;
        } else {
            strcat(desc, ", 4:3");
        }
    } else if (fabs(aspect - 16.0/9.0) < 0.01 || fabs(aspect - 9.0/16.0) < 0.01) {
        score += 60.0;  // 16:9
        if (first) {
            strcat(desc, "(16:9");
            first = false;
        } else {
            strcat(desc, ", 16:9");
        }
    } else if (fabs(aspect - 3.0/2.0) < 0.01 || fabs(aspect - 2.0/3.0) < 0.01) {
        score += 50.0;  // 3:2
        if (first) {
            strcat(desc, "(3:2");
            first = false;
        } else {
            strcat(desc, ", 3:2");
        }
    } else if (fabs(aspect - 16.0/10.0) < 0.01 || fabs(aspect - 10.0/16.0) < 0.01) {
        score += 50.0;  // 16:10
        if (first) {
            strcat(desc, "(16:10");
            first = false;
        } else {
            strcat(desc, ", 16:10");
        }
    }

    // 妥当な範囲ボーナス
    if (width >= 8 && width <= 4096 && height >= 8 && height <= 4096) {
        score += 20.0;
    }

    // 極端なアスペクト比にペナルティ
    if (aspect > 10.0 || aspect < 0.1) {
        score -= 50.0;
    }

    return score;
}

// description を設定する補助関数
void set_description(SizeCandidate* candidate) {
    size_t width = candidate->width;
    size_t height = candidate->height;

    char desc[64] = "";
    bool first = true;

    // 正方形
    if (width == height) {
        strcat(desc, "(square");
        first = false;
    }

    // 2のべき乗
    bool width_pow2 = is_power_of_2(width);
    bool height_pow2 = is_power_of_2(height);

    if (width_pow2 && height_pow2) {
        if (first) {
            strcat(desc, "(power-of-2");
            first = false;
        } else {
            strcat(desc, ", power-of-2");
        }
    } else if (width_pow2 || height_pow2) {
        if (first) {
            strcat(desc, "(power-of-2");
            first = false;
        } else {
            strcat(desc, ", power-of-2");
        }
    }

    // アスペクト比
    double aspect = (double)width / (double)height;

    if (fabs(aspect - 4.0/3.0) < 0.01 || fabs(aspect - 3.0/4.0) < 0.01) {
        if (first) {
            strcat(desc, "(4:3");
            first = false;
        } else {
            strcat(desc, ", 4:3");
        }
    } else if (fabs(aspect - 16.0/9.0) < 0.01 || fabs(aspect - 9.0/16.0) < 0.01) {
        if (first) {
            strcat(desc, "(16:9");
            first = false;
        } else {
            strcat(desc, ", 16:9");
        }
    } else if (fabs(aspect - 3.0/2.0) < 0.01 || fabs(aspect - 2.0/3.0) < 0.01) {
        if (first) {
            strcat(desc, "(3:2");
            first = false;
        } else {
            strcat(desc, ", 3:2");
        }
    }

    if (!first) {
        strcat(desc, ") ");
    }

    strcpy(candidate->description, desc);
}

// 1ピクセルをANSIカラーコードで表示
void print_pixel(RGB pixel) {
    printf("\033[48;2;%d;%d;%dm  \033[0m", pixel.r, pixel.g, pixel.b);
}

// 画像をターミナルに表示（原寸大）
void display_image(const unsigned char* data, size_t width, size_t height) {
    printf("\nImage preview (%zux%zu):\n", width, height);

    // 上枠
    printf("┌");
    for (size_t x = 0; x < width; x++) {
        printf("──");
    }
    printf("┐\n");

    // 画像データ
    for (size_t y = 0; y < height; y++) {
        printf("│");
        for (size_t x = 0; x < width; x++) {
            size_t offset = (y * width + x) * 3;
            RGB pixel;
            pixel.r = data[offset];
            pixel.g = data[offset + 1];
            pixel.b = data[offset + 2];
            print_pixel(pixel);
        }
        printf("│\n");
    }

    // 下枠
    printf("└");
    for (size_t x = 0; x < width; x++) {
        printf("──");
    }
    printf("┘\n");
}

// 候補を比較してソートするための関数
int compare_candidates(const void* a, const void* b) {
    const SizeCandidate* ca = (const SizeCandidate*)a;
    const SizeCandidate* cb = (const SizeCandidate*)b;

    if (cb->score > ca->score) return 1;
    if (cb->score < ca->score) return -1;
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file.rgb.b64>\n", argv[0]);
        return 1;
    }

    const char* filename = argv[1];

    // ファイル読み込み
    char* base64_data = read_file(filename);
    if (!base64_data) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", filename);
        return 1;
    }

    // Base64デコード
    DecodedImage img = decode_image(base64_data);
    free(base64_data);

    // ファイル情報表示
    printf("File: %s\n", filename);

    if (!img.is_valid) {
        printf("\nError: %s\n", img.error_msg);
        if (img.data) free(img.data);
        return 2;
    }

    printf("Decoded size: %zu bytes (%zu pixels)\n", img.data_len, img.pixel_count);
    printf("Status: Valid RGB data\n\n");

    // サイズ候補を計算
    SizeCandidate candidates[1000];  // 最大1000候補
    int num_candidates = find_factor_pairs(img.pixel_count, candidates, 1000);

    // スコア計算と説明文字列の設定
    for (int i = 0; i < num_candidates; i++) {
        candidates[i].score = calculate_score(candidates[i].width, candidates[i].height);
        set_description(&candidates[i]);
    }

    // スコアでソート
    qsort(candidates, num_candidates, sizeof(SizeCandidate), compare_candidates);

    // 結果表示（上位5件）
    printf("Most likely dimensions:\n");
    int display_count = num_candidates < 5 ? num_candidates : 5;

    for (int i = 0; i < display_count; i++) {
        printf("  %d. %zux%zu %s[score: %.1f]\n",
               i + 1,
               candidates[i].width,
               candidates[i].height,
               candidates[i].description,
               candidates[i].score);
    }

    if (num_candidates > 5) {
        printf("\n(and %d more candidates...)\n", num_candidates - 5);
    }

    // 最上位の候補で画像を表示
    if (num_candidates > 0) {
        display_image(img.data, candidates[0].width, candidates[0].height);
    }

    // メモリ解放
    free(img.data);

    return 0;
}
