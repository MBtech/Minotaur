import numpy as np

#header 
#userid, locationid, bpm, baseline, regularity, p_val, p_length, qrs_val, qrs_length

normal_pattern = [90, 1, 1, 1, 100, 3, 90]
af_pattern = [300, 0, 0, 0, 0, 3, 110]
vf_pattern = [300, 0, 0, 0, 0, 0, 0]
pattern = [normal_pattern, af_pattern, vf_pattern]
data = list()
for j in range(0, 10000):
    i = int(np.random.choice([0, 1, 2], 1, p=[0.90, 0.05, 0.05]))
    loc = int(np.random.choice(range(0,5), 1))
    line = list()
    line.append(j)
    line.append(loc)
    line.extend(pattern[i])
    data.append(line)
data = np.asarray(data)
np.savetxt("heart.csv", data, delimiter=",", fmt='%d')
