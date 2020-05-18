#!/usr/bin/python3
# -*- coding: utf-8 -*-

import re
import math
import csv
import nltk
import numpy as np
from nltk.corpus import stopwords
import collections
import sys

def read_file(filename):
    with open(filename, "r", encoding="utf-8") as file:
        file = file.read().lower().splitlines()
    return file


def save_data(filename, vector):
    with open(filename, "w", newline="") as file:
        writer = csv.writer(file, delimiter=",")
        writer.writerows(vector)


def clean_data(data):
    cleaned = [
        [lemma_stem(re.sub("[^a-z]", "", term)) for term in doc[0].split()]
        for doc in data
    ]
    return cleaned


def lemma_stem(word):
    stemmer = nltk.PorterStemmer()
    lemmatizer = nltk.WordNetLemmatizer()

    word = stemmer.stem(word)
    word = lemmatizer.lemmatize(word)
    return word


def term_frequency(data, stopwords):
    tf = []
    dic = collections.Counter()

    for doc in data:
        bag = collections.Counter()
        d = collections.Counter()

        for term in doc:
            if len(term) > 1:  # remove stopwords
                if term not in stopwords:
                    d[term] = 1
                    bag[term] += 1

        bag = {term: math.log(1 + bag[term]) for term, count in bag.items()}
        tf.append(bag)
        dic += d

    return tf, dic


def inverse_doc_freq(data, dic):
    idf = {
        term: math.log(float(len(data)) / float(dic.get(term, 1)))
        for doc in data
        for term in doc
    }
    return idf


def tfidf(tf, idf):
    tf_idf = []
    for doc in tf:
        ti = {}
        for term, freq in doc.items():
            ti[term] = freq * idf[term]
        tf_idf.append(ti)
    return tf_idf


def create_vector(tf_idf, dic):
    vector = []

    for doc in tf_idf:
        v = []
        for term in dic:
            if term in doc:
                v.append(doc[term])
            else:
                v.append(0)
        vector.append(v)
    return vector


def reduce_features(dic, max_feature=None):
    if max_feature == None:
        return dic

    max_feature = len(dic) if max_feature > len(dic) else max_feature

    return dict(dic.most_common(max_feature))


def random_projection(mat, k):
    if k >= len(mat[0]):
        return mat
    # s = 1 / ( 1 / math.sqrt(k))
    A = np.asarray(mat)
    P = np.random.choice([-math.sqrt(3), 0, math.sqrt(3)], size=[len(mat[0]), k], p=[1/6, 2/3, 1/6])
    # P = np.random.choice([-(math.sqrt(s / k)), 0, (math.sqrt(s / k))], size=[len(mat[0]), k], p=[1/(2*s), 1 - 1 / s, 1/(2*s)])
    R = np.dot(A, P)

    return R.tolist()


def process(filename, max_features, mode):
    nltk.download("wordnet")
    nltk.download("stopwords")

    stop_words = set(stopwords.words("french"))
    data = read_file(filename)

    data = [line.rsplit(",", 1) for line in data]

    tokens = clean_data(data)

    tf, dic = term_frequency(tokens, stop_words)

    if int(mode) == 0:
        dic = reduce_features(dic, max_features)

    idf = inverse_doc_freq(tf, dic)
    tf_idf = tfidf(tf, idf)

    vector = create_vector(tf_idf, dic)
    
    if int(mode) == 1:
        vector_reduced = random_projection(vector, max_features)
        vector = vector_reduced
    
    for i, l in enumerate(vector):
        print(data[i][-1])
        l.append(data[i][-1])

    s = "\n".join(",".join(str(n) for n in line) for line in vector)
   
    save_data(filename.split(".")[0] + "_processed.csv", vector)

    return s


if __name__ == "__main__":
    if len(sys.argv) == 4:
        process(sys.argv[1], int(sys.argv[2]), int(sys.argv[3]))
    else:
        print("Missing argument(s)")