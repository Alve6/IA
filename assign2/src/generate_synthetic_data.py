import os
import numpy as np
import pandas as pd

np.random.seed(42)

os.makedirs("data", exist_ok=True)

n = 8000

gender = np.random.choice([0, 1], n)
age = np.random.randint(0, 100, n)
scholarship = np.random.binomial(1, 0.12, n)
hypertension = np.random.binomial(1, 0.22, n)
diabetes = np.random.binomial(1, 0.09, n)
alcoholism = np.random.binomial(1, 0.04, n)
sms_received = np.random.binomial(1, 0.45, n)
waiting_days = np.random.randint(0, 60, n)

previous_no_shows = np.random.poisson(0.5, n)
distance_km = np.random.gamma(2.2, 2.5, n).round(1)

risk = (
    0.08
    + 0.014 * waiting_days
    + 0.16 * previous_no_shows
    + 0.018 * distance_km
    - 0.18 * sms_received
    + 0.07 * scholarship
    + 0.06 * diabetes
    + 0.05 * hypertension
    + 0.03 * alcoholism
)

risk = np.clip(risk, 0.02, 0.90)

no_show = np.random.binomial(1, risk)

df = pd.DataFrame({
    "Gender": gender,
    "Age": age,
    "Scholarship": scholarship,
    "Hipertension": hypertension,
    "Diabetes": diabetes,
    "Alcoholism": alcoholism,
    "SMS_received": sms_received,
    "waiting_days": waiting_days,
    "previous_no_shows": previous_no_shows,
    "distance_km": distance_km,
    "No-show": no_show
})

df.to_csv("data/synthetic_medical_appointments.csv", index=False)

print("Synthetic dataset created successfully.")
print(df.head())
print(df["No-show"].value_counts(normalize=True))