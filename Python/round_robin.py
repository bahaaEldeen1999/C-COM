process = [
    {
        "id": 0,
        "start": 0,
        "finish": 2,
        "started": 0,
        "runtime": 2
    },
    {
        "id": 1,
        "start": 0,
        "finish": 4,
        "started": 0,
        "runtime": 4
    },
    {
        "id": 2,
        "start": 1,
        "finish": 6,
        "started": 0,
        "runtime": 5
    },
    {
        "id": 3,
        "start": 2,
        "finish": 9,
        "started": 0,
        "runtime": 9
    },
    {
        "id": 4,
        "start": 8,
        "finish": 19,
        "started": 0,
        "runtime": 7
    }
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
                remaining_time.append(process[i]["runtime"])
                curr_index += 1
            else:
                break
        if len(q) == 0:
            time += 1
            continue
        p = q.pop(0)
        rem_time = remaining_time.pop(0)
        if p["started"] == 0:
            p["wait"] = time-p["start"]
            print("At time "+str(time)+" process "+str(p["id"])+" started arr "+str(p["start"])+" total "+str(
                p["runtime"])+" remain "+str(p["runtime"])+" wait "+str(p["wait"]))
        else:
            p["wait"] += time-p["last"]
            print("At time "+str(time)+" process "+str(p["id"])+" resumed arr "+str(p["start"])+" total "+str(
                p["runtime"])+" remain "+str(rem_time)+" wait "+str(p["wait"]))

        if rem_time > quantum:
            rem_time -= quantum
            time += quantum
        else:
            time += rem_time
            rem_time = 0

        if rem_time > 0:
            p["started"] = 1
            p["last"] = time
            q.append(p)
            remaining_time.append(rem_time)
            print("At time "+str(time)+" process "+str(p["id"])+" stopped arr "+str(p["start"])+" total "+str(
                p["runtime"])+" remain "+str(rem_time)+" wait "+str(p["wait"]))
        else:
            print("At time "+str(time)+" process "+str(p["id"])+" finished arr "+str(p["start"])+" total "+str(
                p["runtime"])+" remain "+str(rem_time)+" wait "+str(p["wait"])+" TA "+str(time-p["start"])+" WTA "+str(round((time-p["start"])/(p["runtime"]), 2)))


round_robin(process, 2)
