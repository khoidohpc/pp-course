from pyspark.sql import SparkSession, DataFrame, Row
from pyspark.sql.types import *
from pyspark.sql.functions import explode, collect_list, size
from math import sqrt


def cosine_similarity(x: list, y: list) -> float:
    if len(x) == 0 or len(y) == 0 or len(x) != len(y):
        return 2.0

    sum_product: int = 0
    sum_x_sq: int = 0
    sum_y_sq: int = 0

    for i in range(len(x)):
        sum_product += x[i]*y[i]
        sum_x_sq += x[i]*x[i]
        sum_y_sq += y[i]*y[i]

    if sum_x_sq == 0 or sum_y_sq == 0:
        return 0.0

    return sum_product / (sqrt(sum_x_sq)*sqrt(sum_y_sq))


def distance(x: dict, y: dict) -> float:
    index = set(list(x.keys()) + list(y.keys()))
    vector_x = list()
    vector_y = list()

    for i in index:
        try:
            vector_x.append(x[i])
        except:
            vector_x.append(0)

        try:
            vector_y.append(y[i])
        except:
            vector_y.append(0)

    return cosine_similarity(vector_x, vector_y)


def analyze(ds_user: DataFrame) -> None:
    """
    Input data: [user_id:long, history:[(song_id:long, count:long)]]
    Output data: [user:long, sim_list:[(sim_user:long, distance: double)]]
    """

    # Explodes the user history data
    # ds_exploded_user: [user_id:int, song_id:int, count:int]
    ds_exploded_user = ds_users \
        .select("id", explode("history")) \
        .withColumnRenamed("id", "user_id") \
        .withColumnRenamed("key", "song_id") \
        .withColumnRenamed("value", "song_count")

    # Analyzes the song history data
    # ds_songs: [song_id:int, user_set:[user_id:int]]
    ds_songs = ds_exploded_user.groupBy("song_id") \
        .agg(collect_list("user_id").alias("user_set"))

    # The user list of one song is the user cluster
    # clusters: [[user_id:int]]
    clusters = ds_songs.drop('song_id').collect()
    clusters = [x.user_set for x in clusters]

    # Compute user distances
    for idx, cluster in enumerate(clusters):
        ds_user_cluster = ds_users.filter(ds_users.id.isin(cluster))
        drv_user_cluster = ds_user_cluster.collect()
        bc_cluster_user = spark.sparkContext.broadcast(drv_user_cluster)

        def map_func(user):
            similary = dict()
            cluster_user = bc_cluster_user.value
            for sim_user in cluster_user:
                similary[sim_user.id] = distance(
                    user.history, sim_user.history)
            return {"user": user.id, "sim_list": similary}

        ds_user_similarity = ds_user_cluster.rdd.map(map_func).toDF()
        ds_user_similarity.write.parquet('../data/distance/%s' % (idx))


if __name__ == "__main__":

    try:
        spark = SparkSession         \
            .builder                 \
            .master("local[*]")      \
            .appName("Python Spark") \
            .getOrCreate()

        ds_users = spark.read.parquet('../data/user/')

        analyze(ds_users)

    finally:
        spark.stop()
