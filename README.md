# P-Net-MINIAOD-Pre-Processing: Jet Data Preprocessing Framework for High Energy Physics Analysis
This repository contains a set of CMSSW modules and helper scripts designed to preprocess particle physics data (specifically from MINIAOD files) into flat `TTree` ROOT files. The resulting files are optimized for subsequent analysis, particularly for training machine learning models.

The framework is configured to handle both **AK8** and **AK15** jets and automatically splits the output into **training**, **validation**, and **test** datasets.

---
## Getting Started: Setup and Compilation

Follow these steps to set up the environment and compile the code. These commands should be run from your main project directory (e.g., `~/CMSSW_14_0_15/`).

1.  **Set up the CMS Environment**:
    This command loads the necessary software and environment variables for CMSSW.
    ```bash
    source /cvmfs/cms.cern.ch/cmsset_default.sh
    ```

2.  **Source Local WISC Environment (if applicable)**:
    This sets up any site-specific configurations for running at the University of Wisconsin-Madison.
    ```bash
    source /afs/hep.wisc.edu/cms/setup/bashrc
    ```

3.  **Navigate to the CMSSW Source Directory**:
    Go into the `src` directory of your CMSSW release area.
    ```bash
    cd CMSSW_14_0_15/src
    ```

4.  **Set up the Local CMSSW Environment**:
    This command sets environment variables specific to your local CMSSW area.
    ```bash
    cmsenv
    ```

5.  **Compile the Code**:
    This compiles all the C++ modules in your project. The `-j8` flag uses 8 parallel processes to speed up compilation.
    ```bash
    scram b -j8
    ```

---
## Running the Preprocessing

After compiling, you can run the preprocessing to generate the flat ROOT trees.

### Step 1: Configure the Job
Before running, you must edit the Python configuration file (e.g., `runFlatTree_ak15cfg.py`) to point to your data and specify the sample type.

* **Navigate to the test directory**:
    ```bash
    cd $CMSSW_BASE/src/MyAnalysis/AK8FlatTreeProducer/test
    ```
* **Open the configuration file** (e.g., `runFlatTree_ak15cfg.py`) in a text editor.
* **Update the `input_dirs` list** with the paths to your MINIAOD files.
    ```python
    input_dirs = [
        "/hdfs/store/user/your_name/path/to/your/data_sample_1",
        "/hdfs/store/user/your_name/path/to/your/data_sample_2",
    ]
    ```
* **Set the `isSignal` flag**: Change this to `True` for signal samples and `False` for background.
    ```python
    # For a background sample
    isSignal = cms.bool(False)

    # For a signal sample
    isSignal = cms.bool(True)
    ```

### Step 2: Run the Job
Execute the job using the `cmsRun` command. The command below is for AK15 jets; use the corresponding `_ak8cfg.py` file for AK8 jets.

```bash
cmsRun runFlatTree_ak15cfg.py
```
### Step 3: Check the Output
The process will produce three ROOT files in your current directory, split into an 80/10/10 ratio:
* `ak15_train.root` (80% of events)
* `ak15_val.root` (10% of events)
* `ak15_test.root` (10% of events)

---
## Post-processing and Utility Scripts

This project includes several scripts for managing and inspecting your data *after* preprocessing.

### Combining ROOT Files with `hadd`
If you run the preprocessing job in batches on different sets of files, you will end up with multiple sets of `train`, `val`, and `test` files. You can merge them using the standard ROOT utility `hadd`.

```bash
# Example: Combine all training files into one
hadd ak15_train_combined.root ak15_train_part1.root ak15_train_part2.root ...
```
### Other Utility Scripts

* **`eventTest.py`**: A quick diagnostic script to count the number of events in the "Events" tree of any ROOT file.
    * **Usage**: Edit the `filename` variable in the script and run `python3 eventTest.py`.

* **`SingleTreeExtractor.cc` & `runTTreeCopycfg.py`**: A CMSSW module that performs a simple, complete copy of the "Events" TTree from one ROOT file to another.
    * **Usage**: It's designed for tasks like cleaning or stripping a file. Configure the input and output file paths in `runTTreeCopycfg.py` and execute it with `cmsRun`.

* **`split.py`**: A post-processing script that provides an **alternative** way to split data. If you have one massive dataset, this script can be used to manually split that file into train/val/test sets. **It is not needed if you use the updated C++ producers.**

---

## Running Machine Learning Training (Example with ParticleNet)

Once you have preprocessed your data into `train`, `val`, and `test` ROOT files, you can use them to train a machine learning model like ParticleNet, MLP, or DeepAK8. The following is an example workflow using the `weaver-benchmark` framework.

### Step 1: Clone the ML Framework Repository
First, you need to download the software for training the models. This example uses a framework that contains implementations for various taggers. The `--recursive` flag is important as it downloads necessary sub-modules.

```bash
git clone --recursive [https://github.com/colizz/weaver-benchmark.git](https://github.com/colizz/weaver-benchmark.git)
cd weaver-benchmark
```
Note: Depending on the framework, you may need to perform additional setup steps, such as creating symbolic links or installing specific python packages. Refer to the framework's documentation at https://cms-ml.github.io/documentation/inference/particlenet.html.

### Step 2: Configure Training Variables

```bash
# Set a prefix for all output file names
PREFIX=particlenet_ak15

# Specify the model and data configuration files
# (Change these for different models like MLP, DeepAK8, etc.)
MODEL_CONFIG=particlenet_pf.py
DATA_CONFIG=pf_points_features.yaml

# Set the path to your preprocessed ROOT files
# (This is the directory containing your train/val/test files)
PATH_TO_SAMPLES=/path/to/your/preprocessed/data/
```
### Step 3: Run Training
This command starts the training process. It uses the training and validation datasets, loads the model and data configurations, and saves the trained model checkpoints and log files to an `output/` directory.

```bash
python weaver/train.py \
    --data-train "${PATH_TO_SAMPLES}ak15_train.root" \
    --data-val "${PATH_TO_SAMPLES}ak15_val.root" \
    --data-config weaver/top_tagging/data/${DATA_CONFIG} \
    --network-config weaver/top_tagging/networks/${MODEL_CONFIG} \
    --model-prefix "output/${PREFIX}" \
    --gpus "0,1" \
    --batch-size 128 \
    --start-lr 5e-3 \
    --num-epochs 20 \
    --optimizer ranger \
    --fetch-by-file \
    --num-workers 3 \
    --log "output/${PREFIX}.train.log"
```

### Step 4: Run Inference (Prediction)
After training is complete, this command runs the best-trained model on the test dataset. It loads the model state (_best_epoch_state.pt) and saves the network's predictions into a new ROOT file for performance evaluation (e.g., making ROC curves).

```bash
python weaver/train.py --predict \
    --data-test "${PATH_TO_SAMPLES}ak15_test.root" \
    --data-config weaver/top_tagging/data/${DATA_CONFIG} \
    --network-config weaver/top_tagging/networks/${MODEL_CONFIG} \
    --model-prefix "output/${PREFIX}_best_epoch_state.pt" \

    --gpus "0,1" \
    --batch-size 128 \
    --num-workers 3 \
    --predict-output "output/${PREFIX}_predict.root"
```

