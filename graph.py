import matplotlib.pyplot as plt

ms = []
tps = []
with open("unoptimised", "r") as f:
    for line in f.read().split("\n"):
        if line == "": continue;
        a, b = [float(n) for n in line.split(" ")]
        ms.append(a)
        tps.append(b)

ys = range(0, len(ms))
import numpy as np

#plt.plot(xs, np.poly1d(np.polyfit(xs, ys, 1))(xs))


plt.plot(ys, ms)
plt.plot(ys, tps)
plt.show()