
for /l %%x in (1, 1, 10) do (
    @REM test text length vs time
    cpu\build\Release\kmp_cpu.exe text_files\text%%x.txt text_files\key1.txt results\cpu-mp-text%%x-key1.csv multithread
    cpu\build\Release\kmp_cpu.exe text_files\text%%x.txt text_files\key1.txt results\cpu-sp-text%%x-key1.csv singlethread
    gpu\build\Release\kmp_gpu.exe gpu\string_search_kernel.cl text_files\text%%x.txt text_files\key1.txt results\gpu-text%%x-key1.csv

    @REM test keyword length vs time
    cpu\build\Release\kmp_cpu.exe text_files\text10.txt text_files\key%%x.txt results\cpu-mp-text10-key%%x.csv multithread
    cpu\build\Release\kmp_cpu.exe text_files\text10.txt text_files\key%%x.txt results\cpu-sp-text10-key%%x.csv singlethread
    gpu\build\Release\kmp_gpu.exe gpu\string_search_kernel.cl text_files\text10.txt text_files\key%%x.txt results\gpu-text10-key%%x.csv

)