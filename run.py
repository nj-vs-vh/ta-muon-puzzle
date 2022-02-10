from pathlib import Path
from tqdm.contrib.concurrent import process_map
import os

from data_extraction import process_dat
from data_extraction.filenames import set_data_dir, collect_dat_names


DATA_DIR_PATH = "data"  # relative to run.py
N_PROCESSES = 4
CUT_DAT_NAMES = 10


if __name__ == "__main__":
    set_data_dir((Path(__file__).parent / DATA_DIR_PATH).resolve())
    dat_names = collect_dat_names()
    if CUT_DAT_NAMES:
        dat_names = dat_names[:CUT_DAT_NAMES]
    print(f"Processing: {','.join(dat_names)}")

    def process_dat_threadsafe(dat_name: str):
        threadsafe_output_csv = f"output.{os.getpid()}.csv"
        process_dat(dat_name, threadsafe_output_csv)

    process_map(process_dat_threadsafe, dat_names)
