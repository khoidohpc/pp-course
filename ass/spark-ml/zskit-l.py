from enum import Enum

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from scipy.sparse.csr import csr_matrix
from sklearn import preprocessing
from sklearn.ensemble import RandomForestClassifier
from sklearn.feature_extraction.text import *
from sklearn.linear_model import LogisticRegression
from sklearn.model_selection import cross_val_score, train_test_split
from sklearn.naive_bayes import MultinomialNB
from sklearn import metrics


def read_dataset(file: str, verbose=False) -> pd.DataFrame:
    df = pd.read_csv(file)
    df = df[['Category', 'Descript']]

    if verbose:
        print(df)

    return df


def transform_dataset(df: pd.DataFrame) -> (csr_matrix, np.array):
    # Remove stop words and bag of words
    add_stopwords = ["http", "https", "amp", "rt", "t", "c", "the"]
    count_vect = CountVectorizer(stop_words=add_stopwords, min_df=5)
    X_train_counts = count_vect.fit_transform(df['Descript'])
    # TF - IDF
    tfidf_transformer = TfidfTransformer()
    X_train_tfidf = tfidf_transformer.fit_transform(X_train_counts)
    # String label indexing
    le = preprocessing.LabelEncoder()
    le.fit(df['Category'])

    X_out = X_train_tfidf
    y_out = le.transform(df['Category'])

    return X_out, y_out


class Classifier(Enum):
    LogisticRegression = 1
    NaiveBayes = 2
    RandomForest = 3


def train_model(X: csr_matrix, y: np.array, type: Classifier):
    clasifiter = None

    if type == Classifier.LogisticRegression:
        # Use Logistic Regression
        clasifiter = LogisticRegression(random_state=0)
    elif type == Classifier.NaiveBayes:
        # Use Naive Bayes
        clasifiter = MultinomialNB()
    elif type == Classifier.RandomForest:
        # Use Random Forest
        clasifiter = RandomForestClassifier(
            n_estimators=200, max_depth=3, random_state=0)
    else:
        raise "Algorithm is not implemented."

    X_train, X_test, y_train, y_test = train_test_split(X, y, random_state=0)

    model = clasifiter.fit(X_train, y_train)
    y_pred = model.predict(X_test)

    print(metrics.classification_report(y_test, y_pred))


if __name__ == "__main__":
    df = read_dataset('data/train.csv')
    X_data, y_data = transform_dataset(df)

    print(X_data.shape)

    # train_model(X_data, y_data, Classifier.LogisticRegression)
    train_model(X_data, y_data, Classifier.NaiveBayes)
    # train_model(X_data, y_data, Classifier.RandomForest)
