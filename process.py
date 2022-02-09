from pathlib import Path
import subprocess


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


def reconstruct(events_dst: Path) -> str:
    events_dst_name = events_dst.name

    temp_dst_dir = events_dst.parent / "temp_dst"
    temp_dst_dir.mkdir(exist_ok=True)

    rufptn_out = temp_dst_dir / (events_dst_name + ".rufptn.dst.gz")
    sdtrgbk_out = temp_dst_dir / (events_dst_name + ".sdtrgbk.dst.gz")
    rufldf_out = temp_dst_dir / (events_dst_name + ".rufldf.dst.gz")

    res = subprocess.run(
        ["rufptn.run", events_dst, "-o1f", rufptn_out, "-v", VERBOSITY, "-f"],
        capture_output=True,
    )
    check_result(res)

    res = subprocess.run(
        ["sdtrgbk.run", rufptn_out, "-o1f", sdtrgbk_out, "-v", VERBOSITY, "-f"],
        capture_output=True,
    )
    check_result(res)

    res = subprocess.run(
        ["rufldf.run", sdtrgbk_out, "-o1f", rufldf_out, "-v", VERBOSITY, "-f", "-no_bw"],
        capture_output=True,
    )
    check_result(res)

    res = subprocess.run(
        ["nuf.i12g.run", rufldf_out, "-MU"],
        capture_output=True,
    )
    check_result(res)

    print(f"OK {events_dst_name}\n")

    return res.stdout.decode("utf-8")


def process_dat(datname: str):
    full_dst = DATA_DIR / (datname + "_0_gea.dst.gz")
    nuf_mu_output = reconstruct(full_dst)
    print(nuf_mu_output.splitlines()[0])
    

if __name__ == "__main__":
    process_dat("DAT009512")
