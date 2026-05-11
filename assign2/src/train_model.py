import pandas as pd
import joblib
import json

from sklearn.model_selection import train_test_split
from sklearn.ensemble import RandomForestClassifier
from sklearn.metrics import accuracy_score, classification_report

#load data
df = pd.read_csv("data/KaggleV2-May-2016.csv")

#data cleaning
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

#use only top 10 neighbourhoods
top_neighbourhoods = df["Neighbourhood"].value_counts().head(10).index

df["Neighbourhood"] = df["Neighbourhood"].apply(
    lambda x: x if x in top_neighbourhoods else "Other"
)

#one-hot encoding for neighbourhood
df = pd.get_dummies(
    df,
    columns=["Neighbourhood"],
    drop_first=True
)

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

#add neighbourhood columns automatically
neighbourhood_features = [
    col for col in df.columns
    if col.startswith("Neighbourhood_")
]

features = features + neighbourhood_features

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
    max_depth=10,
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


#save model and features
model_data = {
    "model": model,
    "features": features,
    "top_neighbourhoods": list(top_neighbourhoods)
}

joblib.dump(model_data, "models/no_show_model.joblib")

print("\nModel saved successfully!")


#save metrics
metrics = {
    "accuracy": round(float(accuracy), 4),
    "features_used": features,
    "top_neighbourhoods": list(top_neighbourhoods)
}

with open("reports/metrics.json", "w") as f:
    json.dump(metrics, f, indent=4)

print("Metrics saved successfully!")