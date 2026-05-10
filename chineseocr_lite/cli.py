import argparse
import json
import re
import sys
import time
from pathlib import Path

import numpy as np
from PIL import Image


def parse_args(argv=None):
    parser = argparse.ArgumentParser(
        prog="chineseocr",
        description="Run chineseocr_lite OCR on an image and print JSON output.",
    )
    parser.add_argument("image", help="Path to the image file to recognize.")
    parser.add_argument(
        "--compress",
        type=int,
        default=960,
        help="Resize short side before detection. Must be at least 64.",
    )
    parser.add_argument(
        "--output",
        "-o",
        help="Write JSON result to this file instead of stdout.",
    )
    parser.add_argument(
        "--draw",
        help="Write an image with detected text boxes to this path.",
    )
    return parser.parse_args(argv)


def normalize_text(text):
    return re.sub(r"^\s*\d+、\s*", "", str(text)).strip()


def normalize_box(box):
    return np.asarray(box).round().astype(int).reshape(4, 2).tolist()


def result_to_dict(results, elapsed):
    blocks = []
    texts = []
    for box, text, score in results:
        clean_text = normalize_text(text)
        if clean_text:
            texts.append(clean_text)
        blocks.append(
            {
                "text": clean_text,
                "score": round(float(score), 4),
                "box": normalize_box(box),
            }
        )

    return {
        "text": "\n".join(texts),
        "blocks": blocks,
        "elapsed": round(float(elapsed), 3),
    }


def to_json(data):
    return json.dumps(data, ensure_ascii=False, indent=2) + "\n"


def normalize_short_size(short_size):
    if short_size < 64:
        raise ValueError("--compress must be at least 64")
    return 32 * (short_size // 32)


def load_image(path):
    image = Image.open(path)
    try:
        exif = image._getexif() if hasattr(image, "_getexif") else None
        if exif:
            orientation = exif.get(274, 0)
            if orientation == 3:
                image = image.rotate(180, expand=True)
            elif orientation == 6:
                image = image.rotate(270, expand=True)
            elif orientation == 8:
                image = image.rotate(90, expand=True)
    except Exception:
        pass
    return image.convert("RGB")


def draw_result(image, results, output_path):
    from PIL import ImageDraw

    draw = ImageDraw.Draw(image)
    colors = ["red", "green", "blue", "purple"]
    for index, (box, _text, _score) in enumerate(results):
        points = normalize_box(box)
        color = colors[index % len(colors)]
        for start, end in zip(points, points[1:] + points[:1]):
            draw.line((*start, *end), fill=color, width=2)
    image.save(output_path)


def run_ocr(image_path, compress):
    from model import OcrHandle

    image = load_image(image_path)
    short_size = normalize_short_size(compress)

    handle = OcrHandle()
    start = time.time()
    results = handle.text_predict(image, short_size)
    elapsed = time.time() - start
    return image, results, elapsed


def main(argv=None):
    args = parse_args(argv)
    image_path = Path(args.image)
    if not image_path.exists():
        print(f"image not found: {image_path}", file=sys.stderr)
        return 2
    if not image_path.is_file():
        print(f"image is not a file: {image_path}", file=sys.stderr)
        return 2

    try:
        image, results, elapsed = run_ocr(image_path, args.compress)
        data = result_to_dict(results, elapsed)
        output = to_json(data)
        if args.output:
            Path(args.output).write_text(output, encoding="utf-8")
        else:
            print(output, end="")
        if args.draw:
            draw_result(image.copy(), results, args.draw)
    except Exception as exc:
        print(f"ocr failed: {exc}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
