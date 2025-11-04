#!/usr/bin/env python3
import sys, re

if len(sys.argv) < 3:
    print("用法: python3 dot_to_txt.py input.dot output.txt")
    sys.exit(1)

node_id_map = {}
next_id = 0
edges = []

with open(sys.argv[1]) as f:
    for line in f:
        # 匹配 Node0x... -> Node0x... [color=xxx]
        m = re.search(r'(Node0x[0-9a-fA-F]+)\s*->\s*(Node0x[0-9a-fA-F]+)\[color=(\w+)\]', line)
        if m:
            src, dst, label = m.groups()
            if src not in node_id_map:
                node_id_map[src] = next_id
                next_id += 1
            if dst not in node_id_map:
                node_id_map[dst] = next_id
                next_id += 1
            edges.append((node_id_map[src], node_id_map[dst], label))

with open(sys.argv[2], "w") as out:
    for src, dst, label in edges:
        out.write(f"{src} {dst} {label}\n")

print(f"[dot2txt] 转换完成: {len(edges)} 条边, {len(node_id_map)} 个节点 → {sys.argv[2]}")
