# Split a text file into n incrementaly larger files

parts_count = 10
in_file_name = "key_full.txt"
dir_name = "text_files"
in_file_path = f"{dir_name}/{in_file_name}"

out_file_name = "key"

with open(in_file_path) as in_file:
    data = in_file.read()

for x in range(parts_count):
    end = (x + 1) * int(len(data) / parts_count)
    out_file_path = f"{dir_name}/{out_file_name}{x+1}.txt"
    
    with open(out_file_path, "w") as out_file:
        out_file.write(data[0:end])
        print(end)

        