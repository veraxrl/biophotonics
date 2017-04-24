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
    n = 15
    print("Start captureing " + str(n) + " 1.9MP images...")

    for x in range(0, n):
        print("Start Capturing")
        path = "fetch/img" + str(x) + ".jpg"
        capture(path)
        print("Capturing Done.")
        end = time.clock()
        print("exe time: " + str(end))

        print("sleep for 1 secs")
        time.sleep(1)

    print("All Done")
