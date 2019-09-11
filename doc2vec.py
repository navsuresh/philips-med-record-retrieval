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
from gensim.test.utils import get_tmpfile
import glob
import pandas as pd
from sklearn import metrics
import gensim.models as g
import codecs

path = '../clicr-master/dataset/no-ent/cnnlike/test/*'
docs = glob.glob(path)
doc_rows = []
for doc in docs:
    f = open(doc, 'r')
    doc_rows.append([f.read()])
    f.close()
	
doc_rows = [[i[0].split('\n')[2]] for i in doc_rows]    
train = pd.DataFrame(doc_rows, columns = (['text']))

LabeledSentence1 = gensim.models.doc2vec.TaggedDocument
all_content_train = []
j = 0

for em in train['text'].values:
    all_content_train.append(LabeledSentence1(em,[j]))
    j+=1
print("Number of texts processed: ",j)

d2v_model = Doc2Vec(all_content_train, size = 100, window = 10, min_count = 500, workers=7, dm = 1,alpha=0.025, min_alpha=0.001)
d2v_model.train(all_content_train, total_examples=d2v_model.corpus_count, epochs=10, start_alpha=0.002, end_alpha=-0.016)

fname = get_tmpfile("my_doc2vec_model")
print(fname)
#load model from saved file
d2v_model.save(fname)
d2v_model = Doc2Vec.load(fname)  

# you can continue training with the loaded model!
#If you’re finished training a model (=no more updates, only querying, reduce memory usage), you can do: 
d2v_model.delete_temporary_training_data(keep_doctags_vectors=True, keep_inference=True)


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
 
 
#Infer vector for a new document:
#Here our text paragraph just 2 words
vector = d2v_model.infer_vector(["echocardiogram"])
print (vector)
print(len(vector))

#inference hyper-parameters
start_alpha=0.01
infer_epoch=1000

test_docs="test_doc2.txt"
#load model
m = Doc2Vec.load(fname)
test_docs = [ x.strip().split() for x in codecs.open(test_docs, "r", "utf-8").readlines() ]
 
X=[]
for d in test_docs:
     
    X.append( m.infer_vector(d, alpha=start_alpha, steps=infer_epoch) )
    
 
k=4
 
from sklearn.cluster import Birch
 
brc = Birch(branching_factor=50, n_clusters=k, threshold=0.1, compute_labels=True)
brc.fit(X)
 
clusters = brc.predict(X)
 
labels = brc.labels_
 
 
print ("Clusters: ")
print (clusters)
 
 
silhouette_score = metrics.silhouette_score(X, labels, metric='euclidean')
 
print ("Silhouette_score: ")

print (silhouette_score)