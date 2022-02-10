from pathlib import Path
from tqdm.contrib.concurrent import process_map
import os
import argparse
from wurlitzer import pipes

from data_extraction import process_dat
from data_extraction.filenames import set_data_dir, collect_dat_names


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("data_dir_path")
    parser.add_argument("output_dir_path", default="..")
    parser.add_argument("logs_dir_path", default="..")
    parser.add_argument("--nproc", help="number of processes to run in parallel", type=int, default=1)
    parser.add_argument("--nfirst", help="process only first n files", type=int, default=0)
    args = parser.parse_args()

    set_data_dir((Path(__file__).parent / args.data_dir_path).resolve())
    dat_names = collect_dat_names()
    if args.nfirst:
        dat_names = dat_names[:args.nfirst]
    print(f"Processing: {len(dat_names)} files: from {dat_names[0]} to {dat_names[-1]}")

    def process_dat_threadsafe(dat_name: str):
        pid = os.getpid()
        threadsafe_output_csv = Path(args.output_dir_path) / f"output.{pid}.csv"
        log_file = Path(args.logs_dir_path) / f"{pid}.log"
        with open(log_file, "a") as log:
            with pipes(stdout=log, stderr=log):
                process_dat(dat_name, threadsafe_output_csv)

    process_map(process_dat_threadsafe, dat_names, max_workers=args.nproc)
