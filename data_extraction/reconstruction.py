from __future__ import annotations

from pathlib import Path
import subprocess
from dataclasses import dataclass
from collections import defaultdict
from datetime import datetime

from typing import Dict, Optional

from .filenames import get_dst_file


@dataclass
class DetectorData:
    datetime_: datetime
    xxyy: int
    theta: float
    energy: float
    r: float
    r_plane: float
    S: float
    delta_t: float
    dt_aop: float
    dt_peaks: float
    dtul_asymm: float
    dtapcnt_u: float
    dtapcnt_l: float

    # filled later
    top_S_all: Optional[float] = None
    top_S_mu: Optional[float] = None
    bot_S_all: Optional[float] = None
    bot_S_mu: Optional[float] = None


DetectorIdx = int
DetectorDataByEvent = Dict[datetime, Dict[DetectorIdx, DetectorData]]


def parse_detector_data_by_event(nuf_output: str) -> DetectorDataByEvent:
    res = defaultdict(dict)
    for line in nuf_output.splitlines():
        try:
            assert line.startswith("MU ")
            line = line.replace("MU", "")
            tokens = line.split()
            date_str = tokens[0]
            hhmmss_str, usec_str = tokens[1].split(".")
            hhmmss_str = hhmmss_str.rjust(6, "0")
            datetime_ = datetime.strptime(f"{date_str} {hhmmss_str} {usec_str}", r"%Y%m%d %H%M%S %f")
            xxyy = int(tokens[4].lstrip('0'))
            res[datetime_][xxyy] = DetectorData(
                datetime_=datetime_,
                theta=float(tokens[2]),
                energy=float(tokens[3]),
                xxyy=xxyy,
                r=float(tokens[5]),
                r_plane=float(tokens[6]),
                S=float(tokens[7]),
                delta_t=float(tokens[8]),
                dt_aop=float(tokens[9]),
                dt_peaks=float(tokens[10]),
                dtul_asymm=float(tokens[11]),
                dtapcnt_u=float(tokens[12]),
                dtapcnt_l=float(tokens[13]),
            )
        except Exception as e:
            print(f"Can't parse line in nuf output: {line}\n\nError: {e}\n")
    return res


def reconstruct(dat_name: Path, temp_dir: Optional[str]) -> DetectorDataByEvent:
    VERBOSITY = "1"

    full_dst = get_dst_file(dat_name)
    events_dst_name = full_dst.name

    temp_dir = temp_dir or full_dst.parent / "temp_dst"
    temp_dir = Path(temp_dir)
    temp_dir.mkdir(exist_ok=True)

    rufptn_out = temp_dir / (events_dst_name + ".rufptn.dst.gz")
    sdtrgbk_out = temp_dir / (events_dst_name + ".sdtrgbk.dst.gz")
    rufldf_out = temp_dir / (events_dst_name + ".rufldf.dst.gz")

    if not rufptn_out.exists():
        res = subprocess.run(
            ["rufptn.run", full_dst, "-o1f", rufptn_out, "-v", VERBOSITY, "-f"],
            capture_output=True,
        )
        _check_result(res)
    else:
        print("OK skipped rufptn")

    if not sdtrgbk_out.exists():
        res = subprocess.run(
            ["sdtrgbk.run", rufptn_out, "-o1f", sdtrgbk_out, "-v", VERBOSITY, "-f"],
            capture_output=True,
        )
        _check_result(res)
    else:
        print("OK skipped sdtrgbk")

    if not rufldf_out.exists():
        res = subprocess.run(
            ["rufldf.run", sdtrgbk_out, "-o1f", rufldf_out, "-v", VERBOSITY, "-f", "-no_bw"],
            capture_output=True,
        )
        _check_result(res)
    else:
        print("OK skipped rufldf")

    res = subprocess.run(
        ["nuf.i12g.run", rufldf_out, "-MU"],
        capture_output=True,
    )
    _check_result(res)

    return parse_detector_data_by_event(res.stdout.decode("utf-8"))


def _check_result(res: subprocess.CompletedProcess):
    cmd = " ".join([str(a) for a in res.args])
    if res.returncode == 0:
        print("OK " + cmd)
    else:
        print("FAIL " + cmd)
        print(res.stdout.decode("utf-8"))
        print(res.stderr.decode("utf-8"))
        raise RuntimeError()
