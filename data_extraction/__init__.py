from pathlib import Path
from dataclasses import fields, asdict
import csv
import traceback

from typing import Optional

from .reconstruction import reconstruct, DetectorData
from .waveform_processing import add_mu_data_to_detector_data


HAS_WRITTEN_BEFORE = False


def process_dat(dat_name: str, output_csv: Path, temp_dir: Optional[str] = None):
    try:
        print(f"\n====================\nProcessing {dat_name}")
        print("Reconstruction")
        detector_data_by_event = reconstruct(dat_name, temp_dir)
        print("Calculating muon ratio from waveforms")
        add_mu_data_to_detector_data(dat_name, detector_data_by_event)
    except Exception:
        print(f"ERROR processing {dat_name}:\n{traceback.format_exc()}\n")
        return

    # dumping csv
    global HAS_WRITTEN_BEFORE
    with open(output_csv, "a" if HAS_WRITTEN_BEFORE else "w") as f:
        writer = csv.DictWriter(f, fieldnames=[f.name for f in fields(DetectorData)])
        if not HAS_WRITTEN_BEFORE:
            writer.writeheader()
        for data_by_detector in detector_data_by_event.values():
            for data in data_by_detector.values():
                # this should be true most of the time
                if all(
                    v is not None
                    for v in (data.top_integral_all, data.top_integral_mu, data.bot_integral_all, data.bot_integral_mu)
                ):
                    writer.writerow(asdict(data))
    HAS_WRITTEN_BEFORE = True
