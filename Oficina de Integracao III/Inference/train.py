import os
import numpy as np
import tensorflow as tf
import matplotlib.pyplot as plt
import matplotlib.image as mpimg
from tensorflow.keras.applications import EfficientNetB4
from tensorflow.keras import Sequential
from tensorflow.keras import layers
from tensorflow.keras.callbacks import ModelCheckpoint
from tensorflow.keras.utils import image_dataset_from_directory
from silence_tensorflow import silence_tensorflow
from config import CLASSES, BATCH_SIZE, EPOCHS, INPUT_SHAPE, OUTPUT_DIR, LEARNING_RATE, ACTIVATION

# Disable Info and Warning messages
silence_tensorflow()


def plot_hist(hist, model):
    plt.plot(hist.history["loss"])
    plt.plot(hist.history["val_loss"])
    plt.title("model loss")
    plt.ylabel("loss")
    plt.xlabel("epoch")
    plt.legend(["train", "validation"], loc="upper left")
    plt.savefig(os.path.join('models', f"{model}_LOSS.jpg"))
    plt.cla()
    plt.plot(hist.history["accuracy"])
    plt.plot(hist.history["val_accuracy"])
    plt.title("model accuracy")
    plt.ylabel("accuracy")
    plt.xlabel("epoch")
    plt.legend(["train", "validation"], loc="upper left")
    plt.savefig(os.path.join('models', f"{model}_ACC.jpg"))


def main():
    train_ds = image_dataset_from_directory(
        os.path.join(OUTPUT_DIR, 'train'),
        image_size=INPUT_SHAPE,
        labels="inferred",
        label_mode="categorical",
        class_names=CLASSES,
        batch_size=BATCH_SIZE,
        shuffle=True,
    )

    test_ds = image_dataset_from_directory(
        os.path.join(OUTPUT_DIR, 'test'),
        image_size=INPUT_SHAPE,
        labels="inferred",
        label_mode="categorical",
        class_names=CLASSES,
        batch_size=BATCH_SIZE,
        shuffle=True,
    )

    inputs = layers.Input(shape=(INPUT_SHAPE[0], INPUT_SHAPE[1], 3))
    model = EfficientNetB4(weights="imagenet", include_top=False, input_tensor=inputs)
    model.trainable = False

    # Rebuild top
    x = layers.GlobalAveragePooling2D(name="avg_pool")(model.output)
    x = layers.BatchNormalization()(x)

    top_dropout_rate = 0.2
    x = layers.Dropout(top_dropout_rate, name="top_dropout")(x)
    outputs = layers.Dense(len(CLASSES), activation=ACTIVATION, name="pred")(x)

    # Compile
    model = tf.keras.Model(inputs, outputs, name="EfficientNetB4")
    optimizer = tf.keras.optimizers.SGD(learning_rate=LEARNING_RATE)
    prefix_name = f"EfficientNetB4_{EPOCHS}E_{len(CLASSES)}C"
    model_name = os.path.join('models', f"{prefix_name}.hdf5")

    model.compile(
        optimizer=optimizer,
        loss="categorical_crossentropy",
        metrics=["accuracy"]
    )

    checkpoint = ModelCheckpoint(
        model_name,
        monitor='accuracy',
        verbose=1,
        save_best_only=True,
        mode='auto',
        period=1
    )

    history = model.fit(
        train_ds,
        epochs=EPOCHS,
        validation_data=test_ds,
        verbose=1,
        use_multiprocessing=True,
        workers=4,
        callbacks=[checkpoint],
    )

    plot_hist(history, f"{prefix_name}")


if __name__ == '__main__':
    main()
