import numpy as np
from matplotlib import pyplot as plt
result = np.loadtxt("fractal.csv",delimiter=',')
plt.imshow(result)
plt.show()
