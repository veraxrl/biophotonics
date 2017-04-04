import urllib.request
import random
import time


def hello():
    print("hello")


def capture(path):
    seed = random.random()
    # print(seed)
    urllib.request.urlretrieve("http://10.146.13.107/capture?t=" + str(seed),
                               path)


if __name__ == "__main__":
    n = 2
    print("Start captureing " + str(n) + " 5MP images...")

    for x in range(0, n):
        path = "Mon tests/img" + str(n) + ".jpg"
        capture(path)
        end = time.clock()
        print("exe time: " + str(end))

        print("sleep for 5 secs")
        time.sleep(5)

    print("Done")
