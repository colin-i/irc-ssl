
autoreconf -i && ./configure CFLAGS='-fsanitize=address,undefined -g' --disable-cpp && cd s && ./headless

# LSAN_OPTIONS=suppressions=<(echo "leak:libfontconfig") ./sirc
