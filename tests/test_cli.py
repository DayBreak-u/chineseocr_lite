import json

import pytest
from PIL import Image

from chineseocr_lite import cli


def test_result_to_dict_formats_agent_friendly_json():
    result = cli.result_to_dict(
        [
            (
                [[1.2, 2.8], [10.0, 3.0], [10.2, 8.7], [1.0, 8.0]],
                "1、 测试文字",
                0.9321,
            )
        ],
        elapsed=1.234,
    )

    assert result == {
        "text": "测试文字",
        "blocks": [
            {
                "text": "测试文字",
                "score": 0.9321,
                "box": [[1, 3], [10, 3], [10, 9], [1, 8]],
            }
        ],
        "elapsed": 1.234,
    }


def test_json_output_is_utf8_and_stable():
    output = cli.to_json({"text": "中文", "blocks": [], "elapsed": 0.1})

    assert json.loads(output) == {"text": "中文", "blocks": [], "elapsed": 0.1}
    assert "\\u4e2d" not in output
    assert output.endswith("\n")


def test_parse_args_defaults_to_json_output():
    args = cli.parse_args(["sample.jpg"])

    assert args.image == "sample.jpg"
    assert args.compress == 960
    assert args.output is None
    assert args.draw is None


def test_normalize_short_size_rejects_too_small_values():
    with pytest.raises(ValueError, match="at least 64"):
        cli.normalize_short_size(32)


def test_normalize_short_size_rounds_down_to_multiple_of_32():
    assert cli.normalize_short_size(970) == 960


def test_main_returns_error_for_missing_image(capsys):
    code = cli.main(["missing.jpg"])

    captured = capsys.readouterr()
    assert code == 2
    assert "image not found" in captured.err


def test_main_writes_json_and_drawn_image(tmp_path, monkeypatch):
    source = tmp_path / "source.jpg"
    output = tmp_path / "result.json"
    drawn = tmp_path / "drawn.jpg"
    Image.new("RGB", (20, 20), "white").save(source)

    def fake_run_ocr(_image_path, compress):
        assert compress == 960
        image = Image.new("RGB", (20, 20), "white")
        results = [
            (
                [[1, 1], [10, 1], [10, 10], [1, 10]],
                "1、 CLI",
                0.9,
            )
        ]
        return image, results, 0.5

    monkeypatch.setattr(cli, "run_ocr", fake_run_ocr)

    code = cli.main([str(source), "--output", str(output), "--draw", str(drawn)])

    assert code == 0
    assert json.loads(output.read_text(encoding="utf-8")) == {
        "text": "CLI",
        "blocks": [
            {
                "text": "CLI",
                "score": 0.9,
                "box": [[1, 1], [10, 1], [10, 10], [1, 10]],
            }
        ],
        "elapsed": 0.5,
    }
    assert drawn.exists()
