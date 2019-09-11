from gensim.models import Word2Vec
from gensim.models import KeyedVectors
from nltk import word_tokenize
from nltk.corpus import stopwords
import glob
model = KeyedVectors.load_word2vec_format('../pubmed2018_w2v_200D/pubmed2018_w2v_200D.bin', binary=True)

#Normalize
model.init_sims(replace=True)

path = '../clicr-master/dataset/no-ent/cnnlike/test/*'
docs = glob.glob(path)
doc_rows = []
for doc in docs:
    f = open(doc, 'r')
    doc_rows.append([f.read()])
    f.close()
	
doc_rows = [[i[0].split('\n')[2]] for i in doc_rows]    



from gensim.similarities import WmdSimilarity
num_best = 10
instance = WmdSimilarity(doc_rows, model, num_best=10)

def preprocess(doc):
    doc = doc.lower()  # Lower the text.
    doc = word_tokenize(doc)  # Split into words.
    doc = [w for w in doc if not w in stopwords]  # Remove stopwords.
    doc = [w for w in doc if w.isalpha()]  # Remove numbers and punctuation.
    return doc

sent = 'Very good, you should seat outdoor.'
query = preprocess(sent)

sims = instance[query]  # A query is simply a "look-up" in the similarity class.

print('Query:')
print(sent)
for i in range(num_best):
    print()
    print('sim = %.4f' % sims[i][1])