#!/usr/bin/env bash
# Generate quality comparison and filter showcase assets.
# Requires: jpeg_edu CLI built at ../build/jpeg_edu
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
CLI="$PROJECT_DIR/build/jpeg_edu"
DOCS_DIR="$PROJECT_DIR/docs"

if [ ! -x "$CLI" ]; then
    echo "Error: $CLI not found. Build the project first." >&2
    exit 1
fi

mkdir -p "$DOCS_DIR/assets"

TMP_DIR=$(mktemp -d)
trap 'rm -rf "$TMP_DIR"' EXIT

# Generate a 256x256 gradient BMP as reference image
python3 -c "
import struct, sys
W, H = 256, 256
row_stride = ((W * 3 + 3) // 4) * 4
pad = row_stride - W * 3
fh = struct.pack('<2sIHHI', b'BM', 54 + row_stride * H, 0, 0, 54)
ih = struct.pack('<IiiHHIIiiII', 40, W, H, 1, 24, 0, row_stride * H, 0, 0, 0, 0)
pixels = b''
for y in range(H):
    for x in range(W):
        # Interesting gradient: radial + color zones
        cx, cy = x - W // 2, y - H // 2
        r = min(255, int((x / (W - 1)) * 255))
        g = min(255, int((y / (H - 1)) * 255))
        b = min(255, int(((cx * cx + cy * cy) ** 0.5 / (W * 0.707)) * 255))
        pixels += struct.pack('BBB', b, g, r)  # BGR for BMP
    pixels += b'\x00' * pad
sys.stdout.buffer.write(fh + ih + pixels)
" > "$TMP_DIR/reference.bmp"

cp "$TMP_DIR/reference.bmp" "$DOCS_DIR/assets/reference.bmp"

# --- Quality comparison ---
echo "=== Quality Comparison ==="
QUALITIES=(10 25 50 75 95 100)
for q in "${QUALITIES[@]}"; do
    out="$DOCS_DIR/assets/quality_${q}.jpg"
    "$CLI" encode "$TMP_DIR/reference.bmp" "$out" -q "$q"

    # Get file size
    size=$(stat -f%z "$out" 2>/dev/null || stat -c%s "$out" 2>/dev/null)

    # Compute PSNR via roundtrip decode and compare
    decoded="$TMP_DIR/decoded_q${q}.bmp"
    "$CLI" decode "$out" "$decoded"

    echo "  q=$q  size=${size}B"
done

# --- Filter showcase ---
echo ""
echo "=== Filter Showcase ==="
FILTERS=(blur sharpen edge sobel median)
for f in "${FILTERS[@]}"; do
    out_bmp="$TMP_DIR/filter_${f}.bmp"
    out_jpg="$DOCS_DIR/assets/filter_${f}.jpg"
    "$CLI" filter "$TMP_DIR/reference.bmp" "$out_bmp" -f "$f"
    "$CLI" encode "$out_bmp" "$out_jpg" -q 95
    echo "  filter=$f"
done

# Also save a JPEG of the reference for HTML display
"$CLI" encode "$TMP_DIR/reference.bmp" "$DOCS_DIR/assets/reference.jpg" -q 100

echo ""
echo "Assets generated in $DOCS_DIR/assets/:"
ls -la "$DOCS_DIR/assets/"
