import matplotlib.pyplot as plt

vm_data = """
1 100000 3.60
2 100000 3.00
4 100000 5.00
8 100000 3.27
16 100000 3.52
32 100000 3.15
64 100000 4.33
128 100000 5.03
256 100000 4.83
512 100000 4.80
1024 100000 4.87
2048 100000 15.61
4096 100000 25.29
"""

mac_data = """
1 100000 3.86
2 100000 2.90
4 100000 2.63
8 100000 2.52
16 100000 2.93
32 100000 3.55
64 100000 3.99
128 100000 4.36
256 100000 4.37
512 100000 4.35
1024 100000 4.26
2048 100000 6.13
4096 100000 12.09
"""

def parse(data):
    pages, times = [], []
    for line in data.strip().splitlines():
        parts = line.split()
        pages.append(int(parts[0]))
        times.append(float(parts[2]))
    return pages, times

vm_p, vm_t = parse(vm_data)
mac_p, mac_t = parse(mac_data)

plt.figure(figsize=(8, 5))
plt.plot(vm_p, vm_t, marker="o", label="VM (Ubuntu 24.04, KVM)")
plt.plot(mac_p, mac_t, marker="s", label="Mac (i9-9880H)")

plt.xscale("log", base=2)
plt.xticks(vm_p, vm_p)
plt.xlabel("Number of pages touched")
plt.ylabel("Time per access (ns)")
plt.title("TLB size measurement")

plt.axvline(64, color="gray", linestyle="--", alpha=0.6)
plt.text(70, 0.5, "L1 dTLB (64 entries)", rotation=90, fontsize=8)
plt.axvline(512, color="gray", linestyle="--", alpha=0.6)
plt.text(530, 0.5, "L2 cache", rotation=90, fontsize=8)
plt.axvline(4096, color="gray", linestyle="--", alpha=0.6)
plt.text(4200, 0.5, "L3 / main memory", rotation=90, fontsize=8)

plt.legend()
plt.grid(True, alpha=0.3)
plt.tight_layout()
plt.show()
