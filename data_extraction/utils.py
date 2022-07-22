from datetime import datetime

from dstreader.bank import Bank  # type: ignore


def parse_rusdraw_datetime(rusdraw: Bank) -> datetime:
    return datetime.strptime(f"{rusdraw['yymmdd']:06} {rusdraw['hhmmss']:06} {rusdraw['usec']}", r"%y%m%d %H%M%S %f")
