import uproot
import numpy as np
from sklearn.metrics import roc_curve, auc
import matplotlib.pyplot as plt

def load_scores_labels(root_path, score_branch="score_is_signal_new", label_branch="is_signal_new"):
    tree = uproot.open(root_path)["Events"]
    scores = tree[score_branch].array(library="np")
    labels = tree[label_branch].array(library="np")
    return scores, labels

# Define model output files and labels
models = {
    "ParticleNet": "output/particlenet_predict.root",
    "MLP":         "output/mlp_predict.root",
    "DeepAK8":     "output/deepak8_predict.root"
}

# ROC Curve Plot
plt.figure(figsize=(6,5))

for model_name, path in models.items():
    scores, labels = load_scores_labels(path)
    fpr, tpr, _ = roc_curve(labels, scores)
    roc_auc = auc(fpr, tpr)
    plt.plot(fpr, tpr, label=f"{model_name} (AUC = {roc_auc:.5f})")

plt.plot([0, 1], [0, 1], linestyle='--', color='gray')
plt.xlabel("False Positive Rate")
plt.ylabel("True Positive Rate")
plt.title("ROC Curve Comparison")
plt.legend(loc="lower right")
plt.grid(True)
plt.tight_layout()
plt.savefig("roc_comparison.png", dpi=300)
plt.show()

# Score Distribution Plot
for model_name, path in models.items():
    scores, labels = load_scores_labels(path)
    sig_scores = scores[labels == 1]
    bkg_scores = scores[labels == 0]

    plt.figure(figsize=(6,5))
    plt.hist(bkg_scores, bins=50, alpha=0.6, label="Background", color='red', histtype='stepfilled', density=True)
    plt.hist(sig_scores, bins=50, alpha=0.6, label="Signal", color='blue', histtype='stepfilled', density=True)
    plt.xlabel("Model Output Score (f_L)")
    plt.ylabel("Normalized Count")
    plt.title(f"{model_name} Output Score Distribution")
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.savefig(f"{model_name.lower()}_score_dist.png", dpi=300)
    plt.show()
