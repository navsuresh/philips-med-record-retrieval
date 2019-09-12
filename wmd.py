from gensim.models import Word2Vec
from gensim.models import KeyedVectors
from nltk import word_tokenize
from nltk.corpus import stopwords
import glob
import copy
model = KeyedVectors.load_word2vec_format('../pubmed2018_w2v_200D/pubmed2018_w2v_200D.bin', binary=True)

#Normalize
model.init_sims(replace=True)

path = '../Philips/clicr/clicr/no-ent/cnnlike/test/*'
docs = glob.glob(path)
doc_rows_og = []
for doc in docs:
    f = open(doc, 'r')
    doc_rows_og.append([f.read()])
    f.close()
	
doc_rows_og = [i[0].split('\n')[2] for i in doc_rows_og]
 
doc_rows = copy.deepcopy(doc_rows_og)

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


for i in range(len(doc_rows)):
    doc_rows[i] = preprocess(doc_rows[i])
#    doc_rows[i][0] = ' '.join(doc_rows[i][0])

    



from gensim.similarities import WmdSimilarity
num_best = 10

instance = WmdSimilarity(doc_rows, model, num_best=10)

sent = 'Very good, you should seat outdoor.'
query = preprocess(sent)

sims = instance[query]  # A query is simply a "look-up" in the similarity class.

print('Query:')
print(sent)
for i in range(num_best):
    print('sim = %.4f' % sims[i][1])