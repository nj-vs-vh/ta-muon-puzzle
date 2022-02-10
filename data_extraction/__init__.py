from pathlib import Path
from dataclasses import fields, asdict
import csv

from .reconstruction import reconstruct, DetectorData
from .waveform_processing import add_mu_ratios_to_detector_data


HAS_WRITTEN_BEFORE = False


def process_dat(dat_name: str, output_csv: Path):
    try:
        detector_data_by_event = reconstruct(dat_name)
        print("Calculating muon ratio from waveforms")
        add_mu_ratios_to_detector_data(dat_name, detector_data_by_event)
    except Exception:
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
                if data.mu_signal_ratio_bot is not None and data.mu_signal_ratio_top is not None:
                    writer.writerow(asdict(data))
    HAS_WRITTEN_BEFORE = True

