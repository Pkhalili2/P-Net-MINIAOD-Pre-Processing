#!/bin/bash

PREFIX=particlenet
MODEL_CONFIG=particlenet_pf.py
DATA_CONFIG=pf_points_features.yaml
PATH_TO_SAMPLES=$1
CLUSTER=$2
PROCESS=$3
WORKDIR=`pwd`

# CUDA environment setup
tar -xzf pnet_env.tar.gz
source pnet_env/bin/activate

tar -xzf weaver-benchmark.tar.gz
cd weaver-benchmark/weaver/

echo "=== Starting ParticleNet training ==="

# Training, using 2 GPU
python train.py \
 --data-train ${PATH_TO_SAMPLES}'top_train.root' \
 --data-val ${PATH_TO_SAMPLES}'top_val.root' \
 --fetch-by-file --fetch-step 1 --num-workers 3 \
 --data-config top_tagging/data/${DATA_CONFIG} \
 --network-config top_tagging/networks/${MODEL_CONFIG} \
 --model-prefix output/${PREFIX} \
--tensorboard runs/${PREFIX} \
--gpus 0,1 --batch-size 512 --start-lr 5e-3 --num-epochs 20 --optimizer ranger \
 --log output/${PREFIX}.train.log

echo "=== Finished ParticleNet training ==="

echo "=== Starting ParticleNet pridicting==="

# Predicting score, using 2 GPU
python train.py --predict \
 --data-test ${PATH_TO_SAMPLES}'top_test.root' \
 --num-workers 3 \
 --data-config top_tagging/data/${DATA_CONFIG} \
 --network-config top_tagging/networks/${MODEL_CONFIG} \
 --model-prefix output/${PREFIX}_best_epoch_state.pt \
 --gpus 0,1 --batch-size 512 \
 --predict-output output/${PREFIX}_predict.root

echo "=== Finished ParticleNet pridicting==="

PREFIX=deepak8
MODEL_CONFIG=deepak8_pf.py
DATA_CONFIG=pf_features.yaml

echo "=== Starting DeepAK8 training==="

# Training, using 2 GPU
python train.py \
 --data-train ${PATH_TO_SAMPLES}'top_train.root' \
 --data-val ${PATH_TO_SAMPLES}'top_val.root' \
 --fetch-by-file --fetch-step 1 --num-workers 3 \
 --data-config top_tagging/data/${DATA_CONFIG} \
 --network-config top_tagging/networks/${MODEL_CONFIG} \
 --model-prefix output/${PREFIX} \
--tensorboard runs/${PREFIX} \
--gpus 0,1 --batch-size 512 --start-lr 5e-3 --num-epochs 20 --optimizer ranger \
 --log output/${PREFIX}.train.log

echo "=== Finished DeepAK8 training==="

echo "=== Starting DeepAK8 pridicting==="

# Predicting score, using 2 GPU
python train.py --predict \
 --data-test ${PATH_TO_SAMPLES}'top_test.root' \
 --num-workers 3 \
 --data-config top_tagging/data/${DATA_CONFIG} \
 --network-config top_tagging/networks/${MODEL_CONFIG} \
 --model-prefix output/${PREFIX}_best_epoch_state.pt \
 --gpus 0,1 --batch-size 512 \
 --predict-output output/${PREFIX}_predict.root

echo "=== Finished DeepAK8 pridicting==="

PREFIX=mlp
MODEL_CONFIG=mlp_pf.py

echo "=== Starting MLP training==="

# Training, using 2 GPU
python train.py \
 --data-train ${PATH_TO_SAMPLES}'top_train.root' \
 --data-val ${PATH_TO_SAMPLES}'top_val.root' \
 --fetch-by-file --fetch-step 1 --num-workers 3 \
 --data-config top_tagging/data/${DATA_CONFIG} \
 --network-config top_tagging/networks/${MODEL_CONFIG} \
 --model-prefix output/${PREFIX} \
--tensorboard runs/${PREFIX} \
--gpus 0,1 --batch-size 512 --start-lr 5e-3 --num-epochs 20 --optimizer ranger \
 --log output/${PREFIX}.train.log

echo "=== Finished MLP training==="

echo "=== Starting MLP pridicting==="

# Predicting score, using 2 GPU
python train.py --predict \
 --data-test ${PATH_TO_SAMPLES}'top_test.root' \
 --num-workers 3 \
 --data-config top_tagging/data/${DATA_CONFIG} \
 --network-config top_tagging/networks/${MODEL_CONFIG} \
 --model-prefix output/${PREFIX}_best_epoch_state.pt \
 --gpus 0,1 --batch-size 512 \
 --predict-output output/${PREFIX}_predict.root
 
echo "=== Finished MLP pridicting===" 

echo "=== Creating ROC curve==="

python roc_compare.py

echo "=== Finished ROC Curve==="

cd $WORKDIR

echo "=== Creating Output Tar==="

if [ -d "weaver-benchmark/weaver/runs/" ]; then
    tar -caf output.tar weaver-benchmark/weaver/output/ weaver-benchmark/weaver/runs/
else
    tar -caf output.tar weaver-benchmark/weaver/output/
fi

echo "=== Copying Output Tar==="

cp weaver-benchmark/weaver/output.tar /nfs_scratch/pkhalili2/particlenet_outputs/output_${CLUSTER}_${PROCESS}.tar
echo "=== Run Complete==="
