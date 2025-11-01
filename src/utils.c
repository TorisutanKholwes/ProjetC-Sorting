/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */
#include "utils.h"
#include "app.h"
#include "logger.h"

Position* Position_new(const float x, const float y) {
    Position* pos = calloc(1, sizeof(Position));
    if (!pos) {
        error("Failed to allocate memory for Position");
        return NULL;
    }
    pos->x = x;
    pos->y = y;
    return pos;
}

void Position_destroy(Position* pos) {
    if (!pos) return;
    safe_free((void**)&pos);
}

bool Position_equals(const Position* a, const Position* b) {
    return (a->x == b->x) && (a->y == b->y);
}

bool Position_isNull(const Position* pos) {
    return pos == NULL || pos->x < 0 || pos->y < 0;
}

void safe_free(void** ptr) {
    if (ptr && *ptr) {
        free(*ptr);
        *ptr = NULL;
    }
}

void safe_freeArray(void** arr, int len) {
    for (int i = 0; i < len; i++) {
        if (arr[i]) {
            safe_free((void**)&arr[i]);
        }
    }
}

int init() {
    Uint32 sdlFlags = SDL_INIT_VIDEO | SDL_INIT_AUDIO;

    if (SDL_Init(sdlFlags) < 0) {
        error("Unable to initialize SDL: %s", SDL_GetError());
        return EXIT_FAILURE;
    }

    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
        error("Unable to initialize SDL_image: %s", SDL_GetError());
        return EXIT_FAILURE;
    }

    if (TTF_Init() == -1) {
        error("Unable to initialize SDL_ttf: %s", SDL_GetError());
        return EXIT_FAILURE;
    }

    if (Mix_Init(MIX_INIT_WAVPACK) < 0) {
        error("Unable to initialize SDL_mixer: %s", SDL_GetError());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

SDL_FRect SDL_CreateRect(const float x, const float y, const float w, const float h, bool from_center) {
    SDL_FRect rect = { 0 };
    rect.w = w;
    rect.h = h;
    if (!from_center) {
        rect.x = x;
        rect.y = y;
        return rect;
    }
    rect.x = fmaxf(0, x - (w / 2));
    rect.y = fmaxf(0, y - (h / 2));

    return rect;
}

void SDL_RenderStroke(SDL_Renderer* renderer, const SDL_FRect* rect, const float thickness) {
    const SDL_FRect top = { rect->x, rect->y, rect->w, thickness };
    const SDL_FRect bottom = { rect->x, rect->y + rect->h - thickness, rect->w, thickness };
    const SDL_FRect left = { rect->x, rect->y, thickness, rect->h };
    const SDL_FRect right = { rect->x + rect->w - thickness, rect->y, thickness, rect->h };

    SDL_RenderFillRectF(renderer, &top);
    SDL_RenderFillRectF(renderer, &bottom);
    SDL_RenderFillRectF(renderer, &left);
    SDL_RenderFillRectF(renderer, &right);
}

void SDL_drawThickLine(SDL_Renderer* renderer,
                                  float x1, float y1,
                                  float x2, float y2,
                                  float thickness) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    float len = sqrtf(dx * dx + dy * dy);
    if (len == 0) return;
    float nx = -dy / len;
    float ny = dx / len;
    for (int b = -((int)(thickness / 2)); b <= (int)(thickness / 2); b++) {
        float ox = nx * b;
        float oy = ny * b;
        SDL_RenderDrawLine(renderer, x1 + ox, y1 + oy, x2 + ox, y2 + oy);
    }
}



Color* Color_rgb(const int r, const int g, const int b) {
    return Color_rgba(r, g, b, 255);
}

Color* Color_rgba(const int r, const int g, const int b, const int a) {
    Color* color = (Color*)calloc(1, sizeof(Color));
    if (!color) {
        error("Failed to allocate memory for Color");
        return NULL;
    }
    color->r = r;
    color->g = g;
    color->b = b;
    color->a = a;
    return color;
}

Color* Color_hsv(float h, float s, float v) {
    float dR, dG, dB;

    h = fmodf(h, 360.0f);
    float c = v * s;
    float x = c * (1.f - fabsf(fmodf(h / 60.f, 2.f) - 1.f));
    float m = v - c;

    if (h < 60) {
        dR = c; dG = x; dB = 0;
    } else if (h < 120) {
        dR = c; dG = x, dB = 0;
    } else if (h < 180) {
        dR = 0; dG = c; dB = x;
    } else if (h < 240) {
        dR = 0; dG = x; dB = c;
    } else if (h < 300) {
        dR = x; dG = 0; dB = c;
    } else {
        dR = c; dG = 0; dB = x;
    }

    int r = (int) roundf((dR + m) * 255.f);
    int g = (int) roundf((dG + m) * 255.f);
    int b = (int) roundf((dB + m) * 255.f);
    return Color_rgb(r, g, b);
}

