import copy as cp

def start_action(time, id, arr, total, remain, wait):
    return "At time {0} process {1} started arr {2} total {3}  remain {4} wait {5}".format(time, id, arr, total, remain, wait)

def resume_action(time, id, arr, total, remain, wait):
    return "At time {0} process  {1} resumed arr {2} total {3}  remain {4} wait {5}".format(time, id, arr, total, remain, wait)

def stop_action(time, id, arr, total, remain, wait):
    return "At time {0} process {1} stopped arr {2} total {3}  remain {4} wait {5}".format(time, id, arr, total, remain, wait)

def finish_action(time, id, arr, total, remain, wait, ta, wta):
    return "At time {0} process {1} finished arr {2} total {3}  remain {4} wait {5} TA {6} WTA {7}".format(time, id, arr, total, remain, wait, ta, wta)


def srtn(queue):
    # queue is a list of dictionaries of the coming processes
    # The function returns a list with 3 cells :
    # the first is a list of strings which simulates the needed output of the real program
    # the second is average TA
    # the third one is average WTA

    for i in range(len(queue)):
        queue[i]['remain'] = queue[i]['total'] = queue[i]['finish'] - queue[i]['start']
        queue[i]['wait'] = 0
        queue[i]['has_response'] = False

    total_processes = len(queue)
    run_idx = -1 # the index of process in the todo array
    clk = 0
    todo = [] # list of processes
    log_output = [] # List of strings
    ta_avg = 0
    wta_avg = 0

    while len(queue) or len(todo):
        if run_idx != -1 and todo[run_idx]['remain'] == 0:
            # Running process is done
            todo[run_idx]['wait'] = clk - todo[run_idx]['finish']
            todo[run_idx]['ta'] = clk - todo[run_idx]['start']
            todo[run_idx]['wta'] = round(todo[run_idx]['ta'] / (todo[run_idx]['total']), 2)

            ta_avg += todo[run_idx]['ta']
            wta_avg += todo[run_idx]['wta']

            log_output.append(finish_action(clk, todo[run_idx]['id'], todo[run_idx]['start'],
                                           todo[run_idx]['total'], todo[run_idx]['remain'], todo[run_idx]['wait'],
                                            todo[run_idx]['ta'], todo[run_idx]['wta']))
            todo.remove( todo[run_idx] )
            run_idx = -1

        delete_process_from_queue = []
        for i in range(len(queue)):
            # Storing arrived processes
            if queue[i]['start'] == clk :
                delete_process_from_queue.append(queue[i])
                coming = cp.copy(queue[i])
                todo.append(coming)
            # else:
            #     break

        for proc in delete_process_from_queue:
            queue.remove(proc)

        shortest_rem_idx = -1
        for i in range(len(todo)):
            # Get the best process with shortest rem
            if shortest_rem_idx == -1 or todo[i]['remain'] < todo[shortest_rem_idx]['remain']:
                shortest_rem_idx = i

        if shortest_rem_idx != run_idx:
            if run_idx != -1:
                running = cp.copy(todo[run_idx])
                log_output.append(stop_action(clk, running['id'], running['start'],
                                  running['total'], running['remain'], running['wait']))

            new = todo[shortest_rem_idx]
            run_idx = shortest_rem_idx
            new['wait'] = new['remain'] - (new['finish'] - clk)

            if new['has_response']:
                log_output.append(resume_action(clk, new['id'], new['start'],
                                               new['total'], new['remain'], new['wait']))
            else:
                log_output.append(start_action(clk, new['id'], new['start'],
                                               new['total'], new['remain'], new['wait']))
                new['has_response'] = True

        if run_idx != -1:
            todo[run_idx]['remain'] -= 1

        clk += 1

    return log_output


data = [
    {
        "id": 0,
        "start": 0,
        "finish": 2,
        "started": 0
    },
    {
        "id": 1,
        "start": 0,
        "finish": 4,
        "started": 0
    },
    {
        "id": 2,
        "start": 1,
        "finish": 6,
        "started": 0
    },
    {
        "id": 3,
        "start": 2,
        "finish": 9,
        "started": 0
    },
    {
        "id": 4,
        "start": 8,
        "finish": 19,
        "started": 0
    }
]
ans = srtn(data)

for x in ans:
    print(x)