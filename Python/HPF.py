
from fileHandler import *

process = fileHandler()


def HPF(processes, path="../schedulerp.log"):
    process_queue = sorted(
        processes, key=lambda x: x['priority'], reverse=False)
    time = 0
    output_file = open(path, "w")
    output_file.write(
        "#At time x process y state arr w total z remain y wait k \n")
    while len(process_queue):
        servied = False
        for process in process_queue:
            if process['start'] <= time:
                output_file.write("At time {} process {} started arr {} total {} remain {} wait {}\n".format(
                    time,
                    process['id'],
                    process['start'],
                    process['runtime'],
                    process['runtime'],
                    time-process['start']))
                time += process['runtime']
                ind = processes.index(process)
                processes[ind]['finish'] = time
                servied = True
                WTA = round(
                    (process['finish']-process['start'])/process['runtime'], 2)
                output_file.write("At time {} process {} finished arr {} total {} remain {} wait {} TA {} WTA {}\n".format(
                    time,
                    process['id'],
                    process['start'],
                    process['runtime'],
                    0,
                    time-process['runtime']-process['start'],
                    process['finish']-process['start'],
                    WTA if WTA > int(WTA) else int(WTA)))
                process_queue.remove(process)
                break
        if not servied:
            time += 1


test = [
    {
        "id": 1,
        "start": 7,
        "priority": 0,
        "runtime": 1,
        "finish": 0
    },
    {
        "id": 2,
        "start": 1,
        "priority": 2,
        "runtime": 6,
        "finish": 0
    },
    {
        "id": 3,
        "start": 6,
        "priority": 3,
        "runtime": 4,
        "finish": 0
    },
    {
        "id": 4,
        "start": 0,
        "priority": 4,
        "runtime": 2,
        "finish": 0
    }
]
HPF(process)
