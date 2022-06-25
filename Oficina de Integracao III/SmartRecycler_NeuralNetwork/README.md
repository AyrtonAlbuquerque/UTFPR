# Smart Recycler Neural Network

## Python version used
```
Python 3.8
```

## Project packages (Requires a Conda environment)
```
conda install cudatoolkit=11.2 cudnn=8.1 -c=conda-forge
pip install tensorflow
pip install --upgrade tensorflow==2.6.2
pip install numpy
pip install matplotlib
pip install pillow
pip install imgaug
```

### Dataset folder hierarchy
```
Inside the project folder a dataset folder must exists containing all the images divided by classes for your network.
Their division should be: ~10% validation, ~20% for testing and ~70% for training.
The amount of images per class must be the same.

dataset
├── test
│   ├── chips_bag
│   ├── juice_box
│   ├── metal_can
│   ├── paper_ball
│   ├── plastic_bottle
│   └── plastic_cup
├── train
│   ├── chips_bag
│   ├── juice_box
│   ├── metal_can
│   ├── paper_ball
│   ├── plastic_bottle
│   └── plastic_cup
└── validation
    ├── chips_bag
    ├── juice_box
    ├── metal_can
    ├── paper_ball
    ├── plastic_bottle
    └── plastic_cup
    
The dataset images used to train the networks can be found at:
[Dataset](https://drive.google.com/file/d/1eMBLUFDI6Ke2rkUYL8SJIaOmJPe9vk7r/view?usp=sharing)
```

### Compiles the training script
```
python3 train.py
```

### Compiles the prediction script
```
python3 predict.py