void Color_destroy(Color* color) {
    if (!color) return;
    safe_free((void**)&color);
}

const char* Color_toHex(const Color* color) {
    if (!color) return NULL;
    char* hex = String_format("#%02X%02X%02X%02X", color->r, color->g, color->b, color->a);
    return hex;
}

Color* Color_copy(Color* color) {
    if (!color) return NULL;
    return Color_rgba(color->r, color->g, color->b, color->a);
}

SDL_Color Color_toSDLColor(Color* color) {
    return (SDL_Color){ color->r, color->g, color->b, color->a };
}

Color* interpolateColor(Color* start, Color* end, float t) {
    return Color_rgba(
        start->r + (end->r - start->r) * t,
        start->g + (end->g - start->g) * t,
        start->b + (end->b - start->b) * t,
        start->a + (end->a - start->a) * t
    );
}

bool Color_equals(const Color* a, const Color* b) {
    return (a->r == b->r) && (a->g == b->g) && (a->b == b->b) && (a->a == b->a);
}

Color* Color_fromHex(const char* hex) {
    if (String_isNullOrEmpty(hex)) {
        return NULL;
    }

    unsigned int r = 0, g = 0, b = 0, a = 255;
    size_t len = strlen(hex);

    if (hex[0] == '#') {
        hex++;
        len--;
    }

    if (len == 6) {
        sscanf(hex, "%02x%02x%02x", &r, &g, &b);
    } else if (len == 8) {
        sscanf(hex, "%02x%02x%02x%02x", &r, &g, &b, &a);
    } else {
        error("Invalid hex color format: %s", hex);
        return NULL;
    }

    return Color_rgba(r, g, b, a);
}

char* Strdup(const char* str) {
    if (!str) return NULL;
    size_t len = strlen(str);
    char* copy = (char*)malloc(len + 1);
    if (!copy) {
        error("Failed to allocate memory for string duplication");
        return NULL;
    }
    for (size_t i = 0; i <= len; i++) {
        copy[i] = str[i];
    }
    copy[len] = '\0';
    return copy;
}

bool String_isNullOrEmpty(const char* str) {
    if (!str) return true;
    return strlen(str) == 0;
}

bool String_equals(const char* a, const char* b) {
    if (!a && !b) return true;
    if (!a || !b) return false;
    return strcmp(a, b) == 0;
}

int String_parseInt(const char* str, int defaultValue) {
    if (String_isNullOrEmpty(str)) {
        return defaultValue;
    }
    char* endPtr;
    long value = strtol(str, &endPtr, 10);
    if (endPtr == str) {
        return defaultValue;
    }
    return (int)value;
}

float String_parseFloat(const char* str, float defaultValue) {
    if (String_isNullOrEmpty(str)) {
        return defaultValue;
    }
    char* endPtr;
    float value = strtof(str, &endPtr);
    if (endPtr == str) {
        return defaultValue;
    }
    return value;
}

bool String_isNumeric(const char* str) {
    if (String_isNullOrEmpty(str)) {
        return false;
    }
    char* endPtr;
    strtof(str, &endPtr);
    return endPtr != str && *endPtr == '\0';
}

char* String_format(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int size = vsnprintf(NULL, 0, fmt, args);
    va_end(args);

    if (size < 0) {
        error("String_format: vsnprintf failed");
        return NULL;
    }

    char* buffer = (char*)malloc(size + 1);
    if (!buffer) {
        error("String_format: Failed to allocate memory");
        return NULL;
    }

    va_start(args, fmt);
    vsnprintf(buffer, size + 1, fmt, args);
    va_end(args);

    return buffer;
}

int modulo(const int a, const int b) {
    return ((a % b) + b) % b;
}

long arrayMax(const long* arr, const int len) {
    long maximum = arr[0];
    for (int i = 1; i < len; i++) {
        if (arr[i] > maximum) {
            maximum = arr[i];
        }
    }
    return maximum;
}

long arrayMin(const long* arr, const int len) {
    long minimum = arr[0];
    for (int i = 1; i < len; i++) {
        if (arr[i] < minimum) {
            minimum = arr[i];
        }
    }
    return minimum;
}

char* String_max(const char** arr, int len) {
    char* maxStr = NULL;
    for (int i = 0; i < len; i++) {
        if (i == 0) {
            maxStr = (char*)arr[i];
            continue;
        }
        if (strlen(arr[i]) > strlen(maxStr)) {
            maxStr = (char*)arr[i];
            continue;
        }
        if (strlen(arr[i]) == strlen(maxStr) && strcmp(arr[i], maxStr) > 0) {
            maxStr = (char*)arr[i];
        }
    }
    return maxStr;
}

