
import json
import pathlib
from dataclasses import dataclass
from tempfile import NamedTemporaryFile

import numpy as np
import xgboost as xgb

DEFAULT_MODELS_DIR = pathlib.Path(__file__).parent / "trained_models"
DEFAULT_MODELS_DIR.mkdir(exist_ok=True)


@dataclass
class SMuEstimator:
    name: str
    xgbost_model: xgb.Booster
    a_95cl: float

    def confidence_interval_halfwidth(self, S_mu_predicted: float) -> float:
        """Predicted S_mu +/- this value gives the 95% confidence interval"""
        return self.a_95cl * np.log(S_mu_predicted + 1)

    @staticmethod
    def _file_name(name: str) -> str:
        return f"{name}-regressor.json"

    def dump(self, models_dir: pathlib.Path = DEFAULT_MODELS_DIR):
        file_path = models_dir / self._file_name(self.name)
        self.xgbost_model.save_model(file_path)
        with open(file_path) as f:
            dump = json.load(f)
        dump["a_95cl"] = self.a_95cl
        with open(file_path, "w") as f:
            json.dump(dump, f)

    @classmethod
    def load(cls, name: str, models_dir: pathlib.Path = DEFAULT_MODELS_DIR) -> "SMuEstimator":
        file_path = models_dir / cls._file_name(name)
        with open(file_path) as f:
            dump = json.load(f)
        a_95cl = dump.pop("a_95cl")
        with NamedTemporaryFile() as tempfile:
            with open(tempfile.name, "w") as f:
                json.dump(dump, f)
            xgb_model = xgb.Booster()
            xgb_model.load_model(tempfile.name)
        return SMuEstimator(
            name=name,
            xgbost_model=xgb_model,
            a_95cl=a_95cl,
        )
