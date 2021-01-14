def test_order(file1, file2):
    ord1 = []
    ord2 = []
    for line in file1.readlines():
        ord1.append(line[18])
    for line in file2.readlines():
        ord2.append(line[18])
    diff = []
    for i in range(len(ord1)):
        if ord1[i] != ord2[i]:
            diff.append((ord1[i], ord2[i]))
    return diff
