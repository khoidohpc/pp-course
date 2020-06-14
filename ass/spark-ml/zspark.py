from pyspark import SparkContext
from pyspark.sql import SQLContext, DataFrame
import pyspark.sql.functions as F
from pyspark.ml import Pipeline
from pyspark.ml.feature import *
from pyspark.ml.classification import *
from pyspark.ml.evaluation import *
from enum import Enum

# Initialize spark


def start_spark() -> (SparkContext, SQLContext):
    spark_context = SparkContext()
    sql_context = SQLContext(spark_context)

    return spark_context, sql_context

# Load dataset


def read_dataset(sql_context: SQLContext, file: str, verbose=False) -> DataFrame:
    csv_reader = sql_context.read.options(
        inferschema='true',
        header='true').format('csv')

    df = csv_reader.load(file)
    df = df.select(['Category', 'Descript'])

    if verbose:
        df.printSchema()

        df.groupBy("Category").count()\
            .orderBy(F.col("count").desc()).show()

        df.groupBy("Descript").count() \
            .orderBy(F.col("count").desc()).show()

    return df

# Transform data


def transform_dataset(dataset: DataFrame, verbose=False):
    # regular expression tokenizer
    regexTokenizer = RegexTokenizer(
        inputCol="Descript", outputCol="words", pattern="\\W")
    # stop words
    add_stopwords = ["http", "https", "amp", "rt", "t", "c", "the"]
    stopwordsRemover = StopWordsRemover(
        inputCol="words", outputCol="filtered").setStopWords(add_stopwords)
    # bag of words count
    countVectors = CountVectorizer(
        inputCol="filtered", outputCol="features", vocabSize=3000, minDF=5)
    # label string index
    label_stringIdx = StringIndexer(inputCol="Category", outputCol="label")

    pipeline = Pipeline(
        stages=[regexTokenizer, stopwordsRemover, countVectors, label_stringIdx])

    # Fit the pipeline to training documents.
    pipelineFit = pipeline.fit(dataset)
    trans_dataset = pipelineFit.transform(dataset)

    if verbose:
        trans_dataset.show(5)

    return trans_dataset


# Train model

class Classifier(Enum):
    LogisticRegression = 1
    NaiveBayes = 2
    RandomForest = 3


def train_model(train_data: DataFrame, test_data: DataFrame, type: Classifier, verbose=False):
    clasifiter = None

    if type == Classifier.LogisticRegression:
        # Use Logistic Regression
        clasifiter = LogisticRegression(
            maxIter=20, regParam=0.3, elasticNetParam=0)
    elif type == Classifier.NaiveBayes:
        # Use Logistic Regression
        clasifiter = NaiveBayes(smoothing=1)
    elif type == Classifier.RandomForest:
        # Use Logistic Regression
        clasifiter = RandomForestClassifier(labelCol="label", featuresCol="features",
                                            numTrees=100, maxDepth=4, maxBins=32)
    else:
        raise "Algorithm is not implemented."

    model = clasifiter.fit(train_data)
    predictions = model.transform(test_data)

    # Evaluate model
    evaluator = MulticlassClassificationEvaluator(predictionCol="prediction")
    print(evaluator.evaluate(predictions))

    if verbose:
        predictions.filter(predictions['prediction'] == 0) \
            .select("Descript", "Category", "probability", "label", "prediction") \
            .orderBy("probability", ascending=False) \
            .show(n=10, truncate=30)


if __name__ == "__main__":
    try:
        spark_context, sql_context = start_spark()

        data = read_dataset(sql_context, 'data/train.csv')

        trans_data = transform_dataset(data)

        (train_data, test_data) = trans_data.randomSplit([0.7, 0.3], seed=100)

        train_model(train_data, test_data, Classifier.RandomForest, True)

    finally:
        spark_context.stop()
