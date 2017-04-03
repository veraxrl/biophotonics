import urllib.request
import random

seed = random.random()
print(seed)

urllib.request.urlretrieve("http://10.146.13.107/capture?t="+str(seed), "test.jpg")
