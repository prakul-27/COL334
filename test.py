import json
from textwrap import dedent
import pingparsing

parser = pingparsing.PingParsing()
s = open("sample.txt", "r").read()
stats = parser.parse(dedent(s))

# print("[extract ping statistics]")
# print(json.dumps(stats.as_dict(), indent=4))

d = stats.as_dict()

def packetLost():
    return d["packet_loss_count"] == 1

def parse():
    result = []
    if packetLost():
        # parse hardcoded, return IP
        result.append(d["destination"])
    else:
        # return IP, rtt values
        result.append(d["destination"])
        result.append(d["rtt_min"])
        result.append(d["rtt_avg"])
        result.append(d["rtt_max"])
        result.append(d["rtt_mdev"])
    return result

result = parse()
print(result)