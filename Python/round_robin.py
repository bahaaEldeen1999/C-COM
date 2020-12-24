process = [
    {
        "id": 0,
        "start": 0,
        "finish": 2,
        "starterd": 0
    },
    {
        "id": 1,
        "start": 0,
        "finish": 4,
        "starterd": 0
    },
    {
        "id": 2,
        "start": 1,
        "finish": 6,
        "starterd": 0
    },
    {
        "id": 3,
        "start": 2,
        "finish": 9,
        "starterd": 0
    },
    {
        "id": 4,
        "start": 8,
        "finish": 19,
        "starterd": 0
    },
    {
        "id": 5,
        "start": 108,
        "finish": 190,
        "starterd": 0
    },
    {
        "id": 6,
        "start": 1008,
        "finish": 1090,
        "starterd": 0
    },
]


def round_robin(process, quantum):

    q = []
    q.append(process[0])
    time = q[0]["start"]
    remaining_time = []
    remaining_time.append(q[0]["finish"]-q[0]["start"])
    curr_index = 1
    while len(q) != 0 or curr_index < len(process):
        temp = curr_index
        for i in range(temp, len(process)):
            if process[i]["start"] <= time:
                q.append(process[i])
                remaining_time.append(process[i]["finish"]-process[i]["start"])
                curr_index += 1
            else:
                break
        if len(q) == 0:
            time += 1
            continue
        p = q.pop(0)
        rem_time = remaining_time.pop(0)
        # if p["started"] == 0:
        #    print("At time "+str(time)+" process "+str(p["id"]+" "))
        if rem_time > quantum:
            rem_time -= quantum
            time += quantum
        else:
            time += rem_time
            rem_time = 0

        if rem_time > 0:
            p["started"] = 1
            q.append(p)
            remaining_time.append(rem_time)
        else:
            print("finnished process "+str(p["id"]) + " at "+str(time))


round_robin(process, 2)
