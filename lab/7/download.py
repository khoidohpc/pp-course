import urllib.request as req
import os

base_link = 'http://cran-logs.rstudio.com/'

for year in range(2013, 2020):
    for month in range(1, 13):
        for day in range(1, 29):
            print("GET: " + base_link + ext_link)
            ext_link = '%4d/%4d-%02d-%02d.csv.gz'%(year, year, month, day)
            req.urlretrieve(base_link + ext_link, './data/' + ext_link[5:])

os.system("gunzip ./data/*")