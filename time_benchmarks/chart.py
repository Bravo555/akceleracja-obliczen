import csv
from matplotlib import pyplot as plt

text_dir = "text_files"
res_dir = "results"
count = 10

def get_average(lst: list[int]):
    return sum(lst) / len(lst)

def get_text_lengths(file_prefix):
    file_lengths = []

    for x in range(count):
        file_path = f"{text_dir}/{file_prefix}{x+1}.txt"
        
        with open(file_path, "r") as txt_file:
            data = txt_file.read()
            file_lengths.append(len(data))
    
    return file_lengths

def get_exec_time(file_path: str):
    times = []
    with open(file_path, "r", newline="") as csvfile:
        file_reader = csv.reader(csvfile)

        # skip header
        next(file_reader)

        for lines in file_reader:
            times.append(int(lines[0]))

    return get_average(times)        

def get_text_len_vs_time():
    gpu_times = []
    cpu_sp_times = []
    cpu_mp_times = []

    for x in range(1, count+1):
        gpu_path = f"{res_dir}/gpu-text{x}-key1.csv"
        cpu_sp_path = f"{res_dir}/cpu-sp-text{x}-key1.csv"
        cpu_mp_path = f"{res_dir}/cpu-mp-text{x}-key1.csv"

        gpu_time = get_exec_time(gpu_path)
        cpu_sp_time = get_exec_time(cpu_sp_path)
        cpu_mp_time = get_exec_time(cpu_mp_path)

        gpu_times.append(gpu_time)
        cpu_sp_times.append(cpu_sp_time)
        cpu_mp_times.append(cpu_mp_time)

    return gpu_times, cpu_sp_times, cpu_mp_times


def get_key_len_vs_time():
    gpu_times = []

    for x in range(1, count+1):
        gpu_path = f"{res_dir}/gpu-text{count}-key{x}.csv"
        gpu_time = get_exec_time(gpu_path)
        gpu_times.append(gpu_time)

    return gpu_times

def main():
    gpu_times, cpu_sp_times, cpu_mp_times = get_text_len_vs_time()

    print(gpu_times)

    x = get_text_lengths("text")
   
    f, ar = plt.subplots(1,2)

    ar[0].plot(x, gpu_times, color="r", marker="*", label="GPU")
    ar[0].plot(x, cpu_sp_times, color="g", marker="*", label="CPU")
    ar[0].plot(x, cpu_mp_times, color="b", marker="*", label="CPU multithread")
    ar[0].set_xlabel("Liczba znaków tekstu")
    ar[0].set_ylabel("Czas wykonania [μs]")
    ar[0].set_title("Czas względem długości tekstu")
    ar[0].legend()

    x = get_text_lengths("key")

    gpu_times_key = get_key_len_vs_time()

    ar[1].plot(x, gpu_times_key, color="r", marker="*", label="GPU")
    ar[1].set_xlabel("Liczba znaków wzorca")
    ar[1].set_ylabel("Czas wykonania [μs]")
    ar[1].set_title("Czas względem długości wzorca")
    ar[1].legend()

    plt.show()


if __name__ == "__main__":
    main()