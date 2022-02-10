from pathlib import Path

from data_extraction import process_dat
from data_extraction.filenames import set_data_dir


if __name__ == "__main__":
    set_data_dir(Path(__file__).parent / "data")
    process_dat("DAT009512", Path("example.csv"))
