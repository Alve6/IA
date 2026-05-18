import streamlit as st
import pandas as pd
import joblib


st.set_page_config(
    page_title="Medical Appointment No-Show Predictor",
    layout="centered"
)

model_data = joblib.load("models/no_show_models.joblib")

models = model_data["models"]
features = model_data["features"]
results = model_data["results"]

st.title("Medical Appointment No-Show Predictor")

st.write(
    "This application predicts the risk of a patient missing a medical appointment."
)

st.sidebar.title("Model Settings")

selected_model_name = st.sidebar.selectbox(
    "Choose prediction algorithm",
    list(models.keys())
)

model = models[selected_model_name]

st.info(f"Selected model: {selected_model_name}")

st.subheader("Patient Information")

gender_text = st.selectbox("Gender", ["Male", "Female"])

age = st.slider("Age", 0, 100, 35)

scholarship_text = st.selectbox(
    "Scholarship / Social Support",
    ["No", "Yes"]
)

hypertension_text = st.selectbox("Hypertension", ["No", "Yes"])

diabetes_text = st.selectbox("Diabetes", ["No", "Yes"])

alcoholism_text = st.selectbox("Alcoholism", ["No", "Yes"])

sms_received_text = st.selectbox("SMS Received", ["No", "Yes"])

waiting_days = st.slider(
    "Days between scheduling and appointment",
    0,
    100,
    7
)

previous_no_shows = st.slider(
    "Previous missed appointments",
    0,
    10,
    0
)

distance_km = st.slider(
    "Distance to clinic in km",
    0.0,
    50.0,
    5.0
)

gender = 0 if gender_text == "Male" else 1
scholarship = 1 if scholarship_text == "Yes" else 0
hypertension = 1 if hypertension_text == "Yes" else 0
diabetes = 1 if diabetes_text == "Yes" else 0
alcoholism = 1 if alcoholism_text == "Yes" else 0
sms_received = 1 if sms_received_text == "Yes" else 0

input_data = pd.DataFrame([{
    "Gender": gender,
    "Age": age,
    "Scholarship": scholarship,
    "Hipertension": hypertension,
    "Diabetes": diabetes,
    "Alcoholism": alcoholism,
    "SMS_received": sms_received,
    "waiting_days": waiting_days,
    "previous_no_shows": previous_no_shows,
    "distance_km": distance_km
}])

input_data = input_data.reindex(columns=features, fill_value=0)

st.subheader("Prediction")

if st.button("Predict No-Show Risk"):
    prediction = model.predict(input_data)[0]

    if hasattr(model, "predict_proba"):
        probability = model.predict_proba(input_data)[0][1]
    else:
        probability = prediction

    if probability >= 0.70:
        st.error("High risk: call the patient to confirm the appointment.")
    elif probability >= 0.40:
        st.warning("Medium risk: send an SMS reminder.")
    else:
        st.success("Low risk: no extra action needed.")

    st.write(
        f"Probability of missing the appointment: **{probability * 100:.2f}%**"
    )

st.subheader("Selected Model Performance")

results_df = pd.DataFrame(results)

selected_result = results_df[
    results_df["model"] == selected_model_name
]

st.dataframe(selected_result, use_container_width=True)

st.subheader("Comparison Between Algorithms")

st.dataframe(results_df, use_container_width=True)

st.subheader("Input Data Used by the Model")

st.dataframe(input_data, use_container_width=True)