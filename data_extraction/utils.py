from datetime import datetime

from dstreader.bank import Bank  # type: ignore


def parse_rusdraw_datetime(rusdraw: Bank) -> datetime:
    yymmdd = rusdraw['yymmdd']
    hhmmss = rusdraw['hhmmss']
    usec = rusdraw['usec']
    string = f"{yymmdd:06} {hhmmss:06} {usec}"
    try:
        return datetime.strptime(string, r"%y%m%d %H%M%S %f")
    except Exception:
        raise RuntimeError(f"Failed to parse rusdraw datetime: {string = } {yymmdd = } {hhmmss = } {usec = }")
