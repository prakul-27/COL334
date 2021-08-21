def parse():
    import json
    from textwrap import dedent
    import pingparsing

    parser = pingparsing.PingParsing()
    s = open("sample.txt", "r").read()
    stats = parser.parse(dedent(s))
    d = stats.as_dict()

    result = ()
    if d["packet_loss_count"] == 1:
        # parse hardcoded, return IP
        start = 7 + 19 + 2 * len(d["destination"]) + 2 + 5 + 3
        ip = ""
        for i in range(start, len(s)):
            if s[i] == ' ':
                break
            ip += s[i]
        result = (ip,)
    else:
        # return IP, rtt values
        result = (d["destination"], str(d["rtt_min"]), str(d["rtt_avg"]), str(d["rtt_max"]), str(d["rtt_mdev"]))
    return result

def plot():
    import pandas as pd
    import matplotlib.pyplot as plt
    df = pd.read_csv("dataPoints.csv")

    hops = list(df.hops)
    rtt = list(df.rtt)

    plt.plot(hops, rtt)
    plt.xlabel("Hops")
    plt.ylabel("RTT (in microseconds)")
    plt.title("Hops vs RTT")

    plt.savefig('graph.png')

    return 1