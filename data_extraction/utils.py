from datetime import datetime, timedelta

from dstreader.bank import Bank  # type: ignore


def parse_rusdraw_datetime(rusdraw: Bank) -> datetime:
    yymmdd = rusdraw['yymmdd']
    hhmmss = rusdraw['hhmmss']
    usec = rusdraw['usec']  # this sometimes may exceed 10^6, so we parse it separately
    string = f"{yymmdd:06} {hhmmss:06}"
    try:
        coarse_dt = datetime.strptime(string, r"%y%m%d %H%M%S")
        return coarse_dt + timedelta(microseconds=usec)
    except Exception:
        raise RuntimeError(f"Failed to parse rusdraw datetime: {string = } {yymmdd = } {hhmmss = } {usec = }")
