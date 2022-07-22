import numpy as np
from datetime import datetime
from collections import defaultdict
from dstreader import DstFile  # type: ignore
from dstreader.bank import Bank  # type: ignore

from typing import List, Optional, Tuple, Dict

from .reconstruction import DetectorDataByEvent, DetectorIdx
from .filenames import get_dst_file, DstFileType


def parse_rusdraw_datetime(rusdraw: Bank) -> datetime:
    return datetime.strptime(
        f"{rusdraw['yymmdd']:06} {rusdraw['hhmmss']:06} {rusdraw['usec']}", r"%y%m%d %H%M%S %f"
    )


def assemble_waveforms(wf_part_by_id: Dict[DetectorIdx, Dict[int, np.ndarray]]) -> Tuple[np.ndarray, np.ndarray]:
    """Assemble waveforms from their 128-bin parts"""
    wf_top: List[float] = []
    wf_bot: List[float] = []
    for wf_id in sorted(wf_part_by_id.keys()):
        wf_top.extend(wf_part_by_id[wf_id][1])  # 1 is top in rusdraw!
        wf_bot.extend(wf_part_by_id[wf_id][0])  # 0 is bot in rusdraw!
    return np.array(wf_top), np.array(wf_bot)


VEM_PER_MIP = 1.22
DETECTOR_AREA = 3.0  # m^2


def add_mu_data_to_detector_data(dat_name: str, detector_data_by_event: DetectorDataByEvent):
    full_dst = DstFile(get_dst_file(dat_name, type=DstFileType.FULL))
    mu_dst = DstFile(get_dst_file(dat_name, type=DstFileType.MU))
    noise_dst = DstFile(get_dst_file(dat_name, type=DstFileType.NOISE))

    with full_dst, mu_dst, noise_dst:

        #                                                  ( event dt,           { xxyy:      (S top, S bot) } )
        def process_rusdraw(rusdraw: Bank) -> Optional[Tuple[datetime, Dict[DetectorIdx, Tuple[float, float]]]]:
            try:
                datetime_ = parse_rusdraw_datetime(rusdraw)
            except ValueError:
                print(
                    "Can't parse datetime from rusdraw: "
                    + f"{rusdraw['yymmdd']:06} {rusdraw['hhmmss']:06} {rusdraw['usec']}"
                )
                return None
            if datetime_ not in detector_data_by_event:
                # this means that waveforms are present but didn't pass triggers / cuts
                # print(f"Found datetime not present in nuf output: {datetime_}")
                return None

            fadcti_arr: np.ndarray = rusdraw['fadcti']
            mip_arr: np.ndarray = rusdraw['mip']
            xxyy_arr: np.ndarray = rusdraw['xxyy']
            S_by_detector: Dict[DetectorIdx, np.ndarray] = defaultdict(lambda: np.zeros((2,)))
            for i in range(rusdraw['nofwf']):
                S_by_detector[xxyy_arr[i]] += VEM_PER_MIP * fadcti_arr[i] / mip_arr[i] / DETECTOR_AREA
            return (
                datetime_,
                {xxyy: tuple(S_top_bot) for xxyy, S_top_bot in S_by_detector.items()},  # type: ignore
            )

        S_all: Dict[datetime, Dict[DetectorIdx, Tuple[float, float]]] = dict()
        S_mu: Dict[datetime, Dict[DetectorIdx, Tuple[float, float]]] = dict()
        S_noise: Dict[datetime, Dict[DetectorIdx, Tuple[float, float]]] = dict()
        for dst in (full_dst, mu_dst, noise_dst):
            for _ in dst.events():
                res = process_rusdraw(dst.get_bank('rusdraw'))
                if res is None:
                    continue
                dt, S_by_detector = res
                if dst is full_dst:
                    S_smth = S_all
                elif dst is mu_dst:
                    S_smth = S_mu
                else:
                    S_smth = S_noise
                S_smth[dt] = S_by_detector

        for dt in S_all:
            detector_data = detector_data_by_event[dt]
            for xxyy in S_all[dt]:
                try:
                    # otherwise this detector is not recognized as part of the shower plane fit
                    # or something like that...
                    assert xxyy in detector_data
                    for i in (0, 1):
                        S_noiseless = S_all[dt][xxyy][i] - S_noise[dt][xxyy][i]
                        S_noiseless = max(S_noiseless, 0.0)
                        if i == 1:  # rusdraw convention is 0 for bot, 1 for top
                            detector_data[xxyy].top_S_all = S_noiseless
                            detector_data[xxyy].top_S_mu = S_mu[dt][xxyy][i]
                        else:
                            detector_data[xxyy].bot_S_all = S_noiseless
                            detector_data[xxyy].bot_S_mu = S_mu[dt][xxyy][i]
                except Exception:
                    pass
