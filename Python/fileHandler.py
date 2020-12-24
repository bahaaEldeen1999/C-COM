def fileHandler(path="../code/processes.txt"):
    lines = open(path, "r").readlines()
    processes = []

    for line in lines:
        if line[0] == '#':
            continue
        x = list(map(int, line.split("\t")))
        processes.append({"id": x[0], "start": x[1],
                          "finish": x[2]+x[1], "priority": x[3]})
    return processes
