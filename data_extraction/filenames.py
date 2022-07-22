from enum import Enum
from pathlib import Path
from typing import List, Optional, Set, cast


class DstFileType(Enum):
    FULL = 0
    GAMMA = 1
    EL = 2
    MU = 3
    HADRONS = 4
    NOISE = 5


DATA_DIR: Optional[Path] = None


def assert_data_dir_set():
    if DATA_DIR is None:
        raise RuntimeError("Call set_data_dir first")


def set_data_dir(new: Path):
    global DATA_DIR
    DATA_DIR = new


def collect_dat_names() -> List[str]:
    assert_data_dir_set()
    dir = cast(Path, DATA_DIR)
    return sorted({file.name[:9] for file in dir.iterdir() if file.is_file() and file.name.startswith("DAT")})


def get_dst_file(dat_name: str, type: DstFileType = DstFileType.FULL) -> Path:
    assert_data_dir_set()
    return cast(Path, DATA_DIR) / f"{dat_name}_{type.value}_gea.dst.gz"
