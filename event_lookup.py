"""
Simple script to iterate over a dataset of Takeishi-style _gea.dat files and find
DATxxxxxx nams of the files containing specified datetimes.
"""

import argparse
import sys
from datetime import datetime
from pathlib import Path
from typing import Set

from dstreader import DstFile  # type: ignore
from tqdm import tqdm  # type: ignore

from data_extraction.filenames import (DstFileType, collect_dat_names,
                                       get_dst_file, set_data_dir)
from data_extraction.waveform_processing import parse_rusdraw_datetime

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--data_dir", help="path to dir containing _gea.dat files")
    parser.add_argument("--output", help="tsv output file")
    parser.add_argument("timestamps", nargs="+")
    args = parser.parse_args()
    data_dir_path = Path(args.data_dir).resolve()
    output_file = Path(args.output).resolve()
    if not data_dir_path.exists():
        sys.exit("data dir path does not exits")
    if output_file.exists():
        sys.exit("output path already exists")
    timestamp_strs: Set[str] = set(args.timestamps)
    lookup_timestamps = [datetime.fromisoformat(ts) for ts in timestamp_strs]

    print("Looking for events with timestamps:\n" + "\n".join([str(ts) for ts in lookup_timestamps]))
    
    set_data_dir(data_dir_path)

    with open(output_file, 'w') as out:
        for dat_name in tqdm(collect_dat_names(), unit="DAT", file=sys.stdout):
            dst_file = DstFile(get_dst_file(dat_name, DstFileType.FULL))
            try:
                with dst_file:
                    for _ in dst_file.events():
                        try:
                            event_timestamp = parse_rusdraw_datetime(dst_file.get_bank("rusdraw"))
                            if event_timestamp in lookup_timestamps:
                                print(f"MATCH FOUND: {event_timestamp} {dat_name}!")
                                print(f"{event_timestamp}\t{dat_name}", file=out)
                        except Exception as e:
                            print(f"ERROR parsing event: {e}")
            except Exception as e:
                print(f"ERROR reading file {e}")
