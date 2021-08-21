import numpy as np
from matplotlib import pyplot as plt
result = np.loadtxt("fractal.csv",delimiter=',')
#plot on log scale
result = np.log(result)
plt.imshow(result)
plt.show()
