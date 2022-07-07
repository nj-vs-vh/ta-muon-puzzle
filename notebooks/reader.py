from typing import Tuple
import pandas as pd
from pathlib import Path
import numpy as np


ROOT_DIR = Path(__file__).parent.parent
SAMPLE_CSV = ROOT_DIR / "data/muon-puzzle-data-iron-fraction-v3.csv"


def read_data_raw() -> Tuple[pd.DataFrame, pd.DataFrame]:
    df = pd.read_csv(SAMPLE_CSV)
    x_columns = df.columns[2:-4]
    y_columns = df.columns[-4:]
    X = df[x_columns]
    y = df[y_columns].clip(lower=0)
    return X, y


def read_data_preprocessed() -> Tuple[pd.DataFrame, np.ndarray]:
    X, y = read_data_raw()
    return X, 0.5 * (y["top_S_mu"].to_numpy() + y["bot_S_mu"].to_numpy())
