import pandas as pd
import numpy as np
import joblib
import json

from sklearn.model_selection import train_test_split
from sklearn.ensemble import RandomForestClassifier
from sklearn.metrics import accuracy_score, classification_report

#load data
df = pd.read_csv("data/KaggleV2-May-2016.csv")

#data cleaning
#convert target variable
df["No-show"] = df["No-show"].map({
    "Yes": 1,
    "No": 0
})

#convert dates
df["ScheduledDay"] = pd.to_datetime(df["ScheduledDay"])
df["AppointmentDay"] = pd.to_datetime(df["AppointmentDay"])

#create waiting days feature
df["waiting_days"] = (
    df["AppointmentDay"] - df["ScheduledDay"]
).dt.days

#remove negative waiting days
df = df[df["waiting_days"] >= 0]

#convert gender
df["Gender"] = df["Gender"].map({
    "M": 0,
    "F": 1
})


#select features
features = [
    "Gender",
    "Age",
    "Scholarship",
    "Hipertension",
    "Diabetes",
    "Alcoholism",
    "SMS_received",
    "waiting_days"
]

X = df[features]
y = df["No-show"]

#train test split
X_train, X_test, y_train, y_test = train_test_split(
    X,
    y,
    test_size=0.2,
    random_state=42
)


#train model
model = RandomForestClassifier(
    n_estimators=100,
    random_state=42
)

model.fit(X_train, y_train)

#evaluation
predictions = model.predict(X_test)

accuracy = accuracy_score(y_test, predictions)

print("\nModel Accuracy:")
print(round(accuracy * 100, 2), "%")

print("\nClassification Report:")
print(classification_report(y_test, predictions))

#save model
joblib.dump(model, "models/no_show_model.joblib")

print("\nModel saved successfully!")

#save metrics
metrics = {
    "accuracy": round(float(accuracy), 4)
}

with open("reports/metrics.json", "w") as f:
    json.dump(metrics, f, indent=4)

print("Metrics saved successfully!")