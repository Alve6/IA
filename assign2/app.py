import streamlit as st
import pandas as pd
import joblib


#load model
model = joblib.load("models/no_show_model.joblib")


st.set_page_config(
    page_title="Medical Appointment No-Show Predictor",
    layout="centered"
)

st.title("Medical Appointment No-Show Predictor")

st.write(
    "This app predicts whether a patient is likely to miss a medical appointment."
)


st.subheader("Patient Information")

gender_text = st.selectbox(
    "Gender",
    ["Male", "Female"]
)

age = st.slider(
    "Age",
    min_value=0,
    max_value=100,
    value=35
)

scholarship = st.selectbox(
    "Scholarship / Social Support",
    ["No", "Yes"]
)

hypertension = st.selectbox(
    "Hypertension",
    ["No", "Yes"]
)

diabetes = st.selectbox(
    "Diabetes",
    ["No", "Yes"]
)

alcoholism = st.selectbox(
    "Alcoholism",
    ["No", "Yes"]
)

sms_received = st.selectbox(
    "SMS Received",
    ["No", "Yes"]
)

waiting_days = st.slider(
    "Days between scheduling and appointment",
    min_value=0,
    max_value=100,
    value=7
)


#convert inputs to numbers
gender = 0 if gender_text == "Male" else 1
scholarship = 1 if scholarship == "Yes" else 0
hypertension = 1 if hypertension == "Yes" else 0
diabetes = 1 if diabetes == "Yes" else 0
alcoholism = 1 if alcoholism == "Yes" else 0
sms_received = 1 if sms_received == "Yes" else 0


input_data = pd.DataFrame([{
    "Gender": gender,
    "Age": age,
    "Scholarship": scholarship,
    "Hipertension": hypertension,
    "Diabetes": diabetes,
    "Alcoholism": alcoholism,
    "SMS_received": sms_received,
    "waiting_days": waiting_days
}])


st.subheader("Prediction")

if st.button("Predict"):
    prediction = model.predict(input_data)[0]
    probability = model.predict_proba(input_data)[0][1]

    if prediction == 1:
        st.error("High risk: The patient is likely to miss the appointment.")
    else:
        st.success("Low risk: The patient is likely to attend the appointment.")

    st.write(f"Probability of missing the appointment: **{probability * 100:.2f}%**")


st.subheader("Input Data")
st.dataframe(input_data)