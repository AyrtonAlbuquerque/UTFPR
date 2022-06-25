import numpy as np
from tensorflow.keras.preprocessing import image
from tensorflow.keras.applications.efficientnet import preprocess_input, decode_predictions
from keras.models import load_model
from src.config import MODEL_PATH, PREDICTION_THRESHOLD, INPUT_SHAPE, CLASSES
from silence_tensorflow import silence_tensorflow

# Disable tensorflow messages
silence_tensorflow()

def load_image(img_path):
  img = image.load_img(img_path, target_size=INPUT_SHAPE)
  x = image.img_to_array(img)
  x = np.expand_dims(x, axis=0)
  x = preprocess_input(x)
  return x

def decode_predictions(predictions):
  print('prediction', predictions)
  max_confidence_prediction = max(predictions)
  print('max_confidence_prediction', max_confidence_prediction)
  if max_confidence_prediction > PREDICTION_THRESHOLD:
    return np.where(predictions == max_confidence_prediction)[0][0].item()
  return -1


# load model
model = load_model(MODEL_PATH)

def classify(image_path):
  image = load_image(image_path)
  prediction = model.predict(image)
  classification = decode_predictions(prediction[0])
  print('Classified as:', CLASSES[classification] if classification >= 0 else 'other')
  return classification