#!/usr/bin/env python3
"""
SAPHAL OS - Owner Avatar Image Converter
Converts any image file (PNG, JPG, BMP) into the raw PPM format
required by the SAPHAL OS Mobile Framebuffer Bootloader.
"""

import sys
import os

def create_sample_avatar_if_needed(target_path):
    """Generates a default test avatar pattern if no photo is supplied."""
    w, h = 150, 150
    os.makedirs(os.path.dirname(os.path.abspath(target_path)), exist_ok=True)
    with open(target_path, "wb") as f:
        f.write(f"P6\n{w} {h}\n255\n".encode("ascii"))
        for y in range(h):
            for x in range(w):
                # Gradient cyber-shield pattern
                r = int((x / w) * 100)
                g = int((y / h) * 245)
                b = int(212)
                f.write(bytes([r, g, b]))
    print(f"[+] Created sample owner avatar: {target_path}")

def convert_image(input_path, output_path):
    try:
        from PIL import Image
        im = Image.open(input_path).convert("RGB")
        im = im.resize((150, 150))
        im.save(output_path, "PPM")
        print(f"[+] Successfully converted {input_path} -> {output_path} (150x150 PPM)")
    except ImportError:
        print("[!] Pillow (PIL) not installed. Generating high-tech avatar directly:")
        create_sample_avatar_if_needed(output_path)

if __name__ == "__main__":
    out_file = "assets/owner_avatar.ppm"
    if len(sys.argv) > 1:
        in_file = sys.argv[1]
        out_file = sys.argv[2] if len(sys.argv) > 2 else out_file
        convert_image(in_file, out_file)
    else:
        create_sample_avatar_if_needed(out_file)
