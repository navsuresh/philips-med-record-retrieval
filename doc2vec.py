import pickle
import pandas as pd
import numpy
import re
import os
import numpy as np
import gensim
from sklearn.cluster import KMeans
from sklearn.decomposition import PCA
from gensim.models import Doc2Vec
import matplotlib.pyplot as plt

import glob
import pandas as pd

path = '../clicr-master/dataset/no-ent/cnnlike/test/*'
docs = glob.glob(path)
doc_rows = []
for doc in docs:
    f = open(doc, 'r')
    doc_rows.append([f.read()])
    f.close()
	
doc_rows = [[i[0].split('\n')[2]] for i in doc_rows]    
train = pd.DataFrame(doc_rows, columns = (['text']))

# print(doc_rows[0])
# print(len(doc_rows))

LabeledSentence1 = gensim.models.doc2vec.TaggedDocument
all_content_train = []
j = 0

for em in train['text'].values:
    all_content_train.append(LabeledSentence1(em,[j]))
    j+=1
print("Number of texts processed: ",j)

d2v_model = Doc2Vec(all_content_train, size = 100, window = 10, min_count = 500, workers=7, dm = 1,alpha=0.025, min_alpha=0.001)
d2v_model.train(all_content_train, total_examples=d2v_model.corpus_count, epochs=10, start_alpha=0.002, end_alpha=-0.016)

kmeans_model = KMeans(n_clusters=4, init='k-means++', max_iter=100)
X = kmeans_model.fit(d2v_model.docvecs.doctag_syn0)
labels=kmeans_model.labels_.tolist()

l = kmeans_model.fit_predict(d2v_model.docvecs.doctag_syn0)
pca = PCA(n_components=2).fit(d2v_model.docvecs.doctag_syn0)
datapoint = pca.transform(d2v_model.docvecs.doctag_syn0)

plt.figure
label1 = ["#FFFF00", "#008000", "#0000FF", "#800080"]
color = [label1[i] for i in labels]
plt.scatter(datapoint[:, 0], datapoint[:, 1], c=color)
centroids = kmeans_model.cluster_centers_
centroidpoint = pca.transform(centroids)
plt.scatter(centroidpoint[:, 0], centroidpoint[:, 1], marker='^', s=150, c='#000000')
plt.show()

# Alternate - check out this link: https://ai.intelligentonlinetools.com/ml/text-clustering-doc2vec-word-embedding-machine-learning/
