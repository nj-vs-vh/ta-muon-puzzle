from pathlib import Path


CURRENT_DIR = Path(__name__).parent

ROOT_DIR = (CURRENT_DIR / "..").resolve()

TRAINING_DATA_CSV = ROOT_DIR / "data/mu-frac-iron-shower-signals.csv"
