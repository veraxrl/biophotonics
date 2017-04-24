import urllib.request
import random
import time


def hello():
    print("hello")


def capture(path):
    seed = random.random()
    # print(seed)
    urllib.request.urlretrieve("http://10.146.19.124/capture?t=" + str(seed),
                               path)


if __name__ == "__main__":
    path = "samples/Ilhan/img" + str(0) + ".jpg"
    capture(path)
    print("All Done")