char* String_min(const char** arr, int len) {
    char* minStr = NULL;
    for (int i = 0; i < len; i++) {
        if (i == 0) {
            minStr = (char*)arr[i];
            continue;
        }
        if (strlen(arr[i]) < strlen(minStr)) {
            minStr = (char*)arr[i];
            continue;
        }
        if (strlen(arr[i]) == strlen(minStr) && strcmp(arr[i], minStr) < 0) {
            minStr = (char*)arr[i];
        }
    }
    return minStr;
}

int String_compare(const char* a, const char* b) {
    if (strlen(a) == strlen(b)) {
        return strcmp(a, b);
    }
    return strlen(a) - strlen(b);
}

char* String_formatTime(const Uint32 milliseconds) {
    if (milliseconds == 0) {
        return String_format("0s");
    }

    Uint32 total_ms = milliseconds;
    Uint32 total_sec = total_ms / 1000;
    Uint32 ms = total_ms % 1000;
    Uint32 hours = total_sec / 3600;
    Uint32 minutes = (total_sec % 3600) / 60;
    Uint32 seconds = total_sec % 60;
    if (hours > 0) {
        if (minutes > 0 && seconds > 0) {
            return String_format("%uh %um %us", hours, minutes, seconds);
        }
        if (minutes > 0) {
            return String_format("%uh %um", hours, minutes);
        }
        if (seconds > 0) {
            return String_format("%uh %us", hours, seconds);
        }
        return String_format("%uh", hours);
    }
    if (minutes > 0) {
        if (seconds > 0) {
            return String_format("%um %us", minutes, seconds);
        }
        return String_format("%um", minutes);
    }
    if (seconds <= 0) {
        return String_format("%ums", ms);
    }
    float secf = seconds + (ms / 1000.0f);
    if (secf < 0.01f) secf = 0.0f;
    return String_format("%.2fs", secf);
}

int* voidToIntArray(void** arr, int len) {
    if (!arr || len <= 0) return NULL;
    int* intArr = calloc(len, sizeof(int));
    if (!intArr) {
        error("Failed to allocate memory for int array conversion");
        return NULL;
    }
    for (int i = 0; i < len; i++) {
        intArr[i] = (int)(long)arr[i];
    }
    return intArr;
}

long* voidToLongArray(void** arr, int len) {
    if (!arr || len <= 0) return NULL;
    long* longArr = calloc(len, sizeof(long));
    if (!longArr) {
        error("Failed to allocate memory for long array conversion");
        return NULL;
    }
    for (int i = 0; i < len; i++) {
        longArr[i] = (long)arr[i];
    }
    return longArr;
}

void** longToVoidArray(const long* arr, int len) {
    if (!arr || len <= 0) return NULL;
    void** voidArr = calloc(len, sizeof(void*));
    if (!voidArr) {
        error("Failed to allocate memory for void array conversion");
        return NULL;
    }
    for (int i = 0; i < len; i++) {
        voidArr[i] = (void*)arr[i];
    }
    return voidArr;
}

void** intToVoidArray(const int* arr, int len) {
    if (!arr || len <= 0) return NULL;
    void** voidArr = calloc(len, sizeof(void*));
    if (!voidArr) {
        error("Failed to allocate memory for void array conversion");
        return NULL;
    }
    for (int i = 0; i < len; i++) {
        voidArr[i] = (void*)(long)arr[i];
    }
    return voidArr;
}

bool is_power_of_two(int n) {
    return (n > 0) && ((n & (n - 1)) == 0);
}

void SDL_RenderRotateFillRect(SDL_Renderer* renderer, const SDL_FRect* rect, float angle, Color* color) {
    if (!renderer) return;

    SDL_Texture* tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                                 SDL_TEXTUREACCESS_STATIC, 1, 1);
    if (!tex) {
        return;
    }
    Uint32 white = 0xFFFFFFFF;
    SDL_UpdateTexture(tex, NULL, &white, sizeof(Uint32));
    SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
    SDL_SetTextureColorMod(tex, color->r, color->g, color->b);
    SDL_SetTextureAlphaMod(tex, color->a);

    SDL_Rect dst = {
        (int)rect->x,
        (int)rect->y,
        (int)rect->w,
        (int)rect->h
    };
    SDL_Point center = { dst.w / 2, dst.h / 2 };
    SDL_RenderCopyEx(renderer, tex, NULL, &dst, angle, &center, SDL_FLIP_NONE);
    SDL_DestroyTexture(tex);
}