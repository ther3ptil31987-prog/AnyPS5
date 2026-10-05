from pathlib import Path
import subprocess
import sys
import tempfile


def main():
    relinker = Path(sys.argv[1]).resolve()
    cases = [
        ("tiny", bytes(10), "File too small for ELF header"),
        ("truncated", b"\x7fELF\x02\x01\x01" + bytes(25), "File too small for ELF header"),
        ("header-only", b"\x7fELF\x02\x01\x01" + bytes(57), "No PT_DYNAMIC segment found"),
        ("bad-magic", bytes(64), "Invalid ELF magic number"),
    ]
    with tempfile.TemporaryDirectory(prefix="anyps5-elf-header-") as directory:
        for name, data, error in cases:
            source = Path(directory) / (name + ".elf")
            output = source.with_suffix(".out")
            source.write_bytes(data)
            result = subprocess.run([str(relinker), "--skip-sce-module", str(source), str(output)],
                                    capture_output=True, text=True, timeout=20)
            assert result.returncode == 2 and error in result.stderr and not output.exists(), (name, result)
    print("ELF header tests passed")


if __name__ == "__main__":
    main()
