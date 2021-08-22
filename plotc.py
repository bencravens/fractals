import numpy as np
from matplotlib import pyplot as plt
from moviepy.editor import ImageSequenceClip

result = []
for i in range(10):
    result.append(np.log(np.loadtxt("{}.csv".format(i),delimiter=',')))

# save it as a gif
clip = ImageSequenceClip(result, fps=20)
clip.write_gif('test.gif', fps=20)
