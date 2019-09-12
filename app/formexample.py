from flask import Flask, render_template, request, flash  
from forms import QueryForm 

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
from nltk import word_tokenize
from nltk.corpus import stopwords
import copy

app = Flask(__name__)  
app.secret_key = 'development key'  
fname = "../my_doc2vec.model"

from flask_table import Table, Col



stopwords_1 = stopwords.words('english')
stopwords_1 = set(stopwords_1)
def preprocess(doc):
    doc = doc.lower()  # Lower the text.
    doc = word_tokenize(doc)  # Split into words.
    
    global stopwords_1 
#    stopwords_1 = set(stopwords_1)
    doc = [w for w in doc if not w in stopwords_1]  # Remove stopwords.
    doc = [w for w in doc if w.isalpha()]  # Remove numbers and punctuation.
    return doc
 
class Results(Table):
   score = Col('Score')
   title = Col('Title')
 
@app.route('/', methods = ['GET', 'POST'])  
def search():  
   form = QueryForm()  
   if form.validate() == False:  
      flash('All fields are required.')  
   return render_template('search.html', form = form, table = "")  
 
 
 
@app.route('/success',methods = ['GET','POST'])  
def success():  
   form = QueryForm()  
   if form.validate() == False:  
      flash('All fields are required.')  
   req = request.form['query']
   print("REQUEST: ", req)
   results = search(req)
   table = Results(results)
   table.border = True
   return render_template("search.html", table = table, form = form) 

def search(req):
   s = [req]
   s[0] = preprocess(s[0])
   # s[0] = ' '.join(s[0])
   d2v_model = Doc2Vec.load(fname)
   infer_vector = d2v_model.infer_vector(s[0])
   similar_documents = d2v_model.docvecs.most_similar(positive = [infer_vector], topn = 25)
   # similar_documents = d2v_model.most_similar_cosmul(positive = s[0])
   for i in range(len(similar_documents)):
      similar_documents[i] = dict(score = str(round(similar_documents[i][1], 2)), title = str(similar_documents[i][0]))
   return similar_documents
  
if __name__ == '__main__':  
   app.run(debug = True)  