def fileHandler(path="../C/processes.txt"):
    lines = open(path, "r").readlines()
    processes = []

    for line in lines:
        if line[0] == '#':
            continue
        x = list(map(int, line.split("\t")))
        processes.append({"id": x[0], "start": x[1],
                          "runtime": x[2], "finish": 0, "priority": x[3], "started": 0})
    return processes

# print(fileHandler())
