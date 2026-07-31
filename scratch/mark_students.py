with open("storage/data/students.txt", "r", encoding="utf-8") as f:
    lines = f.readlines()

for i in range(32, 42):
    lines[i] = lines[i][:-2] + "1\n"

with open("storage/data/students.txt", "w", encoding="utf-8") as f:
    f.writelines(lines)

print("Marked 10 students with status 1.")
