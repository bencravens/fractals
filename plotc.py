import numpy as np
from matplotlib import pyplot as plt
from PIL import Image

for i in range(35):
    arr = np.log(np.loadtxt("{}.csv".format(i),delimiter=','))
    plt.pcolor(arr)
    plt.savefig("{}.png".format(i))
    plt.close()
