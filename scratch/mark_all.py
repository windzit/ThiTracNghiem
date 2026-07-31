# 1. Classes: mark 2 classes deleted (lines 19, 20 -> status '1')
with open("storage/data/classes.txt", "r", encoding="utf-8") as f:
    c_lines = f.readlines()
for i in range(len(c_lines)-2, len(c_lines)):
    if len(c_lines[i]) > 2:
        c_lines[i] = c_lines[i][:-2] + "1\n"
with open("storage/data/classes.txt", "w", encoding="utf-8") as f:
    f.writelines(c_lines)

# 2. Students: mark 10 students deleted (lines 32 to 41 -> status '1')
with open("storage/data/students.txt", "r", encoding="utf-8") as f:
    st_lines = f.readlines()
for i in range(32, 42):
    if i < len(st_lines):
        st_lines[i] = st_lines[i][:-2] + "1\n"
with open("storage/data/students.txt", "w", encoding="utf-8") as f:
    f.writelines(st_lines)

# 3. Subjects: mark 4 subjects deleted (last 4 lines -> status '1')
with open("storage/data/subjects.txt", "r", encoding="utf-8") as f:
    sb_lines = f.readlines()
for i in range(len(sb_lines)-4, len(sb_lines)):
    if len(sb_lines[i]) > 2:
        sb_lines[i] = sb_lines[i][:-2] + "1\n"
with open("storage/data/subjects.txt", "w", encoding="utf-8") as f:
    f.writelines(sb_lines)

# 4. Questions: mark 15 questions deleted (last 15 lines -> status '1')
with open("storage/data/questions.txt", "r", encoding="utf-8") as f:
    q_lines = f.readlines()
for i in range(len(q_lines)-15, len(q_lines)):
    if len(q_lines[i]) > 2:
        q_lines[i] = q_lines[i][:-2] + "1\n"
with open("storage/data/questions.txt", "w", encoding="utf-8") as f:
    f.writelines(q_lines)

print("Marked sample deleted items across all 4 entity types.")
