import numpy as np
import matplotlib.pyplot as plt

def simulate_fairness(max_length=1000, trials=1000):
    lengths = np.unique(np.logspace(0, 3, 100).astype(int))
    fairness_results = []

    for L in lengths:
        first_finish_times = []
        second_finish_times = []

        for _ in range(trials):
            job1, job2, time = 0, 0, 0

            while job1 < L and job2 < L:
                if np.random.rand() < 0.5:
                    job1 += 1
                else:
                    job2 += 1
                time += 1

            first_finish = time
            second_finish = 2 * L

            first_finish_times.append(first_finish)
            second_finish_times.append(second_finish)

        avg_fairness = np.mean(np.array(first_finish_times) / np.array(second_finish_times))
        fairness_results.append(avg_fairness)
    return lengths, fairness_results

# run the sim
lengths, fairness = simulate_fairness(1000, 1000)

plt.figure(figsize=(6, 5))
plt.plot(lengths, fairness, color='orange', linewidth=3)
plt.axhline(1.0, color='black', linestyle='--', linewidth=1)

plt.xscale('log')
plt.xlim(1, 1000)
plt.ylim(0, 1.05)
plt.xticks([1, 10, 100, 1000], ['1', '10', '100', '1000'])
plt.yticks([0.0, 0.2, 0.4, 0.6, 0.8, 1.0])

plt.xlabel('Job Length', fontsize=14)
plt.ylabel('Fairness', fontsize=14)

plt.gca().spines[['top', 'right']].set_visible(False)
plt.tight_layout()
plt.show()
