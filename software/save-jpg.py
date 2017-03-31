import urllib.request
import random

seed = random.random()
print(seed)

urllib.request.urlretrieve("http://192.168.0.6/capture?t="+str(seed), "local.jpg")
