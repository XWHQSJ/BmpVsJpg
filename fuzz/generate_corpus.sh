#!/usr/bin/env bash
# Generate seed corpus files for fuzz targets.
# Requires: jpeg_edu CLI built at ../build/jpeg_edu
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
CLI="$PROJECT_DIR/build/jpeg_edu"
CORPUS_DIR="$SCRIPT_DIR/corpus"

if [ ! -x "$CLI" ]; then
    echo "Error: $CLI not found. Build the project first." >&2
    exit 1
fi

mkdir -p "$CORPUS_DIR"

TMP_DIR=$(mktemp -d)
trap 'rm -rf "$TMP_DIR"' EXIT

# --- Generate a small 16x16 BMP gradient for seed ---
# We use a tiny Python one-liner to create a minimal BMP
python3 -c "
import struct, sys
W, H = 16, 16
row_stride = ((W * 3 + 3) // 4) * 4
pad = row_stride - W * 3
fh = struct.pack('<2sIHHI', b'BM', 54 + row_stride * H, 0, 0, 54)
ih = struct.pack('<IiiHHIIiiII', 40, W, H, 1, 24, 0, row_stride * H, 0, 0, 0, 0)
pixels = b''
for y in range(H):
    for x in range(W):
        r = int(x * 255 / (W - 1))
        g = int(y * 255 / (H - 1))
        b = 128
        pixels += struct.pack('BBB', b, g, r)  # BGR
    pixels += b'\x00' * pad
sys.stdout.buffer.write(fh + ih + pixels)
" > "$TMP_DIR/seed.bmp"

# Encode at various quality levels for JPEG corpus
for q in 10 25 50 75 95 100; do
    "$CLI" encode "$TMP_DIR/seed.bmp" "$CORPUS_DIR/seed_q${q}.jpg" -q "$q"
done

# Progressive JPEG seed
"$CLI" encode "$TMP_DIR/seed.bmp" "$CORPUS_DIR/seed_progressive.jpg" -q 75 -progressive

# Grayscale JPEG seed
"$CLI" encode "$TMP_DIR/seed.bmp" "$CORPUS_DIR/seed_gray.jpg" -q 75 -gray

# BMP seed
cp "$TMP_DIR/seed.bmp" "$CORPUS_DIR/seed.bmp"

echo "Seed corpus generated in $CORPUS_DIR:"
ls -la "$CORPUS_DIR/"
