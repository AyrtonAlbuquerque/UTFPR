PORT = 8081
JWT_KEY = 'secret_key'
STORAGE_PATH = 'images'
MODEL_PATH = 'models/EfficientNetB4_150E_6C.hdf5'
CLASSES = ['metal_can', 'plastic_bottle', 'paper_ball', 'plastic_cup', 'juice_box', 'chips_bag']
LOAD_TEST_DATA = True
INPUT_SHAPE = (380, 380)  # shape of the image (dependent on network)
PREDICTION_THRESHOLD = 0.8  # if a prediction is more than this level of confidence, it will be considered
