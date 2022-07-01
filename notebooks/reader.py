from typing import Tuple
import pandas as pd
from pathlib import Path
import numpy as np


ROOT_DIR = Path(__file__).parent.parent
SAMPLE_CSV = ROOT_DIR / "data/muon-puzzle-data-proton-fraction.csv"


def read_data_raw() -> Tuple[pd.DataFrame, pd.DataFrame]:
    df = pd.read_csv(SAMPLE_CSV)
    x_columns = df.columns[2:-4]
    y_columns = df.columns[-4:]
    X = df[x_columns]
    y = df[y_columns].clip(lower=0)
    return X, y


def read_data_preprocessed() -> Tuple[pd.DataFrame, np.ndarray]:
    df = pd.read_csv(SAMPLE_CSV)
    x_columns = df.columns[2:-4]
    y_columns = df.columns[-4:]
    X = df[x_columns]
    y = df[y_columns].clip(lower=0)
    S_mu_top = y["top_integral_mu"].to_numpy() * 0.009494832094158289
    S_mu_bot = y["bot_integral_mu"].to_numpy() * 0.009718791373649492
    return X, 0.5 * (S_mu_top + S_mu_bot)

