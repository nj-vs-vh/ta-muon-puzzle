from __future__ import annotations

from pathlib import Path
import subprocess
from dataclasses import dataclass
from collections import defaultdict

from typing import Dict


VERBOSITY = "1"


DATA_DIR = Path(__file__).parent / "data"


def check_result(res: subprocess.CompletedProcess):
    cmd = " ".join([str(a) for a in res.args])
    if res.returncode == 0:
        print("OK " + cmd)
    else:
        print("FAIL " + cmd)
        print(res.stdout.decode("utf-8"))
        print(res.stderr.decode("utf-8"))
        raise RuntimeError()


def reconstruct(full_dst: Path) -> str:
    events_dst_name = full_dst.name

    temp_dst_dir = full_dst.parent / "temp_dst"
    temp_dst_dir.mkdir(exist_ok=True)

    rufptn_out = temp_dst_dir / (events_dst_name + ".rufptn.dst.gz")
    sdtrgbk_out = temp_dst_dir / (events_dst_name + ".sdtrgbk.dst.gz")
    rufldf_out = temp_dst_dir / (events_dst_name + ".rufldf.dst.gz")

    if not rufptn_out.exists():
        res = subprocess.run(
            ["rufptn.run", full_dst, "-o1f", rufptn_out, "-v", VERBOSITY, "-f"],
            capture_output=True,
        )
        check_result(res)
    else:
        print("Skipped rufptn")

    if not sdtrgbk_out.exists():
        res = subprocess.run(
            ["sdtrgbk.run", rufptn_out, "-o1f", sdtrgbk_out, "-v", VERBOSITY, "-f"],
            capture_output=True,
        )
        check_result(res)
    else:
        print("Skipped sdtrgbk")

    if not rufldf_out.exists():
        res = subprocess.run(
            ["rufldf.run", sdtrgbk_out, "-o1f", rufldf_out, "-v", VERBOSITY, "-f", "-no_bw"],
            capture_output=True,
        )
        check_result(res)
    else:
        print("Skipped rufldf")

    res = subprocess.run(
        ["nuf.i12g.run", rufldf_out, "-MU"],
        capture_output=True,
    )
    check_result(res)

    print(f"OK {events_dst_name}\n")

    return res.stdout.decode("utf-8")


@dataclass
class NufOutput:
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

    @classmethod
    def parse_datetime_xxyy_mapping(cls, nuf_output: str) -> Dict[str, Dict[int, NufOutput]]:
        res = defaultdict(dict)
        for line in nuf_output.splitlines():
            try:
                assert line.startswith("MU ")
                line = line.replace("MU", "")
                tokens = line.split()
                datetime = " ".join(tokens[:2])
                xxyy = int(tokens[4].lstrip('0'))
                res[datetime][xxyy] = NufOutput(
                    theta=float(tokens[2]),
                    energy=float(tokens[3]),
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


def process_dat(datname: str):
    full_dst = DATA_DIR / (datname + "_0_gea.dst.gz")
    nuf_mu_output = reconstruct(full_dst)
    nuf_out_mapping = NufOutput.parse_datetime_xxyy_mapping(nuf_mu_output)
    
    print(nuf_out_mapping)
    

if __name__ == "__main__":
    process_dat("DAT009512")
