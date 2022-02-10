from pathlib import Path

from enum import Enum


class DstFileType(Enum):
    FULL = 0
    GAMMA = 1
    EL = 2
    MU = 3
    HADRONS = 4
    NOISE = 5


DATA_DIR = None


def set_data_dir(new: Path):
    global DATA_DIR
    DATA_DIR = new


def get_dst_file(dat_name: str, type: DstFileType = DstFileType.FULL) -> Path:
    if DATA_DIR is None:
        raise RuntimeError("Call set_data_dir first")
    return DATA_DIR / f"{dat_name}_{type.value}_gea.dst.gz"
