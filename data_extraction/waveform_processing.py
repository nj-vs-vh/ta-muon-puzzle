import numpy as np
from datetime import datetime
from collections import defaultdict
from dstreader import DstFile
from dstreader.bank import Bank


from typing import Optional, Tuple, Dict

from .reconstruction import DetectorDataByEvent, DetectorIdx
from .filenames import get_dst_file, DstFileType


def get_datetime(rusdraw: Bank) -> datetime:
    return datetime.strptime(
        f"{rusdraw['yymmdd']:06} {rusdraw['hhmmss']:06} {rusdraw['usec']}", r"%y%m%d %H%M%S %f"
    )


def assemble_waveforms(wf_part_by_id: Dict[DetectorIdx, Dict[int, np.ndarray]]) -> Tuple[np.ndarray, np.ndarray]:
    """Assemble waveforms from their 128-bin parts"""
    wf_top = []
    wf_bot = []
    for wf_id in sorted(wf_part_by_id.keys()):
        wf_top.extend(wf_part_by_id[wf_id][1])  # 1 is top in rusdraw!
        wf_bot.extend(wf_part_by_id[wf_id][0])  # 0 is bot in rusdraw!
    return np.array(wf_top), np.array(wf_bot)


def add_mu_ratios_to_detector_data(dat_name: str, detector_data_by_event: DetectorDataByEvent):
    full_dst = DstFile(get_dst_file(dat_name, type=DstFileType.FULL))
    mu_dst = DstFile(get_dst_file(dat_name, type=DstFileType.MU))
    noise_dst = DstFile(get_dst_file(dat_name, type=DstFileType.NOISE))

    with full_dst, mu_dst, noise_dst:

        #                                                                          xxyy: waveform integrals top/bot
        def process_rusdraw(rusdraw: Bank) -> Optional[Tuple[datetime, Dict[DetectorIdx, Tuple[float, float]]]]:
            try:
                datetime_ = get_datetime(rusdraw)
            except ValueError:
                print(
                    "Can't parse datetime from rusdraw: "
                    + f"{rusdraw['yymmdd']:06} {rusdraw['hhmmss']:06} {rusdraw['usec']}"
                )
                return
            if datetime_ not in detector_data_by_event:
                # this means that waveforms are present but didn't pass triggers / cuts
                # print(f"Found datetime not present in nuf output: {datetime_}")
                return None

            fadcti_arr: np.ndarray = rusdraw['fadcti']
            xxyy_arr: np.ndarray = rusdraw['xxyy']
            fadc_integrals_by_detector = defaultdict(lambda: np.zeros((2,)))
            for i in range(rusdraw['nofwf']):
                fadc_integrals_by_detector[xxyy_arr[i]] += fadcti_arr[i]

            return (
                datetime_,
                {xxyy: tuple(top_bot_integrals) for xxyy, top_bot_integrals in fadc_integrals_by_detector.items()},
            )

        all_integrals, mu_integrals, noise_integrals = dict(), dict(), dict()
        for dst in (full_dst, mu_dst, noise_dst):
            for _ in dst.events():
                res = process_rusdraw(dst.get_bank('rusdraw'))
                if res is None:
                    continue
                dt, integrals_by_detector = res
                if dst is full_dst:
                    integrals = all_integrals
                elif dst is mu_dst:
                    integrals = mu_integrals
                else:
                    integrals = noise_integrals
                integrals[dt] = integrals_by_detector

        for dt in all_integrals:
            detector_data = detector_data_by_event[dt]
            for xxyy in all_integrals[dt]:
                try:
                    # otherwise this detector is not recognized as part of the shower plane fit
                    # or something like that...
                    assert xxyy in detector_data
                    mu_ratios = []
                    for i in (0, 1):
                        full_signal_integral_wo_noise = all_integrals[dt][xxyy][i] - noise_integrals[dt][xxyy][i]
                        assert not np.isclose(full_signal_integral_wo_noise, 0)
                        mu_ratio = mu_integrals[dt][xxyy][i] / full_signal_integral_wo_noise
                        if mu_ratio > 1:
                            # pure muon signals sometimes get fractions like 1.001 because of integral estimation error
                            # print(f"Anomalous mu ratio {mu_ratio}: {dt} {xxyy}")
                            mu_ratio = 1.0
                        mu_ratios.append(mu_ratio)

                    detector_data[xxyy].mu_signal_ratio_top = mu_ratios[1]  # rusdraw convention is 0 for bot, 1 for top
                    detector_data[xxyy].mu_signal_ratio_bot = mu_ratios[0]
                except Exception:
                    pass
