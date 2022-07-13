"""
Simple script to iterate over a dataset of Takeishi-style _gea.dat files and find
DATxxxxxx nams of the files containing specified datetimes.
"""

import argparse
from datetime import datetime
from pathlib import Path
import sys
from typing import Set

from dstreader import DstFile

from data_extraction.filenames import set_data_dir, collect_dat_names, get_dst_file, DstFileType
from data_extraction.waveform_processing import get_datetime


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--data_dir_path", help="path to dir containing _gea.dat files")
    parser.add_argument("timestamps", nargs="+")
    args = parser.parse_args()
    data_dir_path = Path(args.data_dir_path).resolve()
    if not data_dir_path.exists():
        sys.exit("data dir path does not exits")
    timestamp_strs: Set[str] = set(args.timestamps)
    timestamps = {datetime.fromisoformat(ts) for ts in timestamp_strs}
    
    set_data_dir(data_dir_path)

    for dat_name in collect_dat_names():
        dst_file = DstFile(get_dst_file(dat_name, DstFileType.FULL))
        with dst_file:
            for _ in dst_file.events():
                event_datetime = get_datetime(dst_file.get_bank("rusdraw"))
                if event_datetime in timestamps:
                    print(f"{event_datetime}\t{dat_name}")
