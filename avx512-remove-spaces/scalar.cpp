char* remove_spaces__scalar(const char* src, char* dst, size_t n) {
    for (size_t i=0; i < n; i++) {
        if (src[i] == ' ' || src[i] == '\r' || src[i] == '\n')
            continue;

        *dst++ = src[i];
    }

    return dst;
}
