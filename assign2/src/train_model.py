import os
import json
import joblib
import pandas as pd

from sklearn.model_selection import train_test_split
from sklearn.metrics import (
    accuracy_score,
    precision_score,
    recall_score,
    f1_score,
    classification_report
)

from sklearn.linear_model import LogisticRegression
from sklearn.tree import DecisionTreeClassifier
from sklearn.ensemble import RandomForestClassifier, GradientBoostingClassifier


os.makedirs("models", exist_ok=True)
os.makedirs("reports", exist_ok=True)


FEATURES = [
    "Gender",
    "Age",
    "Scholarship",
    "Hipertension",
    "Diabetes",
    "Alcoholism",
    "SMS_received",
    "waiting_days",
    "previous_no_shows",
    "distance_km"
]


def load_real_data():
    df = pd.read_csv("data/KaggleV2-May-2016.csv")

    df["No-show"] = df["No-show"].map({
        "Yes": 1,
        "No": 0
    })

    df["ScheduledDay"] = pd.to_datetime(df["ScheduledDay"])
    df["AppointmentDay"] = pd.to_datetime(df["AppointmentDay"])

    df["waiting_days"] = (
        df["AppointmentDay"] - df["ScheduledDay"]
    ).dt.days

    df = df[df["waiting_days"] >= 0]

    df["Gender"] = df["Gender"].map({
        "M": 0,
        "F": 1
    })

    df["previous_no_shows"] = 0
    df["distance_km"] = 0

    df = df.dropna()

    return df[FEATURES], df["No-show"]


def load_synthetic_data():
    df = pd.read_csv("data/synthetic_medical_appointments.csv")
    df = df.dropna()

    return df[FEATURES], df["No-show"]


X_real, y_real = load_real_data()
X_synthetic, y_synthetic = load_synthetic_data()

X = pd.concat([X_real, X_synthetic], ignore_index=True)
y = pd.concat([y_real, y_synthetic], ignore_index=True)

X_train, X_test, y_train, y_test = train_test_split(
    X,
    y,
    test_size=0.2,
    random_state=42,
    stratify=y
)

models = {
    "Logistic Regression": LogisticRegression(
        max_iter=3000,
        class_weight="balanced",
        solver="liblinear"
    ),

    "Decision Tree": DecisionTreeClassifier(
        max_depth=12,
        min_samples_split=20,
        min_samples_leaf=10,
        class_weight="balanced",
        random_state=42
    ),

    "Random Forest": RandomForestClassifier(
        n_estimators=400,
        max_depth=20,
        min_samples_split=8,
        min_samples_leaf=3,
        class_weight="balanced",
        random_state=42
    ),

    "Gradient Boosting": GradientBoostingClassifier(
        n_estimators=350,
        learning_rate=0.05,
        max_depth=4,
        random_state=42
    )
}

trained_models = {}
results = []

for name, model in models.items():
    model.fit(X_train, y_train)

    predictions = model.predict(X_test)

    accuracy = accuracy_score(y_test, predictions)
    precision = precision_score(y_test, predictions, zero_division=0)
    recall = recall_score(y_test, predictions, zero_division=0)
    f1 = f1_score(y_test, predictions, zero_division=0)

    trained_models[name] = model

    results.append({
        "model": name,
        "accuracy": round(float(accuracy), 4),
        "precision": round(float(precision), 4),
        "recall": round(float(recall), 4),
        "f1_score": round(float(f1), 4)
    })

    print("\n====================================")
    print(name)
    print("====================================")
    print(classification_report(y_test, predictions, zero_division=0))


model_data = {
    "models": trained_models,
    "features": FEATURES,
    "results": results,
    "datasets_used": [
        "KaggleV2-May-2016.csv",
        "synthetic_medical_appointments.csv"
    ]
}

joblib.dump(model_data, "models/no_show_models.joblib")

metrics = {
    "results": results,
    "features_used": FEATURES,
    "datasets_used": [
        "KaggleV2-May-2016.csv",
        "synthetic_medical_appointments.csv"
    ]
}

with open("reports/metrics.json", "w") as file:
    json.dump(metrics, file, indent=4)

print("\nTraining completed successfully.")
print("All models saved in: models/no_show_models.joblib")
print("Metrics saved in: reports/metrics.json")